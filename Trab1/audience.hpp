//===---- audience.hpp - Cabeçalho da Audiência ---------------------------===//
//
// Autor: João Victor Briganti de Oliveira
// Data: 15/12/2024
//
//===----------------------------------------------------------------------===//
//
// Definição da estrutura Audience, e da função que inicializa sua thread.
//
//===----------------------------------------------------------------------===//

#ifndef AUDIENCE_HPP
#define AUDIENCE_HPP

#include <pthread.h>   // phtread_mutex_t
#include <semaphore.h> // sem_t, sem_post(), sem_wait()
#include <vector>      // vector

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

  // Semáforo para sincronizar ao assistir os testes
  sem_t *semTest;
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