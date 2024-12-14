#ifndef YODA_HPP
#define YODA_HPP

#include <pthread.h>
#include <semaphore.h>

// Estrutura do Yoda
struct Yoda {
  // Define se as sessões já acabaram
  bool *sessionOver;

  // Número de espectadores esperando para entrar no salão
  int *waitAudience;

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

  // Mutex para acesso ao número de espectadores esperando entrar no salão
  pthread_mutex_t *mutexWaitAudience;

  // Semáforo para a fila de padawans entrando no salão
  sem_t *semWaitPadawans;

  // Semáforo para a fila de padawans querendo fazer teste
  sem_t *semTestPadawans;

  // Semáforo para a fila de padawans que terminaram o teste
  sem_t *semFinishPadawans;

  // Semáforo para a fila de espectadores entrando no salão
  sem_t *semWaitAudience;
};

/**
 * @brief Inicializa o Yoda
 *
 * Cria a thread que vai iniciar os Yoda.
 *
 * @param tid Thread que representa algumas ações do Yoda
 * @param yoda Estrutura do yoda
 *
 * @return 0 se tudo ocorreu bem, -1 se algum problema aconteceu
 */
int init_yoda(pthread_t &tid, Yoda *yoda);

#endif // YODA_HPP