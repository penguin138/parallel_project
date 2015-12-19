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
  stopped = false;
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
  std::cout << threadNumber <<": receiving initial part.";
  MPI_Recv(&chunkWidth,1,MPI_LONG_LONG,0,INIT_TAG,MPI_COMM_WORLD,&status1);
  MPI_Recv(&chunkHeightWithBorders,1,MPI_LONG_LONG,0,INIT_TAG,MPI_COMM_WORLD,&status2);
  chunkHeight = chunkHeightWithBorders - 2;
  bool* initialPartArray = (bool*) malloc(sizeof(bool)*chunkWidth*chunkHeightWithBorders);
  MPI_Recv(initialPartArray,chunkWidth*chunkHeightWithBorders,MPI::BOOL,0,INIT_TAG,MPI_COMM_WORLD,&status3);
  std::cout << threadNumber << ": part received." << std::endl;
  for(int i = 0;i < chunkHeightWithBorders; i++) {
    myPartWithBorders.push_back(std::vector<bool>());
    for (int j = 0; j < chunkWidth;j++) {
        //std::cout << i << " " << j << std::endl;
        myPartWithBorders[i].push_back(initialPartArray[i*chunkWidth + j]);
    }
  }
  std::cout << threadNumber <<": part unpacked." << std::endl;
  /*for (int i = 0; i < myPartWithBorders.size(); i++) {
    std::cout << std::endl;
    for (int j = 0; j < myPartWithBorders[0].size(); j++) {
      std::cout << (myPartWithBorders[i][j]) << "  ";
    }
  }*/
}

void Thread::updateIterations(ll numberOfIterations) {
  this->numberOfIterations = numberOfIterations + currentIteration;
}

void Thread::sendComputedPart() {
  fieldType computedPart;
  for (int i = 0; i < chunkHeight; i++) {
    computedPart.push_back(myPartWithBorders[i+1]);
  }
  /*
  for (int i = 0; i < computedPart.size(); i++) {
    std::cout << std::endl;
    for (int j = 0; j < computedPart[0].size();j++) {
      std::cout << computedPart[i][j] << " ";
    }
  }*/
  bool* computedPartArray = pack(computedPart);
  MPI_Send(computedPartArray,chunkHeight*chunkWidth,MPI::BOOL,0,FIELD_TAG,MPI_COMM_WORLD);
  free(computedPartArray);
  //std::cout << threadNumber << " : here!" << std::endl;
}

void Thread::run() {
    receiveInitialPart();
    while(!quit) {
      //MPI_Status status;
      std::cout << threadNumber << ": receiving msgs" << std::endl;
      receivePendingMessages(false);
      std::cout << threadNumber << ": computing." << std::endl;
      //std::cout << threadNumber << " " << currentIteration  << " " << numberOfIterations << std::endl;
      while (currentIteration < numberOfIterations && !stopped && !quit) {
          std::cout << threadNumber << " " << currentIteration << std::endl;
          for (int i = 0; i < myPartWithBorders.size(); i++) {
            std::cout << std::endl;
            for (int j = 0; j < myPartWithBorders[0].size(); j++) {
              std::cout << (myPartWithBorders[i][j]) << "  ";
            }
          }
          oneIteration();
          currentIteration++;
      }
    }
    //sendComputedPart();
    MPI_Finalize();
    std::cout << threadNumber << ": done" << std::endl;
}
void Thread::oneIteration() {
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
  std::cout << std::endl;
  for (int i = 0; i < myPartWithBorders.size(); i++) {
    std::cout << std::endl;
    for (int j = 0; j < myPartWithBorders[0].size(); j++) {
      std::cout << myPartWithBorders[i][j] << "  ";
    }
  }
  exchangeBorders();
}

