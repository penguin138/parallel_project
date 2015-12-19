#include <mpi.h>
#include <iostream>

using namespace std;

int main(int argc, char** argv) {
  int result;
  result = MPI_Init(&argc, &argv);
  if (result == MPI_SUCCESS) {
    cout << argc << endl;
    for (int i = 0; i < argc; i++) {
      cout << argv[i] << endl;
    }
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);
    cout << "rank: " << rank << endl;
  } else {
    cout << "init failed" << endl;
  }
  result = MPI_Finalize();
  cout << result;
}
