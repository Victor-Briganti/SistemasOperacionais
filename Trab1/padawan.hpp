#ifndef PADAWAN_HPP
#define PADAWAN_HPP

#include <pthread.h>
#include <semaphore.h>
#include <vector>

struct Padawan {
  // Identificador do padawan
  int id;

  // Número de padawans esperando
  int *waitingPadawan;

  // Número de padawans finalizados
  int *finishedPadawan;

  // Controla os padawans querendo entrar na sala
  sem_t *semSaloonPadawan;

  // Controla os padawans querendo sair da sala
  sem_t *semFinishedPadawan;

  // Controla os padawans querendo realizar o teste
  sem_t *semTestPadawan;

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
void inicia_padawan(std::vector<pthread_t> tid, std::vector<Padawan> padawan);

#endif // PADAWAN_HPP
