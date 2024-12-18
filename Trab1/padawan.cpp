//===---- padawan.cpp - Implementação das funções do Padawan --------------===//
//
// Autores: Hendrick Felipe Scheifer e João Victor Briganti de Oliveira
// Data: 15/12/2024
//
//===----------------------------------------------------------------------===//
//
// Implementação das ações que o Padawan irá realizar ao longo da sessão de
// testes
//
//===----------------------------------------------------------------------===//

#include "padawan.hpp"

#include <cstdio> // perror(), printf(), size_t
#include <pthread.h> // pthread_create(), pthread_mutex_lock(), pthread_mutex_unlock()
#include <unistd.h> // usleep()

namespace {

// Contador para identificar Padawans
int countPadawan = 0;

/**
 * @brief Entra no salão
 *
 * Aguarda sua entrada ser liberada e assim que aceito entra no salão e
 * cumprimenta os mestres.
 *
 * @param padawan Estrutura que armazena variavéis globais utilizadas pelo
 * Padawan
 */
void entra_salao(Padawan *padawan) {

  pthread_mutex_lock(padawan->mutex);
  // Incrementa o contador do id
  countPadawan++;

  // Salva o ID do Padawan na fila de espera
  int idPadawan = countPadawan;
  padawan->waitQueue->push_back(idPadawan);

  std::printf("[Padawan %d] aguardando entrada\n", idPadawan);
  pthread_mutex_unlock(padawan->mutex);

  // Aguarda liberação do salão
  sem_wait(padawan->semWait);

  std::printf("[Padawan %d] entrou\n", idPadawan);
  std::printf("[Padawan %d] cumprimenta mestres\n", idPadawan);
  usleep(500);
}

/**
 * @brief Aguarda o resultado e realiza a ação conforme o resultado obtido
 *
 * Aguarda os resultados dos testes. Após liberado verifica se passou ou não e
 * então realiza sua ação.
 * Ao sair do salão verifica se foi é o último Padawan em testes e se for avisa
 * o Yoda.
 *
 * @param padawan Estrutura que armazena variavéis globais utilizadas pelo
 */
void aguarda_resultado(Padawan *padawan) {
  // Espera os resultados serem liberados
  sem_wait(padawan->semResult);

  pthread_mutex_lock(padawan->mutex);
  auto pwd = padawan->resultQueue->front();
  padawan->resultQueue->pop_front();

  if (pwd.second) {
    std::printf("[Padawan %d] aguarda corte trança\n", pwd.first);
    std::printf("[Yoda] corta trança de %d\n", pwd.first);
  } else {
    std::printf("[Padawan %d] cumprimenta Yoda\n", pwd.first);
  }

  std::printf("[Padawan %d] sai do salão\n", pwd.first);

  // Se este é o último padawan na sala sinaliza
  if (padawan->resultQueue->empty() == true) {
    sem_post(padawan->semFinish);
  }
  pthread_mutex_unlock(padawan->mutex);
}

/**
 * @brief Inicializa as ações dos padawans
 *
 * @param arg Ponteiro de void para estrutura Padawan
 *
 * @return nullptr
 */
void *start(void *arg) {
  Padawan *padawan = static_cast<Padawan *>(arg);

  entra_salao(padawan);
  aguarda_resultado(padawan);

  return nullptr;
}

} // namespace

int init_padawan(std::vector<pthread_t *> padawanThreads, Padawan *padawan) {
  for (size_t i = 0; i < padawanThreads.size(); i++) {
    if (pthread_create(padawanThreads[i], nullptr, start, padawan)) {
      std::printf("[Padawan %zu] ", i);
      std::perror("pthread_create");
      return -1;
    }
  }

  return 0;
}
