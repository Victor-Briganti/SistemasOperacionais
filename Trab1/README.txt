# Problemas Clássicos de Concorrência e Sincronização
 
## Como Compilar

Para compilar basta usar o comando

```sh
make
```

## Como Executar

Após a compilação para executar o programa basta realizar o seguinte:

```sh
./main
```

Em caso de querer alterar a quantidade de Padawans ou Audiência que estão aptos 
a entrar na sessão, se faz necessário alterar o arquivo 'common.hpp' e 
recompilar a aplicação.

## Bibliotecas usadas

<chrono>
    - duration_cast(): Converte uma duração (como segundos, milissegundos, etc.) 
      para outra unidade de tempo.

    - milliseconds(): Função que cria uma duração em milissegundos. 
      Usada para representar o tempo em unidades de milissegundos.

    - steady_clock(): Retorna o tempo de um relógio monotônico.


<cstdio>
    - printf(): Imprime na saída padrão.

    - perror(): Exibe uma mensagem de erro associada a ao valor de 
      `errno`(código de erro global).

<cstdlib>
    - rand(): Retorna um valor pseudo-aleatório.

<list>
    - list: Estrutura de lista duplamente encadeada. Usada para implementar as 
      filas do programa.

<pthread.h>
    - pthread_t: Representa uma thread.
    
    - pthread_mutex_t: Representa um mutex. Permite que somente uma thread por 
      vez acesse uma região crítica.

    - pthread_create(): Cria uma nova thread. Essa criação é associada a thread
      e a uma função que ela irá executar.

    - pthread_mutex_init(): Inicializa o mutex.

    - pthread_mutex_lock(): Bloqueia um mutex. Quando uma thread chama essa
      função ela tenta adquirir o controle sobre o mutex, se outra thread já
      tiver o controle do mutex está que chamou irá ficar bloqueada.

    - pthread_mutex_unlock(): Desbloqueia um mutex. Quando a thread termina de
      executar, ela chama essa função para liberar o mutex, permitindo que 
      outras threads acessem a região.

    - pthread_join(): Aguarda o término de uma thread. A função bloqueia a 
      thread que executou a função até que a thread referenciada tenha
      terminado sua execução.


<semaphore.h>
    - sem_t: Representa um semáforo, que ajuda a controlar e sincronizar o 
      acesso a recursos pelas threads.

    - sem_init(): Inicializa um semáforo, definindo um valor inicial.

    - sem_post(): Aumenta o valor do semáforo. Liberando uma thread que estava
      esperando para acessar a seção crítica ou sinaliza que um recurso foi 
      liberado

    - sem_wait(): Diminui o valor do semáforo. Se o valor do semáforo for maior 
      que 0, ele é decrementado e a thread continua executando. Se o valor for 
      0, a thread é bloqueada até que o semáforo seja incrementado por outra 
      thread.

<unistd.h>
    - usleep(): Coloca a thread em um estado de bloqueio por x microsegundos.

<vector>
    - vectors: Estrutura similar a um array que tem tamanho variado.