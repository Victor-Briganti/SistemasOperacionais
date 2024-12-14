#ifndef PADAWAN_HPP
#define PADAWAN_HPP

#include <pthread.h>
#include <semaphore.h>

struct Padawan {
  // Número de padawans esperando
  int *waitingPadawan;

  // Número de padawans finalizados
  int *finishedPadawan;

  // Controla os padawans querendo entrar na sala
  sem_t *semWaitingPadawan;

  // Controla os padawans querendo sair da sala
  sem_t *semFinishedPadawan;

  // Exclusão mútua para padawans esperando
  pthread_mutex_t *mutexWaitingPadawan;

  // Exclusão mútua para padawans saindo
  pthread_mutex_t *mutexFinishedPadawan;
};

/**
 * @brief Inicializando os padawans.
 *
 * Inicializa as threads dos padawans.
 */
int inicia_padawan(pthread_t *tid, Padawan *padawan);

#endif // PADAWAN_HPP
