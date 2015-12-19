#include "../../../common_lib/headers/shell_command.h"
#include "../../headers/field_manager.h"
#include <cstdlib>
#include "../../../common_lib/headers/exceptions.h"

class StartCommand: public ShellCommand {
  std::ostream& out;
  FieldManager& manager;
public:
  StartCommand(std::ostream& out, FieldManager& manager):out(out),manager(manager) {
    ShellCommand::name = std::string("start");
  }
  virtual void run(std::string args) {
    std::vector<std::string> parsedArgs = parseArguments(args);
    int argsSize = parsedArgs.size();
    if (checkNumberOfArguments(argsSize, 2, 3, out)) {
      if (argsSize == 2) {
        try {
          manager.start(parsedArgs[0], atoi(parsedArgs[1].c_str()));
        } catch (fileNotFoundException e) {
          out << e.what() << std::endl;
        } catch (wrongFileFormatException e) {
          out << e.what() << std::endl;
        }
      } else {
        manager.start(atol(parsedArgs[0].c_str()), atol(parsedArgs[1].c_str()), atoi(parsedArgs[2].c_str()));
      }
    }
  }
};
