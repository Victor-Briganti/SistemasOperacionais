#include "padawan.hpp"

#include <cstdio>
#include <pthread.h>
#include <unistd.h>

namespace {

void entra_salao(Padawan *padawan) {
  pthread_mutex_lock(padawan->mutex);
  // Salva o ID do Padawan na fila de espera
  int idPadawan = static_cast<int>(padawan->waitQueue->size()) + 1;
  padawan->waitQueue->push_back(idPadawan);

  std::printf("[Padawan %d] aguardando entrada\n", idPadawan);
  pthread_mutex_unlock(padawan->mutex);

  // Aguarda liberação do salão
  sem_wait(padawan->semWait);
  std::printf("[Padawan %d] cumprimenta mestres\n", idPadawan);
  usleep(500);
}

/**
 * @brief Aguarda o resultado e realiza a ação conforme o resultado obtido
 *
 * @param padawan Estrutura do padawan
 */
void aguarda_resultado(Padawan *padawan) {
  // Espera os resultados serem liberados
  sem_wait(padawan->semResult);

  pthread_mutex_lock(padawan->mutex);
  auto pwd = padawan->resultQueue->front();
  padawan->resultQueue->pop_front();
  pthread_mutex_unlock(padawan->mutex);

  if (pwd.second) {
    std::printf("[Padawan %d] aguarda corte trança\n", pwd.first);
    std::printf("[Yoda] corta trança de %d\n", pwd.first);
  } else {
    std::printf("[Padawan %d] cumprimenta Yoda\n", pwd.first);
  }

  std::printf("[Padawan %d] sai do salão\n", pwd.first);

  // Se este é o último padawan na sala sinaliza
  pthread_mutex_lock(padawan->mutex);
  if (padawan->resultQueue->empty() == true) {
    sem_post(padawan->semFinish);
  }
  pthread_mutex_unlock(padawan->mutex);
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
  aguarda_resultado(padawan);

  return nullptr;
}

} // namespace

int init_padawan(std::vector<pthread_t *> padawanThreads, Padawan *padawan) {
  for (size_t i = 0; i < padawanThreads.size(); i++) {
    if (pthread_create(padawanThreads[i], nullptr, start, padawan)) {
      std::printf("[Padawan %zu] ", i);
      std::perror("pthread_create");
      return -1;
    }
  }

  return 0;
}
