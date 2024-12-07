# Leitores/Escritores

Este programa tem como objetivo, resolver o problema Leitores/Escritores.

## Compilação

Para compilar o seguinte comando pode ser utilizado:

```sh
make
```
## Execução

Para executar este basta realizar o seguinte:

```sh
./main
```

## Bibliotecas

As bibliotecas utilizadas foram:

  - <iostream>
    Utilizada para entrada e saída padrão no terminal.
      Exibe mensagens que indicam as ações de leitura e escrita, como: "Lendo" e "Escrevendo".
      Exibe mensagens de erro, como: "Não foi possível criar escritor".

  - <pthread.h>
    Fornece suporte para criação e gerenciamento de threads e sincronização.
      pthread_mutex_t: Cria o mutex utilizado para proteger regiões críticas e sincronizar o acesso às variáveis compartilhadas readers e writers.
      pthread_cond_t: Cria uma variável condicional usada para fazer threads aguardarem ou sinalizarem eventos.
      pthread_create: Cria as threads leitoras e escritoras.
      pthread_join: Aguarda a conclusão das threads.
      pthread_mutex_lock e pthread_mutex_unlock: Bloqueiam e desbloqueiam o mutex para garantir exclusão mútua.
      pthread_cond_wait: Suspende uma thread até que uma condição seja sinalizada.
      pthread_cond_signal e pthread_cond_broadcast: Notificam threads aguardando por uma condição.
      pthread_mutex_init: Inicializa o mutex antes do uso.
      pthread_cond_init: Inicializa a variável condicional.

  - <unistd.h>
    Fornece chamadas de sistema POSIX, incluindo funções para pausar a execução.
      sleep: Simula o tempo gasto nas operações de leitura e escrita.

  - <vector>
    Fornece vetores dinâmicos da biblioteca padrão do C++.
    Armazena os identificadores de threads dos leitores (readersTid) e escritores (writersTid), permitindo gerenciamento dinâmico.






