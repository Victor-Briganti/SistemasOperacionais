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

  int insideAudience = 0;
  int insidePadawan = 0;

  // O loop continuará até o tempo aleatório ter passado
  while (true) {
    // Verifica o tempo decorrido
    auto now = std::chrono::steady_clock::now();
    auto elapsed =
        std::chrono::duration_cast<std::chrono::milliseconds>(now - start);

    // Se o tempo aleatório foi atingido ou a quantidade de pessoas já atingiu o
    // limite começa testes.
    if ((elapsed.count() >= randomTime) ||
        (insideAudience == AUDIENCE_MAX_ENTRY &&
         insidePadawan == PADAWAN_MAX_ENTRY)) {
      break;
    }

    // Pega o número máximo de pessoas que querem assistir a apresentação
    pthread_mutex_lock(yoda->audience->mutexWait);
    int maxAudience = (*yoda->audience->countWait) >= AUDIENCE_MAX_ENTRY
                          ? AUDIENCE_MAX_ENTRY
                          : (*yoda->audience->countWait);
    insideAudience += maxAudience;
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

    // Pega o número máximo de padawans que querem testara a apresentação
    pthread_mutex_lock(yoda->padawan->mutex);
    int maxPadawan = yoda->padawan->waitQueue->size() >= PADAWAN_MAX_ENTRY
                         ? PADAWAN_MAX_ENTRY
                         : yoda->padawan->waitQueue->size();
    insidePadawan += maxPadawan;
    pthread_mutex_unlock(yoda->padawan->mutex);

    for (int i = 0; i < maxPadawan; i++) {
      // Remove o padawan da fila de espera e os coloca na fila de teste
      pthread_mutex_lock(yoda->padawan->mutex);
      yoda->padawan->testQueue->push_back(yoda->padawan->waitQueue->front());
      yoda->padawan->waitQueue->pop_front();
      pthread_mutex_unlock(yoda->padawan->mutex);

      // Libera todos os padawans da audiência que estavam esperando na fila
      sem_post(yoda->padawan->semWait);
    }
  }
}

/**
 * @brief Realiza as avaliações dos Padawans
 *
 * @param yoda Ponteiro para estrutura Yoda
 */
void avalia(Yoda *yoda) {
  pthread_mutex_lock(yoda->padawan->mutex);
  for (auto a : (*yoda->padawan->testQueue)) {
    std::printf("[Padawan %d] aguarda avaliação\n", a);
  }
  pthread_mutex_unlock(yoda->padawan->mutex);

  pthread_mutex_lock(yoda->padawan->mutex);
  std::printf("[Yoda] realiza avaliação\n");
  for (int i = 0; i < yoda->padawan->testQueue->size(); i++) {
    int idPadawan = yoda->padawan->testQueue->front();
    yoda->padawan->testQueue->pop_front();

    std::printf("[Padawan %d] realiza avaliação\n", idPadawan);

    if (std::rand() % 2) {
      yoda->padawan->resultQueue->push_back({idPadawan, true});
    } else {
      yoda->padawan->resultQueue->push_back({idPadawan, false});
    }
  }
  pthread_mutex_unlock(yoda->padawan->mutex);
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
  std::printf("[Yoda] finaliza sessão\n");

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

  while (true) {
    pthread_mutex_lock(yoda->padawan->mutex);
    if (yoda->padawan->waitQueue->empty() != false) {
      pthread_mutex_unlock(yoda->padawan->mutex);
      break;
    }
    pthread_mutex_unlock(yoda->padawan->mutex);
    libera_entrada(yoda);
    avalia(yoda);
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
