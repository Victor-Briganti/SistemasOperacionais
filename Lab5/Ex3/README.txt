# Memória Compartilhada

Programa onde um processo cria um livro e o outro processo pega este livro. A ideia deste programa e demonstrar o uso de memória compartilhada em uma situação produtor consumidor

## Compilação

Para compilar este programa o seguinte comando pode ser utilizado:

```sh
make
```

## Execução

Para executar este basta realizar o seguinte:

```sh
./main
```

## Bibliotecas

    - <fcntl.h>: 
      Contém definições para manipulação de arquivos e descritores de arquivos.
      O_CREAT, macro usada para criar um novo semáforo, se ele ainda não existir. Usado em sem_open

    - <iostream>, desta biblioteca foram utilizados as classes cout e cerr para imprimir informações na tela.
      Biblioteca padrão C++ para entrada e saída.
      cout e cerr, classes usadas para imprimir mensagens no console (saída padrão e saída de erros)
      cin, classe padrão para ler entradas de usuário.

    - <semaphore.h>, Fornece funções para trabalhar com semaphores POSIX.
      sem_open(), cria ou abre um semáforo.
      sem_wait(), Decrementa o semáforo, bloqueando até que seja seguro prosseguir.
      sem_post(), Incrementa o semáforo, liberando outros processos bloqueados.
      sem_close(), Fecha um semáforo aberto.
      sem_unlink(), Remove o semáforo do sistema.
  
    - <sys/mman.h>, Utilizada para realizar mapeamento de memória.
      mmap(), cria uma região de memória que pode ser acessada por diferentes processos.
      munmap(), Desmapeia a memória compartilhada ao final do programa.
      MAP_SHARED, macro que permite que os processos compartilhem alterações feitas na memória.
      MAP_ANONYMOUS, macro que permite um mapeamento anônimo, sem associação a arquivos.
      PROT_READ | PROT_WRITE, macros que permite leitura e escrita na região mapeada.

    - <sys/types.h>, Define tipos de dados básicos para interação com o sistema.
      pid_t, define o tipo do ID dos processos
 
    - <unistd.h>, Fornece funções de sistema para Unix/Linux.
      fork(), cria um novo processo filho.
      exit(), finaliza um processo.
 
    - <wait.h>, Fornece funções de espera para processos flhos.
      wait(), faz com que o processo pai esper o processo filho

    - <string>, 
      Biblioteca padrão de C++ para manipulação de strings.
      string, classe padrão string de C++
