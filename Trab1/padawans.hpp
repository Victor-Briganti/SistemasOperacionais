#ifndef PADAWAN_HPP
#define PADAWAN_HPP

#include "common.hpp"

#include <pthread.h>
#include <semaphore.h>
#include <vector>

// Estrutura do Padawan
struct Padawan {
  // Identificador do padawan
  int id;

  // Fila para padawans esperando para entrar no salão
  std::vector<int> *waitPadawanQueue;

  // Fila para padawans esperando para realizar teste
  std::vector<int> *testPadawanQueue;

  // Fila para padawans aguardando resultado
  std::vector<Result> *resultPadawanQueue;

  // Fila para padawans aguardando corte na trança
  std::vector<int> *cutPadawanQueue;

  // Mutex para acesso ao número de padawans esperando entrar no salão
  pthread_mutex_t *mutexWaitPadawan;

  // Mutex para acesso ao número de padawans aguardando resultado
  pthread_mutex_t *mutexResultPadawan;

  // Mutex para acesso ao número de padawans esperando teste no salão
  pthread_mutex_t *mutexTestPadawan;

  // Mutex para acesso ao número de padawans aguardando corte na trança
  pthread_mutex_t *mutexCutPadawan;

  // Semáforo para a fila de padawans entrando no salão
  sem_t *semWaitPadawans;

  // Semáforo para a fila de padawans aguardando resultado
  sem_t *semResultPadawans;

  // Semáforo para a fila de padawans querendo fazer teste
  sem_t *semTestPadawans;

  // Semáforo para a fila de padawans aguardando corte na trança
  sem_t *semCutPadawans;
};

/**
 * @brief Inicializa os padawans que vão assistir as sessões
 *
 * Cria as threads que vão iniciar os padawans.
 *
 * @param tid Vetor com todas as threads que vão executar
 * @param padawan Vetor com todas as pessoas da Padawans
 *
 * @return 0 se tudo ocorreu bem, -1 se algum problema aconteceu
 */
int init_padawan(std::vector<pthread_t> tid, std::vector<Padawan> padawan);

#endif // PADAWAN_HPP