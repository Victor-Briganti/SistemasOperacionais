#include "yoda.hpp"
#include "defines.hpp"

#include <cstdio>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

namespace {

int numPadawansSession = 0;

void libera_entrada(int &numPadawans, Yoda *yoda) {
  std::printf("[Yoda] liberando entradas\n");
  for (int i = 0; i < 10; i++) {
    pthread_mutex_lock(yoda->mutexWaitAudience);
    if (*yoda->waitAudience) {
      (*yoda->waitAudience)--;
      sem_post(yoda->semWaitAudience);
    }
    pthread_mutex_unlock(yoda->mutexWaitAudience);

    pthread_mutex_lock(yoda->mutexWaitPadawan);
    if (*yoda->waitPadawan) {
      (*yoda->waitPadawan)--;
      sem_post(yoda->semWaitPadawans);
      numPadawansSession++;
      numPadawans--;
    }
    pthread_mutex_unlock(yoda->mutexWaitPadawan);

    usleep(500);
  }
}

void inicia_testes(Yoda *yoda) {
  std::printf("[Yoda] inicia testes\n");
  sleep(5);

  int numPadawansTesting = numPadawansSession;

  while (true) {
    if (numPadawansTesting == 0) {
      break;
    }

    pthread_mutex_lock(yoda->mutexTestPadawan);
    if ((*yoda->testPadawan)) {
      (*yoda->testPadawan)--;
      numPadawansTesting--;
      sem_post(yoda->semWaitPadawans);
    }
    pthread_mutex_unlock(yoda->mutexTestPadawan);
  }
}

void finaliza_testes(Yoda *yoda) {
  std::printf("[Yoda] finaliza testes\n");
  sleep(5);

  int numPadawansFinish = numPadawansSession;

  while (true) {
    if (numPadawansFinish == 0) {
      break;
    }

    pthread_mutex_lock(yoda->mutexFinishPadawan);
    if ((*yoda->finishPadawan)) {
      (*yoda->finishPadawan)--;
      numPadawansFinish--;
      sem_post(yoda->semFinishPadawans);
    }
    pthread_mutex_unlock(yoda->mutexFinishPadawan);
  }
}

void discurso() {
  std::printf("[Yoda] discurso\n");
  sleep(5);
}

void *start(void *arg) {
  Yoda *yoda = static_cast<Yoda *>(arg);
  int numPadawans = NUM_PADAWANS;

  while (numPadawans) {
    libera_entrada(numPadawans, yoda);
    inicia_testes(yoda);
    finaliza_testes(yoda);
    numPadawansSession = 0;
  }

  (*yoda->sessionOver) = true;
  discurso();

  return nullptr;
}

} // namespace

int init_yoda(pthread_t tid, Yoda *yoda) {
  if (pthread_create(&tid, nullptr, start, yoda) != 0) {
    std::printf("[Yoda] ");
    std::perror("pthread_create ");
    return -1;
  }

  if (pthread_join(tid, nullptr) != 0) {
    std::printf("[Yoda] ");
    std::perror("pthread_join ");
    return -1;
  }

  return 0;
}
