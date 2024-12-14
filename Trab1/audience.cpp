#include "audience.hpp"
#include <cstdio>
#include <cstdlib>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

namespace {

void sai_salao(int idAudience) {
  std::printf("[Audience %d] Saindo da sala\n", idAudience);
}

void assiste_testes(int idAudience) {
  int watchTime = std::rand() % 10 + 1;
  std::printf("[Audience %d] Assistindo por %ds\n", idAudience, watchTime);
  sleep(watchTime);
}

void entra_salao(Audience *audience) {
  // Aumenta o número de pessoas na audiência querendo ver a sessão
  pthread_mutex_lock(audience->mutexWaitAudience);
  std::printf("[Audience %d] Esperando na fila\n", audience->id);
  (*audience->waitingAudience)++;
  pthread_mutex_unlock(audience->mutexWaitAudience);

  sem_wait(audience->semWaitAudience);
}

void *start(void *arg) {
  Audience *audience = static_cast<Audience *>(arg);

  // Assiste as sessões enquanto elas existirem
  while (*(audience->sessionOver) == false) {
    entra_salao(audience);
    assiste_testes(audience->id);
    sai_salao(audience->id);
  }

  return nullptr;
}

} // namespace

int init_audience(std::vector<pthread_t> tid, std::vector<Audience> audience) {
  for (size_t i = 0; i < tid.size(); i++) {
    if (pthread_create(&tid[i], nullptr, start, &audience[i]) != 0) {
      std::printf("[Audience %zu] ", i);
      std::perror("pthread_create ");
      return -1;
    }
  }

  for (size_t i = 0; i < tid.size(); i++) {
    if (pthread_join(tid[i], nullptr) != 0) {
      std::printf("[Audience %zu] ", i);
      std::perror("pthread_join ");
      return -1;
    }
  }

  return 0;
}
