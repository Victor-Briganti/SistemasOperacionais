#include "audience.hpp"
#include "common.hpp"
#include "padawan.hpp"
#include "yoda.hpp"

#include <cstdio>
#include <cstdlib>
#include <pthread.h>
#include <semaphore.h>

//===------------------------------------------------------------------------===
// Audiência
//===------------------------------------------------------------------------===

namespace {

// Vetor de threads da audiência
std::vector<pthread_t *> audienceThreads;

// Estrutura global que armazena informações sobre a audiência
Audience *audience = nullptr;

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

  audience->countWait = new int(0);
  if (audience->countWait == nullptr) {
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

  audience->mutexWait = new pthread_mutex_t;
  if (audience->mutexWait == nullptr) {
    std::printf("Erro ao alocar mutexWaitAudience\n");
    return -1;
  }

  if (pthread_mutex_init(audience->mutexWait, nullptr)) {
    std::printf("Erro ao inicializar mutexWaitAudience\n");
    return -1;
  }

  audience->semWait = new sem_t;
  if (audience->semWait == nullptr) {
    std::printf("Erro ao alocar semWaitAudience\n");
    return -1;
  }

  if (sem_init(audience->semWait, 0, 0)) {
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
// Padawan
//===------------------------------------------------------------------------===

// Vetor de threads da audiência
std::vector<pthread_t *> padawanThreads;

// Estrutura global que armazena informações sobre a audiência
Padawan *padawan = nullptr;

/**
 * @brief Cria a estrutura da audiência e inicializa suas threads
 *
 * @return 0 se tudo ocorreu bem, -1 se algo deu errado
 */
int create_padawan() {
  padawan = new Padawan;
  if (padawan == nullptr) {
    return -1;
  }

  padawan->waitQueue = new std::list<int>;
  if (padawan->waitQueue == nullptr) {
    std::printf("Erro não foi possível alocar waitQueue\n");
    return -1;
  }

  padawan->testQueue = new std::list<int>;
  if (padawan->testQueue == nullptr) {
    std::printf("Erro não foi possível alocar testQueue\n");
    return -1;
  }

  padawan->resultQueue = new std::list<int>;
  if (padawan->resultQueue == nullptr) {
    std::printf("Erro não foi possível alocar resultQueue\n");
    return -1;
  }

  padawan->mutex = new pthread_mutex_t;
  if (padawan->mutex == nullptr) {
    std::printf("Erro não foi possível alocar mutex\n");
    return -1;
  }

  if (pthread_mutex_init(padawan->mutex, nullptr)) {
    std::printf("Erro ao iniciar mutex\n");
    std::perror("pthread_mutex_init\n");
    return -1;
  }

  padawan->semWait = new sem_t;
  if (padawan->semWait == nullptr) {
    std::printf("Erro não foi possível alocar semWait\n");
    return -1;
  }

  if (sem_init(padawan->semWait, 0, 0)) {
    std::printf("Erro ao iniciar o semáforo semWait\n");
    std::perror("sem_init\n");
    return -1;
  }

  padawan->semTest = new sem_t;
  if (padawan->semTest == nullptr) {
    std::printf("Erro não foi possível alocar semTest\n");
    return -1;
  }

  if (sem_init(padawan->semTest, 0, 0)) {
    std::printf("Erro ao iniciar o semáforo semTest\n");
    std::perror("sem_init\n");
    return -1;
  }

  padawan->semResult = new sem_t;
  if (padawan->semResult == nullptr) {
    std::printf("Erro não foi possível alocar semResult\n");
    return -1;
  }

  if (sem_init(padawan->semResult, 0, 0)) {
    std::printf("Erro ao iniciar o semáforo semResult\n");
    std::perror("sem_init\n");
    return -1;
  }

  for (int i = 0; i < PADAWAN_NUM; i++) {
    pthread_t *thread = new pthread_t;
    if (thread == nullptr) {
      std::printf("Erro ao iniciar a thread de Padawans\n");
      return -1;
    }

    padawanThreads.push_back(thread);
  }

  return init_padawan(padawanThreads, padawan);
}

/**
 * @brief Realiza o join nas threads dos padawanss
 *
 * @return 0 se tudo ocorreu bem, -1 se algo deu errado
 */
int join_padawan() {
  for (size_t i = 0; i < PADAWAN_NUM; i++) {
    if (pthread_join(*padawanThreads[i], nullptr)) {
      std::printf("[Padawan %zu] ", i);
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

  // Salva as estruturas
  yoda->audience = audience;
  yoda->padawan = padawan;

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

  if (create_padawan()) {
    exit(-1);
  }

  if (create_yoda()) {
    exit(-1);
  }

  if (join_audience()) {
    exit(-1);
  }

  if (join_padawan()) {
    exit(-1);
  }

  if (join_yoda()) {
    exit(-1);
  }
}