#include "field_manager.h"
#include <climits>
#include <stdlib.h>
#include <ctime>
#include <fstream>
#include <unistd.h>
#include <pthread.h>
#include "../common_lib/headers/exceptions.h"
#include <omp.h>
#include <cstdarg>

  FieldManager::FieldManager(std::ostream& out):out(out){
    threadsCreated = false;
    newCommandArrived = false;
  }
  void FieldManager::waitForIt() {
    //std::cout << "wait for it!" << std::endl;
    while(true) {
      //std::cout << newCommandArrived << std::endl;
      if (newCommandArrived) {
        newCommandArrived = false;
        //std::cout << "arrived" << std::endl;
        if (command == "start") {
            if (numberOfArgs == 2) {
              start(stringArg, intArg);
            } else {
              start(longLongArg1,longLongArg2,intArg);
            }
            //std::cout << "This will never be printed because of my stupidity" << std::endl;
        } else if (command == "run") {
          run(longLongArg1);
        } else if (command == "status") {
          //std::cout << "second status" << std::endl;
          status();
        } else if (command == "stop") {
          stop();
        } else if (command == "exit") {
          exit(true);
          break;
        }
      }
    }
  }
  void FieldManager::newCommand(const std::string& newCommand,int count,...) {
    command = newCommand;
    numberOfArgs = count;
    va_list args;
    va_start(args, count);
    if (command == "start") {
      if (count == 3) {
          longLongArg1 = va_arg(args, long long);
          longLongArg2 = va_arg(args, long long);
          intArg = va_arg(args, int);
      } else {
          stringArg = va_arg(args, std::string);
          intArg = va_arg(args, int);
      }
    }  else if (command == "run") {
      longLongArg1 = va_arg(args, long long);
    } else if (running) {
        if (command == "stop"){
          stop();
        } else if (command == "exit") {
          exit(true);
        } else if (command == "status") {
          status();
        }
    }
    va_end(args);
    newCommandArrived = true;
    //std::cout << "set" << std::endl;
  }
  void FieldManager::start(std::string fileName, int numberOfThreads) {
    currentIteration = 0;
    stopped = true;
    running = false;
    numberOfiterations = 0;
    //omp_set_num_threads(numberOfThreads);
    this->numberOfThreads = 1;
    this->numberOfThreads1 = numberOfThreads;
    parseCSV(fileName);
    //pthread_mutex_init(&stopMutex, NULL);
    //pthread_cond_init(&needToStop, NULL);
    if (threadsCreated) {
      destroyThreads(false);
    }
    createThreads();
    threadsCreated = true;
  }
  void FieldManager::start(ll width, ll height, int numberOfThreads) {
    if (height < numberOfThreads) {
      numberOfThreads = height;
      out << "too many threads, made number of threads equal height of field" << std::endl;
    }
    currentIteration = 0;
    stopped = true;
    running = false;
    numberOfiterations = 0;
    this->numberOfThreads = 1;
    this->numberOfThreads1 = numberOfThreads;
    generateField(width, height);
    //pthread_mutex_init(&stopMutex, NULL);
    //pthread_cond_init(&needToStop, NULL);
    if (threadsCreated) {
      destroyThreads(false);
    }
    createThreads();
    threadsCreated = true;
  }

  bool FieldManager::wasStopped() {
    return stopped;
  }

  void FieldManager::status() {
    //out << "1" << std::endl;
    if (running) {
      out << "still running, call stop" << std::endl;
      return;
    }
    for (ll i = 0; i < field.size(); i++) {
        for(ll j = 0; j < field[i].size(); j++) {
            if (field[i][j]) {
              out << "* ";
            } else {
              out << ". ";
            }
        }
        out << std::endl;
    }
  }

  void FieldManager::createThreads() {
    threads = (Thread*) malloc(numberOfThreads*sizeof(Thread));
    for (int i = 0; i < numberOfThreads; i++) {
      new (threads + i) Thread(i, getThreadPart(i), getThreadBorders(i),
       0, //&needToStop, &stopMutex,
       *this, numberOfThreads1);
    }
    for (int i = 0; i < numberOfThreads; i++) {
      threads[i].getAdjacentThreads(
        threads[(numberOfThreads + i - 1)%numberOfThreads], threads[(numberOfThreads + i + 1)%numberOfThreads]);
    }
  }
  void FieldManager::destroyThreads(bool wait) {
    for (int i = 0; i < numberOfThreads; i++) {
        threads[i].cancel(wait);
    }
    for (int i = 0; i < numberOfThreads; i++) {
      (threads + i)->~Thread();
    }
    free(threads);
    //pthread_cond_destroy(&needToStop);
    //pthread_mutex_destroy(&stopMutex);
  }
  void FieldManager::run(ll numberOfIterations) {
    for (int i = 0; i < numberOfThreads; i++) {
      threads[i].updateIterations(numberOfIterations);
    }
    //pthread_mutex_lock(&stopMutex);
    stopped = false;
    running = true;
    for (int i = 0; i < numberOfThreads; i++) {
        threads[i].run();
    }
    //pthread_cond_broadcast(&needToStop);
    //pthread_mutex_unlock(&stopMutex);
  }

  fieldType FieldManager::getThreadPart(int threadNumber) {
    std::pair<ll,ll> pair = getChunkHeightAndFirstLine(threadNumber);
    ll rowHeight = pair.first;
    ll firstLineNumber = pair.second;
    fieldType threadPart;
    for (int i = 0; i < rowHeight; i++) {
        threadPart.push_back(field[firstLineNumber + i]);
    }
    return threadPart;
  }
  fieldType FieldManager::getThreadBorders(int threadNumber) {
    std::pair<ll,ll> pair = getChunkHeightAndFirstLine(threadNumber);
    ll rowHeight = pair.first;
    ll firstLineNumber = pair.second;
    fieldType borders;
    ll fieldHeight = field.size();
    borders.push_back(field[(firstLineNumber + fieldHeight - 1)% fieldHeight]);
    borders.push_back(field[(firstLineNumber + rowHeight + fieldHeight)% fieldHeight]);
    return borders;
  }

  std::pair<ll, ll> FieldManager::getChunkHeightAndFirstLine(int threadNumber) {
    ll rowHeight;
    ll firstLineNumber;
    ll extraLines = field.size() % numberOfThreads;
    ll fullChunkStdHeight = field.size() / numberOfThreads;
    if (threadNumber < extraLines) {
      rowHeight = fullChunkStdHeight + 1;
      firstLineNumber = rowHeight * threadNumber;
    } else {
      rowHeight = fullChunkStdHeight;
      firstLineNumber = (rowHeight + 1) * extraLines + (threadNumber - extraLines) * rowHeight;
    }
    return std::make_pair(rowHeight, firstLineNumber);
  }
  void FieldManager::stop() {
      stopped = true;
      running = false;
      gatherField();
      currentIteration = threads[0].getCurrentIteration();
      out << "iteration number: " << currentIteration << std::endl;
  }
  void FieldManager::gatherField() {
    std::pair<ll,ll> pair;
    ll rowHeight;
    ll firstLineNumber;
    fieldType threadPart;
    for(int i = 0; i < numberOfThreads;i++) {
      pair = getChunkHeightAndFirstLine(i);
      rowHeight = pair.first;
      firstLineNumber = pair.second;
      threadPart = threads[i].getComputedPart();
      for (int i = 0; i < rowHeight; i++) {
          field[firstLineNumber + i] = threadPart[i];
      }
    }
  }
  void FieldManager::quit(bool wait) {
      if (!threadsCreated) {
        return;
      }
      //pthread_mutex_lock(&stopMutex);
      stopped = false;
      running = true;
      //pthread_cond_broadcast(&needToStop);
      //pthread_mutex_unlock(&stopMutex);
      destroyThreads(wait);
  }
  void FieldManager::parseCSV(const std::string& fileName) {
    if (fileName.rfind(".csv") != fileName.length() - 4) {
      throw wrongFileFormatException("file is not .csv file");
    }
    std::fstream fin(fileName.c_str());
    if (!fin.good()) {
      fin.close();
      throw fileNotFoundException();
    }
    std::string buf;
    field = fieldType();
    char symbol;
    std::vector<bool> line;
    while(std::getline(fin, buf, '\n')) {
      for(ll i = 0; i < buf.size(); i++) {
        symbol = buf[i];
        if (symbol != ',') {
          if (symbol == '1') {
            line.push_back(true);
          } else if (symbol == '0'){
            line.push_back(false);
          } else if (!std::isspace(symbol)) {
            fin.close();
            throw wrongFileFormatException("incorrect CSV file");
          }
        }
      }
      field.push_back(line);
      line.clear();
    }
    fin.close();
  }

  void FieldManager::generateField(ll wigth, ll height) {
    field = fieldType(height, std::vector<bool>(wigth,0));
    srand(time(0));
    for(int i = 0; i < height; i++) {
      for(int j = 0; j < wigth; j++) {
        field[i][j] = rand()%2;
      }
    }
  }
