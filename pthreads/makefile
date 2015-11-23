COMMON_LIB = -I "../common_lib/"
INCLUDE1 = -I "./headers/"

life: clean
		g++ -pthread -lrt -O3 -o life ${COMMON_LIB} ${INCLUDE1} ./src/field_manager.cpp ./src/thread.cpp ./src/main.cpp $(wildcard ../common_lib/shell/*.cpp)

test: clean
		g++ -pthread -lrt -O3 -o test ${COMMON_LIB} ${INCLUDE1} ./src/field_manager.cpp ./src/thread.cpp ./src/benchmark.cpp $(wildcard ../common_lib/shell/*.cpp)

clean:
		-rm -f life test
