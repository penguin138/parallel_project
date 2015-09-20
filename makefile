all:
		clang++ -g ./src/shell.cpp ./src/shell_command.cpp ./src/start_command.cpp ./src/exit_command.cpp ./src/main.cpp -o life
