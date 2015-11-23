INCLUDES1 = -I "./headers"
INCLUDES2 = -I "./src/shell"

life:
		g++ -pthread -lrt -O3 -o life ${INCLUDES1}  ${INCLUDES2} ./src/field_manager.cpp ./src/thread.cpp ./src/main.cpp $(wildcard ./src/shell/*.cpp)

test:
		g++ -pthread -lrt -O3 -o test ${INCLUDES1}  ${INCLUDES2} ./src/field_manager.cpp ./src/thread.cpp ./src/benchmark.cpp $(wildcard ./src/shell/*.cpp)

clean:
		-rm -f life test
