all: seq par

seq: jacobiseq.c
	gcc -fopenmp jacobiseq.c -o execSeq

par: jacobipar.c
	gcc -fopenmp jacobipar.c -o execPar

run-seq:
	./execSeq 1000 1

run-par:
	./execPar 1000 2


