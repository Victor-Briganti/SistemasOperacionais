#include "professor.hpp"
#include "common.hpp"

#include <cstdio>
#include <cstdlib>
#include <pthread.h>
#include <unistd.h>

/**
 * @brief Recebe as atividades dos alunos
 *
 * Realiza a chamada para a entrega e aguarda os alunos entregarem. Após
 * entregarem decrementa a quantidade de alunos na turma.
 *
 * @param numAlunos Quantidade de alunos na turma
 * @param barrier Barreira para sincronizar com os alunos entregando as tarefas
 */
static void receberAtividade(int numAlunos, pthread_barrier_t *barrier) {
  std::printf("Professor[%d] recebendo atividades\n", ID);

  do {
    entrega(PROFESSOR, barrier);
    numAlunos -= 3;
  } while (numAlunos);
}

/**
 * @brief Imprime que as tarefas foram finalizadas
 */
static void finalizarEntregaAtividades() {
  std::printf("Professor[%d] finalizando entregas\n", ID);
}

/**
 * @brief Thread do professor
 *
 * Usada junto do pthread_create inicializa o professor, e chama suas funções.
 *
 * @param arg Ponteiro void para a turma
 *
 * @return Retorna um nullptr
 */
static void *professor(void *arg) {
  Turma *turma = static_cast<Turma *>(arg);
  if (turma == nullptr) {
    std::printf("Turma vazia(nil).\n");
    return nullptr;
  }

  std::printf("Professor[%d] abrindo a sala\n", ID);
  receberAtividade(turma->numAlunos, turma->barrier);
  finalizarEntregaAtividades();
  return nullptr;
}

int iniciaProfessor(pthread_t *tid, Turma *turma) {
  if (pthread_create(tid, nullptr, professor, turma) < 0) {
    std::perror("pthread_create");
    return -1;
  }

  return 0;
}
