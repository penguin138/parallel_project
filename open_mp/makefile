COMMON_LIB = -I "../common_lib/"
INCLUDE1 = -I "./headers/"
INCLUDE2 = -I "./src/shell_commands"

life: clean
		g++ -pthread -fopenmp -lrt -O3 -o life ${COMMON_LIB} ${INCLUDE1} ${INCLUDE2} ./src/field_manager.cpp ./src/thread.cpp ./src/main.cpp $(wildcard ../common_lib/shell/*.cpp) $(wildcard ./src/shell_commands/*.cpp)

test: clean
		g++ -pthread -fopenmp -lrt -O3 -o test ${COMMON_LIB} ${INCLUDE1} ${INCLUDE2} ./src/field_manager.cpp ./src/thread.cpp ./src/benchmark.cpp $(wildcard ../common_lib/shell/*.cpp) $(wildcard ./src/shell_commands/*.cpp)

omp_test: clean
		g++ -fopenmp ./src/open_mp_test.cpp -o omp_test

clean:
		-rm -f life test omp_test
