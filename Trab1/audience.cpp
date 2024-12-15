#include "audience.hpp"
#include "common.hpp"

#include <cstdio>
#include <cstdlib>
#include <pthread.h>
#include <unistd.h>

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
 */
void entra_salao(int idAudience, Audience *audience) {
  pthread_mutex_lock(audience->mutexWaitAudience);
  if ((*audience->waitAudience) == AUDIENCE_MAX_ENTRY) {
    // Salão está cheio tenta entrar na fila depois
    pthread_mutex_unlock(audience->mutexWaitAudience);
    return;
  } else {
    // Aumenta quantidade de pessoas na fila
    (*audience->waitAudience)++;
    std::printf("[Audience %d] aguardando entrada\n", idAudience);
    pthread_mutex_unlock(audience->mutexWaitAudience);

    // Espera ser liberado sua entrada
    sem_post(audience->semWaitAudience);
  }
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
void assiste_teste(int idAudience) {
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
void sai_salao(int idAudience) {
  std::printf("[Audience %d] saindo da sala\n", idAudience);
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
  pthread_mutex_lock(audience->mutexWaitAudience);
  audienceCount++;
  int idAudience = audienceCount;
  pthread_mutex_unlock(audience->mutexWaitAudience);

  // Tenta entrar na sala e assistir aos testes
  while (true) {
    // Verifica se a sessão já terminou.
    pthread_mutex_lock(audience->mutexSessionOver);
    if (*audience->sessionOver) {
      pthread_mutex_unlock(audience->mutexSessionOver);
      break;
    }
    pthread_mutex_unlock(audience->mutexSessionOver);

    entra_salao(idAudience, audience);
    assiste_teste(idAudience);
    sai_salao(idAudience);
  }

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
