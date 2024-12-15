#include "audience.hpp"
#include "common.hpp"

#include <cstdio>
#include <cstdlib>
#include <pthread.h>
#include <semaphore.h>

//===------------------------------------------------------------------------===
// Audiência
//===------------------------------------------------------------------------===

// Vetor de threads da audiência
std::vector<pthread_t *> audienceThreads;

// Estrutura global que armazena informações sobre a audiência
Audience *audience = nullptr;

namespace {

/**
 * @brief Cria a estrutura da audiência e inicializa suas threads
 *
 * @return 0 se tudo ocorreu bem, -1 se algo deu errado
 */
int create_audience() {
  audience = new Audience;
  if (audience == nullptr) {
    return -1;
  }

  audience->sessionOver = new bool(false);
  if (audience->sessionOver == nullptr) {
    printf("Erro ao alocar sessionOver\n");
    return -1;
  }

  audience->waitAudience = new int(0);
  if (audience->waitAudience == nullptr) {
    printf("Erro ao alocar waitAudience\n");
    return -1;
  }

  audience->insideAudience = new int(0);
  if (audience->insideAudience == nullptr) {
    printf("Erro ao alocar insideAudience\n");
    return -1;
  }

  audience->mutexSessionOver = new pthread_mutex_t;
  if (audience->mutexSessionOver == nullptr) {
    printf("Erro ao alocar mutexSessionOver\n");
    return -1;
  }

  if (pthread_mutex_init(audience->mutexSessionOver, nullptr)) {
    printf("Erro ao inicializar mutexSessionOver\n");
    return -1;
  }

  audience->mutexWaitAudience = new pthread_mutex_t;
  if (audience->mutexWaitAudience == nullptr) {
    printf("Erro ao alocar mutexWaitAudience\n");
    return -1;
  }

  if (pthread_mutex_init(audience->mutexWaitAudience, nullptr)) {
    printf("Erro ao inicializar mutexWaitAudience\n");
    return -1;
  }

  audience->mutexInsideAudience = new pthread_mutex_t;
  if (audience->mutexInsideAudience == nullptr) {
    printf("Erro ao alocar mutexInsideAudience\n");
    return -1;
  }

  if (pthread_mutex_init(audience->mutexInsideAudience, nullptr)) {
    printf("Erro ao inicializar mutexInsideAudience\n");
    return -1;
  }

  audience->semWaitAudience = new sem_t;
  if (audience->semWaitAudience == nullptr) {
    printf("Erro ao alocar semWaitAudience\n");
    return -1;
  }

  if (sem_init(audience->semWaitAudience, 0, 0)) {
    printf("Erro ao inicializar semWaitAudience\n");
    return -1;
  }

  for (int i = 0; i < AUDIENCE_NUM; i++) {
    pthread_t *audience = new pthread_t;
    if (audience == nullptr) {
      printf("Erro ao alocar threads de audiência\n");
      return -1;
    }

    audienceThreads.push_back(audience);
  }

  return init_audience(audienceThreads, audience);
}

/**
 * @brief Realiza o join nas threads de audiência
 *
 * @return 0 se tudo ocorreu bem, -1 se algo deu errado
 */
int join_audience() {
  for (size_t i = 0; i < AUDIENCE_NUM; i++) {
    if (pthread_join(*audienceThreads[i], nullptr)) {
      printf("[Audience %zu] ", i);
      perror("pthread_join");
      return -1;
    }
  }

  return 0;
}

} // namespace

//===------------------------------------------------------------------------===
// Main
//===------------------------------------------------------------------------===

int main() {
  if (create_audience()) {
    exit(-1);
  }

  if (join_audience()) {
    exit(-1);
  }
}