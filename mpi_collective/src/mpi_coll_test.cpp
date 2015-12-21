#include <mpi.h>
#include <cstdlib>
#include <iostream>

 int main(int argc, char** argv) {
   MPI_Init(&argc,&argv);
   int size, rank;
   MPI_Comm_size(MPI_COMM_WORLD, &size);
   MPI_Comm_rank(MPI_COMM_WORLD, &rank);
   if (rank == 0) {
     std::cout << rank << ": " << size <<std:: endl;
     int* array = (int*) malloc((size - 1)*sizeof(int));
     for (int i = 0; i < size - 1; i++) {
       array[i] = i;
     }
     int recvBuf;
     MPI_Scatter(array, 1, MPI_INT, &recvBuf, 1, MPI_INT, 0, MPI_COMM_WORLD);
     MPI_Finalize();
   } else {
     int a;
     MPI_Scatter(NULL, 1, MPI_INT, &a, 1, MPI_INT, 0, MPI_COMM_WORLD);
     std::cout << rank << ": " << size << " " << a << std::endl;
     MPI_Finalize();
   }
 }
