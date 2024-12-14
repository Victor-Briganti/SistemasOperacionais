#ifndef AUDIENCE_HPP
#define AUDIENCE_HPP

#include <pthread.h>
#include <semaphore.h>

struct Audience {
  // Número de pessoas esperando na audiência
  int *waitingAudience;

  // Controla a audiência querendo entrar na sala
  sem_t *semWaitingAudience;

  // Controla o número de pessoas na audiência
  pthread_mutex_t *mutexAudience;
};

/**
 * @brief Inicializando audiência.
 *
 * Inicializa as threads de audiência.
 */
int inicia_audiencia(pthread_t *tid, Audience *audience);

#endif // AUDIENCE_HPP
