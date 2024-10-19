#include "common.hpp"

#include <cstdio>
#include <pthread.h>

void entrega(int pessoa, pthread_barrier_t *barrier) {
  int res = pthread_barrier_wait(barrier);
  if (res != 0 && res != PTHREAD_BARRIER_SERIAL_THREAD) {
    std::perror("pthread_barrier_t");
    return;
  }

  switch (pessoa) {
  case ALUNO_1:
    std::printf("Aluno[%d] Entregou atividade 1\n", ID);
    break;
  case ALUNO_2:
    std::printf("Aluno[%d] Entregou atividade 2\n", ID);
    break;
  case PROFESSOR:
    std::printf("Professor[%d] Recebeu atividades\n", ID);
    break;
  default:
    std::printf("ERROR: thread[%d] não identificada\n", ID);
  }
}
