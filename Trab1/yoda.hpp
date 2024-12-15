#ifndef YODA_HPP
#define YODA_HPP

#include "audience.hpp"
#include "padawan.hpp"

// Estrutura básica do Yoda
struct Yoda {
  // Ponteiro para a estrutura de audiência
  Audience *audience;

  // Ponteiro para a estrutura de padawans
  Padawan *padawan;
};

/**
 * @brief Inicializa a thread do Yoda
 *
 * @param yodaThread Thread que representa o Yoda
 * @param yoda Ponteiro para a estrutura Yoda
 *
 * @return 0 se tudo ocorreu bem, -1 caso contrário
 */
int init_yoda(pthread_t *yodaThread, Yoda *yoda);

#endif // YODA_HPP