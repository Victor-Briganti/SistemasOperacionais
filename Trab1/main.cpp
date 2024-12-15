#include "audience.hpp"
#include "common.hpp"
#include "yoda.hpp"

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
    std::printf("Erro ao alocar sessionOver\n");
    return -1;
  }

  audience->waitAudience = new int(0);
  if (audience->waitAudience == nullptr) {
    std::printf("Erro ao alocar waitAudience\n");
    return -1;
  }

  audience->mutexSessionOver = new pthread_mutex_t;
  if (audience->mutexSessionOver == nullptr) {
    std::printf("Erro ao alocar mutexSessionOver\n");
    return -1;
  }

  if (pthread_mutex_init(audience->mutexSessionOver, nullptr)) {
    std::printf("Erro ao inicializar mutexSessionOver\n");
    return -1;
  }

  audience->mutexWaitAudience = new pthread_mutex_t;
  if (audience->mutexWaitAudience == nullptr) {
    std::printf("Erro ao alocar mutexWaitAudience\n");
    return -1;
  }

  if (pthread_mutex_init(audience->mutexWaitAudience, nullptr)) {
    std::printf("Erro ao inicializar mutexWaitAudience\n");
    return -1;
  }

  audience->semWaitAudience = new sem_t;
  if (audience->semWaitAudience == nullptr) {
    std::printf("Erro ao alocar semWaitAudience\n");
    return -1;
  }

  if (sem_init(audience->semWaitAudience, 0, 0)) {
    std::printf("Erro ao inicializar semWaitAudience\n");
    return -1;
  }

  for (int i = 0; i < AUDIENCE_NUM; i++) {
    pthread_t *audience = new pthread_t;
    if (audience == nullptr) {
      std::printf("Erro ao alocar threads de audiência\n");
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
      std::printf("[Audience %zu] ", i);
      perror("pthread_join");
      return -1;
    }
  }

  return 0;
}

//===------------------------------------------------------------------------===
// Yoda
//===------------------------------------------------------------------------===

// Thread do Yoda
pthread_t *yodaThread;

// Estrutura global que armazena informações sobre o Yoda
Yoda *yoda = nullptr;

/**
 * @brief Cria a estrutura do Yoda e inicializa sua thread
 *
 * @return 0 se tudo ocorreu bem, -1 se algo deu errado
 */
int create_yoda() {
  // Inicializa a estrutura
  Yoda *yoda = new Yoda;
  if (yoda == nullptr) {
    std::printf("Erro ao alocar Yoda\n");
    return -1;
  }
  yoda->audience = audience;

  // Inicializa a thread
  yodaThread = new pthread_t;
  if (yoda == nullptr) {
    std::printf("Erro ao alocar thread Yodas\n");
    return -1;
  }

  return init_yoda(yodaThread, yoda);
}

/**
 * @brief Realiza o join na thread do Yoda
 *
 * @return 0 se tudo ocorreu bem, -1 se algo deu errado
 */
int join_yoda() {
  if (pthread_join(*yodaThread, nullptr)) {
    std::printf("[Yoda] ");
    std::perror("pthread_join");
    return -1;
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

  if (create_yoda()) {
    exit(-1);
  }

  if (join_audience()) {
    exit(-1);
  }

  if (join_yoda()) {
    exit(-1);
  }
}