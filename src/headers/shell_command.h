#pragma once
#include <vector>
#include <string>
#include <iostream>

class ShellCommand { //all shell commands will be inherited from this class.
protected:
    std::string name;
public:
  ShellCommand();

//starts a command with provided args, which include command name.
  virtual void run(std::string arguments);

//returns the name of a command
  std::string getName() {
    return name;
  }

  //checks whether the number of arguments is correct, used in all commands.
  void checkNumberOfArguments(int real, int expected, std::ostream& out);



  //parses command arguments, used in all commands.
  std::vector<std::string> parseArguments(std::string notParsedArguments);

};
