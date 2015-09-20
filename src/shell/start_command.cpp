#include "shell_command.h"
#include "field_manager.h"
#include <cstdlib>

class StartCommand: public ShellCommand {
  std::ostream& out;
  FieldManager manager;
public:
  StartCommand(std::ostream& out, FieldManager& manager):out(out),manager(manager) {
    ShellCommand::name = std::string("start");
  }
  virtual void run(std::string args) {
    std::vector<std::string> parsedArgs = parseArguments(args);
    int argsSize = parsedArgs.size();
    if (checkNumberOfArguments(argsSize, 2, 3, out)) {
      if (argsSize == 2) {
        manager.start(parsedArgs[0], atoi(parsedArgs[1].c_str()));
      } else {
        manager.start(atol(parsedArgs[0].c_str()), atol(parsedArgs[1].c_str()), atoi(parsedArgs[2].c_str()));
      }
    }
  }
};
