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

    // Se o tempo aleatório foi atingido ou a quantidade de pessoas já atingiu o
    // limite começa testes.
    if (elapsed.count() >= randomTime) {
      break;
    }

    // Pega o número máximo de pessoas que podem estar dentro do salão
    pthread_mutex_lock(yoda->audience->mutexCount);
    while ((*yoda->audience->countWait) &&
           (*yoda->audience->countInside) < AUDIENCE_MAX_ENTRY) {

      // Diminui a quantidade de pessoas na fila e aumenta a quantidade de
      // pessoas dentro do salão
      (*yoda->audience->countWait)--;
      (*yoda->audience->countInside)++;

      // Libera todas as pessoas da audiência que estavam esperando na fila
      sem_post(yoda->audience->semWait);
    }
    pthread_mutex_unlock(yoda->audience->mutexCount);

    // Pega o número máximo de padawans que podem entrar no salão
    pthread_mutex_lock(yoda->padawan->mutex);
    while (yoda->padawan->waitQueue->empty() == false &&
           yoda->padawan->testQueue->size() <= PADAWAN_MAX_ENTRY) {

      // Remove os padawans da fila de espera e coloca na de teste
      yoda->padawan->testQueue->push_back(yoda->padawan->waitQueue->front());
      yoda->padawan->waitQueue->pop_front();

      // Libera todos os padawans da audiência que estavam esperando na fila
      sem_post(yoda->padawan->semWait);
    }
    pthread_mutex_unlock(yoda->padawan->mutex);
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
 * @brief Anuncia os resultados de cada Padawan
 *
 * @param yoda Ponteiro para estrutura Yoda
 */
void anuncia_resultados(Yoda *yoda) {
  pthread_mutex_lock(yoda->padawan->mutex);
  // Armazena o tamanho total da fila de resultados
  int resultQueueSize = yoda->padawan->resultQueue->size();

  // Anuncia os resultados individuais de cada Padawan
  for (auto pwd : (*yoda->padawan->resultQueue)) {
    if (pwd.second) {
      std::printf("[Yoda] %d aceito\n", pwd.first);
    } else {
      std::printf("[Yoda] %d rejeitado\n", pwd.first);
    }
  }

  // Libera os Padawans aguardando o resultado
  while (resultQueueSize) {
    resultQueueSize--;
    sem_post(yoda->padawan->semResult);
  }
  pthread_mutex_unlock(yoda->padawan->mutex);

  // Aguarda todos os padawans terminarem de ver o resultado
  sem_wait(yoda->padawan->semFinish);
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

  // Libera todas as pessoas da audiência que podem estar esperando na fila
  pthread_mutex_lock(yoda->audience->mutexCount);
  for (int i = 0; i < AUDIENCE_NUM; i++) {
    sem_post(yoda->audience->semWait);
  }
  pthread_mutex_unlock(yoda->audience->mutexCount);
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
    anuncia_resultados(yoda);
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
