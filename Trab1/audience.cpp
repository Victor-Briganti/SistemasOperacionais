#include "audience.hpp"
#include "common.hpp"
#include <cstdio>
#include <pthread.h>
#include <unistd.h>

namespace {

/**
 * @brief Entra no salão para assistir as apresentações
 *
 * @param audience Ponteiro pra a estrutura audiência
 */
void entra_salao(Audience *audience) {
  pthread_mutex_lock(audience->mutexWaitAudience);
  std::printf("[Audience %d] quer entrar no salão\n", audience->id);
  (*audience->waitAudience)++;
  pthread_mutex_unlock(audience->mutexWaitAudience);

  sem_wait(audience->semWaitAudience);
  std::printf("[Audience %d] entra no salão\n", audience->id);
}

/**
 * @brief Assiste as sessões
 *
 * Assiste os testes por um período de tempo e então sai.
 *
 * @param audienceId Identificador da audiência
 */
void assiste_testes(int audienceId) {
  int watchTime = AUDIENCE_WATCH_TIME;
  std::printf("[Audience %d] assiste testes %ds\n", audienceId, watchTime);
  sleep(watchTime);
}

/**
 * @brief Sai do salão
 *
 * @param audienceId Identificação da audiência
 */
void sai_salao(int audienceId) {
  std::printf("[Audience %d] sai do salão\n", audienceId);
}

/**
 * @brief Inicia as ações da audiência
 *
 * @param arg Ponteiro void para estrutura audiência
 *
 * @return nullptr
 */
void *start(void *arg) {
  Audience *audience = static_cast<Audience *>(arg);

  while (true) {
    pthread_mutex_lock(audience->mutexSessionOver);
    if ((*audience->sessionOver) == true) {
      pthread_mutex_unlock(audience->mutexSessionOver);
      break;
    }
    pthread_mutex_unlock(audience->mutexSessionOver);

    entra_salao(audience);
    assiste_testes(audience->id);
    sai_salao(audience->id);
  }

  return nullptr;
}

} // namespace

int init_audience(std::vector<pthread_t *> audienceThreads,
                  std::vector<Audience *> audienceList) {
  for (size_t i = 0; i < audienceThreads.size(); i++) {
    if (pthread_create(audienceThreads[i], nullptr, start, audienceList[i])) {
      std::printf("[Audience %zu] ", i);
      std::perror("pthread_create");
      return -1;
    }
  }

  return 0;
}