#include "audience.hpp"
#include "common.hpp"
#include "yoda.hpp"

#include <cstdio>
#include <pthread.h>
#include <semaphore.h>
#include <vector>

//===----------------------------------------------------------------------===//
// Audiência
//===----------------------------------------------------------------------===//

// Lista da estrutura Audiência
std::vector<Audience *> audienceList;

// Lista de threads da audiência
std::vector<pthread_t *> audienceThreads;

// Armazena o número de pessoas da audiência esperando para entrar
int *waitAudience;

// Mutex para acesso da fila de audiência
pthread_mutex_t *mutexWaitAudience;

// Semáforo para controle de acesso da audiência
sem_t *semWaitAudience;

//===----------------------------------------------------------------------===//
// Yoda
//===----------------------------------------------------------------------===//

// Estrutura que define o Yoda
Yoda *yoda;

// Thread que define o Yoda
pthread_t *yodaThread;

// Define se a sessão de testes acabou
bool *sessionOver;

//===----------------------------------------------------------------------===//
// Funções Padrões
//===----------------------------------------------------------------------===//

namespace {

/** @brief Inicializa todas as globais do sistema */
int init_globals() {
  waitAudience = new int(0);
  if (waitAudience == nullptr) {
    std::printf("Erro alocando waitAudience\n");
    return -1;
  }

  sessionOver = new bool(false);
  if (sessionOver == nullptr) {
    std::printf("Erro alocando sessionOver\n");
    return -1;
  }

  mutexWaitAudience = new pthread_mutex_t;
  if (mutexWaitAudience == nullptr) {
    std::printf("Erro alocando mutexWaitAudience\n");
    return -1;
  }

  if (pthread_mutex_init(mutexWaitAudience, nullptr)) {
    std::printf("Erro iniciando mutexWaitAudience\n");
    return -1;
  }

  semWaitAudience = new sem_t;
  if (semWaitAudience == nullptr) {
    std::printf("Erro alocando semWaitAudience\n");
    return -1;
  }

  if (sem_init(semWaitAudience, 0, 0)) {
    std::printf("Erro iniciando semWaitAudience\n");
    return -1;
  }

  return 0;
}

/**
 * @brief Cria a audiência
 *
 * Cria as estruturas inicias e as threads da audiência.
 *
 * @return 0 se tudo ocorreu bem, -1 se algo deu errado.
 */
int create_audience() {
  for (int i = 0; i < AUDIENCE_NUM; i++) {
    Audience *audience = new Audience;
    if (audience == nullptr) {
      std::printf("Não foi possível inicializar audiência\n");
      return -1;
    }
    audience->id = i;
    audience->sessionOver = sessionOver;
    audience->waitAudience = waitAudience;
    audience->mutexWaitAudience = mutexWaitAudience;
    audience->semWaitAudience = semWaitAudience;
    audienceList.push_back(audience);

    pthread_t *audienceThread = new pthread_t;
    if (audienceThread == nullptr) {
      std::printf("Não foi possível inicializar thread da audiência\n");
      return -1;
    }

    audienceThreads.push_back(audienceThread);
  }

  return init_audience(audienceThreads, audienceList);
}

/**
 * @brief Cria o Yoda
 *
 * Cria a estrutura inicial e a thread do Yoda.
 *
 * @return 0 se tudo ocorreu bem, -1 se algo deu errado.
 */
int create_yoda() {
  yoda = new Yoda;
  if (yoda == nullptr) {
    std::printf("Não foi possível inicializar Yoda\n");
    return -1;
  }

  yoda->sessionOver = sessionOver;
  yoda->waitAudience = waitAudience;
  yoda->mutexWaitAudience = mutexWaitAudience;
  yoda->semWaitAudience = semWaitAudience;

  yodaThread = new pthread_t;
  if (yodaThread == nullptr) {
    std::printf("Não foi possível inicializar thread Yoda\n");
    return -1;
  }

  return init_yoda(yodaThread, yoda);
}

/**
 * @brief Realiza o join com todas as threads do tipo audiência
 *
 * @return 0 se tudo ocorreu bem, -1 se algo deu errado.
 */
int join_audience() {
  for (int i = 0; i < AUDIENCE_NUM; i++) {
    if (pthread_join(*audienceThreads[i], nullptr)) {
      std::printf("[Audience %d] ", i);
      std::perror("pthread_join");
      return -1;
    }
  }

  return 0;
}

/**
 * @brief Realiza o join com a thread Yoda
 *
 * @return 0 se tudo ocorreu bem, -1 se algo deu errado.
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

//===----------------------------------------------------------------------===//
// Main
//===----------------------------------------------------------------------===//

int main() {
  init_globals();
  if (create_audience()) {
    exit(1);
  }

  if (create_yoda()) {
    exit(1);
  }

  if (join_audience()) {
    exit(1);
  }

  if (join_yoda()) {
    exit(1);
  }
}