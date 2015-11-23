#include "shell.h"
#include "field_manager.h"
#include "start_command.cpp"
#include "exit_command.cpp"
#include "run_command.cpp"
#include "status_command.cpp"
#include "stop_command.cpp"
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
