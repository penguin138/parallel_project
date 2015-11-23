INCLUDES = -I "./headers"

#test version
test: clean
		mpiCC ./src/mpi_test.cpp -o mpi_test

#interactive version
life: clean
		mpiCC ${INCLUDES} -O2 ./src/field_manager.cpp ./src/thread.cpp ./src/main.cpp  -o mpi_life

clean:
		-rm -f mpi_life mpi_test
