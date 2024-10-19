#ifndef ALUNO_HPP
#define ALUNO_HPP

#include <pthread.h>
#include <semaphore.h>

struct Aluno {
  int tipo;                   // Define a atividade que o aluno está realizando
  sem_t *semaphore;           // Semaforo para definir quando poderá entrar
  pthread_barrier_t *barrier; // Primitiva de sincronização
};

/**
 * @brief Pequeno aviso para que os alunos começem a fazer a atividade1.
 *
 * Inicializa as threads dos alunos e especifica quais as atividades que deverão
 * ser feitas por cada um deles.
 *
 * @param tid Ponteiro para thread
 * @param alunosAtividade1 Especifica a quantidade de alunos realizando a
 * atividade1
 * @param alunos Estrutura que armazena informações de sincronização dos alunos
 */
int iniciaAlunosAtividade1(pthread_t *tid, int alunosAtividade, Aluno *alunos);

/**
 * @brief Pequeno aviso para que os alunos começem a fazer a atividade2.
 *
 * Inicializa as threads dos alunos e especifica quais as atividades que deverão
 * ser feitas por cada um deles.
 *
 * @param tid Ponteiro para thread
 * @param alunosAtividade2 Especifica a quantidade de alunos realizando a
 * atividade2
 * @param alunos Estrutura que armazena informações de sincronização dos alunos
 */
int iniciaAlunosAtividade2(pthread_t *tid, int alunosAtividade, Aluno *alunos);

#endif // ALUNO_HPP
