#include "./headers/shell.h"

Shell::Shell(const std::map<std::string, ShellCommand>& newCommands, std::ostream& out, std::istream& in):
commands(newCommands),in(in),out(out){}

void Shell::start() {
  std::string commandWithArgs;
  while(std::getline(in, commandWithArgs)) {
    ShellCommand cmd = commands[getCommandName(commandWithArgs)];
    if (cmd.getName() == std::string("")) {
      out << "command not found";
    } else {
      cmd.run(commandWithArgs);
      if (cmd.getName() == "quit") {
        break;
      }
    }
  }
}

std::string Shell::getCommandName(const std::string commandWithArgs) const {
  int indexOfNonWhitespace = 0;
  while (isspace(commandWithArgs[indexOfNonWhitespace])) {
    indexOfNonWhitespace++;
  };
  std::string commandName("");
  char symbol;
  for (int i = indexOfNonWhitespace; i < commandWithArgs.length(); i++) {
    symbol = commandWithArgs[i];
    if (! isspace(symbol)) {
      commandName+=symbol;
    } else {
      break;
    }
  }
  return commandName;
}
