#include <pthread.h>
#include <semaphore.h>
#include <stdbool.h>
#include <stdio.h>

// Define a quantidade de clientes
#define CLIENTS 6

// Define a quantidade de cadeiras
#define CHAIRS 5

// Armazena a quantidade de clientes esperando
int waitingCostumers = 0;

// Mutex para atualização do número de clientes
pthread_mutex_t waitingMutex = PTHREAD_MUTEX_INITIALIZER;

// Semáforo que controlar o barbeiro
sem_t barberSem;

// Semáforo que controlar os clientes
sem_t costumerSem;

void *costumer(void *arg) {
  while (true) {
    pthread_mutex_lock(&waitingMutex);
    if (waitingCostumers > CHAIRS) {
      printf("Indo embora\n");
      pthread_mutex_unlock(&waitingMutex);
      continue;
    }
    waitingCostumers++;

    printf("Esperando o barbeiro\n");
    pthread_mutex_unlock(&waitingMutex);

    sem_post(&costumerSem);
    sem_wait(&barberSem);
  }

  return NULL;
}

void *barber(void *arg) {
  while (true) {
    sem_wait(&costumerSem);

    pthread_mutex_lock(&waitingMutex);
    waitingCostumers--;
    printf("Cortando cabelo\n");
    pthread_mutex_unlock(&waitingMutex);

    sem_post(&barberSem);
  }

  return NULL;
}

int main() {
  sem_init(&barberSem, 0, 0);
  sem_init(&costumerSem, 0, 0);

  pthread_t barberThread;
  pthread_t costumerThread[CLIENTS];

  pthread_create(&barberThread, NULL, barber, NULL);

  for (int i = 0; i < CLIENTS; i++) {
    pthread_create(&costumerThread[i], NULL, costumer, NULL);
  }

  pthread_join(barberThread, NULL);
  for (int i = 0; i < CLIENTS; i++) {
    pthread_join(costumerThread[i], NULL);
  }

  sem_destroy(&barberSem);
  sem_destroy(&costumerSem);
}
