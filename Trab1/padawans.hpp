#ifndef PADAWAN_HPP
#define PADAWAN_HPP

#include <pthread.h>
#include <semaphore.h>
#include <vector>

// Estrutura do Padawan
struct Padawan {
  // Identificador do padawan
  int id;

  // Número de padawans esperando para entrar no salão
  int *waitPadawan;

  // Número de padawans esperando para realizar teste
  int *testPadawan;

  // Número de padawans esperando para sair do salão
  int *finishPadawan;

  // Mutex para acesso ao número de padawans esperando entrar no salão
  pthread_mutex_t *mutexWaitPadawan;

  // Mutex para acesso ao número de padawans esperando teste no salão
  pthread_mutex_t *mutexTestPadawan;

  // Mutex para acesso ao número de padawans esperando para sair do salão
  pthread_mutex_t *mutexFinishPadawan;

  // Semáforo para a fila de padawans entrando no salão
  sem_t *semWaitPadawans;

  // Semáforo para a fila de padawans querendo fazer teste
  sem_t *semTestPadawans;

  // Semáforo para a fila de padawans que terminaram o teste
  sem_t *semFinishPadawans;
};

/**
 * @brief Inicializa os padawans que vão assistir as sessões
 *
 * Cria as threads que vão iniciar os padawans.
 *
 * @param tid Vetor com todas as threads que vão executar
 * @param padawan Vetor com todas as pessoas da Padawans
 */
int init_padawan(std::vector<pthread_t> tid, std::vector<Padawan> padawan);

#endif // PADAWAN_HPP