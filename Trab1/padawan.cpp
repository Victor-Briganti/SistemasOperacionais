#include "padawan.hpp"

#include <cstdio>
#include <pthread.h>
#include <unistd.h>

namespace {

void entra_salao(Padawan *padawan) {
  pthread_mutex_lock(padawan->mutex);
  // Salva o ID do Padawan na fila de espera
  int idPadawan = padawan->waitQueue->size() + 1;
  padawan->waitQueue->push(idPadawan);

  std::printf("[Padawan %d] aguardando entrada\n", idPadawan);
  pthread_mutex_unlock(padawan->mutex);

  // Aguarda liberação do salão
  sem_wait(padawan->semWait);
  std::printf("[Padawan %d] cumprimenta mestres\n", idPadawan);
  usleep(500);
}

/**
 * @brief Inicializa as ações dos padawans
 *
 * @param arg Ponteiro de void para estrutura Padawan
 *
 * @return nullptr
 */
void *start(void *arg) {
  Padawan *padawan = static_cast<Padawan *>(arg);

  entra_salao(padawan);

  return nullptr;
}

} // namespace

int init_padawan(std::vector<pthread_t *> padawanThreads, Padawan *padawan) {
  for (int i = 0; i < padawanThreads.size(); i++) {
    if (pthread_create(padawanThreads[i], nullptr, start, padawan)) {
      std::printf("[Padawan %d] ", i);
      std::perror("pthread_create");
      return -1;
    }
  }

  return 0;
}
