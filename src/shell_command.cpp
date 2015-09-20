#include "./headers/shell_command.h"

  ShellCommand::ShellCommand():name(""){}

  void ShellCommand::checkNumberOfArguments(int real, int expected, std::ostream& out) {
    if (real < expected) {
      out << "too few arguments";
    } else if (real > expected){
        out << "too many arguments";
    }
  }
  void ShellCommand::run(std::string arguments) {
   std::cout << "base!";
  }

  std::vector<std::string> ShellCommand::parseArguments(std::string notParsedArguments) {
    std::vector<std::string> parsedArgs = std::vector<std::string>();
    std::string argument("");
    char symbol;
    for (int i = 0; i < notParsedArguments.length(); i++) {
      symbol = notParsedArguments[i];
      if ( ! std::isspace(symbol)) {
        argument+=symbol;
      } else {
        parsedArgs.push_back(argument);
        argument.clear();
      }
    }
    parseArgs.push_back(argument);
    parsedArgs.erase(parsedArgs.begin()); //because it's the command name
    return parsedArgs;
  }
