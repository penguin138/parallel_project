INCLUDES1 = -I "./headers"
INCLUDES2 = -I "./src/shell_commands"
COMMON_LIB = -I "../common_lib/"

#test version
test: clean
		mpiCC ${INCLUDES} ${COMMON_LIB} -O2 ./src/field_manager.cpp ./src/thread.cpp ./src/benchmark.cpp -o test
#interactive version
life: clean
		mpiCC ${INCLUDES} ${COMMON_LIB} -O2 ./src/field_manager.cpp ./src/thread.cpp ./src/main.cpp $(wildcard ../common_lib/shell/*.cpp) $(wildcard ./src/shell_commands/*.cpp) -o life
scatter_test: clean
		mpiCC ./src/mpi_coll_test.cpp -o scatter
clean:
		-rm -f life test scatter
