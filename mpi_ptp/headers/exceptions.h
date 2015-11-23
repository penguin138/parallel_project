#include <exception>
#include <stdexcept>

class fileNotFoundException: public std::exception {
public:
  virtual const char* what() const throw() {
    return "file not found";
  }
};

class wrongFileFormatException: public std::runtime_error {
public:
  wrongFileFormatException(const char* what):std::runtime_error(what){}
};
