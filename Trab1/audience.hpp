#ifndef AUDIENCE_HPP
#define AUDIENCE_HPP

#include <pthread.h>
#include <semaphore.h>
#include <vector>

// Estrutura inicial da audiência
struct Audience {
  // Identificador da audiência
  int id;

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
 * @brief Inicializa a platéia que vai assistir ao evento
 *
 * @param audienceThreads Threads que representam cada pessoa na plateia
 * @param audienceList Vetor com estruturas que armazenam informações da plateia
 *
 * @return 0 se tudo ocorreu bem, -1 se algo deu errado.
 */
int init_audience(std::vector<pthread_t *> audienceThreads,
                  std::vector<Audience *> audienceList);

#endif // AUDIENCE_HPP