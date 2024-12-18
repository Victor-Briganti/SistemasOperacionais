//===---- main.cpp - Arquivo principal de incialização --------------------===//
//
// Autores: Hendrick Felipe Scheifer e João Victor Briganti de Oliveira
// Data: 15/12/2024
//
//===----------------------------------------------------------------------===//
//
// Início do programa. Inicializa as estruturas necessárias e então coloca cada
// uma das threads para executar.
//
//===----------------------------------------------------------------------===//

#include "audience.hpp"
#include "common.hpp"
#include "padawan.hpp"
#include "yoda.hpp"

#include <cstdio>      // perror(), printf(), size_t
#include <cstdlib>     // exit()
#include <pthread.h>   // pthread_join(), pthread_mutex_init()
#include <semaphore.h> // sem_init(), sem_t

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
  // Inicializa a estrutura
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
    std::printf("Erro ao alocar countWait\n");
    return -1;
  }

  audience->countInside = new int(0);
  if (audience->countInside == nullptr) {
    std::printf("Erro ao alocar countInside\n");
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

  audience->mutexCount = new pthread_mutex_t;
  if (audience->mutexCount == nullptr) {
    std::printf("Erro ao alocar mutexWaitAudience\n");
    return -1;
  }

  if (pthread_mutex_init(audience->mutexCount, nullptr)) {
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

  audience->semTest = new sem_t;
  if (audience->semTest == nullptr) {
    std::printf("Erro ao alocar semTestAudience\n");
    return -1;
  }

  if (sem_init(audience->semTest, 0, 0)) {
    std::printf("Erro ao inicializar semTestAudience\n");
    return -1;
  }

  // Inicializa a thread
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

/**
 * @brief Destrói todas as estruturas alocadas para inicializar a audiência
 */
void destroy_audience() {
  // Destroi todos os mutexes
  pthread_mutex_destroy(audience->mutexSessionOver);
  pthread_mutex_destroy(audience->mutexCount);

  // Destroi todos os semáforos
  sem_destroy(audience->semTest);
  sem_destroy(audience->semWait);

  // Desaloca demais estruturas alocadas
  delete audience->sessionOver;
  delete audience->countWait;
  delete audience->countInside;
  delete audience->mutexSessionOver;
  delete audience->mutexCount;
  delete audience->semTest;
  delete audience->semWait;
  delete audience;

  // Desaloca cada uma das threads
  for (int i = 0; i < AUDIENCE_NUM; i++) {
    delete audienceThreads[i];
  }
}

//===------------------------------------------------------------------------===
// Padawan
//===------------------------------------------------------------------------===

// Vetor de threads da audiência
std::vector<pthread_t *> padawanThreads;

// Estrutura global que armazena informações sobre a audiência
Padawan *padawan = nullptr;

/**
 * @brief Cria a estrutura padawan e inicializa suas threads
 *
 * @return 0 se tudo ocorreu bem, -1 se algo deu errado
 */
int create_padawan() {
  // Inicializa a estrutura
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

  padawan->resultQueue = new std::list<std::pair<int, bool>>;
  if (padawan->resultQueue == nullptr) {
    std::printf("Erro não foi possível alocar resultQueue\n");
    return -1;
  }

  padawan->cutQueue = new std::list<int>;
  if (padawan->cutQueue == nullptr) {
    std::printf("Erro não foi possível alocar cutQueue\n");
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

  padawan->semPosition = new sem_t;
  if (padawan->semPosition == nullptr) {
    std::printf("Erro não foi possível alocar semPosition\n");
    return -1;
  }

  if (sem_init(padawan->semPosition, 0, 0)) {
    std::printf("Erro ao iniciar o semáforo semPosition\n");
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

  padawan->semTestFinish = new sem_t;
  if (padawan->semTestFinish == nullptr) {
    std::printf("Erro não foi possível alocar semTestFinish\n");
    return -1;
  }

  if (sem_init(padawan->semTestFinish, 0, 0)) {
    std::printf("Erro ao iniciar o semáforo semTestFinish\n");
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

  padawan->semResultFinish = new sem_t;
  if (padawan->semResultFinish == nullptr) {
    std::printf("Erro não foi possível alocar semResultFinish\n");
    return -1;
  }

  if (sem_init(padawan->semResultFinish, 0, 0)) {
    std::printf("Erro ao iniciar o semáforo semResultFinish\n");
    std::perror("sem_init\n");
    return -1;
  }

  padawan->semCut = new sem_t;
  if (padawan->semCut == nullptr) {
    std::printf("Erro não foi possível alocar semCut\n");
    return -1;
  }

  if (sem_init(padawan->semCut, 0, 0)) {
    std::printf("Erro ao iniciar o semáforo semCut\n");
    std::perror("sem_init\n");
    return -1;
  }

  // Inicializa a thread
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

/**
 * @brief Destrói todas as estruturas alocadas para inicializar os padawans
 */
void destroy_padawan() {
  // Destroi o mutex
  pthread_mutex_destroy(padawan->mutex);

  // Destroi todos os semáforos
  sem_destroy(padawan->semWait);
  sem_destroy(padawan->semPosition);
  sem_destroy(padawan->semTest);
  sem_destroy(padawan->semTestFinish);
  sem_destroy(padawan->semResult);
  sem_destroy(padawan->semResultFinish);
  sem_destroy(padawan->semCut);

  // Desaloca demais estruturas alocadas
  delete padawan->waitQueue;
  delete padawan->testQueue;
  delete padawan->resultQueue;
  delete padawan->cutQueue;
  delete padawan->mutex;
  delete padawan->semWait;
  delete padawan->semPosition;
  delete padawan->semTest;
  delete padawan->semTestFinish;
  delete padawan->semResult;
  delete padawan->semResultFinish;
  delete padawan->semCut;
  delete padawan;

  // Desaloca cada uma das threads
  for (int i = 0; i < PADAWAN_NUM; i++) {
    delete padawanThreads[i];
  }
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
  yoda = new Yoda;
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
  std::printf("[Yoda] Faz discurso\n");
  std::printf("[Yoda] Finaliza sessão\n");

  if (pthread_join(*yodaThread, nullptr)) {
    std::printf("[Yoda] ");
    std::perror("pthread_join");
    return -1;
  }

  return 0;
}

/**
 * @brief Destrói as estruturas alocadas para inicializar o Yoda
 */
void destroy_yoda() {
  // Desaloca estrutura alocada
  delete yoda;

  // Desaloca sua thread
  delete yodaThread;
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

  destroy_audience();
  destroy_padawan();
  destroy_yoda();
}
