#include "padawan.hpp"

#include <cstdio>
#include <cstdlib>
#include <pthread.h>
#include <unistd.h>

namespace {

// Contador que identifica os Padawans
int idPadawan = 0;

void entra_salao(Padawan *padawan) {
  pthread_mutex_lock(padawan->mutexWaitPadawan);
  // Atualiza o contador e salva seu valor
  idPadawan++;
  int id = idPadawan;

  padawan->listWaitPadawan->push_back(id);

  std::printf("[Padawan %d] esperando para entrar no salão\n", id);
  pthread_mutex_unlock(padawan->mutexWaitPadawan);

  // Espera ser liberado
  sem_wait(padawan->semWaitPadawan);

  std::printf("[Padawan %d] entrou no salão\n", id);
  std::printf("[Padawan %d] cumprimenta todos os mestres\n", id);
  usleep(500);
}

void realiza_avaliacao(Padawan *padawan) {
  // Aguarda ser liberado
  sem_wait(padawan->semTestPadawan);

  pthread_mutex_lock(padawan->mutexTestPadawan);

  int idPadawan = padawan->listTestPadawan->front();
  padawan->listTestPadawan->pop_front();

  std::printf("[Padawan %d] realiza avaliaçao\n", idPadawan);
  pthread_mutex_unlock(padawan->mutexTestPadawan);

  usleep(500);
}

void aguarda_corte_tranca(int &idPadawan) {
  std::printf("[Padawan %d] aguarda corte tranca\n", idPadawan);
}

void cumprimenta_Yoda(int &idPadawan) {
  std::printf("[Padawan %d] cumprimenta yoda\n", idPadawan);
}

void sai_salao(int &idPadawan) {
  std::printf("[Padawan %d] sai do salão\n", idPadawan);
}

/**
 * @brief Inicia as ações do padawan
 *
 * @param arg Ponteiro void para estrutura Padawan
 *
 * @return nullptr
 */
void *start(void *arg) {
  Padawan *padawan = static_cast<Padawan *>(arg);

  int idPadawan = 0;

  entra_salao(padawan);
  realiza_avaliacao(padawan);
  return nullptr;
}

} // namespace

int init_padawan(std::vector<pthread_t *> padawanThreads,
                 std::vector<Padawan *> padawanList) {
  for (size_t i = 0; i < padawanThreads.size(); i++) {
    if (pthread_create(padawanThreads[i], nullptr, start, padawanList[i])) {
      std::printf("[Padawan %zu] ", i);
      std::perror("pthread_create");
      return -1;
    }
  }

  return 0;
}