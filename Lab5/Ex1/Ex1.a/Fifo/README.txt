# FIFO

Este programa tem como objetivo realizar a comunicação entre processos por meio de FIFO

## Bibliotecas

As bibliotecas utilizadas foram:

- cctype, desta bibliotca foi utilizado as funções isalpha() e isspace para verificar o tipo do caracter da mensagem.
- fcntl.h, está biblioteca possui funções para controle de um arquivo. Daqui foram usados macros para definição de permissão O_WRONLY (apenas escrita) e O_RDONLY (apenas leitura). Além das macros também foram utilizadas as funções open() para abrir um descritor e o close() para fecha-lo.
- iostream, biblioteca padrão de C++ usada para mostrar informações na tela. 
- string, biblioteca padrão de C++ para manipulação de strings.
- sys/stat.h, biblioteca usada para a criação do fifo com a função mkfifo().
- unistd.h, desta biblioteca foi utilizado a função read() e write(), usadas para ler e escrever no descritor (neste caso o FIFO).

## Compilação

Para compilar está biblioteca o seguinte comando pode ser utilizado:

```sh
make
```

## Execução

Para executar este basta realizar o seguinte:

```sh
./sender
./receiver
```