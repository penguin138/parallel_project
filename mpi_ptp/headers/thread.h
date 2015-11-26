#pragma once
#include "../common_lib/headers/typedefs.h"
#include <vector>
#include <queue>

class FieldManager;

class Thread {
  int threadNumber; // = MPI_Comm_rank
  int numberOfThreads; // = MPI_Comm_size - 1, becase we have master
  ll chunkWidth;
  ll chunkHeight;//it's height only of thread's part, without borders;
  ll numberOfIterations;
  ll currentIteration;
  int leftThread;
  int rightThread;
  bool stopped;
  bool quit;
  bool bordersNotUsed;
  fieldType myPartWithBorders; // has size = chunkHeight + 2;
  borderQueue leftBorders;     // <- they are needed for storing incoming borders,
  borderQueue rightBorders;    // <- because otherwise we could receive borders from another iteration.
                               //    which would've caused inconsistent state of field

public:
  //constructs a new thread wrapper object.
  Thread(int threadNumber,int numberOfThreads);

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

  //updates number of iterations needed to complete.
  void updateIterations(ll numberOfIterations);

  //receives all pending messages from neighbours and master.
  void receivePendingMessages();

  //packs our fieldType to bool array, which can be sent by MPI_Send
  bool* pack(fieldType unpacked);

};
