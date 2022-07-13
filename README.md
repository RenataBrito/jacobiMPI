# jacobiMPI

Para a execução gerar os binarios
* make

Executar os binarios
* make run-seq
* make run-par

Vale ressaltar que a ordem da matriz, o número de processos e a quantidade de threads deverá ser passado como argumento da execução do paralelo. 
Como padrão estamos enviando a ordem da matriz 1000, 5 processos e 2 threads para o paralelo sendo executados nos hals de 02 a 06. 

Caso queira passar argumentos diferentes deverá editar o makefile ou passar os parametros por exemplo:
* ./execSeq 10000 1
* mpirun -np 1 -host hal02,hal03,hal04,hal10 --oversubscribe ./jacobipar 10000 5 4