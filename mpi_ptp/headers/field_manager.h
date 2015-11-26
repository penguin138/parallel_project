#pragma once
#include <vector>
#include <string>
#include <iostream>
#include "../common_lib/headers/typedefs.h"
#include "thread.h"

class FieldManager {
  fieldType field;
  int numberOfThreads;
  bool stopped;
  std::ostream& out;
public:
  //constructs field manager.
  FieldManager(std::ostream& out);

  //starts Game of Life  with provided distribution.
  void start(const std::string fileName, int numberOfThreads);

  //starts Game of Life with random distribution.
  void start(ll width, ll height, int numberOfThreads);

  // quits
  void quit(bool b);

  //stops Game of Life
  void stop();

  //launches Game of Life for given number of iterations.
  void run(ll iterations);

  //shows current iteration.
  void status();

  //sends initial parts to workers.
  void sendInitialParts();

  //polls workers and gathers their computed parts of field.
  void gatherField();

private:
  //unpacks received bool array to fieldType.
  fieldType unpack(bool* packed,ll rowWidth, ll height);

  //packs fieldType to bool array which can be sent by MPI_Send.
  bool* pack(fieldType unpacked);

  //determines a part of field, on which a thread will work.
  fieldType getThreadPart(int threadNumber);

  //returns thread part borders.
  fieldType getThreadBorders(int threadNumber);

  //determines coordinates of chunk for specific thread.
  std::pair<ll, ll> getChunkHeightAndFirstLine(int threadNumber);

  //parses CSV file.
  void parseCSV(const std::string& fileName);

  //generates field of provided size.
  void generateField(ll wigth, ll height);
};
