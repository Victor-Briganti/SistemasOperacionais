Busca em Vetor

Este programa tem como objetivo mostrar o uso da criação de processos para dividir a busca em um vetor.

## Bibliotecas

As bibliotecas usadas neste programa foram:

- algorithm, desta biblioteca foi utilizado o find() que permite buscar um valor dentro de um vetor.
- cstdio, desta biblioteca foi utilizado a função printf() que imprime caracteres na tela.
- cstdlib, desta biblioteca foi utilizado o srand() configura uma "seed" para a geração de valores aleatórios que serão gerados com a rand() que também foi utilizada.
- iostream, desta biblioteca foi utilizado o cout que permite imprimir caracteres na tela.
- vector, desta biblioteca foi utilizado a estrutura de dados vector.
- sys/wait.h, desta biblioteca foi utilizado o wait() que permite que o processo pai espere e recolha o valor de retorno do processo filho.
- unistd.h, desta biblioteca foi utilizado o seguinte:
  - getpid(), retorna o ID do processo atual
  - fork() usado para criar um filho do processo atual, por meio de um clone.
  - _exit() usado para finalizar um processo. Diferentemente do exit() este finaliza o processo sem execuções de funções de cleanup. Junto a esta função as macros EXIT_FAILURE e EXIT_SUCCESS também foram usadas, para definir se a saída foi bem sucedida ou não.
  - sysconf(), permite configurar e obter certos valores de configuração do sistema. 

## Compilação

Para compilar está biblioteca o seguinte comando pode ser utilizado:

make

## Execução

Para executar este basta realizar o seguinte:

./main <num_processes> <vector_size> <num_searched>

Onde num_process é a quantidade de processos que serão criados, vector_size é o tamanho do vetor e num_searched é o número sendo buscado no vetor.


