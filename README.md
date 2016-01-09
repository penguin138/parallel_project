# parallel_project
[Conway's Game of Life](https://en.wikipedia.org/wiki/Conway%27s_Game_of_Life) project for Parallel and Distributed Computing course at MIPT 2015.

## Versions

- [x] single threaded (it was created for tests)
- [x] using pthreads
- [x] using MPI point to point operations
- [x] using MPI collective operations
- [x] using OpenMP

## Usage

#### Compiling:

* ```make life``` creates interactive version of Game of Life called life
* ```make test``` creates benchmark version called test

#### When using interactive version you can execute the following commands:

* ```start width height number_of_threads``` - game set up
* ```start field_layout.csv number_of_threads``` - game set up
* ```run number_of_iterations``` - launches the game
* ```stop``` - stops the game
* ```status``` - shows field
* ```quit``` - waits for workers to finish and quits

#### Benchmark usage depends on version:

* pthreads and single threaded:

  ```./test width height number_of_threads number_of_iterations```
* both MPI versions:

  ``` mpirun -np number_of_threads ./test width height number_of_iterations```


Also you can run series of tests by executing ```./cluster_test.sh```
