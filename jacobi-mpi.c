// para compilar: mpicc jacobi-mpi.c -o jacobi-mpi-exec -Wall
// para rodar (use uma delas): 
// mpirun -np 4 ./jacobi-mpi-exec   (supondo 04 cores) precisa do ./ antes do nome do executavel.
// mpirun --use-hwthread-cpus -np 8 jacobi-mpi-exec (supondo 4 cores e 2 threads por core)
// mpirun --oversubscribe -np 10 jacobi-mpi-exec (sobreposicao)
// mpirun -np 8 --hostfile hosts jacobi-mpi-exec (segue o hostfile e deve obedecer limitacoes)
// mpirun -np 8 --host hostname:slots jacobi-mpi-exec (segue -host e segue limitacoes. slots � opcional (default 1))
// mpirun --map-by node --hostfile ./halley.txt -np 4 . jacobi-mpi-exec : -np 4 . jacobi-mpi-exec  (espalha processos em n�s)
// mpirun --map-by core --hostfile ./halley.txt -np 4 . jacobi-mpi-exec : -np 4 . jacobi-mpi-exec (espalha processos em cores)
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mpi.h"
#include "utils.h"
#define MESSAGE "Hello World running!!!! " 


int  main(int argc, char *argv[])  {
	testArguments(argc);
	int npes, myrank, src, dest, msgtag, ret;
	char *bufrecv, *bufsend;
	char processor_name[MPI_MAX_PROCESSOR_NAME];
	int name_len;

	int i,j;
    int orderOfMatrix = atoi(argv[1]);
   	int numberOfThreads = atoi(argv[2]);

	MPI_Status status;
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &npes);
	MPI_Comm_rank(MPI_COMM_WORLD, &myrank);

	MPI_Get_processor_name(processor_name, &name_len);

	bufrecv = (char *) malloc (strlen(MESSAGE)+12);
   	msgtag = 1;
	if ( myrank == 0) {
		printf("MPI Processes: %d \nMatrix Order: %d \nNumber of Threads: %d \n\n", npes, orderOfMatrix, numberOfThreads);
        printf("There are %d processes.\n", npes);
		printf("I am process %d from processor %s. Message with %d char. \n", myrank, processor_name, (int)strlen(MESSAGE));
		
		for (dest = 1; dest<npes; dest++)
		  MPI_Send((void*)MESSAGE, strlen(MESSAGE)+1, MPI_CHAR, dest, msgtag, MPI_COMM_WORLD);
		
		msgtag = 2;
		printf("Receiving msgs from slaves: ");
		for (src = 1; src < npes; src++) {
			MPI_Recv(bufrecv, strlen(MESSAGE)+12, MPI_CHAR, MPI_ANY_SOURCE, msgtag, MPI_COMM_WORLD, &status);
			printf("(%d)", (int) strlen(bufrecv));
		}
		printf("\n");

		// Criacao da matriz de ordem N e do Vetor B
		int matrix[3][3] = {
       		{4, 2, 1},
       		{1, 3, 1},
       		{2, 3, 6}};
  		int bVector[3] = {7, -8, 6};

		//Matriz print
		for(i=0;i<orderOfMatrix;i++){
			for(j=0;j<orderOfMatrix;j++)
				printf("%d ", matrix[i][j]);
			printf("\n");
		}
		//bVector print
		for(i=0; i<orderOfMatrix; i++)
			printf("%d ", bVector[i]);

	} else {
		printf("\nThere are %d processes. I am process %d from processor %s.\n", npes, myrank, processor_name);
		
		bufsend = (char *) malloc (strlen(MESSAGE)+12);
		src = dest = 0;
		MPI_Recv(bufrecv, strlen(MESSAGE)+1, MPI_CHAR, src, msgtag, MPI_COMM_WORLD, &status);
		msgtag = 2;
		sprintf(bufsend, "%s from %d", bufrecv, myrank);
		MPI_Send(bufsend, strlen(bufsend)+1, MPI_CHAR, dest, msgtag, MPI_COMM_WORLD);
		free(bufsend);
	}
	free(bufrecv);
	fflush(0);
	ret = MPI_Finalize();
	if (ret == MPI_SUCCESS)
		printf("\nMPI_Finalize success! \n");
	return(0);
}
