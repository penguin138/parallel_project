#include "../../../common_lib/headers/shell_command.h"
#include "../../headers/field_manager.h"

class StatusCommand: public ShellCommand {
  std::ostream& out;
  FieldManager& manager;
public:
  StatusCommand(std::ostream& out, FieldManager& manager):out(out),manager(manager) {
    ShellCommand::name = "status";
  }
  virtual void run(std::string notParsedArgs) {
    out << "in status!\n";
    std::vector<std::string> parsedArgs = parseArguments(notParsedArgs);
    if (checkNumberOfArguments(parsedArgs.size(), 0, out)) {
      manager.status();
    }
  }
};
