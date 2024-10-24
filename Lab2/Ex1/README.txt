Hierarquia de Processos

Este programa tem como objetivo mostrar a criação da hierarquia de processos de um programa.

## Bibliotecas

As bibliotecas usadas neste programa foram:

- cstdio, desta biblioteca foi utilizado a função printf() que imprime caracteres na tela.
- cstdlib, desta biblioteca foi utilizado o atoi() responsável por transformar um caracter em um número(se possível).
- math.h, usada para realizar o cálculo da quantidade de processos que podem ser criados no sistema.
- string, biblioteca de string padrão da linguagem C++
- sys/wait.h, desta biblioteca foi utilizado o wait() que permite que o processo pai espere e recolha o valor de retorno do processo filho.
- unistd.h, desta biblioteca foi utilizado o seguinte:
  - fork() usado para criar um filho do processo atual, por meio de um clone.
  - _exit() usado para finalizar um processo. Diferentemente do exit() este finaliza o processo sem execuções de funções de cleanup. Junto a esta função as macros EXIT_FAILURE e EXIT_SUCCESS também foram usadas, para definir se a saída foi bem sucedida ou não.
  - sysconf(), permite configurar e obter certos valores de configuração do sistema. 

## Compilação

Para compilar está biblioteca o seguinte comando pode ser utilizado:

make

## Execução

Para executar este basta realizar o seguinte:

./main <num_process>

Onde num_process é o nível da árvore de processos que se deseja criar. 



