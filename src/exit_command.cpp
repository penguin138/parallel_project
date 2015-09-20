#include "./headers/shell_command.h"

class ExitCommand: public ShellCommand {
  std::ostream& out;
public:
  ExitCommand(std::ostream& out): out(out) {
    ShellCommand::name = std::string("quit");
  }
  virtual void run (std::string args) {
    std::vector<std::string> parsedArgs = parseArguments(args);
    //we don't check number of args here, because we assume that
    //shell will terminate anyway, if quit is written.
  }
};
