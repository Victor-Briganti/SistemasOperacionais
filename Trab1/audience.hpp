#ifndef AUDIENCE_HPP
#define AUDIENCE_HPP

#include <pthread.h>
#include <vector>

// Estrutura inicial da audiência
struct Audience {
  // Identificador da audiência
  int id;
};

/**
 * @brief Inicializa a platéia que vai assistir ao evento
 *
 * @param audienceThreads Threads que representam cada pessoa na plateia
 * @param audienceList Vetor com estruturas que armazenam informações da plateia
 *
 * @return 0 se tudo ocorreu bem, -1 se algo deu errado.
 */
int init_audience(std::vector<pthread_t *> audienceThreads,
                  std::vector<Audience *> audienceList);

#endif // AUDIENCE_HPP