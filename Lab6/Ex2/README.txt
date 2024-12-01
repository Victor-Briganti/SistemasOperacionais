# PING PONG

Este programa tem como objetivo realizar ping e pong, de forma intercalada.

Usando threads imprime ping e pong alternadamente na tela. 
A quantidade de threads ping e pong podem ser variaveis.

## Compilação

Para compilar o seguinte comando pode ser utilizado:

```sh
make
```
## Execução

Para executar este basta realizar o seguinte:

```sh
./main <num_ping> <num_pong>
```

<num_ping> -> Número de threads para ping
<num_pong> -> Número de threads para pong

## Bibliotecas

As bibliotecas utilizadas foram:

  - <pthread.h>: 
    Biblioteca utilizada para threads e mutex.
    Funções utilizadas:
      pthread_mutex_t; // Tipo de dado que representa um mutex usado para sincronizar o acesso a recursos compartilhados entre threads.
      pthread_cond_t; // Tipo de dado que representa uma variável de condição usada para sincronização entre threads, permitindo que uma thread espere por um sinal de outra.
      pthread_cond_destroy(); // Destrói uma variável de condição previamente inicializada, liberando recursos associados.
      pthread_cond_init(); // Inicializa uma variável de condição, configurando-a para ser usada em operações de sincronização.
      pthread_cond_signal(); // Sinaliza (ou desperta) uma thread que está esperando na variável de condição especificada.
      pthread_cond_wait(); // Libera o mutex associado e suspende a thread até que um sinal seja recebido na variável de condição, retomando quando o mutex estiver disponível novamente.
      pthread_create(); // Cria uma nova thread de execução, possibilitando execução concorrente de funções.
      pthread_join(); // Aguarda a conclusão de uma thread específica, bloqueando a thread chamadora até que a especificada termine.

  - <iostream>: 
    Biblioteca padrão do c++.
     std::cout // Saída de texto 
     std::endl // Utilizada para mandar um endline para saída
     std::atoi // Converte caracteres para int

  - <vector>: 
    Biblioteca padrão para utilização de vetores em c++.
     std::vector // Classe para manipular vetor