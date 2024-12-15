#ifndef PADAWAN_HPP
#define PADAWAN_HPP

#include <list>
#include <pthread.h>
#include <semaphore.h>
#include <vector>

// Estrutura do Padawan
struct Padawan {
  // Fila de padawans esperando entrar no salão
  std::list<int> *waitQueue;

  // Fila de padawans esperando teste
  std::list<int> *testQueue;

  // Fila de padawans esperando resultado
  std::list<std::pair<int, bool>> *resultQueue;

  // Mutex para acesso da estrutura
  pthread_mutex_t *mutex;

  // Semáforo para padawans esperando entrar no salão
  sem_t *semWait;

  // Semáforo para padawans esperando resultado
  sem_t *semResult;

  // Semáforo para padawans que finalizaram toda sessão
  sem_t *semFinish;
};

/**
 * @brief Inicia as threads dos padawans
 *
 * @param padawanThreads Vetor com as threads dos padawans
 * @param padawan Estrutura que armazena as informações dos padawans
 *
 * @return 0 se tudo ocorreu bem, -1 se algo deu errado.
 */
int init_padawan(std::vector<pthread_t *> padawanThreads, Padawan *padawan);

#endif // PADAWAN_HPP