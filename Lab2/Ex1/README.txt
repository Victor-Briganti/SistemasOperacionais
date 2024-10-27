Hierarquia de Processos

Este programa tem como objetivo mostrar a criação da hierarquia de processos de um programa.

## Bibliotecas

As bibliotecas usadas neste programa foram:

- cstdlib, desta biblioteca foi utilizado o atoi() responsável por transformar um caracter em um número(se possível).
- math.h, usada para realizar o cálculo da quantidade de processos que podem ser criados no sistema.
- iostream, biblioteca padrão da linguagem C++ para entrada e saída
- string, biblioteca de string padrão da linguagem C++
- sys/wait.h, desta biblioteca foi utilizado o wait() que permite que o processo pai espere e recolha o valor de retorno do processo filho.
- thread, desta biblioteca foi utilizado sleep_for, que permite colocar para dormir o processo.
- unistd.h, desta biblioteca foi utilizado o seguinte:
  - fork(), usado para criar um filho do processo atual, por meio de um clone.
  - _exit(), usado para finalizar um processo.
  - sysconf(), permite configurar e obter certos valores de configuração do sistema. 
  - sleep(), faz com que o processo atual durma por x segundos. 

## Compilação

Para compilar está biblioteca o seguinte comando pode ser utilizado:

make

## Execução

Para executar este basta realizar o seguinte:

./main <num_process>

Onde num_process é o nível da árvore de processos que se deseja criar. 



