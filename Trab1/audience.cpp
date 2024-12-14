#include "audience.hpp"
#include <cstdio>
#include <cstdlib>
#include <semaphore.h>
#include <unistd.h>

/**
 * @brief Espectador sai do salão
 *
 * Imprime na tela que está saindo.
 *
 * @param idAudience Identificador do espectador
 */
static void sai_salao(int idAudience) {
  std::printf("[Audience %d]: Saindo\n", idAudience);
}

/**
 * @brief Espectador assiste aos testes
 *
 * Imprime na tela o tempo que irá ficar assistindo e então fica assistindo.
 *
 * @param idAudience Identificador do espectador
 */
static void assiste_testes(int idAudience) {
  int timeSleep = std::rand() % 10 + 1;
  std::printf("[Audience %d]: Assistindo %ds\n", idAudience, timeSleep);
  sleep(timeSleep);
}

/**
 * @brief Entra no salão
 *
 * Aumenta a quantidade de pessoas que está esperando no salão e então espera
 * ser liberado.
 *
 * @param arg Ponteiro void para struct Audience
 */
static void *entra_salao(void *arg) {
  Audience *audience = static_cast<Audience *>(arg);

  // Fecha a região para atualizar audiência esperando
  pthread_mutex_lock(audience->mutexAudience);
  audience->waitingAudience++;
  pthread_mutex_unlock(audience->mutexAudience);

  // Espera no semáforo o Yoda liberar a entrada
  sem_wait(audience->semWaitingAudience);

  assiste_testes(audience->id);
  sai_salao(audience->id);

  return nullptr;
}

void inicia_audiencia(std::vector<pthread_t> tid,
                      std::vector<Audience> audience) {
  while (true) {
    for (size_t i = 0; i < tid.size(); i++) {
      if (pthread_create(&tid[i], nullptr, entra_salao, &audience[i]) < 0) {
        std::perror("pthread_create");
      }
    }

    for (size_t i = 0; i < tid.size(); i++) {
      if (pthread_join(tid[i], nullptr) < 0) {
        std::perror("pthread_join");
      }
    }
  }
}
