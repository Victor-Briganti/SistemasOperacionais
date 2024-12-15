#include "audience.hpp"
#include "common.hpp"
#include "padawan.hpp"
#include "yoda.hpp"

#include <cstdio>
#include <list>
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
// Padawans
//===----------------------------------------------------------------------===//

// Lista da estrutura dos Padawans
std::vector<Padawan *> padawanList;

// Lista de threads dos Padawans
std::vector<pthread_t *> padawanThreads;

// Lista de padawans esperando para entrar
std::list<int> *listWaitPadawan;

// Lista de padawans esperando teste
std::list<int> *listTestPadawan;

// Lista dos resultados dos padawans
std::list<std::pair<int, bool>> *listResultPadawan;

// Mutex para acesso a lista de padawans esperando para entrar
pthread_mutex_t *mutexWaitPadawan;

// Mutex para acesso a lista de padawans esperando teste
pthread_mutex_t *mutexTestPadawan;

// Mutex para acesso a lista de padawans esperando resultados
pthread_mutex_t *mutexResultPadawan;

// Semáforo para esperar a entrada
sem_t *semWaitPadawan;

// Semáforo de padawans esperando teste
sem_t *semTestPadawan;

// Semáforo de padawans esperando resultado
sem_t *semResultPadawan;

//===----------------------------------------------------------------------===//
// Yoda
//===----------------------------------------------------------------------===//

// Estrutura que define o Yoda
Yoda *yoda;

// Thread que define o Yoda
pthread_t *yodaThread;

// Define se a sessão de testes acabou
bool *sessionOver;

// Mutex para acesso da global sessionOver
pthread_mutex_t *mutexSessionOver;

//===----------------------------------------------------------------------===//
// Funções Padrões
//===----------------------------------------------------------------------===//

namespace {

/** @brief Inicializa todas as globais do sistema */
int init_globals() {

  listWaitPadawan = new std::list<int>();
  if (listWaitPadawan == nullptr) {
    std::printf("Erro alocando listWaitPadawan\n");
    return -1;
  }

  listTestPadawan = new std::list<int>();
  if (listTestPadawan == nullptr) {
    std::printf("Erro alocando listTestPadawan\n");
    return -1;
  }

  listResultPadawan = new std::list<std::pair<int, bool>>();
  if (listResultPadawan == nullptr) {
    std::printf("Erro alocando listResultPadawan\n");
    return -1;
  }

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

  mutexSessionOver = new pthread_mutex_t;
  if (mutexSessionOver == nullptr) {
    std::printf("Erro alocando mutexSessionOver\n");
    return -1;
  }

  if (pthread_mutex_init(mutexSessionOver, nullptr)) {
    std::printf("Erro iniciando mutexSessionOver\n");
    return -1;
  }

  mutexResultPadawan = new pthread_mutex_t;
  if (mutexResultPadawan == nullptr) {
    std::printf("Erro alocando mutexResultPadawan\n");
    return -1;
  }

  if (pthread_mutex_init(mutexResultPadawan, nullptr)) {
    std::printf("Erro iniciando mutexResultPadawan\n");
    return -1;
  }

  mutexWaitPadawan = new pthread_mutex_t;
  if (mutexWaitPadawan == nullptr) {
    std::printf("Erro alocando mutexWaitPadawan\n");
    return -1;
  }

  if (pthread_mutex_init(mutexWaitPadawan, nullptr)) {
    std::printf("Erro iniciando mutexWaitPadawan\n");
    return -1;
  }

  mutexTestPadawan = new pthread_mutex_t;
  if (mutexTestPadawan == nullptr) {
    std::printf("Erro alocando mutexTestPadawan\n");
    return -1;
  }

  if (pthread_mutex_init(mutexTestPadawan, nullptr)) {
    std::printf("Erro iniciando mutexTestPadawan\n");
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

  semWaitPadawan = new sem_t;
  if (semWaitPadawan == nullptr) {
    std::printf("Erro alocando semWaitPadawan\n");
    return -1;
  }

  if (sem_init(semWaitPadawan, 0, 0)) {
    std::printf("Erro iniciando semWaitPadawan\n");
    return -1;
  }

  semTestPadawan = new sem_t;
  if (semTestPadawan == nullptr) {
    std::printf("Erro alocando semTestPadawan\n");
    return -1;
  }

  if (sem_init(semTestPadawan, 0, 0)) {
    std::printf("Erro iniciando semTestPadawan\n");
    return -1;
  }

  semResultPadawan = new sem_t;
  if (semResultPadawan == nullptr) {
    std::printf("Erro alocando semResultPadawan\n");
    return -1;
  }

  if (sem_init(semResultPadawan, 0, 0)) {
    std::printf("Erro iniciando semResultPadawan\n");
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
    audience->mutexSessionOver = mutexSessionOver;
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
 * @brief Cria os padawans
 *
 * Cria as estruturas inicias e as threads dos padawans.
 *
 * @return 0 se tudo ocorreu bem, -1 se algo deu errado.
 */
int create_padawan() {
  for (int i = 0; i < PADAWAN_NUM; i++) {
    Padawan *padawan = new Padawan;
    if (padawan == nullptr) {
      std::printf("Não foi possível inicializar padawans\n");
      return -1;
    }

    padawan->listWaitPadawan = listWaitPadawan;
    padawan->listTestPadawan = listTestPadawan;
    padawan->listResultPadawan = listResultPadawan;
    padawan->mutexWaitPadawan = mutexWaitPadawan;
    padawan->mutexTestPadawan = mutexTestPadawan;
    padawan->mutexResultPadawan = mutexResultPadawan;
    padawan->semWaitPadawan = semWaitPadawan;
    padawan->semTestPadawan = semTestPadawan;
    padawan->semResultPadawan = semResultPadawan;
    padawanList.push_back(padawan);

    pthread_t *padawanThread = new pthread_t;
    if (padawanThread == nullptr) {
      std::printf("Não foi possível inicializar thread da audiência\n");
      return -1;
    }

    padawanThreads.push_back(padawanThread);
  }

  return init_padawan(padawanThreads, padawanList);
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
  yoda->listWaitPadawan = listWaitPadawan;
  yoda->listResultPadawan = listResultPadawan;
  yoda->listTestPadawan = listTestPadawan;
  yoda->mutexWaitAudience = mutexWaitAudience;
  yoda->mutexWaitPadawan = mutexWaitPadawan;
  yoda->mutexTestPadawan = mutexTestPadawan;
  yoda->mutexResultPadawan = mutexResultPadawan;
  yoda->mutexSessionOver = mutexSessionOver;
  yoda->semWaitAudience = semWaitAudience;
  yoda->semWaitPadawan = semWaitPadawan;
  yoda->semTestPadawan = semTestPadawan;
  yoda->semResultPadawan = semResultPadawan;

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
 * @brief Realiza o join com todas as threads do tipo padawan
 *
 * @return 0 se tudo ocorreu bem, -1 se algo deu errado.
 */
int join_padawan() {
  for (int i = 0; i < PADAWAN_NUM; i++) {
    if (pthread_join(*padawanThreads[i], nullptr)) {
      std::printf("[Padawan %d] ", i);
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

  if (create_padawan()) {
    exit(1);
  }

  if (create_yoda()) {
    exit(1);
  }

  if (join_audience()) {
    exit(1);
  }

  if (join_padawan()) {
    exit(1);
  }

  if (join_yoda()) {
    exit(1);
  }
}