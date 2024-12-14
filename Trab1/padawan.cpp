#include "padawan.hpp"
#include <cstdio>
#include <cstdlib>

/**
 * @brief Entra no salão
 *
 * Aumenta a quantidade de padawans esperando para entrar no salão, cumprimenta
 * os mestres, realiza os testes e espera para ir embora.
 *
 * @param arg Ponteiro de void para Padawan
 *
 * @return nullptr
 */
static void *entra_salao(void *arg) {
  Padawan *padawan = static_cast<Padawan *>(arg);

  // Fecha a região para atualizar os padawans esperando
  pthread_mutex_lock(padawan->mutexWaitingPadawan);
  padawan->waitingPadawan++;
  pthread_mutex_unlock(padawan->mutexWaitingPadawan);

  // Espera no semáforo o Yoda liberar a entrada
  sem_wait(padawan->semSaloonPadawan);

  std::printf("[Padawan %d]: Cumprimeta mestres\n", padawan->id);
  std::printf("[Padawan %d]: Aguarda avaliação\n", padawan->id);

  // Espera no semáforo o Yoda para o teste
  std::printf("[Padawan %d]: Realiza teste\n", padawan->id);
  sem_wait(padawan->semTestPadawan);

  // Verifica se o Padawan passou ou não
  if (std::rand() % 2) {
    std::printf("[Yoda]: Padawan %d passou\n", padawan->id);
    std::printf("[Padawan %d]: Trança cortada\n", padawan->id);
  } else {
    std::printf("[Yoda]: Padawan %d recusado\n", padawan->id);
    std::printf("[Padawan %d]: Cumprimenta Yoda\n", padawan->id);
  }

  // Espera no semáforo o Yoda para liberar a saida
  sem_wait(padawan->semFinishedPadawan);

  return nullptr;
}

/**
 * @brief Inicializando os padawans.
 *
 * Inicializa as threads dos padawans.
 */
void inicia_padawan(std::vector<pthread_t> tid, std::vector<Padawan> padawan) {
  for (size_t i = 0; i < tid.size(); i++) {
    if (pthread_create(&tid[i], nullptr, entra_salao, &padawan[i]) < 0) {
      std::perror("pthread_create");
    }
  }

  for (size_t i = 0; i < tid.size(); i++) {
    if (pthread_join(tid[i], nullptr) < 0) {
      std::perror("pthread_join");
    }
  }
}
