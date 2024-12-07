/*
 * Jantar dos Filosófos
 * Descrição:
 * Este programa tem como objetivo resolver o problema do jantar dos filosófos.
 *
 * Autores: Hendrick Felipe Scheifer, Victor Briganti, Luiz Takeda
 * Licença: BSD 2
 */
#include <ctime>     // time()
#include <iostream>  // out
#include <pthread.h> // pthread_t, pthread_create(), pthread_join(), pthread_mutex_destroy(), pthread_mutex_init(), pthread_mutex_lock()
#include <semaphore.h> // sem_t, sem_destroy(), sem_init(), sem_post(), sem_wait()
#include <unistd.h>    // sleep()
#include <vector>      // vector

/** Número de filosófos */
#define N 1000

/** Retorna o id do filosófo a esquerda */
#define LEFT(id) (((id) + N - 1) % N)

/** Retorna o id do filosófo a direita */
#define RIGHT(id) (((id) + 1) % N)

/** Estados que os filosófos podem estar */
#define THINKING 0
#define HUNGRY 1
#define EATING 2

/** Define o vetor das threads que irão executar os filosófos */
std::vector<pthread_t> tid(N);

/** Define o estado de cada filosófo */
std::vector<int> states(N, THINKING);

/** Semaforo para controlar acesso aos talheres */
std::vector<sem_t> semaphores(N);

/** Mutex para exclusão de regiões críticas */
pthread_mutex_t mutex;

/**
 * @brief Testa se o filosófo pode começar a comer
 *
 * O filosófo só pode estar no estado comendo, se ele estiver com fome e os
 * filófosos ao seu lado não estiverem comendo.
 *
 * @param id Filosófo que irá pegar os talheres
 */
void test(int id) {
  if (states[id] == HUNGRY && states[LEFT(id)] != EATING &&
      states[RIGHT(id)] != EATING) {
    states[id] = EATING;
    sem_post(&semaphores[id]);
  }
}

/**
 * @brief Filosófo pega os talheres
 *
 * Altera o estado do filósofo para com fome e realiza um wait nos talheres
 *
 * @param id Filosófo que irá pegar os talheres
 */
void take_forks(int id) {
  pthread_mutex_lock(&mutex);
  states[id] = HUNGRY;
  test(id);
  pthread_mutex_unlock(&mutex);
  sem_wait(&semaphores[id]);
}

/**
 * @brief Função que libera os talheres
 *
 * Coloca o filósofo no estado de pensando e verifica se os filósofos a esquerda
 * ou a direita querem comer.
 *
 * @param id Filosófo que irá liberar os talheres
 */
void put_forks(int id) {
  pthread_mutex_lock(&mutex);
  states[id] = THINKING;
  test(LEFT(id));
  test(RIGHT(id));
  pthread_mutex_unlock(&mutex);
}

/**
 * @brief Coloca o filosófo para dormir por x microsegundos
 *
 * @param id Filosófo que está pensando
 */
void think(int id) {
  std::cout << "Filosófo(" << id << ") pensando" << std::endl;
  sleep(1);
}

/**
 * @brief Coloca o filosófo para comer
 *
 * @param id Filosófo que está comendo
 */
void eat(int id) {
  std::cout << "Filosófo(" << id << ") comendo" << std::endl;
  sleep(1);
}

/**
 * @brief Coloca o filosófo no jantar
 *
 * @param arg defini o id to filosófo
 *
 * @return Sempre retorna um nullptr
 */
void *philosopher(void *arg) {
  int *id = static_cast<int *>(arg);

  while (true) {
    think(*id);
    take_forks(*id);
    eat(*id);
    put_forks(*id);
  }

  delete id;
  return nullptr;
}

int main() {
  srand(time(nullptr));

  // Inicializa o mutex
  pthread_mutex_init(&mutex, nullptr);

  // Inicializa os semáforos de cada filósofo
  for (int i = 0; i < N; i++) {
    sem_init(&semaphores[i], 0, 0);
  }

  // Cria cada filósofo
  for (int i = 0; i < N; i++) {
    int *arg = new int(i);
    pthread_create(&tid[i], nullptr, philosopher, arg);
  }

  // Termina os filósofos
  for (int i = 0; i < N; i++) {
    pthread_join(tid[i], nullptr);
  }

  // Destrói as primitivas de sincronização
  pthread_mutex_destroy(&mutex);
  for (int i = 0; i < N; i++) {
    sem_destroy(&semaphores[i]);
  }

  return 0;
}
