#include "../headers/thread.h"
#include "../headers/field_manager.h"
#include <cstdlib>
#include <vector>
#include <iostream>
#include <mpi.h>
#include "../headers/communication_defines.h"

Thread::Thread(int threadNumber, int numberOfThreads):
threadNumber(threadNumber) {
  currentIteration = 0;
  numberOfIterations = 0;
  this->numberOfThreads = numberOfThreads;
  quit = false;
  stopped = true;
  leftThread = threadNumber - 1;
  rightThread = threadNumber + 1;
  if (threadNumber == 1) {
    leftThread = numberOfThreads;
  }
  if (threadNumber == numberOfThreads) {
    rightThread = 1;
  }
}

void Thread::receiveInitialPart() {
  MPI_Status status1,status2,status3;
  ll chunkHeightWithBorders;
  //std::cout << threadNumber <<": receiving initial part.";
  MPI_Recv(&chunkWidth,1,MPI_LONG_LONG,0,INIT_TAG,MPI_COMM_WORLD,&status1);
  MPI_Recv(&chunkHeightWithBorders,1,MPI_LONG_LONG,0,INIT_TAG,MPI_COMM_WORLD,&status2);
  chunkHeight = chunkHeightWithBorders - 2;
  bool* initialPartArray = (bool*) malloc(sizeof(bool)*chunkWidth*chunkHeightWithBorders);
  MPI_Recv(initialPartArray,chunkWidth*chunkHeightWithBorders,MPI::BOOL,0,INIT_TAG,MPI_COMM_WORLD,&status3);
  //std::cout << threadNumber << ": part received." << std::endl;
  for(int i = 0;i < chunkHeightWithBorders; i++) {
    myPartWithBorders.push_back(std::vector<bool>());
    for (int j = 0; j < chunkWidth;j++) {
        //std::cout << i << " " << j << std::endl;
        myPartWithBorders[i].push_back(initialPartArray[i*chunkWidth + j]);
    }
  }
  free(initialPartArray);
}

void Thread::updateIterations(ll numberOfIterations) {
  this->numberOfIterations = numberOfIterations + currentIteration;
}

void Thread::sendComputedPart() {
  fieldType computedPart;
  for (int i = 0; i < chunkHeight; i++) {
    computedPart.push_back(myPartWithBorders[i+1]);
  }
  bool* computedPartArray = pack(computedPart);
  MPI_Send(computedPartArray,chunkHeight*chunkWidth,MPI::BOOL,0,FIELD_TAG,MPI_COMM_WORLD);
  free(computedPartArray);
  //std::cout << threadNumber << " : here!" << std::endl;
}

void Thread::run() {
    receiveInitialPart();
    bordersNotUsed = true;
    leftBorders.push(myPartWithBorders[0]);
    rightBorders.push(myPartWithBorders[chunkHeight+1]);
    while(!quit) {
      stopped = true;
      //std::cout << threadNumber << ": receiving msgs" << std::endl;
      receivePendingMessages();
      //std::cout << threadNumber << ": computing." << std::endl;
      //std::cout << threadNumber << " " << currentIteration  << " " << numberOfIterations << std::endl;
      while (currentIteration < numberOfIterations && !stopped && !quit) {
          oneIteration();
          currentIteration++;
          if (currentIteration == numberOfIterations) {
            sendComputedPart();
          }
      }
    }
    //sendComputedPart();
    MPI_Finalize();
    //std::cout << threadNumber << ": done" << std::endl;
}
void Thread::oneIteration() {
  myPartWithBorders[0] = leftBorders.front();
  leftBorders.pop();
  myPartWithBorders[chunkHeight+1] = rightBorders.front();
  rightBorders.pop();
  int sum;
  fieldType myNewPart(myPartWithBorders);
  for (ll i = 1; i < chunkHeight + 1; i++) {
    for (ll j = 0; j < chunkWidth; j++) {
      sum = numberOfNeighbours(i, j);
      //std::cout << threadNumber << ": [" << i << ", " << j << "] = " << sum << std::endl;
      if (myPartWithBorders[i][j]) {
        myNewPart[i][j] = (sum == 2) || (sum == 3);
      } else {
        myNewPart[i][j] = (sum == 3);
      }
    }
  }
  myPartWithBorders = myNewPart;
  exchangeBorders();
}

