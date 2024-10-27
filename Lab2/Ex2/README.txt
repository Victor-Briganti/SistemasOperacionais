Execução de Processos Filhos

Este programa tem como objetivo mostrar a utilização do comando execvp().

## Bibliotecas

As bibliotecas usadas neste programa foram:

- cerrno, define o valor inteiro da errno que tem seu valor definido caso tenha ocorrido algum erro durante uma system call.
- cstdio, desta biblioteca foi utilizado a função printf() que imprime caracteres na tela.
- cstring, desta biblioteca foi utilizado a função strerror() que traduz o valor do errno em uma string.
- sys/wait.h, desta biblioteca foi utilizado o wait() que permite que o processo pai espere e recolha o valor de retorno do processo filho.
- unistd.h, desta biblioteca foi utilizado as seguintes funções:
  - getpid(), retorna o ID do processo atual
  - fork() usado para criar um filho do processo atual, por meio de um clone.
  - execvp() usado para substituir a imagem do processo atual pela imagem de outro programa. Está versão do programa realiza a busca do programa por meio do PATH.

## Compilação

Para compilar está biblioteca o seguinte comando pode ser utilizado:

make

## Execução

Para executar este programa basta realizar o seguinte:

./main <programa> [args...]

Onde "programa" é o nome do programa a ser executado e "args" são seus argumentos se houver um.



