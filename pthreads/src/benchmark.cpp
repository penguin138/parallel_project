#include "field_manager.h"
#include <time.h>
#include <unistd.h>
#include <cstdlib>

int main(int argc, char** argv) {
  FieldManager manager(std::cout);
  ll width, height, numberOfThreads, numberOfIterations;
  if (argc > 4) {
    width = atol(argv[1]);
    height = atol(argv[2]);
    numberOfThreads = atoi(argv[3]);
    numberOfIterations = atoi(argv[4]);
  } else {
    std::cout << "incorrect number of arguments" << std:: endl;
    return 1;
  }

  //std::cin >> width >> height >> numberOfThreads >> numberOfIterations;
  struct timespec start, finish;
  double time;
  clock_gettime(CLOCK_MONOTONIC, &start);
  manager.start(width, height, numberOfThreads);
  manager.run(numberOfIterations);
  manager.quit(true);
  clock_gettime(CLOCK_MONOTONIC, &finish);
  time = (finish.tv_sec - start.tv_sec);
  time += (finish.tv_nsec - start.tv_nsec) / 1000000000.0;
  std:: cout << time << " s";
  return 0;
}
