#ifndef AUDIENCE_HPP
#define AUDIENCE_HPP

#include <pthread.h>
#include <semaphore.h>
#include <vector>

// Estrutura do espectador
struct Audience {
  // Identificador do espectador
  int id;

  // Define se as sessões já acabaram
  bool *sessionOver;

  // Número de espectadores esperando para entrar no salão
  int *waitingAudience;

  // Mutex para acesso ao número de espectadores esperando entrar no salão
  pthread_mutex_t *mutexWaitAudience;

  // Semáforo para a fila de espectadores entrando no salão
  sem_t *semWaitAudience;
};

/**
 * @brief Inicializa a audiência que vai assistir as sessões
 *
 * Cria as threads que vão iniciar a audiência.
 *
 * @param tid Vetor com todas as threads que vão executar
 * @param audience Vetor com todas as pessoas da audiência
 *
 * @return 0 se tudo ocorreu bem, -1 se algum problema aconteceu
 */
int init_audience(std::vector<pthread_t> tid, std::vector<Audience> audience);

#endif // AUDIENCE_HPP