# parallel_project
[Conway's Game of Life](https://en.wikipedia.org/wiki/Conway%27s_Game_of_Life) project for Parallel and Distributed Computing course at MIPT 2015.

There are two versions of the project: single threaded version(it's created for tests) is on branch master
and multi-threaded version is on branch multithreaded-version.

Here is how to use them:

* ```make life``` creates interactive version of Game of Life called life
* ```make test``` creates benchmark version called test

When using interactive version you can execute the following commands:

* ```start width height number_of_threads``` - game set up
* ```start field_layout.csv number_of_threads``` - game set up
* ```run number_of_iterations``` - launches the game
* ```stop``` - stops the game
* ```status``` - shows field
* ```quit``` - waits for threads to finish and quits

When using benchmark version you just type 4 parameters:
width, height, number of threads and number of iterations.
