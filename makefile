all: seq par

seq: jacobiseq.c
	gcc -fopenmp jacobiseq.c -o execSeq

par: jacobi-mpi.c
	mpicc jacobi-mpi.c -o execPar -fopenmp

run-seq:
	./execSeq 100 1

run-par:
	mpirun –np 5 execPar




