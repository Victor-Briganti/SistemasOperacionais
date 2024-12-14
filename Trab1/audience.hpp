#ifndef AUDIENCE_HPP
#define AUDIENCE_HPP

#include <pthread.h>
#include <semaphore.h>
#include <vector>

struct Audience {
  // Identificador do espectador
  int id;

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
void inicia_audiencia(std::vector<pthread_t> tid,
                      std::vector<Audience> audience);

#endif // AUDIENCE_HPP
