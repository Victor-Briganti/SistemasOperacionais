# FIFO

Este programa tem como objetivo realizar a comunicação entre processos por meio de FIFO

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

## Bibliotecas

As bibliotecas utilizadas foram:

  - <cctype>: 
    Biblioteca usada para classificar caracteres.
    isalpha() verifica se é um caracter alfabetico ou númerico
    isspace() verifica se é um espaço

  - <fcntl.h>: 
    Contém definições para manipulação de arquivos e descritores de arquivos.
    O_RDONLY, usado para abrir os arquivos como somente leitura
    O_WRONLY, usado para abrir os arquivos como somente escrita
    open(), abre os descritores de arquivos
    close(), fecha os descritores de arquivos

  - <iostream>: biblioteca padrão de C++ usada para mostrar informações na tela. 
    Biblioteca padrão C++ para entrada e saída.
    cout e cerr, classes usadas para imprimir mensagens no console (saída padrão e saída de erros)
    cin, classe padrão para ler entradas de usuário.

  - <string>, 
    Biblioteca padrão de C++ para manipulação de strings.
    string, classe padrão string de C++

  - <sys/stat.h>: 
    Biblioteca usada para os diferentes tipos de arquivos no sistema.
    mkfifo(), cria o FIFO

  - <unistd.h>:
    Biblioteca de chamada de sistema para sistemas do tipo UNIX: 
    read(), leitura de arquivos(inclui o FIFO)
    write(), escrita em arquivos(inclui o FIFO)