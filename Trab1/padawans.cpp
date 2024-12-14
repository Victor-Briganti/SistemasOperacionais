#include "padawans.hpp"
#include <cstdio>
#include <cstdlib>
#include <unistd.h>

namespace {

void sai_salao(Padawan *padawan) {
  pthread_mutex_lock(padawan->mutexFinishPadawan);
  std::printf("[Padawan %d] Saindo\n", padawan->id);
  (*padawan->finishPadawan)++;
  pthread_mutex_unlock(padawan->mutexFinishPadawan);

  sem_wait(padawan->semFinishPadawans);
}

void cumprimenta_Yoda(int idPadawan) {
  std::printf("[Padawan %d] Cumprimentou Yoda\n", idPadawan);
}

void aguarda_corte_tranca(int idPadawan) {
  std::printf("[Yoda] Cortou a trança de %d\n", idPadawan);
  std::printf("[Padawan %d] Teve a trança cortada\n", idPadawan);
}

void realiza_avaliacao(int idPadawan) {
  std::printf("[Padawan %d] Realizando avaliação\n", idPadawan);
  if (std::rand() % 2) {
    std::printf("[Yoda] %d aprovado\n", idPadawan);
    aguarda_corte_tranca(idPadawan);
  } else {
    std::printf("[Yoda] %d rejeitado\n", idPadawan);
    cumprimenta_Yoda(idPadawan);
  }
}

void aguarda_avaliacao(Padawan *padawan) {
  // Aumenta o número de padawans esperando para realizar um teste
  pthread_mutex_lock(padawan->mutexTestPadawan);
  std::printf("[Padawan %d] Aguarda avaliação\n", padawan->id);
  (*padawan->testPadawan)++;
  pthread_mutex_unlock(padawan->mutexTestPadawan);

  sem_wait(padawan->semTestPadawans);
  realiza_avaliacao(padawan->id);
}

void cumprimenta_mestres_avaliadores(int idPadawan) {
  std::printf("[Padawan %d] Cumprimenta mestres\n", idPadawan);
  sleep(1);
}

void entra_salao(Padawan *padawan) {
  // Aumenta o número de padawans esperando na fila
  pthread_mutex_lock(padawan->mutexWaitPadawan);
  std::printf("[Padawan %d] Esperando na fila\n", padawan->id);
  (*padawan->waitPadawan)++;
  pthread_mutex_unlock(padawan->mutexWaitPadawan);

  // Espera na fila para ser liberado
  sem_wait(padawan->semWaitPadawans);
}

void *start(void *arg) {
  Padawan *padawan = static_cast<Padawan *>(arg);

  // Entra na fila para entrar no salão
  entra_salao(padawan);

  // Cumprimenta todos os mestres avaliadores
  cumprimenta_mestres_avaliadores(padawan->id);

  // Espera Yoda liberar a avaliação
  aguarda_avaliacao(padawan);

  // Sai do salão
  sai_salao(padawan);

  return nullptr;
}

} // namespace

int init_padawan(std::vector<pthread_t> tid, std::vector<Padawan> padawan) {
  for (size_t i = 0; i < tid.size(); i++) {
    if (pthread_create(&tid[i], nullptr, start, &padawan[i]) != 0) {
      std::printf("[Padawan %zu] ", i);
      std::perror("pthread_create ");
      return -1;
    }
  }

  for (size_t i = 0; i < tid.size(); i++) {
    if (pthread_join(tid[i], nullptr) != 0) {
      std::printf("[Padawan %zu] ", i);
      std::perror("pthread_join ");
      return -1;
    }
  }

  return 0;
}
