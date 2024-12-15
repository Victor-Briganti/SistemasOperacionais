#include "yoda.hpp"
#include "common.hpp"

#include <cstdio>
#include <ctime>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

namespace {

int numPadawans = PADAWAN_NUM;

/**
 * @brief Libera a entrada das pessoas na sala
 *
 * @param arg Ponteiro void para estrutura Yoda
 */
void libera_entrada(Yoda *yoda) {
  std::printf("[Yoda] libera entrada\n");
  time_t tmpTime = time(nullptr);

  // Liber as pessoas conforme o tempo especificado
  while (time(nullptr) - tmpTime < YODA_ENTRY_TIME) {

    // Região crítica de acesso ao salão pela audiência
    pthread_mutex_lock(yoda->mutexWaitAudience);
    if ((*yoda->waitAudience)) {
      (*yoda->waitAudience)--;
      sem_post(yoda->semWaitAudience);
    }
    pthread_mutex_unlock(yoda->mutexWaitAudience);

    // Região crítica de acesso ao salão pelos padawans
    pthread_mutex_lock(yoda->mutexWaitPadawan);
    pthread_mutex_lock(yoda->mutexTestPadawan);
    if (yoda->listWaitPadawan->empty() == false) {
      numPadawans--;

      // Passa os padawans da fila de espera para a fila de testes
      yoda->listTestPadawan->push_back(yoda->listWaitPadawan->front());
      yoda->listWaitPadawan->pop_front();

      sem_post(yoda->semWaitPadawan);
    }
    pthread_mutex_unlock(yoda->mutexTestPadawan);
    pthread_mutex_unlock(yoda->mutexWaitPadawan);
  }
}

/**
 * @brief Inicializa os testes
 */
void inicia_testes(Yoda *yoda) {
  std::printf("[Yoda] inicia testes\n");

  pthread_mutex_lock(yoda->mutexTestPadawan);

  for (auto padawan : (*yoda->listTestPadawan)) {
    std::printf("[Padawan %d] aguardando\n", padawan);
    sem_post(yoda->semTestPadawan);
  }

  pthread_mutex_unlock(yoda->mutexTestPadawan);
}

/**
 * @brief Anuncia quais foram os resultados dos testes
 */
void anuncia_resultado() { std::printf("[Yoda] anuncia resultados\n"); }

/**
 * @brief Corta a trança de quem foi aprovado
 */
void corta_tranca() { std::printf("[Yoda] corta tranças\n"); }

/**
 * @brief Finaliza todos os testes
 */
void finaliza_testes() { std::printf("[Yoda] finaliza testes\n"); }

/**
 * @brief Realiza o discurso que finaliza a sessão
 */
void discurso() { std::printf("[Yoda] faz discurso\n"); }

/**
 * @brief Inicia as ações do Yoda
 *
 * @param arg Ponteiro void para estrutura Yoda
 *
 * @return nullptr
 */
void *start(void *arg) {
  Yoda *yoda = static_cast<Yoda *>(arg);

  while (numPadawans) {
    libera_entrada(yoda);
    inicia_testes(yoda);
    anuncia_resultado();
    corta_tranca();
    finaliza_testes();
  }

  pthread_mutex_lock(yoda->mutexSessionOver);
  (*yoda->sessionOver) = true;
  pthread_mutex_unlock(yoda->mutexSessionOver);

  discurso();

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
