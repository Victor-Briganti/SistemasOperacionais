#ifndef YODA_HPP
#define YODA_HPP

#include <pthread.h>
#include <semaphore.h>

// Estrutura do Yoda
struct Yoda {
  // Define se a sessão de testes acabou
  bool *sessionOver;

  // Armazena o número de pessoas da audiência esperando para entrar
  int *waitAudience;

  // Mutex para acesso da fila de audiência
  pthread_mutex_t *mutexWaitAudience;

  // Semáforo para controle de acesso da audiência
  sem_t *semWaitAudience;
};

/**
 * @brief Inicializa o Yoda
 *
 * @param yodaThreads Thread que representa o Yoda
 * @param yoda Estrutura com todas as informações que o Yoda usa
 *
 * @return 0 se tudo ocorreu bem, -1 se algo deu errado.
 */
int init_yoda(pthread_t *yodaThread, Yoda *yoda);

#endif // YODA_HPP