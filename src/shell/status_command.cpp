#include "shell_command.h"
#include "field_manager.h"

class StatusCommand: public ShellCommand {
  std::ostream& out;
  FieldManager manager;
public:
  StatusCommand(std::ostream& out, FieldManager& manager):out(out),manager(manager) {
    ShellCommand::name = "status";
  }
  virtual void run(std::string notParsedArgs) {
    std::vector<std::string> parsedArgs = parseArguments(notParsedArgs);
    if (checkNumberOfArguments(parsedArgs.size(), 0, out)) {
      manager.status();
    }
  }
};
