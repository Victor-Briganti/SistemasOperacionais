Hierarquia de Processos

Este programa tem como objetivo mostrar a criação da hierarquia de processos de um programa.


## Bibliotecas

As bibliotecas usadas neste programa foram:

- cstring, desta biblioteca foi utilizado o strdup() que duplica uma string.
- iostream, desta biblioteca foi utilizado o cout para imprimir informações. cin foi utilizado para verificar se a entrada era um eof. getline() foi utilizado para obter o que o usuário escreveu.
- cctype, a função isprint foi utilizado para verificar se os caracteres passados são válidos.
- string, desta biblioteca foi utilizado a estrutura string.
- vector, desta biblioteca foi utilizado a estrutura vector
- sys/wait.h, desta biblioteca foi utilizado o wait() que permite que o processo pai espere e recolha o valor de retorno do processo filho.
- unistd.h, desta biblioteca foi utilizado o seguinte:
  - fork() usado para criar um filho do processo atual, por meio de um clone.
  - _exit() usado para finalizar um processo. Diferentemente do exit() este finaliza o processo sem execuções de funções de cleanup. Junto a esta função as macros EXIT_FAILURE e EXIT_SUCCESS também foram usadas, para definir se a saída foi bem sucedida ou não.
  - execvp() usado para substituir a imagem do processo atual pela imagem de outro programa. Está versão do programa realiza a busca do programa por meio do PATH.

## Compilação

Para compilar está biblioteca o seguinte comando pode ser utilizado:

make

## Execução

Para executar este basta realizar o seguinte:

./main <num_process>

Onde num_process é o nível da árvore de processos que se deseja criar. 



