#include "./headers/shell_command.h"

class StartCommand: public ShellCommand {
  std::ostream& out;
public:
  StartCommand(std::ostream& out):out(out) {
    ShellCommand::name = std::string("start");
  }
  virtual void run(std::string args) {
    out << args;
    std::vector<std::string> parsedArgs = parseArguments(args);
    checkNumberOfArguments(parsedArgs.size(), 0, out);
  }
};
