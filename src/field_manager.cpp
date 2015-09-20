#include "field_manager.h"
#include <climits>
#include <stdlib.h>
#include <ctime>
#include <fstream>

  FieldManager::FieldManager(std::ostream& out):out(out){}

  void FieldManager::start(std::string fileName, int numberOfThreads) {
    currentIteration = 0;
    stopped = false;
    numberOfiterations = 3;
    this->numberOfThreads = numberOfThreads;
    parseCSV(fileName);
    run(numberOfiterations);
  }

  void FieldManager::start(long width, long height, int numberOfThreads) {
    currentIteration = 0;
    stopped = false;
    numberOfiterations = 3;
    this->numberOfThreads = numberOfThreads;
    generateField(width, height);
    run(numberOfiterations);
  }

  void FieldManager::status() {
    out << std::endl;
    for (long i = 0; i < field.size(); i++) {
        for(long j = 0; j < field[i].size(); j++) {
            if (field[i][j]) {
              out << '*';
            } else {
              out << ' ';
            }
        }
        out << std::endl;
    }
  }

  void FieldManager::run(long numberOfIterations) {
    for (long i = 0; i < numberOfIterations && ! stopped; i++) {
      currentIteration++;
      runIteration();
    }
  }

  void FieldManager::stop() {
      stopped = true;
      out << "iteration number: " << currentIteration << std::endl;
  }

  void FieldManager::quit() {
    //single threaded version.
  }

  void FieldManager::runIteration() {
    std::vector<std::vector<bool> > newField(field);
    for (int i = 0; i < field.size(); i++) {
      for (int j = 0; j < field[i].size(); j++) {
        int sum = sumOfNeighbours(i,j);
        if (field[i][j]) {
          newField[i][j] = (sum == 2) || (sum == 3);
        } else {
          newField[i][j] = (sum == 3);
        }
      }
    }
    field = newField;
  }

  int FieldManager::sumOfNeighbours(long i, long j) {
    int sum = 0;
    long n = field.size();
    long m = field[0].size();
    for (int deltaI = -1; deltaI < 2; deltaI++) {
      for (int deltaJ = -1; deltaJ < 2; deltaJ++) {
        sum += field[n + i + deltaI % n][m + j + deltaJ % m];
      }
    }
    return sum;
  }
  //TODO: implement correct parser for CSV.
  void FieldManager::parseCSV(std::string fileName) {
    std::fstream fin(fileName.c_str());
    std::string buf;
    char symbol;
    std::string number("");
    while(std::getline(fin, buf, '\n')) {
      for(long i = 0; i < buf.size(); i++) {
        symbol = buf[i];
        if (symbol != ',') {
          number+=symbol;
        } else {

        }
      }
    }
  }

  void FieldManager::generateField(long wigth, long height) {
    field = std::vector<std::vector<bool> >(height, std::vector<bool>(wigth,0));
    srand(time(0));
    for(int i = 0; i < wigth; i++) {
      for(int j = 0; j < height; j++) {
        field[i][j] = rand()%2;
      }
    }
  }
