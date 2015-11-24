#include "../headers/thread.h"
#include "../headers/field_manager.h"
#include <cstdlib>
#include <vector>
#include <iostream>
#include <mpi.h>

Thread::Thread(int threadNumber, int numberOfThreads,
   int numberOfIterations):
threadNumber(threadNumber),
numberOfIterations(numberOfIterations) {
  currentIteration = 0;
  this->numberOfThreads = numberOfThreads;
}

void Thread::receiveInitialPart() {
  MPI_Status status1,status2,status3;
  ll chunkHeightWithBorders;
  //std::cout << threadNumber <<": receiving initial part.";
  MPI_Recv(&chunkWidth,1,MPI_LONG_LONG,0,1,MPI_COMM_WORLD,&status1);
  MPI_Recv(&chunkHeightWithBorders,1,MPI_LONG_LONG,0,2,MPI_COMM_WORLD,&status2);
  chunkHeight = chunkHeightWithBorders - 2;
  bool* initialPartArray = (bool*) malloc(sizeof(bool)*chunkWidth*chunkHeightWithBorders);
  MPI_Recv(initialPartArray,chunkWidth*chunkHeightWithBorders,MPI::BOOL,0,3,MPI_COMM_WORLD,&status3);
  //std::cout << threadNumber << ": part received." << std::endl;
  for(int i = 0;i < chunkHeightWithBorders; i++) {
    myPartWithBorders.push_back(std::vector<bool>());
    for (int j = 0; j < chunkWidth;j++) {
        //std::cout << i << " " << j << std::endl;
        myPartWithBorders[i].push_back(initialPartArray[i*chunkWidth + j]);
    }
  }
  //std::cout << threadNumber <<": part unpacked." << std::endl;
  /*for (int i = 0; i < myPartWithBorders.size(); i++) {
    std::cout << std::endl;
    for (int j = 0; j < myPartWithBorders[0].size(); j++) {
      std::cout << (myPartWithBorders[i][j] + threadNumber) << " ";
    }
  }*/
}

void Thread::sendComputedPart() {
  fieldType computedPart;
  for (int i = 0; i < chunkHeight; i++) {
    computedPart.push_back(myPartWithBorders[i+1]);
  }
  /*for (int i = 0; i < computedPart.size(); i++) {
    std::cout << std::endl;
    for (int j = 0; j < computedPart[0].size();j++) {
      std::cout << computedPart[i][j] << " ";
    }
  }*/
  bool* computedPartArray = pack(computedPart);
  MPI_Send(computedPartArray,chunkHeight*chunkWidth,MPI::BOOL,0,4,MPI_COMM_WORLD);
  free(computedPartArray);
  //std::cout << threadNumber << " : here!" << std::endl;
}

void Thread::run() {
    receiveInitialPart();
    //std::cout << threadNumber << ": computing." << std::endl;
    while (currentIteration < numberOfIterations ) {
        oneIteration();
        //std::cout << threadNumber << " " << currentIteration << std::endl;
        currentIteration++;
    }
    //std::cout << threadNumber << ": done" << std::endl;
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
  exchangeBorders();
}

void Thread::exchangeBorders() {
  bool* myLeftBorder = (bool*) malloc(chunkWidth * sizeof(bool));
  bool* myRightBorder = (bool*) malloc(chunkWidth * sizeof(bool));
  bool* myLeftRecvBorder = (bool*) malloc(chunkWidth * sizeof(bool));
  bool* myRightRecvBorder = (bool*) malloc(chunkWidth * sizeof(bool));
  for (int i = 0;i < chunkWidth; i++) {
    myLeftBorder[i] = myPartWithBorders[1][i];
    myRightBorder[i] = myPartWithBorders[chunkHeight][i];
  }
  leftThread = threadNumber - 1;
  rightThread = threadNumber + 1;
  if (threadNumber == 1) {
    leftThread = numberOfThreads;
  }
  if (threadNumber == numberOfThreads) {
    rightThread = 1;
  }
  //std::cout << threadNumber << ": " << numberOfThreads << std::endl;
  MPI_Request leftRequest;
  MPI_Request rightRequest;
  //std::cout << threadNumber << ": sending to " << leftThread << std::endl;
  MPI_Isend(myLeftBorder,chunkWidth,MPI::BOOL,leftThread,leftThread,MPI_COMM_WORLD,&leftRequest);
  //std::cout << threadNumber << ": sending to " << rightThread << std::endl;
  MPI_Isend(myRightBorder,chunkWidth,MPI::BOOL,rightThread,rightThread,MPI_COMM_WORLD,&rightRequest);
  MPI_Status leftStatus1, leftStatus2;
  MPI_Status rightStatus1, rightStatus2;
  //std::cout << threadNumber << ": receiving from " << leftThread << std::endl;
  MPI_Recv(myLeftRecvBorder,chunkWidth,MPI::BOOL,leftThread,threadNumber,MPI_COMM_WORLD,&leftStatus1);
  //std::cout << threadNumber << ": receiving from " << rightThread << std::endl;
  MPI_Recv(myRightRecvBorder,chunkWidth,MPI::BOOL,rightThread,threadNumber,MPI_COMM_WORLD,&rightStatus1);
  //std::cout << "all received" << std::endl;
  MPI_Wait(&leftRequest, &leftStatus2);
  MPI_Wait(&rightRequest, &rightStatus2);
  //std::cout << "all sent" << std::endl;
  for (int i = 0;i < chunkWidth; i++) {
    myPartWithBorders[0][i] = myLeftRecvBorder[i];
    myPartWithBorders[chunkHeight+1][i] = myRightRecvBorder[i];
  }
  free(myLeftBorder);
  free(myRightBorder);
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
