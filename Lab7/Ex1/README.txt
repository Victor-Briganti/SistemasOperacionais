# Jantar dos Filósofos

Este programa tem como objetivo resolver o problema do jantar dos filósofos.
A solução utilizada para este problema foi a de que um dos filófosos pega os garfos em uma ordem diferente das dos demais, dessa forma a espera circular é quebrada impedindo que o deadlock ocorra.

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

  - <ctime>
    Usada para trabalhar com tempo. 
    No código, é utilizada para inicializar o gerador de números aleatórios com base no horário atual usando time(nullptr) na função srand. 
    Isso garante que a geração de números aleatórios seja diferente em cada execução.

  - <iostream>
    Usada para entrada e saída padrão. 
    No código, é utilizada para exibir mensagens no console, como as ações dos filósofos (ex.: "Filosófo(id) pensando" e "Filosófo(id) comendo").
  
  - <pthread.h>
    Usada para trabalhar com threads. 
    No código, ela é responsável por criar (pthread_create), controlar (pthread_join), e gerenciar threads para cada filósofo que executa a função philosopher.

  - <semaphore.h>
    Usada para trabalhar com semáforos. 
    No código, semáforos são usados para garantir que cada filósofo possa acessar os talheres de forma sincronizada e controlada, evitando condições de corrida.
  
  - <unistd.h>
    Fornece acesso a chamadas do sistema POSIX. No código, é usada para implementar pausas com sleep durante as ações dos filósofos (como pensar ou comer).

  - <vector>
    Usada para criar e manipular vetores dinâmicos. No código, é usada para armazenar:
      As threads de cada filósofo (std::vector<pthread_t> tid).
      O estado de cada filósofo (std::vector<int> states).
      Os semáforos associados a cada filósofo (std::vector<sem_t> semaphores).
