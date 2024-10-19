#include "aluno.hpp"
#include "common.hpp"

#include <cstdio>
#include <cstdlib>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define RA ID

/**
 * @brief Realiza atividade
 *
 * Imprime que está realizando a atividade e realiza um sleep em torno de 5
 * segundos
 */
static void fazerAtividade(int atividade) {
  int horas = rand() % 5 + 1;
  std::printf("Aluno[%d] fazendo atividade %d em %d.\n", RA, atividade, horas);
  sleep(horas);
}

/**
 * @brief Aguarda a data de entrega da atividade
 *
 * Imprime o tempo que irá aguardar para realizar a atividade e então realiza um
 * sleep em torno de 10 segundos
 */
static void aguardarEntrega() {
  int min = rand() % 10 + 1;
  std::printf("Aluno[%d] aguardando %d.\n", RA, min);
  sleep(min);
}

/**
 * @brief Entra na sala para realizar entrega
 *
 * Realiza um sem_wait, e então imprime que está entrando na sala para realizar
 * uma entrega.
 *
 * @param semaphore Primitivo usado para sincronizar com demais threads que vão
 * entregar atividade
 */
static void entrarSala(sem_t *semaphore) {
  sem_wait(semaphore);
  std::printf("Aluno[%d] entrando na sala.\n", RA);
}

/**
 * @brief Realiza a entrega da atividade
 *
 * Verifica o tipo de aluno, e então realiza a requisição para a função entrega,
 * com o tipo e a barreira necessária.
 *
 * @param atividade Define o tipo de atividade que será entregue
 * @param barrier Barreira para sincronizar threads
 */
static void entregarAtividade(int atividade, pthread_barrier_t *barrier) {
  switch (atividade) {
  case ATIVIDADE_1:
    entrega(ALUNO_1, barrier);
    break;
  case ATIVIDADE_2:
    entrega(ALUNO_2, barrier);
    break;
  default:
    std::printf("Aluno[%d]: Com atividade não reconhecida\n", RA);
  }
}

/**
 * @brief Sai da sala após a entrega
 *
 * Realiza um sem_post, e então imprime que está saindo da sala.
 *
 * @param semaphore Primitivo usado para sincronizar com demais threads que vão
 * entregar atividade
 */
static void sairSala(sem_t *semaphore) {
  sem_post(semaphore);
  std::printf("Aluno[%d] saindo da sala.\n", RA);
}

/**
 * @brief Thread do professor
 *
 * Usada junto do pthread_create inicializa o aluno, e chama suas funções.
 *
 * @param arg Ponteiro void para a aluno
 *
 * @return Retorna um nullptr
 */
void *aluno(void *arg) {
  Aluno *aluno = static_cast<Aluno *>(arg);
  if (aluno == nullptr) {
    std::printf("Não foi possível criar aluno(nil).\n");
    return nullptr;
  }

  fazerAtividade(aluno->tipo);
  aguardarEntrega();
  entrarSala(aluno->semaphore);
  entregarAtividade(aluno->tipo, aluno->barrier);
  sairSala(aluno->semaphore);

  return nullptr;
}

int iniciaAlunosAtividade1(pthread_t *tid, int alunosAtividade, Aluno *alunos) {
  for (int i = 0; i < alunosAtividade; i++) {
    if (pthread_create(&tid[i], nullptr, aluno, &alunos[i]) < 0) {
      std::perror("pthread_create");
      std::printf("Aluno %d com atividade 1 não pode ser criado\n", i);
      return -1;
    }
  }

  return 0;
}

int iniciaAlunosAtividade2(pthread_t *tid, int alunosAtividade, Aluno *alunos) {
  for (int i = 0; i < alunosAtividade; i++) {
    if (pthread_create(&tid[i], nullptr, aluno, &alunos[i]) < 0) {
      std::perror("pthread_create");
      std::printf("Aluno %d com atividade 2 não pode ser criado\n", i);
      return -1;
    }
  }

  return 0;
}
