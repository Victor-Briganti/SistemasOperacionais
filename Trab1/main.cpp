#include "aluno.hpp"
#include "common.hpp"
#include "professor.hpp"

#include <cstdio>
#include <cstdlib>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

// Primitivos de sincronização
static pthread_barrier_t *barrier = nullptr;
static sem_t *semaphoreAtividade1 = nullptr;
static sem_t *semaphoreAtividade2 = nullptr;

// Threads
static pthread_t *threadProfessor = nullptr;
static pthread_t *threadsAluno1 = nullptr;
static pthread_t *threadsAluno2 = nullptr;

// Estruturas de Dados
static Turma *turma = nullptr;
static Aluno *alunos1 = nullptr;
static Aluno *alunos2 = nullptr;

/**
 * @brief Aloca recursos necessários para aplicação.
 *
 * Inicializa cada uma das estruturas que serão usadas durante a aplicação.
 *
 * @param numAlunos1 Quantidade de alunos para a primeira atividade
 * @param numAlunos2 Quantidade de alunos para a segunda atividade
 *
 * @return 0 se tudo ocorreu bem, -1 caso contrário.
 */
static int allocResources(int numAlunos1, int numAlunos2) {
  threadsAluno1 = new pthread_t[numAlunos1];
  if (threadsAluno1 == nullptr) {
    goto alunos1;
  }

  threadsAluno2 = new pthread_t[numAlunos2];
  if (threadsAluno2 == nullptr) {
    goto alunos2;
  }

  threadProfessor = new pthread_t;
  if (threadProfessor == nullptr) {
    goto professor;
  }

  barrier = new pthread_barrier_t;
  if (barrier == nullptr || pthread_barrier_init(barrier, nullptr, 4) < 0) {
    goto barrier;
  }

  semaphoreAtividade1 = new sem_t;
  if (semaphoreAtividade1 == nullptr ||
      sem_init(semaphoreAtividade1, 0, 2) < 0) {
    goto atividade1;
  }

  semaphoreAtividade2 = new sem_t;
  if (semaphoreAtividade2 == nullptr ||
      sem_init(semaphoreAtividade2, 0, 1) < 0) {
    goto atividade2;
  }

  alunos1 = new Aluno[numAlunos1];
  if (semaphoreAtividade2 == nullptr) {
    goto alunos1;
  }

  alunos2 = new Aluno[numAlunos2];
  if (semaphoreAtividade2 == nullptr) {
    goto alunos2;
  }

  turma = new Turma;
  if (turma == nullptr) {
    goto turma;
  }

  return 0;

turma:
  delete turma;
alunos2:
  delete[] alunos2;
alunos1:
  delete[] alunos1;
atividade2:
  sem_destroy(semaphoreAtividade2);
  delete semaphoreAtividade2;
atividade1:
  sem_destroy(semaphoreAtividade1);
  delete semaphoreAtividade1;
barrier:
  pthread_barrier_destroy(barrier);
  delete barrier;
professor:
  delete threadProfessor;
threads2:
  delete[] threadsAluno2;
threads1:
  delete[] threadsAluno1;
  return -1;
}

/**
 * @brief Inicializa a thread do professor
 *
 * @param numAlunos Quantidade de alunos na turma
 *
 * @return 0 se tudo ocorreu bem, -1 caso contrário
 */
static int criaThreadProfessor(int numAlunos) {
  turma->numAlunos = numAlunos;
  turma->barrier = barrier;

  if (iniciaProfessor(threadProfessor, turma) < 0) {
    return -1;
  }

  return 0;
}

/**
 * @brief Inicializa as threads dos alunos
 *
 * @param numAlunos Quantidade de alunos que irão realizar a primeira atividade
 *
 * @return 0 se tudo ocorreu bem, -1 caso contrário
 */
