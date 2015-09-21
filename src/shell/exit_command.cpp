#include "shell_command.h"
#include "field_manager.h"

class ExitCommand: public ShellCommand {
  std::ostream& out;
  FieldManager& manager;
public:
  ExitCommand(std::ostream& out, FieldManager& manager): out(out), manager(manager) {
    ShellCommand::name = std::string("quit");
  }
  virtual void run (std::string args) {
    std::vector<std::string> parsedArgs = parseArguments(args);
    //we don't check number of args here, because we assume that
    //shell will terminate anyway, if quit is written.
    manager.quit();
  }
};
