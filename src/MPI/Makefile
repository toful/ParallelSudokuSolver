######################################################################
#                            Makefile
######################################################################

# General defines
CC = mpicc

SRC = P2.2_CPM_ElAzizi_Dauden.c

BIN = parallel.obj

CFLAGS = -fopenmp

######################################################################
all :
		$(CC) -Wall -g -O3 $(SRC) -o $(BIN)

clean :
		rm -rf $(BIN)

run : clean all
		mpirun -c 4 ./$(BIN)

g_test: clean all
	mpiexec -n 4 xterm -hold -e gdb -ex run --args ./parallel.obj