void Thread::exchangeBorders() {
  bool* myLeftBorder = (bool*) malloc(chunkWidth * sizeof(bool));
  bool* myRightBorder = (bool*) malloc(chunkWidth * sizeof(bool));

  for (int i = 0;i < chunkWidth; i++) {
    myLeftBorder[i] = myPartWithBorders[1][i];
    myRightBorder[i] = myPartWithBorders[chunkHeight][i];
  }
  //std::cout << threadNumber << ": " << numberOfThreads << std::endl;
  MPI_Request leftRequest;
  MPI_Request rightRequest;
  int neededTag = currentIteration % numberOfThreads;
  std::cout << threadNumber << ": sending to " << leftThread << std::endl;
  MPI_Issend(myLeftBorder,chunkWidth,MPI::BOOL,leftThread,neededTag,MPI_COMM_WORLD,&leftRequest);
  std::cout << threadNumber << ": sending to " << rightThread << std::endl;
  MPI_Issend(myRightBorder,chunkWidth,MPI::BOOL,rightThread,neededTag,MPI_COMM_WORLD,&rightRequest);
  MPI_Status leftStatus,rightStatus;
  std::cout << threadNumber << ": receiving messages" << leftThread << std::endl;
  receivePendingMessages(true);

  MPI_Wait(&leftRequest, &leftStatus);
  MPI_Wait(&rightRequest, &rightStatus);

  std::cout << "all sent" << std::endl;
  free(myLeftBorder);
  free(myRightBorder);
}
void Thread::receivePendingMessages(bool notOnlyMaster) {
  bool* myLeftRecvBorder = (bool*) malloc(chunkWidth * sizeof(bool));
  bool* myRightRecvBorder = (bool*) malloc(chunkWidth * sizeof(bool));
  bool recvLeft = false;
  bool recvRight = false;
  MPI_Status status,status1,status2;
  int source;
  int tag;
  int mask;
  int neededTag = currentIteration % numberOfThreads;
  //int flag,flag1,flag2;
  /*while((!(recvLeft && recvRight)||stopped) && !quit) {
    mask = recvLeft*4 + recvRight*2 + stopped;
    if ((mask == 0 || mask == 1) && notOnlyMaster) {
      std::cout << threadNumber << ": probe for all" << std::endl;
      MPI_Probe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD,&status);
    } else if ((mask == 2 || mask == 3) && notOnlyMaster) {//right already received.
        std::cout << threadNumber << ": probe for left" << std::endl;
        MPI_Probe(leftThread,neededTag,MPI_COMM_WORLD,&status);
    } else if ((mask == 4 || mask == 5) && notOnlyMaster) {//left already received
        std::cout << threadNumber << ": probe for right" << std::endl;
        MPI_Probe(rightThread,neededTag,MPI_COMM_WORLD,&status);
    } else {
        std::cout << threadNumber << ": probe for master" << std::endl;
        MPI_Probe(0,MPI_ANY_TAG,MPI_COMM_WORLD,&status);
    }
    source = status.MPI_SOURCE;
    tag = status.MPI_TAG;
    if (source == leftThread && tag == neededTag) {
      MPI_Recv(myLeftRecvBorder,chunkWidth,MPI::BOOL,leftThread,neededTag,MPI_COMM_WORLD,&status);
      recvLeft = true;
    } else if (source == rightThread && tag == neededTag) {
      MPI_Recv(myRightRecvBorder,chunkWidth,MPI::BOOL,rightThread,neededTag,MPI_COMM_WORLD,&status);
      recvRight = true;
    } else { //it is master
      if (tag != RUN_TAG) {
        int value;
        MPI_Recv(&value,0,MPI_INT,0,tag,MPI_COMM_WORLD,&status);
        sendComputedPart();
        if (tag == STOP_TAG) {
          stopped = true;
          //std::cout << "sending iter" << std::endl;
          //MPI_Send(&currentIteration,1, MPI_LONG_LONG,0,SYNC_TAG,MPI_COMM_WORLD);
        /*} else if (tag == STOP_TAG_2){
          stopped = true;
          std::cout << threadNumber << ": sending computed part" << std::endl;
          sendComputedPart();
        }else { //it is quit
          quit = true;
        }
      } else { //it is run.
        std::cout << threadNumber << ": run" << std::endl;
        ll iters;
        MPI_Recv(&iters,1,MPI_LONG_LONG,0,tag,MPI_COMM_WORLD,&status);
        std::cout << threadNumber << ": updating iters" << std::endl;
        updateIterations(iters);
        stopped = false;
        if (!notOnlyMaster) {
          break;
        }
      }
    }
  }*/
  if (recvLeft&&recvRight) {
    for (int i = 0;i < chunkWidth; i++) {
      myPartWithBorders[0][i] = myLeftRecvBorder[i];
      myPartWithBorders[chunkHeight+1][i] = myRightRecvBorder[i];
    }
  }
  free(myLeftRecvBorder);
  free(myRightRecvBorder);
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
