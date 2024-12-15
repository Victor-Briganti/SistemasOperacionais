#include "yoda.hpp"
#include "common.hpp"

#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <pthread.h>

namespace {
/**
 * @brief Libera o público e os padawans para entrarem na sala
 *
 * @param yoda Armazena a estrutura do Yoda
 */
void libera_entrada(Yoda *yoda) {
  // Gera um tempo aleatório entre 1 e 500 milissegundos
  int randomTime = rand() % 500 + 1;

  // Registra o tempo de início
  auto start = std::chrono::steady_clock::now();

  // O loop continuará até o tempo aleatório ter passado
  while (true) {
    // Verifica o tempo decorrido
    auto now = std::chrono::steady_clock::now();
    auto elapsed =
        std::chrono::duration_cast<std::chrono::milliseconds>(now - start);

    // Se o tempo aleatório foi atingido, sai do loop
    if (elapsed.count() >= randomTime) {
      break;
    }

    pthread_mutex_lock(yoda->audience->mutexWait);
    int maxAudience = (*yoda->audience->countWait) >= AUDIENCE_MAX_ENTRY
                          ? AUDIENCE_MAX_ENTRY
                          : (*yoda->audience->countWait);
    pthread_mutex_unlock(yoda->audience->mutexWait);

    for (int i = 0; i < maxAudience; i++) {
      // Diminui o número de pessoas esperando e aumenta o número de pessoas
      // dentro da sala.
      pthread_mutex_lock(yoda->audience->mutexWait);
      (*yoda->audience->countWait)--;
      pthread_mutex_unlock(yoda->audience->mutexWait);

      // Libera todas as pessoas da audiência que estavam esperando na fila
      sem_post(yoda->audience->semWait);
    }
  }
}

/**
 * @brief Finaliza as sessões
 *
 * Termina a sessão e manda todos embora.
 *
 * @param arg Ponteiro de void para Yoda
 *
 * @return nullptr
 */
void finaliza_sessao(Yoda *yoda) {
  std::printf("[Yoda] finaliza sessão");

  pthread_mutex_lock(yoda->audience->mutexSessionOver);
  (*yoda->audience->sessionOver) = true;
  pthread_mutex_unlock(yoda->audience->mutexSessionOver);

  pthread_mutex_lock(yoda->audience->mutexWait);
  for (int i = 0; i < *yoda->audience->countWait; i++) {
    // Diminui o número de pessoas esperando e aumenta o número de pessoas
    // dentro da sala.
    (*yoda->audience->countWait)--;

    // Libera todas as pessoas da audiência que estavam esperando na fila
    sem_post(yoda->audience->semWait);
  }
  pthread_mutex_unlock(yoda->audience->mutexWait);
}

/**
 * @brief Inicia as ações do Yoda
 *
 * @param arg Ponteiro de void para Yoda
 *
 * @return nullptr
 */
void *start(void *arg) {
  Yoda *yoda = static_cast<Yoda *>(arg);

  for (int i = 0; i < 2; i++) {
    libera_entrada(yoda);
  }

  finaliza_sessao(yoda);

  return nullptr;
}
} // namespace

int init_yoda(pthread_t *yodaThread, Yoda *yoda) {
  if (pthread_create(yodaThread, nullptr, start, yoda)) {
    std::printf("[Yoda] ");
    std::perror("pthread_create");
    return -1;
  }

  return 0;
}
