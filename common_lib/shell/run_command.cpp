#include "../headers/shell_command.h"
#include "field_manager.h"
#include <cstdlib>

class RunCommand: public ShellCommand {
  std::ostream& out;
  FieldManager& manager;
public:
  RunCommand(std::ostream& out, FieldManager& manager):out(out),manager(manager) {
    ShellCommand::name = "run";
  }
  virtual void run(std::string notParsedArgs) {
    std::vector<std::string> parsedArgs = parseArguments(notParsedArgs);
    if (checkNumberOfArguments(parsedArgs.size(), 1, out)) {
      manager.run(atoi(parsedArgs[0].c_str()));
    }
  }
};
