#include "../headers/field_manager.h"
#include <climits>
#include <stdlib.h>
#include <fstream>
#include "../common_lib/headers/exceptions.h"
#include <mpi.h>
#include "../headers/communication_defines.h"

  FieldManager::FieldManager(std::ostream& out):out(out){}


  void FieldManager::start(std::string fileName, int numberOfThreads) {
    this->numberOfThreads = numberOfThreads;
    stopped = true;
    parseCSV(fileName);
    sendInitialParts();
  }
  void FieldManager::start(ll width, ll height, int numberOfThreads) {
    if (height < numberOfThreads) {
      numberOfThreads = height;
      out << "too many threads, made number of threads equal height of field" << std::endl;
    }
    this->numberOfThreads = numberOfThreads;
    stopped = true;
    generateField(width, height);
    sendInitialParts();
  }

  void FieldManager::stop() {
    if (stopped) {
      out << "already stopped" << std::endl;
    }
    stopped = true;
    int stop;
    ll iters[numberOfThreads];
    MPI_Status status;
    for (int i = 1; i < numberOfThreads + 1;i++) {
      MPI_Send(&stop,0,MPI_INT,i,STOP_TAG,MPI_COMM_WORLD);
      MPI_Recv(iters + i - 1,1,MPI_LONG_LONG,i,SYNC_TAG,MPI_COMM_WORLD,&status);
    }
    ll maxIt = 0;
    for (int i=0;i< numberOfThreads;i++) {
      //std::cout << i << ": " << iters[i] << std::endl;
      if (iters[i] > maxIt) {
        maxIt = iters[i];
      }
    }
    ll one;

    for (int i = 1; i < numberOfThreads + 1; i++) {
      one = 1 + maxIt - iters[i-1];
      MPI_Send(&one,1,MPI_LONG_LONG,i,RUN_TAG,MPI_COMM_WORLD);
    }
    gatherField();
    //MPI_Status status;
    //MPI_Recv(&iter,1,MPI_LONG_LONG,1,5,MPI_COMM_WORLD,&status);
    out << "iteration number: " << maxIt << std::endl;
  }
  void FieldManager::run(ll iterations) {
    if (!stopped) {
      out << "already running, call stop first" << std::endl;
      return;
    }
    stopped = false;
    for(int i = 1; i < numberOfThreads + 1; i++) {
      MPI_Send(&iterations, 1, MPI_LONG_LONG, i, RUN_TAG, MPI_COMM_WORLD);
    }
    out << "manager: sent run" << std::endl;
  }

  void FieldManager::quit(bool b) {
    int quit;
    for (int i = 1; i < numberOfThreads + 1;i++) {
      MPI_Send(&quit, 0, MPI_INT, i, QUIT_TAG, MPI_COMM_WORLD);
    }
    MPI_Finalize();
  }

  void FieldManager::status() {
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

  void FieldManager::sendInitialParts() {
    fieldType threadBorders;
    fieldType threadPartWithBorders;
    bool* threadPartArray;
    ll width,height;
    out << "sending parts..." << std::endl;
    for(int i = 1; i < numberOfThreads + 1; i++) {
      threadPartWithBorders = getThreadPart(i - 1);
      threadBorders = getThreadBorders(i - 1);
      threadPartWithBorders.insert(threadPartWithBorders.begin(),threadBorders[0]);
      threadPartWithBorders.push_back(threadBorders[1]);
      threadPartArray = pack(threadPartWithBorders);
      height = threadPartWithBorders.size();
      width = 0;
      if (height > 0) {
        width = threadPartWithBorders[0].size();
      }
      int result1, result2, result3;
      out << "sending to thread " << i << std::endl;
      //TODO: normal init send without multiple MPI_Send's
      result1 = MPI_Send(&width, 1, MPI_LONG_LONG, i, INIT_TAG, MPI_COMM_WORLD);
      result2 = MPI_Send(&height, 1, MPI_LONG_LONG, i, INIT_TAG, MPI_COMM_WORLD);
      result3 = MPI_Send(threadPartArray, height * width, MPI::BOOL, i, INIT_TAG, MPI_COMM_WORLD);
      if (result1 != MPI_SUCCESS || result2 != MPI_SUCCESS || result3 != MPI_SUCCESS) {
        out << "initial send failed" << std::endl;
        MPI_Finalize();
        free(threadPartArray);
        return;
      }
      out << "sent" << std::endl;
    }
    free(threadPartArray);
    out << "sending done" << std::endl;
  }

  bool* FieldManager::pack(fieldType unpacked) {
    ll n = unpacked.size();
    ll m = 0;
    if (n > 0) {
      m = unpacked[0].size();
    }
    bool* packed = (bool*) malloc(sizeof(bool)*n*m);
    for (ll i = 0; i < n; i++) {
      for (ll j = 0; j < m; j++) {
        packed[i*m + j] = unpacked[i][j];
      }
    }
    return packed;//user frees pointer.
  }
  fieldType FieldManager::unpack(bool* packed,ll rowWidth, ll height) {
    fieldType unpacked;
    for (int i = 0; i < height;i++) {
      unpacked.push_back(std::vector<bool>());
      for (int j = 0; j < rowWidth;j++) {
        unpacked[i].push_back(packed[i*rowWidth + j]);
      }
    }
    return unpacked;
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

  void FieldManager::gatherField() {
    std::pair<ll,ll> pair;
    ll rowHeight;
    ll firstLineNumber;
    bool* threadPart;
    ll chunkWidth = field[0].size();
    for(int i = 1; i < numberOfThreads + 1;i++) {
      pair = getChunkHeightAndFirstLine(i-1);
      rowHeight = pair.first;
      firstLineNumber = pair.second;
      MPI_Status status;
      threadPart = (bool*) malloc(sizeof(bool) * rowHeight * chunkWidth);
      out << "manager: receiving computed part from " << i << std::endl;
      MPI_Recv(threadPart,rowHeight*chunkWidth,MPI::BOOL,i,FIELD_TAG,MPI_COMM_WORLD,&status);
      out << "manager: received from " << i << std::endl;
      for (int i = 0; i < rowHeight; i++) {
        for(int j = 0; j < chunkWidth;j++) {
          field[firstLineNumber + i][j] = threadPart[i*chunkWidth + j];
        }
      }
    //  out << "manager: field updated" << std::endl;
      free(threadPart);
      //out << "temporary part freed" << std::endl;
    }
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
