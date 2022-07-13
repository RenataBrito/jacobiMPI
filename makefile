all: seq par

seq: jacobiseq.c
        gcc -fopenmp jacobiseq.c -o execSeq

par: jacobi-mpi.c
        mpicc jacobi-mpi.c -o jacobipar -fopenmp

run-seq:
        ./execSeq 1000 1

run-par:
        mpirun -np 1 -host hal02,hal03,hal04,hal05,hal06 --oversubscribe ./jacobipar 1000 5 2