static int criaThreadAlunos1(int numAlunos) {
  for (int i = 0; i < numAlunos; i++) {
    alunos1[i].tipo = ATIVIDADE_1;
    alunos1[i].barrier = barrier;
    alunos1[i].semaphore = semaphoreAtividade1;
  }

  if (iniciaAlunosAtividade1(threadsAluno1, numAlunos, alunos1) < 0) {
    return -1;
  }

  return 0;
}

/**
 * @brief Inicializa as threads dos alunos
 *
 * @param numAlunos Quantidade de alunos que irão realizar a segunda atividade
 *
 * @return 0 se tudo ocorreu bem, -1 caso contrário
 */
static int criaThreadAlunos2(int numAlunos) {
  for (int i = 0; i < numAlunos; i++) {
    alunos2[i].tipo = ATIVIDADE_2;
    alunos2[i].barrier = barrier;
    alunos2[i].semaphore = semaphoreAtividade2;
  }

  if (iniciaAlunosAtividade2(threadsAluno2, numAlunos, alunos2) < 0) {
    return -1;
  }

  return 0;
}

/**
 * @brief Realiza um join entre todas as threads do sistema
 *
 * @param numAlunos1 Quantidade de alunos com atividade 1
 * @param numAlunos1 Quantidade de alunos com atividade 2
 *
 * @return 0 se tudo ocorreu bem, -1 caso contrário
 */
static int threadJoin(int numAlunos1, int numAlunos2) {
  for (int i = 0; i < numAlunos1; i++) {
    if (pthread_join(threadsAluno1[i], nullptr)) {
      std::printf("[Alunos1] ");
      std::perror("pthread_join");
      return -1;
    }
  }

  for (int i = 0; i < numAlunos2; i++) {
    if (pthread_join(threadsAluno2[i], nullptr)) {
      std::printf("[Alunos2] ");
      std::perror("pthread_join");
      return -1;
    }
  }

  if (pthread_join(*threadProfessor, nullptr)) {
    std::printf("[Professor] ");
    std::perror("pthread_join");
    return -1;
  }

  return 0;
}

int main(int argc, char *argv[]) {
  if (argc < 2) {
    std::printf("%s <num_alunos>", argv[0]);
    return 1;
  }

  int numAlunos = std::atoi(argv[1]);

  if (numAlunos % 3 != 0) {
    std::printf("O número de alunos deve ser múltiplo de 3.\nExemplo: 9\n");
    return 1;
  }

  int numAlunos2 = numAlunos / 3;
  int numAlunos1 = numAlunos2 * 2;

  srand(static_cast<unsigned int>(time(NULL)));

  // Define a variável de retorno
  int res = EXIT_SUCCESS;

  // Aloca as threas que serão usadas
  if (allocResources(numAlunos1, numAlunos2) < 0) {
    res = EXIT_FAILURE;
    goto alloc;
  }

  if (criaThreadProfessor(numAlunos) < 0) {
    res = EXIT_FAILURE;
    goto error;
  }

  if (criaThreadAlunos1(numAlunos1) < 0) {
    res = EXIT_FAILURE;
    goto error;
  }

  if (criaThreadAlunos2(numAlunos2) < 0) {
    res = EXIT_FAILURE;
    goto error;
  }

  if (threadJoin(numAlunos1, numAlunos2) < 0) {
    res = EXIT_FAILURE;
  }

error:
  // Destroi primitivos de sincronização
  pthread_barrier_destroy(barrier);
  sem_destroy(semaphoreAtividade1);
  sem_destroy(semaphoreAtividade2);

  // Libera Memória de primitivos de sincronização
  delete barrier;
  delete semaphoreAtividade1;
  delete semaphoreAtividade2;

  // Libera Memória de threads alocadas
  delete threadProfessor;
  delete[] threadsAluno1;
  delete[] threadsAluno2;

  // Libera Memória de estrutura de dados
  delete turma;
  delete[] alunos1;
  delete[] alunos2;
alloc:
  return res;
}
