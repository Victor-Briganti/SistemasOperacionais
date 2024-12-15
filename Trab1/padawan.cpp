#include "padawan.hpp"

#include <cstdio>
#include <pthread.h>

namespace {

void entra_salao(int &idPadawan, Padawan *padawan) {
  pthread_mutex_lock(padawan->mutexWaitPadawan);
  // Salva o id do padawan enquanto ele espera
  idPadawan = padawan->listWaitPadawan->size();
  std::printf("[Padawan %d] esperando para entrar no salão\n", idPadawan);
  padawan->listWaitPadawan->push_back(padawan->listWaitPadawan->size());
  pthread_mutex_unlock(padawan->mutexWaitPadawan);

  sem_wait(padawan->semWaitPadawan);
  std::printf("[Padawan %d] entrou no salão\n", idPadawan);
}

void cumprimenta_mestres_avaliadores(int &idPadawan) {
  std::printf("[Padawan %d] cumprimenta todos os mestres\n", idPadawan);
}

void aguarda_avaliacao(int &idPadawan) {
  std::printf("[Padawan %d] aguarda avaliaçao\n", idPadawan);
}

void realiza_avaliacao(int &idPadawan) {
  std::printf("[Padawan %d] realiza avaliacao\n", idPadawan);
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

  entra_salao(idPadawan, padawan);
  cumprimenta_mestres_avaliadores(idPadawan);
  aguarda_avaliacao(idPadawan);
  realiza_avaliacao(idPadawan);
  aguarda_corte_tranca(idPadawan);
  cumprimenta_Yoda(idPadawan);
  sai_salao(idPadawan);
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