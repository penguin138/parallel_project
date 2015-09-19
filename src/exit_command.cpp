#include "./headers/shell_command.h"

class ExitCommand: public ShellCommand {
  std::ostream& out;
public:
  ExitCommand(std::ostream& out): out(out) {
    ShellCommand::name = std::string("quit");
  }
  virtual void run (std::string args) {
    std::vector<std::string> parsedArgs = parseArguments(args);
    checkNumberOfArguments(parsedArgs.size(), 0, out);
  }
};
