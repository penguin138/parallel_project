INCLUDES1 = -I "./headers"
INCLUDES2 = -I "./src/shell"

life: clean
		g++ -O3 -lrt -o life ${INCLUDES1}  ${INCLUDES2} ./src/field_manager.cpp ./src/main.cpp $(wildcard ./src/shell/*.cpp)
		
test: clean
		g++ -O3 -lrt -o test ${INCLUDES1}  ${INCLUDES2} ./src/field_manager.cpp ./src/benchmark.cpp $(wildcard ./src/shell/*.cpp)

clean:
	-rm -f life test
