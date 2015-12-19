#include "../headers/field_manager.h"
#include "../headers/field_manager_commands.h"
#include "../../common_lib/headers/shell.h"
#include <map>
#include <cstdlib>
#include <mpi.h>
#include <time.h>

using namespace std;

int main(int argc, char** argv) {
  int result;
  result = MPI_Init(&argc, &argv);
  if (result != MPI_SUCCESS) {
    cout << "mpi init failed" << endl;
    MPI_Finalize();
    return 1;
  }
  int size, rank;
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  if (rank == 0) {
    map<string, ShellCommand*> commands;
    FieldManager manager(std::cout);
    commands.insert(pair<string, ShellCommand* > (string("start"), new StartCommand(cout, manager)));
    commands.insert(pair<string, ShellCommand* > (string("quit"), new ExitCommand(cout, manager)));
    commands.insert(pair<string, ShellCommand* > (string("run"), new RunCommand(cout, manager)));
    commands.insert(pair<string, ShellCommand* > (string("status"), new StatusCommand(cout, manager)));
    commands.insert(pair<string, ShellCommand* > (string("stop"), new StopCommand(cout, manager)));
    Shell shell(commands, cout, cin);
    shell.start();
    for(std::map<string, ShellCommand*>::iterator it = commands.begin();it != commands.end();it++) {
      delete(it->second);
    }
    /*if (argc == 3) {
      manager.start(argv[1], size -  1);
    } else if (argc == 4) {
      manager.start(atol(argv[1]), atol(argv[2]), size - 1);
    } else {
      cout << "incorrect number of args" << endl;
      MPI_Finalize();
    }*/
    //manager.status();
    /*struct timespec start, end;
    double time;
    clock_gettime(CLOCK_MONOTONIC, &start);
    manager.sendInitialParts();
    manager.gatherField();
    clock_gettime(CLOCK_MONOTONIC, &end);
    time = (end.tv_sec - start.tv_sec);
    time += (end.tv_nsec - start.tv_nsec) / 1000000000.0;
    std:: cout << time << " s" << std::endl;
    //manager.status();
    */
    MPI_Finalize();
  } else {
    Thread thread = Thread(rank, size - 1);
    thread.run();
    //cout << "main" << endl;
  }
  //cout << "done" << endl;
}
