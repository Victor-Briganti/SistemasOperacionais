#include "yoda.hpp"
#include "common.hpp"

#include <cstdio>
#include <ctime>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

namespace {

void libera_entrada(Yoda *yoda) {
  std::printf("[Yoda] libera entrada\n");
  time_t tmpTime = time(nullptr);

  while (time(nullptr) - tmpTime < YODA_ENTRY_TIME) {
    pthread_mutex_lock(yoda->mutexWaitAudience);
    if ((*yoda->waitAudience)) {
      (*yoda->waitAudience)--;
      sem_post(yoda->semWaitAudience);
    }
    pthread_mutex_unlock(yoda->mutexWaitAudience);
  }
}

void inicia_testes() { std::printf("[Yoda] inicia testes\n"); }

void anuncia_resultado() { std::printf("[Yoda] anuncia resultados\n"); }

void corta_tranca() { std::printf("[Yoda] corta tranças\n"); }

void finaliza_testes() { std::printf("[Yoda] finaliza testes\n"); }

void discurso() { std::printf("[Yoda] faz discurso\n"); }

/**
 * @brief Inicia as ações do Yoda
 *
 * @param arg Ponteiro void para estrutura Yoda
 *
 * @return nullptr
 */
void *start(void *arg) {
  Yoda *yoda = static_cast<Yoda *>(arg);

  for (int i = 0; i < 10; i++) {
    libera_entrada(yoda);
    inicia_testes();
    anuncia_resultado();
    corta_tranca();
    finaliza_testes();
  }

  (*yoda->sessionOver) = true;
  discurso();

  return nullptr;
}

} // namespace

int init_yoda(pthread_t *yodaThread, Yoda *yoda) {
  if (pthread_create(yodaThread, nullptr, start, yoda)) {
    std::printf("[Yoda] ");
    std::perror("pthread_create");
    return -1;
  }

  return 0;
}
