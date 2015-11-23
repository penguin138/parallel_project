INCLUDES1 = -I "./headers"
INCLUDES2 = -I "./src/shell"
COMMON_LIB = -I "../common_lib"

life: clean
		g++ -O3 -lrt -o life ${INCLUDES1}  ${INCLUDES2} ${COMMON_LIB} ./src/field_manager.cpp ./src/main.cpp $(wildcard ../common_lib/shell/*.cpp)

test: clean
		g++ -O3 -lrt -o test ${INCLUDES1}  ${INCLUDES2} ${COMMON_LIB} s./src/field_manager.cpp ./src/benchmark.cpp $(wildcard ../common_lib/shell/*.cpp)

clean:
	-rm -f life test
