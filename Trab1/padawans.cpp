#include "padawans.hpp"
#include "common.hpp"
#include <cstdio>
#include <cstdlib>
#include <pthread.h>
#include <unistd.h>

namespace {

void sai_salao(int idPadawan) {
  std::printf("[Padawan %d] sai do salão\n", idPadawan);
}

void cumprimenta_Yoda(int idPadawan) {
  std::printf("[Padawan %d] cumprimenta Yoda\n", idPadawan);
}

void aguarda_corte_tranca(Padawan *padawan) {
  std::printf("[Padawan %d] aguarda corte na trança\n", padawan->id);

  while (true) {
    pthread_mutex_lock(padawan->mutexCutPadawan);
    if (padawan->cutPadawanQueue->front() != padawan->id) {
      pthread_mutex_unlock(padawan->mutexCutPadawan);
      sem_wait(padawan->semCutPadawans);
    } else {
      std::printf("[Yoda] cortou trança de %d\n", padawan->id);
      pthread_mutex_unlock(padawan->mutexCutPadawan);
      break;
    }
  }
}

void realiza_avaliacao(Padawan *padawan) {
  std::printf("[Padawan %d] realiza avaliação\n", padawan->id);
  sleep(1);

  while (true) {
    pthread_mutex_lock(padawan->mutexResultPadawan);
    if (padawan->resultPadawanQueue->front().idPadawan != padawan->id) {
      pthread_mutex_unlock(padawan->mutexResultPadawan);
      sem_wait(padawan->semResultPadawans);
    } else {
      int result = padawan->resultPadawanQueue->front().result;

      if (result == APPROVED) {
        std::printf("[Yoda] aprovou %d\n", padawan->id);
        pthread_mutex_unlock(padawan->mutexResultPadawan);
        aguarda_corte_tranca(padawan);
      } else {
        std::printf("[Yoda] reprovou %d\n", padawan->id);
        pthread_mutex_unlock(padawan->mutexResultPadawan);
        cumprimenta_Yoda(padawan->id);
      }

      break;
    }
  }
}

void aguarda_avaliacao(Padawan *padawan) {
  std::printf("[Padawan %d] aguardando avaliação\n", padawan->id);
  while (true) {
    pthread_mutex_lock(padawan->mutexTestPadawan);
    if (padawan->testPadawanQueue->front() != padawan->id) {
      pthread_mutex_unlock(padawan->mutexTestPadawan);
      sem_wait(padawan->semTestPadawans);
    } else {
      pthread_mutex_unlock(padawan->mutexTestPadawan);
      break;
    }
  }
}

void cumprimenta_mestres_avaliadores(int idPadawan) {
  std::printf("[Padawan %d] cumprimenta mestres\n", idPadawan);
}

void entra_salao(Padawan *padawan) {
  pthread_mutex_lock(padawan->mutexWaitPadawan);
  std::printf("[Padawan %d] aguardando na fila\n", padawan->id);
  padawan->waitPadawanQueue->push_back(padawan->id);
  pthread_mutex_unlock(padawan->mutexWaitPadawan);

  sem_wait(padawan->semWaitPadawans);
}

void *start(void *arg) {
  Padawan *padawan = static_cast<Padawan *>(arg);

  cumprimenta_mestres_avaliadores(padawan->id);
  entra_salao(padawan);
  aguarda_avaliacao(padawan);
  realiza_avaliacao(padawan);
  sai_salao(padawan->id);

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
