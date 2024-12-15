#ifndef YODA_HPP
#define YODA_HPP

#include <list>
#include <pthread.h>
#include <semaphore.h>

// Estrutura do Yoda
struct Yoda {
  // Define se a sessão de testes acabou
  bool *sessionOver;

  // Armazena o número de pessoas da audiência esperando para entrar
  int *waitAudience;

  // Lista de padawans esperando para entrar
  std::list<int> *listWaitPadawan;

  // Lista de padawans esperando teste
  std::list<int> *listTestPadawan;

  // Lista dos resultados dos padawans
  std::list<std::pair<int, bool>> *listResultPadawan;

  // Mutex para acesso a sessão
  pthread_mutex_t *mutexSessionOver;

  // Mutex para acesso da fila de audiência
  pthread_mutex_t *mutexWaitAudience;

  // Mutex para acesso a lista de padawans esperando para entrar
  pthread_mutex_t *mutexWaitPadawan;

  // Mutex para acesso a lista de padawans esperando teste
  pthread_mutex_t *mutexTestPadawan;

  // Mutex para acesso a lista de padawans esperando resultados
  pthread_mutex_t *mutexResultPadawan;

  // Semáforo para controle de acesso da audiência
  sem_t *semWaitAudience;

  // Semáforo para esperar a entrada
  sem_t *semWaitPadawan;

  // Semáforo de padawans esperando teste
  sem_t *semTestPadawan;

  // Semáforo de padawans esperando resultado
  sem_t *semResultPadawan;
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