#ifndef AUDIENCE_HPP
#define AUDIENCE_HPP

#include <pthread.h>
#include <semaphore.h>
#include <vector>

// Estrutura da audiência
struct Audience {
  // Flag para definir se a sessão ainda está acontecendo
  bool *sessionOver;

  // Contador de pessoas esperando entrar na sala
  int *countWait;

  // Contador de pessoas dentro da sala
  int *countInside;

  // Mutex para acesso ao sessionOver
  pthread_mutex_t *mutexSessionOver;

  // Mutex para acesso aos contadores
  pthread_mutex_t *mutexCount;

  // Semáforo para acesso a sala
  sem_t *semWait;
};

/**
 * @brief Inicia as threads da audiencia
 *
 * @param audienceThreads Vetor com as threads da audiência
 * @param audience Estrutura que armazena as informações da audiência
 *
 * @return 0 se tudo ocorreu bem, -1 se algo deu errado.
 */
int init_audience(std::vector<pthread_t *> audienceThreads, Audience *audience);

#endif // AUDIENCE_HPP