#include "../../../common_lib/headers/shell_command.h"
#include "../../headers/field_manager.h"

class StopCommand: public ShellCommand {
  std::ostream& out;
  FieldManager& manager;
public:
  StopCommand(std::ostream& out, FieldManager& manager):out(out),manager(manager) {
    ShellCommand::name = "stop";
  }
  virtual void run(std::string notParsedArgs) {
    std::vector<std::string> parsedArgs = parseArguments(notParsedArgs);
    if (checkNumberOfArguments(parsedArgs.size(), 0, out)) {
      manager.stop();
    }
  }
};
