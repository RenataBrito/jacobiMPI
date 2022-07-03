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
	int lineCriteria = 1;
	int reducaoCriterio = 1;

	int orderOfMatrix = atoi(argv[1]);
   	int numberOfThreads = atoi(argv[2]);

	int name_len;
	char processor_name[MPI_MAX_PROCESSOR_NAME];

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

	
	int i,j, k;
	int *vetor_env, *vetor_rec, *vetor_recB;
	//*vet_rec_diag, vet_env_diag;
	vetor_env=(int*)malloc(orderOfMatrix*sizeof(int));
    vetor_rec=(int*)malloc(orderOfMatrix*sizeof(int));
	vetor_recB=(int*)malloc(orderOfMatrix*sizeof(int));
	//vet_rec_diag=(int*)malloc(proporcao*sizeof(int));
	//vet_env_diag=(int*)malloc(orderOfMatrix*sizeof(int));


	int matrix[4][4];

    MPI_Barrier(MPI_COMM_WORLD);
	if ( myrank == 0) {
		printf("\nMPI Processes: %d \nMatrix Order: %d \nNumber of Threads: %d \n\n", npes, orderOfMatrix, numberOfThreads);
        printf("There are %d processes.\n", npes);
		printf("I am process %d from processor %s.\n", myrank, processor_name);

		// Criacao da matriz de ordem N e do Vetor B
		matrix[0][0] =4; //Diagonal Princ
		matrix[0][1] =2;
		matrix[0][2] =1;
		matrix[0][3] =0;
		matrix[1][0] =1;
		matrix[1][1] =3; //Diagonal Princ
		matrix[1][2] =1;
		matrix[1][3] =0;
		matrix[2][0] =2;
		matrix[2][1] =3;
		matrix[2][2] =6; //Diagonal Princ
		matrix[2][3] =0;
		matrix[3][0] =5;
		matrix[3][1] =5;
		matrix[3][2] =5;
		matrix[3][3] =16; //Diagonal Princ
		
  		int bVector[4] = {7, -8, 6, 5};
		vetor_env = bVector;
	}

	MPI_Scatter(matrix[0], orderOfMatrix*proporcao, MPI_INT, vetor_rec, orderOfMatrix*proporcao, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Scatter(vetor_env, proporcao, MPI_INT, vetor_recB, proporcao, MPI_INT, 0, MPI_COMM_WORLD);
	//MPI_Scatter(vet_env_diag, proporcao, MPI_INT, vet_rec_diag, proporcao, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Barrier(MPI_COMM_WORLD);
	if( myrank < orderOfMatrix){
		printf("\nThere are %d processes. I am process %d from processor %s.\n", npes, myrank, processor_name);
	    printf("Received Values \n");
	    for(i=0;i<proporcao*orderOfMatrix;i++){
			printf("%d ", vetor_rec[i]);
	    }
		for(i=0;i<proporcao;i++){
		    printf("\n B Value: %d", vetor_recB[i]);
	    }
		/* for(i=0;i<proporcao;i++){
		    printf("\n Diag Value: %d", vet_rec_diag[i]);
	    }  */

		//**** CRITERIO DE CONVERGENCIA ****//
		MPI_Barrier(MPI_COMM_WORLD);
		printf("\nInicio criterio convergencia\n");
		int* lineSum = malloc(orderOfMatrix*sizeof(long int));
		lineCriteria = 1;

		int** matrixAux = malloc(proporcao*sizeof(*matrix));
   		for(i=0;i<orderOfMatrix;i++){
      		matrixAux[i]=malloc(orderOfMatrix*sizeof(*matrixAux[i]));
   		}
		k=0;
		//#pragma omp parallel for private(i,j,k) num_threads(numberOfThreads)
		printf("Matriz-Auxiliar\n");
		for(i=0;i<proporcao;i++){
			for(j=0;j<orderOfMatrix;j++){
				matrixAux[i][j] = vetor_rec[k];
				k++;
				printf("%d ", matrixAux[i][j]);
			}
			printf("\n");
		}

   		#pragma omp parallel for private(i) num_threads(numberOfThreads) 
   		for (i = 0; i < orderOfMatrix; i++)
   		{ 
      		lineSum[i] = 0;
   		}
		int w= myrank*proporcao;
		#pragma omp parallel for private(i,j) num_threads(numberOfThreads)
		for(i=0 ;i<proporcao; i++){
			if(lineCriteria){
				for(j=0; j<orderOfMatrix; j++){
					if(j != w+i){
						lineSum[i] = lineSum[i] + matrixAux[i][j];
					}
		
					if(lineSum[i] > matrixAux[i][w+i]){
						#pragma omp critical (colunmCriteriaWrite) // é mesmo necessario?
						{
							lineCriteria = 0;
						}
						printf("\nFalhou no critério, na linha: %d\n", w+i+1);
						break;
					}
				}
			}
		}
	}
    
	MPI_Reduce(&lineCriteria, &reducaoCriterio, 1, MPI_INT, MPI_MIN, 0, MPI_COMM_WORLD);
	MPI_Barrier(MPI_COMM_WORLD);
	if(myrank == 0){
		reducaoCriterio ? printf("Passou pelo criterio das linhas.\n") : printf("Nao passou pelo criterio das linhas.\n");
	}
	//INICIO DAS ITERACOES.
	// definicao e preenchimento do vetor de "ultimos resultados" por cada um dos processos
	// lastResults[i] = bVector[i]/elementoDiagPrincial[i]
	//COMEÇA ITERACOES
	//resultadoAtual[i] = 0;
	//para cada elemento da linha (j de 0 até N)
	//   se for diag principal  currentResults[i] += ((float)bVector[i]/(float)matrix[i][i]);
	//  senao currentResults[i] -=  ((float) matrix[i][j] * lastResults[j] / (float) matrix[i][i]);
	//  pega o maximo do currentResults para usar no criterio de parada.
	// define a maior diferenca em relacao ao results anterior.
	//escreve o novo no velho
	//cada processo faz a reducao de maximo para maiorDiff/maiorValor OMP
	//reducao MPI juntando todos os valores de parada.
	// se qualquer valor for maior que o limite continua.
	//senao para
	//precisa do barrier.
	//quando parar faz um gather dos resultados.



	//comparar resultados com uma das linhas (linha escolhida pelo usuario)
	
	
	
	
	
	//free(bufrecv);
	free(vetor_rec);
	//free(lineSum);
    //free(vetor_env);

	//fflush(0);

	ret = MPI_Finalize();
	if (ret == MPI_SUCCESS)
		printf("\nMPI_Finalize success!");
	return(0);
}
