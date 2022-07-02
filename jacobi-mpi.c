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


int  main(int argc, char *argv[])  {
	testArguments(argc);
	int npes, myrank, ret;

	char processor_name[MPI_MAX_PROCESSOR_NAME];
	int name_len;
	
	int i,j;
    int orderOfMatrix = atoi(argv[1]);
   	int numberOfThreads = atoi(argv[2]);
	int *vetor_env, *vetor_rec, *vetor_recB;
	vetor_env=(int*)malloc(orderOfMatrix*sizeof(int));
    vetor_rec=(int*)malloc(orderOfMatrix*sizeof(int));
	vetor_recB=(int*)malloc(orderOfMatrix*sizeof(int));

	int matrix[3][3];

	MPI_Status status;
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &npes);
	MPI_Comm_rank(MPI_COMM_WORLD, &myrank);

	MPI_Get_processor_name(processor_name, &name_len);
    // Logica de separacao dos dados caso a divisao seja exata ou nao. (orderOfMatrix/Nprocess)
	int proporcao = orderOfMatrix/npes;
	if(proporcao == 0){
		proporcao = 1; // TODO controlar DEPOIS que no máximo rank < order of matrix executem 
	}
	if( orderOfMatrix%npes != 0 ){
		printf("\nOrdem da matriz tem que ser multiple de NProcessos.\n");
		exit(EXIT_FAILURE);
	}

    MPI_Barrier(MPI_COMM_WORLD);
	if ( myrank == 0) {
		printf("\nMPI Processes: %d \nMatrix Order: %d \nNumber of Threads: %d \n\n", npes, orderOfMatrix, numberOfThreads);
        printf("There are %d processes.\n", npes);
		printf("I am process %d from processor %s.\n", myrank, processor_name);

		// Criacao da matriz de ordem N e do Vetor B
		matrix[0][0] =4; 
		matrix[0][1] =2;
		matrix[0][2] =1;
		matrix[1][0] =1;
		matrix[1][1] =3;
		matrix[1][2] =1;
		matrix[2][0] =2;
		matrix[2][1] =3;
		matrix[2][2] =6;
		
  		int bVector[3] = {7, -8, 6};
		vetor_env = bVector;
	}

	MPI_Scatter(matrix[0], orderOfMatrix*proporcao, MPI_INT, vetor_rec, orderOfMatrix, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Scatter(vetor_env, proporcao, MPI_INT, vetor_recB, proporcao, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Barrier(MPI_COMM_WORLD);
	if( myrank < orderOfMatrix){
		printf("\nThere are %d processes. I am process %d from processor %s.\n", npes, myrank, processor_name);
	    printf("Received Values \n");
	    for(i=0;i<orderOfMatrix;i++){
		    printf("%d ", vetor_rec[i]);
	    }
	    printf("\n B Value: %d", vetor_recB[0]);
	}

	//free(bufrecv);
	free(vetor_rec);
    //free(vetor_env);

	//fflush(0);

	ret = MPI_Finalize();
	if (ret == MPI_SUCCESS)
		printf("\nMPI_Finalize success!");
	return(0);
}
