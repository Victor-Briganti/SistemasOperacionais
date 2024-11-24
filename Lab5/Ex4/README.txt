# Memória Compartilhada e Pipes

Realizar a soma de dois vetores utilizando processos filhos onde:
  O pai enviará para cada filho a porção que ele precisa somar.
  O filho aguardará até o pai enviar a porção dele, e realizar as operações de soma após receber sua porção.
  A comunicação para enviar a porção será implementada utilizando pipe.
  O acesso aos vetores será implementado utilizando memória compartilhada

## Compilação

Para compilar este programa o seguinte comando pode ser utilizado:

```sh
make
```

## Execução

Para executar este basta realizar o seguinte:

```sh
./main <num_elements> <num_processes> 
```
num_elements -> Quantidade de elementos nos vetores V1 e V2
num_processes -> Quantidade de processos usados para realizar as operações

## Bibliotecas utilizadas

  - <stdio.h> Fornece funções para entrada e saída padrão, como.
    printf(), para imprimir mensagens no terminal.
    scanf(), (não usado neste código) para entrada de dados.
    perror(), imprimi o erro retornado em errno na chamada de sistema.

  - <stdlib.h>, Biblioteca padrão do C que oferece funcionalidades gerais.
    atoi(), Converte strings para inteiros, usado para ler os argumentos da linha de comando (argc e argv).
    exit(), Encerra o programa com um código de saída específico em casos de erro.
      
  - <unistd.h>, Fornece funções relacionadas a operações no sistema Unix.
    fork(), Cria um novo processo (usado para gerar processos filhos).
    pipe(), Cria um pipe para comunicação entre processos.
    read() e write(), Lê e escreve dados nos pipes.
    sysconf(), permite configurar e obter certos valores de configuração do sistema. 
        
  - <sys/ipc.h>, Fornece estruturas e definições para trabalhar com chaves IPC (Inter-Process Communication).
    key_t, Tipo de dado usado para gerar uma chave única para recursos IPC (como memória compartilhada, semáforos, etc.).
    
    Neste código, key_t key = 1 define uma chave fictícia para a memória compartilhada (não essencial no contexto do programa).

  - <sys/shm.h>, Fornece funções específicas para manipular memória compartilhada.
    shmget(), Cria ou acessa um segmento de memória compartilhada.
    shmat(), Anexa um segmento de memória compartilhada ao espaço de endereço do processo.
    shmdt(), Desanexa um segmento de memória compartilhada.
    shmctl(), Controla e remove o segmento de memória compartilhada (usado para liberar recursos no final).
    
  - <sys/wait.h>, Fornece funções para gerenciar a sincronização e o término de processos filhos.
    wait(NULL): Aguarda o término de um processo filho. No código, o laço while(wait(NULL) > 0) garante que o processo pai espere todos os filhos terminarem antes de continuar.
