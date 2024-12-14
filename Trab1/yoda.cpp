#include "yoda.hpp"
#include <cstdio>
#include <cstdlib>
#include <unistd.h>

#define NUM_PADAWANS 10

/**
 * @brief Inicializa os testes do Yoda.
 *
 * @param arg Ponteiro void para o Yoda
 *
 * @return nullptr
 */
static void *inicia_testes(void *arg) {
  Yoda *yoda = static_cast<Yoda *>(arg);
  int numPadawans = NUM_PADAWANS;

  while (numPadawans) {
    int waitTime = std::rand() % 10 + 1;
    for (int i = 0; i < waitTime; i++) {
      pthread_mutex_lock(yoda->mutexWaitingPadawan);
      if (yoda->waitingPadawan != 0) {
        yoda->waitingPadawan--;
        sem_post(yoda->semWaitingPadawan);
      }
      pthread_mutex_unlock(yoda->mutexWaitingPadawan);

      pthread_mutex_lock(yoda->mutexAudience);
      if (yoda->numAudience != 0) {
        yoda->numAudience--;
        sem_post(yoda->semSalaAudience);
      }
      pthread_mutex_unlock(yoda->mutexAudience);

      sleep(1);
    }
  }

  return nullptr;
}

void inicia_yoda(pthread_t tid, Yoda *yoda) {
  if (pthread_create(&tid, nullptr, inicia_testes, &yoda) < 0) {
    std::perror("pthread_create");
  }

  if (pthread_join(tid, nullptr) < 0) {
    std::perror("pthread_join");
  }
}