void Thread::exchangeBorders() {

  bool* myLeftBorder = (bool*) malloc(chunkWidth * sizeof(bool));
  bool* myRightBorder = (bool*) malloc(chunkWidth * sizeof(bool));

  //preparing borders for send.
  for (int i = 0;i < chunkWidth; i++) {
    myLeftBorder[i] = myPartWithBorders[1][i];
    myRightBorder[i] = myPartWithBorders[chunkHeight][i];
  }
  //posting sends.

  //std::cout << threadNumber << ": " << numberOfThreads << std::endl;
  MPI_Request leftRequest;
  MPI_Request rightRequest;
  //std::cout << threadNumber << ": sending to " << leftThread << std::endl;
  MPI_Isend(myLeftBorder,chunkWidth,MPI::BOOL,leftThread,threadNumber,MPI_COMM_WORLD,&leftRequest);
  //std::cout << threadNumber << ": sending to " << rightThread << std::endl;
  MPI_Isend(myRightBorder,chunkWidth,MPI::BOOL,rightThread,threadNumber,MPI_COMM_WORLD,&rightRequest);

  //receive pending messages on this iteration.
  //std::cout << threadNumber << ": receiving messages" << leftThread << std::endl;
  receivePendingMessages();

  //waiting for neighbours to receive our borders.
  MPI_Status leftStatus,rightStatus;
  MPI_Wait(&leftRequest, &leftStatus);
  MPI_Wait(&rightRequest, &rightStatus);

  //std::cout << "all sent" << std::endl;
  free(myLeftBorder);
  free(myRightBorder);
}
void Thread::receivePendingMessages() {
  MPI_Status status;
  bool* myLeftRecvBorder = (bool*) malloc(chunkWidth * sizeof(bool));
  bool* myRightRecvBorder = (bool*) malloc(chunkWidth * sizeof(bool));
  std::vector<bool> leftBorder,rightBorder;
  int source;
  int tag;
  while((leftBorders.empty() ||rightBorders.empty() || stopped) && !quit) {
    MPI_Probe(MPI_ANY_SOURCE,MPI_ANY_TAG,MPI_COMM_WORLD,&status);
    source = status.MPI_SOURCE;
    tag = status.MPI_TAG;
    if (source == leftThread) {
      //std::cout << threadNumber << ": receiving from left thread" << std::endl;
      MPI_Recv(myLeftRecvBorder,chunkWidth,MPI::BOOL,source,tag,MPI_COMM_WORLD,&status);
      leftBorder.clear();
      for (int i = 0; i< chunkWidth;i++) {
        leftBorder.push_back(myLeftRecvBorder[i]);
      }
      leftBorders.push(leftBorder);
    } else if (source == rightThread) {
      //std::cout << threadNumber << ": receiving from right thread" << std::endl;
      MPI_Recv(myRightRecvBorder,chunkWidth,MPI::BOOL,source,tag,MPI_COMM_WORLD,&status);
      rightBorder.clear();
      for (int i = 0; i< chunkWidth;i++) {
        rightBorder.push_back(myRightRecvBorder[i]);
      }
      rightBorders.push(rightBorder);
    } else { //it is master.
      //std::cout << threadNumber << ": receiving from master" << std::endl;
      if (tag == STOP_TAG) {
        //std::cout << threadNumber << ": stop tag" << std::endl;
        int value;
        MPI_Recv(&value,0,MPI_INT,source,tag,MPI_COMM_WORLD,&status);
        stopped = true;
        MPI_Send(&currentIteration,1,MPI_LONG_LONG,0,SYNC_TAG,MPI_COMM_WORLD);
        //std::cout << threadNumber << ": sent iter" << std::endl;
      } /*else if (tag == STOP_TAG_2){
        std::cout << threadNumber << ": stop tag 2" << std::endl;
        int value;
        MPI_Recv(&value,0,MPI_INT,source,tag,MPI_COMM_WORLD,&status);
        stopped = true;
        sendComputedPart();
      }*/ else if (tag == QUIT_TAG) {
        //std::cout << threadNumber << ": quit tag" << std::endl;
        int value;
        MPI_Recv(&value,0,MPI_INT,source,tag,MPI_COMM_WORLD,&status);
        quit = true;
        sendComputedPart();
      } else { //it is run.
        ll newIters;
        MPI_Recv(&newIters,1,MPI_LONG_LONG,source,tag,MPI_COMM_WORLD,&status);
        updateIterations(newIters);
        //std::cout << threadNumber << ": run tag" << newIters << std::endl;
        stopped = false;
      }
    }
  }
  free(myRightRecvBorder);
  free(myLeftRecvBorder);
}
int Thread::numberOfNeighbours(ll i, ll j) {
  int sum = 0;
  ll p,q;
  for (int deltaI = -1; deltaI < 2; deltaI++) {
    for (int deltaJ = -1; deltaJ < 2; deltaJ++) {
      p = i+deltaI;
      q = j+deltaJ;
      if (p >= chunkHeight + 2) {
        p = 0;
      } else if (p < 0) {
        p = chunkHeight + 1;
      }
      if (q >= chunkWidth) {
        q = 0;
      } else if (q < 0) {
        q = chunkWidth - 1;
      }
      sum += myPartWithBorders[p][q];
    }
  }
  //because it was added to sum in for.
  sum -= myPartWithBorders[i][j];
  return sum;
}

bool* Thread::pack(fieldType unpacked) {
  ll n = unpacked.size();
  ll m = 0;
  if (n > 0) {
    m = unpacked[0].size();
  }
  bool* packed = (bool*) malloc(sizeof(bool)*n*m);
  for (ll i = 0; i < n; i++) {
    for (ll j = 0; j < m; j++) {
      packed[i*m + j] = unpacked[i][j];
    }
  }
  return packed;//user frees pointer.
}
