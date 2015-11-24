#include "../common_lib/headers/shell.h"
#include "field_manager.h"
#include "../headers/field_manager_commands.h"
#include <map>

using namespace std;

int main() {
  map<string, ShellCommand*> commands;
  FieldManager manager(std::cout);
  commands.insert(pair<string, ShellCommand* > (string("start"), new StartCommand(cout, manager)));
  commands.insert(pair<string, ShellCommand* > (string("quit"), new ExitCommand(cout, manager)));
  commands.insert(pair<string, ShellCommand* > (string("run"), new RunCommand(cout, manager)));
  commands.insert(pair<string, ShellCommand* > (string("status"), new StatusCommand(cout, manager)));
  commands.insert(pair<string, ShellCommand* > (string("stop"), new StopCommand(cout, manager)));
  Shell shell(commands, cout, cin);
  shell.start();
}
