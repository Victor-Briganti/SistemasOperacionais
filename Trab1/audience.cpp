//===---- audience.cpp - Implementação das funções da Audiência -----------===//
//
// Autores: Hendrick Felipe Scheifer e João Victor Briganti de Oliveira
// Data: 15/12/2024
//
//===----------------------------------------------------------------------===//
//
// Implementação das ações que a Audiência irá realizar ao longo da sessão de
// testes
//
//===----------------------------------------------------------------------===//

#include "audience.hpp"

#include <cstdio>  // printf(), perror(), size_t
#include <cstdlib> // rand()
#include <pthread.h> // pthread_t, pthread_create(), pthread_mutex_lock(), pthread_mutex_unlock()
#include <unistd.h> // usleep()

namespace {

// Contador do identificador das pessoas na audiência
int audienceCount = 0;

/**
 * @brief Tenta entrar no salão para assistir aos testes
 *
 * Entra na sessão crítica de espera, se possível aumenta a quantidade de
 * pessoas esperando e vai para o Mutex. Se o limite de pessoas no salão
 * estourou vai embora e tenta depois.
 *
 * @param idAudience Identificar da audiência
 * @param Audience Estrutura que armazena informações da audiência
 *
 * @return 0 se conseguiu entrar, -1 caso contrário
 */
int entra_salao(int idAudience, Audience *audience) {
  pthread_mutex_lock(audience->mutexCount);
  // Aumenta quantidade de pessoas na fila
  (*audience->countWait)++;
  std::printf("[Audience %d] aguardando entrada\n", idAudience);
  pthread_mutex_unlock(audience->mutexCount);

  // Espera ser liberado sua entrada
  sem_wait(audience->semWait);
  return 0;
}

/**
 * @brief Tenta entrar no salão para assistir aos testes
 *
 * Entra na sessão crítica de espera, se possível aumenta a quantidade de
 * pessoas esperando e vai para o Mutex. Se o limite de pessoas no salão
 * estourou vai embora e tenta depois.
 *
 * @param idAudience Identificar da audiência
 * @param Audience Estrutura que armazena informações da audiência
 */
void assiste_teste(int idAudience, Audience *audience) {
  std::printf("[Audience %d] entrou no salão\n", idAudience);
  // Espera os testes começarem
  sem_wait(audience->semTest);

  int watchTime = std::rand() % 500 + 1;
  std::printf("[Audience %d] assiste teste por %dms\n", idAudience, watchTime);
  usleep(watchTime);
}

/**
 * @brief Sai do salão após assistir aos testes
 *
 * Entra na sessão crítica e diminui a quantidade de pessoas na sala assistindo
 * aos teste.
 *
 * @param idAudience Identificar da audiência
 */
void sai_salao(int idAudience, Audience *audience) {
  pthread_mutex_lock(audience->mutexCount);
  // Diminui a quantidade de pessoas dentro do salão
  (*audience->countInside)--;
  std::printf("[Audience %d] saindo da sala\n", idAudience);
  pthread_mutex_unlock(audience->mutexCount);
}

/**
 * @brief Inicializa as ações da audiência
 *
 * @param arg Ponteiro de void para estrutura Audience
 *
 * @return nullptr
 */
void *start(void *arg) {
  Audience *audience = static_cast<Audience *>(arg);

  // Fornece um ID para cada pessoa da audiência
  pthread_mutex_lock(audience->mutexCount);
  audienceCount++;
  int idAudience = audienceCount;
  pthread_mutex_unlock(audience->mutexCount);

  // Tenta entrar na sala e assistir aos testes
  // Entra no salão
  entra_salao(idAudience, audience);

  // Verifica se a sessão já terminou.
  pthread_mutex_lock(audience->mutexSessionOver);
  if (*audience->sessionOver) {
    pthread_mutex_unlock(audience->mutexSessionOver);
    return nullptr;
  }
  pthread_mutex_unlock(audience->mutexSessionOver);

  // Assiste ao teste e sai do salão
  assiste_teste(idAudience, audience);
  sai_salao(idAudience, audience);

  return nullptr;
}

} // namespace

int init_audience(std::vector<pthread_t *> audienceThreads,
                  Audience *audience) {
  for (size_t i = 0; i < audienceThreads.size(); i++) {
    if (pthread_create(audienceThreads[i], nullptr, start, audience)) {
      std::printf("[Audience %zu] ", i);
      std::perror("pthread_create");
      return -1;
    }
  }
  return 0;
}
