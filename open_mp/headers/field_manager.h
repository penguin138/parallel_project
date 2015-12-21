#pragma once
#include <vector>
#include <string>
#include <iostream>
#include "../common_lib/headers/typedefs.h"
#include "thread.h"

class FieldManager {
  fieldType field;
  Thread* threads;
  ll numberOfiterations;
  ll currentIteration;
  int numberOfThreads;
  int numberOfThreads1;
  std::ostream& out;
  bool stopped;
  bool running;
  bool threadsCreated;
  pthread_cond_t needToStop;
  pthread_mutex_t stopMutex;
public:
  //constructs file manager.
  FieldManager(std::ostream& out);

  //starts Game of Life  with provided distribution.
  void start(const std::string fileName, int numberOfThreads);

  //starts Game of Life with random distribution.
  void start(ll width, ll height, int numberOfThreads);

  //shows current iteration.
  void status();

  //starts Game of Life and runs it for provided numberOfIterations.
  void run(ll numberOfIterations);

  //stops Game of Life.
  void stop();

  //terminates Game of Life.
  void quit(bool wait);

  bool wasStopped();
private:
    //creates threads.
  void createThreads();

  void destroyThreads(bool wait);

  //determines a part of field, on which a thread will work.
  fieldType getThreadPart(int threadNumber);

  //returns thread part borders.
  fieldType getThreadBorders(int threadNumber);

  //determines coordinates of chunk for specific thread.
  std::pair<ll, ll> getChunkHeightAndFirstLine(int threadNumber);

  //polls workers and gathers their computed parts of field.
  void gatherField();

  //parses CSV file.
  void parseCSV(const std::string& fileName);

  //generates field of provided size.
  void generateField(ll wigth, ll height);
};
