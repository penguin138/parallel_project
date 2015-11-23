INCLUDES = -I "./headers"
COMMON_LIB = -I "../common_lib/"

#test version
test: clean
		mpiCC ./src/mpi_test.cpp -o test

#interactive version
life: clean
		mpiCC ${INCLUDES} ${COMMON_LIB} -O2 ./src/field_manager.cpp ./src/thread.cpp ./src/main.cpp  -o life

clean:
		-rm -f life test
