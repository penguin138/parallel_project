#include "./headers/shell.h"
#include "start_command.cpp"
#include "exit_command.cpp"
#include <map>

using namespace std;

int main() {
  map<string, ShellCommand> commands;
  commands.insert(pair<string, ShellCommand> (string("start"), StartCommand(cout)));
  commands.insert(pair<string, ShellCommand> (string("quit"), ExitCommand(cout)));
  Shell shell(commands, cout, cin);
  shell.start();
}
