#ifndef COMMON_HPP
#define COMMON_HPP

#include <pthread.h>

#define ID static_cast<int>(pthread_self() % 1000)

// Definição dos tipos de alunos. Especifica a entrega que ele devera fazer
#define ATIVIDADE_1 1
#define ATIVIDADE_2 2

// Definição dos tipos de pessoa, que estarão na sala de entrega.
#define ALUNO_1 1
#define ALUNO_2 2
#define PROFESSOR 3

/**
 * @brief Local onde ocorre comunicação entre alunos e professor.
 *
 * Está função tem como objetivo representar o momento da entrega entre os
 * alunos e o professor, a sincronização entre as threads irá ocorrer nesta
 * função.
 *
 * @param pessoa Recebe o tipo de pessoa que está na entrega.
 * @param barrier Barreira para sincronização de threads.
 */
void entrega(int pessoa, pthread_barrier_t *barrier);

#endif // COMMON_HPP
