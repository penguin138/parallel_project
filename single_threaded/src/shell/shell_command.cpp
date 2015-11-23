#include "shell_command.h"

  ShellCommand::ShellCommand():name(""){}

  bool ShellCommand::checkNumberOfArguments(int real, int expected, std::ostream& out) {
    if (real < expected) {
      out << "too few arguments: expected "<< expected << std::endl;
      return false;
    } else if (real > expected){
        out << "too many arguments: expected " << expected << std::endl;
        return false;
    }
    return true;
  }
  bool ShellCommand::checkNumberOfArguments(int real, int expected1, int expected2, std::ostream &out) {
    if (real < expected1 && real < expected2) {
      out << "too few arguments: expected "<< expected1 << " or " << expected2 << std::endl;
      return false;
    } else if (real > expected1 && real > expected2) {
        out << "too many arguments: expected " << expected1 << " or " << expected2 << std::endl;
        return false;
    } else if ((real > expected1 && real < expected2) || (real > expected2 && real < expected1)) {
       out << "incorrect number of arguments: expected " << expected1 << " or " << expected2 << std::endl;
       return false;
    }
    return true;
  }
  void ShellCommand::run(std::string arguments) {}

  std::vector<std::string> ShellCommand::parseArguments(std::string notParsedArguments) {
    std::vector<std::string> parsedArgs = std::vector<std::string>();
    std::string argument("");
    char prevSymbol;
    char currSymbol=' ';
    for (int i = 0; i < notParsedArguments.length(); i++) {
      prevSymbol = currSymbol;
      currSymbol = notParsedArguments[i];
      if ( ! std::isspace(currSymbol)) {
        argument+=currSymbol;
      } else if (! std::isspace(prevSymbol)){
        parsedArgs.push_back(argument);
        argument.clear();
      }
    }
    if (!argument.empty()) {
      parsedArgs.push_back(argument);
    }
    parsedArgs.erase(parsedArgs.begin()); //because it's the command name
    return parsedArgs;
  }
