#ifndef YODA_HPP
#define YODA_HPP

#include <pthread.h>
#include <semaphore.h>

struct Yoda {
  // Número de padawans esperando
  int *waitingPadawan;

  // Número de padawans finalizados
  int *finishedPadawan;

  // Número de pessoas na audiência
  int *numAudience;

  // Controla os padawans querendo entrar na sala
  sem_t *semWaitingPadawan;

  // Controla os padawans querendo sair da sala
  sem_t *semFinishedPadawan;

  // Controla a audiência entrando na sala
  sem_t *semSalaAudience;

  // Exclusão mútua para padawans esperando
  pthread_mutex_t *mutexWaitingPadawan;

  // Exclusão mútua para padawans saindo
  pthread_mutex_t *mutexFinishedPadawan;

  // Controla o número de pessoas na audiência
  pthread_mutex_t *mutexAudience;
};

/**
 * @brief Inicializando os padawans.
 *
 * Inicializa as threads dos padawans.
 */
int inicia_yoda(pthread_t *tid, Yoda *yoda);

#endif // YODA_HPP
