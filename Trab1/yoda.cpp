//===---- yoda.cpp - Implementação das funções do Yoda --------------------===//
//
// Autores: Hendrick Felipe Scheifer e João Victor Briganti de Oliveira
// Data: 15/12/2024
//
//===----------------------------------------------------------------------===//
//
// Implementação das ações que o Yoda irá realizar ao longo da sessão de
// testes
//
//===----------------------------------------------------------------------===//

#include "yoda.hpp"
#include "common.hpp"

#include <chrono>  // duration_cast(), milliseconds(), steady_clock()
#include <cstdio>  // perror(), printf(), size_t
#include <cstdlib> // rand()
#include <pthread.h> // pthread_create(), pthread_mutex_lock(), pthread_mutex_unlock()

namespace {

size_t testQueueSize = 0;

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

    // Pega o número máximo de padawans que podem entrar no salão
    pthread_mutex_lock(yoda->padawan->mutex);
    while (yoda->padawan->waitQueue->empty() == false &&
           yoda->padawan->testQueue->size() < PADAWAN_MAX_ENTRY) {

      // Remove os padawans da fila de espera e coloca na de teste
      yoda->padawan->testQueue->push_back(yoda->padawan->waitQueue->front());
      yoda->padawan->waitQueue->pop_front();

      // Aumenta a quantidade de padawans na fila de teste
      testQueueSize++;

      // Libera todos os padawans da audiência que estavam esperando na fila
      sem_post(yoda->padawan->semWait);
    }
    pthread_mutex_unlock(yoda->padawan->mutex);

    // Pega o número máximo de pessoas que podem estar dentro do salão
    pthread_mutex_lock(yoda->audience->mutexCount);

    while (*yoda->audience->countWait &&
           *yoda->audience->countInside <= AUDIENCE_MAX_ENTRY) {
      // Diminui a quantidade de pessoas na fila e aumenta a quantidade de
      // pessoas dentro do salão
      (*yoda->audience->countWait)--;
      (*yoda->audience->countInside)++;

      // Libera todas as pessoas da audiência que estavam esperando na fila
      sem_post(yoda->audience->semWait);
    }
    pthread_mutex_unlock(yoda->audience->mutexCount);
  }
}

/**
 * @brief Realiza as avaliações dos Padawans
 *
 * @param yoda Ponteiro para estrutura Yoda
 */
void anuncia_avaliacao(Yoda *yoda) {
  // Espera padawans se posicionarem
  sem_wait(yoda->padawan->semPosition);

  // Libera a audiência para assistir os testes acontecendo
  pthread_mutex_lock(yoda->audience->mutexCount);
  for (int i = 0; i < *yoda->audience->countInside; i++) {
    sem_post(yoda->audience->semTest);
  }
  pthread_mutex_unlock(yoda->audience->mutexCount);

  pthread_mutex_lock(yoda->padawan->mutex);
  std::printf("[Yoda] realiza avaliação\n");

  // Libera os Padawans aguardando o resultado
  while (testQueueSize) {
    testQueueSize--;
    sem_post(yoda->padawan->semTest);
  }
  pthread_mutex_unlock(yoda->padawan->mutex);

  // Aguarda todos os padawans serem avaliados
  sem_wait(yoda->padawan->semTestFinish);
}

/**
 * @brief Anuncia os resultados de cada Padawan
 *
 * @param yoda Ponteiro para estrutura Yoda
 */
void anuncia_resultados(Yoda *yoda) {
  pthread_mutex_lock(yoda->padawan->mutex);
  // Armazena o tamanho total da fila de resultados
  int resultQueueSize = static_cast<int>(yoda->padawan->resultQueue->size());

  // Libera os Padawans aguardando o resultado
  while (resultQueueSize) {
    resultQueueSize--;
    sem_post(yoda->padawan->semResult);
  }
  pthread_mutex_unlock(yoda->padawan->mutex);

  // Aguarda todos os padawans terminarem de ver o resultado
  sem_wait(yoda->padawan->semResultFinish);
}

/**
 * @brief Corta a trança de cada Padawan
 *
 * @param yoda Ponteiro para estrutura Yoda
 */
void corta_tranca(Yoda *yoda) {
  pthread_mutex_lock(yoda->padawan->mutex);
  // Corta a trança dos padawans que passaram
  for (auto a : (*yoda->padawan->cutQueue)) {
    std::printf("[Yoda] corta trança de %d\n", a);
  }

  size_t cutQueueSize = yoda->padawan->cutQueue->size();
  while (cutQueueSize) {
    // Remove o padawan que teve a trança cortada da fila e libera o mesmo para
    // ir embora.
    yoda->padawan->cutQueue->pop_front();
    sem_post(yoda->padawan->semCut);
    cutQueueSize--;
  }

  pthread_mutex_unlock(yoda->padawan->mutex);
  std::printf("[Yoda] testes finalizados\n");
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
    // Verifica que a fila de pessoas esperando está vazia
    pthread_mutex_lock(yoda->padawan->mutex);
    bool waitPadawan = yoda->padawan->waitQueue->empty();
    pthread_mutex_unlock(yoda->padawan->mutex);

    // Não há mais ninguém esperando
    if (waitPadawan) {
      break;
    }

    libera_entrada(yoda);
    anuncia_avaliacao(yoda);
    anuncia_resultados(yoda);
    corta_tranca(yoda);
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
