Shell Simples

Um shell simples que permite a execução de processos. Ele pode executar comandos em segundo plano ou em primeiro plano, aguardando a conclusão do processo.

## Bibliotecas

As bibliotecas usadas neste programa foram:

- cctype: Utiliza isprint() para verificar se os caracteres são imprimíveis.
- cstring: Utiliza strdup() para duplicar strings.
- iostream: Usa cout para impressão e cin para entrada.
- string: Utiliza a estrutura string.
- sys/wait.h: Usa wait() para aguardar a conclusão do processo filho.
- unistd.h:
 - fork(): Cria um novo processo.
 - _exit(): Finaliza um processo imediatamente.
 - execvp(): Executa um comando substituindo a imagem do processo atual.
- vector: Utiliza a estrutura vector para gerenciar listas de argumentos.

## Compilação

Para compilar está biblioteca o seguinte comando pode ser utilizado:

make

## Execução

Para executar este basta realizar o seguinte:

./main

Realiza os comandos até que o usuário termine a execução com <C-c> ou <C-d>.

