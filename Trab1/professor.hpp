#ifndef PROFESSOR_HPP
#define PROFESSOR_HPP

#include <pthread.h>

struct Turma {
  int numAlunos;              // Número de Alunos na turma
  pthread_barrier_t *barrier; // Barreira para esperar os alunos
};

/**
 * @brief Professor inicializando suas maldades.
 *
 * Inicializa a thread do professor.
 */
int iniciaProfessor(pthread_t *tid, Turma *turma);

#endif // PROFESSOR_HPP
