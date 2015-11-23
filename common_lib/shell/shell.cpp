#include "../headers/shell.h"

Shell::Shell(const std::map<std::string, ShellCommand* >& newCommands, std::ostream& out, std::istream& in):
commands(newCommands),in(in),out(out){}

void Shell::start() {
  std::string commandWithArgs;
  out << PROMPT;
  while(std::getline(in, commandWithArgs, '\n')) {
    if (!commandWithArgs.empty() && !isSpaces(commandWithArgs)) {
      ShellCommand* cmd = commands[getCommandName(commandWithArgs)];
      if (cmd == NULL) {
        out << "command not found" << std::endl;
      } else {
        cmd->run(commandWithArgs);
        if (cmd->getName() == "quit") {
          break;
        }
      }
    }
    out << PROMPT;
  }
}

std::string Shell::getCommandName(const std::string& commandWithArgs) const {
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
bool Shell::isSpaces(const std::string& str) const {
  bool notSpace = false;
  for (int i = 0;i < str.length(); i++) {
    if (!isspace(str[i])) {
      notSpace = true;
      break;
    }
  }
  return !notSpace;
}
