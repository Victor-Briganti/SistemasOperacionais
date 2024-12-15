#ifndef PADAWANS_HPP
#define PADAWANS_HPP

#include <list>
#include <pthread.h>
#include <semaphore.h>
#include <vector>

// Estrutura dos Padawans
struct Padawan {
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
};

/**
 * @brief Inicializa os Padawans
 *
 * @param padawanThreads Threads que representam os Padawans
 * @param padawan Estrutura com todas as informações que os Padawans usam
 *
 * @return 0 se tudo ocorreu bem, -1 se algo deu errado.
 */
int init_padawan(std::vector<pthread_t *> padawanThreads,
                 std::vector<Padawan *> padawanList);

#endif // PADAWANS_HPP