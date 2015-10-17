#include "field_manager.h"
#include <time.h>

int main() {
  FieldManager manager(std::cout);
  long long width, height, numberOfIterations;
  std::cin >> width >> height >> numberOfIterations;
  struct timespec start, finish;
  double time;
  clock_gettime(CLOCK_MONOTONIC, &start);
  manager.start(width, height,1);
  manager.run(numberOfIterations);
  clock_gettime(CLOCK_MONOTONIC, &finish);
  time = (finish.tv_sec - start.tv_sec);
  time += (finish.tv_nsec - start.tv_nsec) / 1000000000.0;
  std:: cout << time << " seconds" << std::endl;
}
