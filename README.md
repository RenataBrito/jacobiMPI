# jacobiMPI

Para a execução gerar os binarios
* make

Executar os binarios
* make run-seq
* make run-par

Vale ressaltar que a ordem da matriz, o numero de processos e a quantidade de threads devera ser passado como argumento da execucao. 
Como padrao estamos enviando a ordem da matriz 100, 5 processos e 2 threads para o paralelo. 
Caso queira diferente deverá editar o makefile ou passar os parametros por exemplo:
* ./execSeq 1000 1
* ./execPar 1000 4 1