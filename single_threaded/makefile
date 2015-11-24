INCLUDES1 = -I "./headers"
INCLUDES2 = -I "./src/shell_commands"
COMMON_LIB = -I "../common_lib"

life: clean
		g++ -O3 -lrt -o life ${INCLUDES1}  ${INCLUDES2} ${COMMON_LIB} ./src/field_manager.cpp ./src/main.cpp $(wildcard ../common_lib/shell/*.cpp) $(wildcard ./src/shell_commands/*.cpp)

test: clean
		g++ -O3 -lrt -o test ${INCLUDES1}  ${INCLUDES2} ${COMMON_LIB} ./src/field_manager.cpp ./src/benchmark.cpp $(wildcard ../common_lib/shell/*.cpp) $(wildcard ./src/shell_commands/*.cpp)

clean:
	-rm -f life test
