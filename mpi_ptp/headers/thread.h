#pragma once
#include "typedefs.h"
#include <vector>

class FieldManager;

class Thread {
  //pthread_t threadDescriptor;
  int threadNumber;
  int numberOfThreads;
  ll chunkWidth;
  ll chunkHeight;//it's height only of thread's part, without borders;
  ll numberOfIterations;
  ll currentIteration;
  int leftThread;
  int rightThread;
  fieldType myPartWithBorders; // has size = chunkHeight + 2;
public:
  //constructs a new thread wrapper object.
  Thread(int threadNumber,int numberOfThreads,
    int numberOfIterations);

  void receiveInitialPart();
  //retrieve computed part from thread.
  void sendComputedPart();

  //runs Game Of Life.
  void run();

private:
  //performs one iteration of Game of Life on provided part of field.
  void oneIteration();

  //sends computed borders to adjacent threads and receives needed borders from them.
  void exchangeBorders();

  //counts live cells around our cell.
  int numberOfNeighbours(ll i, ll j);

  bool* pack(fieldType unpacked);

};
