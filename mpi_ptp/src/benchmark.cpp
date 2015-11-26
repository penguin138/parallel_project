#include "../headers/field_manager.h"
#include <time.h>
#include <unistd.h>
#include <cstdlib>
#include <mpi.h>



int main(int argc, char** argv) {
  int result;
  result = MPI_Init(&argc, &argv);
  if (result != MPI_SUCCESS) {
    std::cout << "mpi init failed" << std::endl;
    MPI_Finalize();
    return 1;
  }
  int size, rank;
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  if (rank == 0) {
    FieldManager manager(std::cout);
    ll width, height, numberOfIterations;
    if (argc > 3) {
      width = atol(argv[1]);
      height = atol(argv[2]);
      numberOfIterations = atoi(argv[3]);
    } else {
      std::cout << "incorrect number of arguments" << std:: endl;
      return 1;
    }
    struct timespec start, end;
    double time;
    manager.start(width, height, size - 1);
    clock_gettime(CLOCK_MONOTONIC, &start);
    manager.run(numberOfIterations);
    manager.gatherField();
    clock_gettime(CLOCK_MONOTONIC, &end);
    time = (end.tv_sec - start.tv_sec);
    time += (end.tv_nsec - start.tv_nsec) / 1000000000.0;
    std:: cout << time << " s" << std::endl;
  } else {
    Thread thread = Thread(rank, size - 1);
    thread.run();
  }

  //std::cin >> width >> height >> numberOfThreads >> numberOfIterations;
  return 0;
}
