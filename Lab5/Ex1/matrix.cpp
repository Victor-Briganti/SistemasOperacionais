/*
 * Realiza a soma das linhas de uma matriz
 * Descrição: Processa uma matriz realizando a soma de todos seus elementos.
 * Cada thread lê uma linha realiza a soma e armazena o resultado em uma
 * váriavel global
 *
 * Author: Victor Briganti, Luiz Takeda
 * License: BSD 2
 */
#include "matrix.hpp"
#include <pthread.h>
#include <vector>

#define THREAD_NUM 5

#define ROWS 20
#define COLUMNS 20

/** @brief Estrutura global de matriz  */
Matrix matrix(ROWS, COLUMNS);

/**
 * @brief Acumulador global atomico.
 *
 * Está variável precisa ser atomica para que todas as threads modifique a mesma
 * sem a necessidade de um mutex. Suas modificações são atomicas.
 */
int count = 0;

/**
 * @brief Vetor de linhas já calculadas.
 *
 * Vetor de booleanos que armazena quais os valores já foram calculados.
 */
std::vector<bool> indexes(ROWS);

/** @brief Mutex para acesso da global count */
pthread_mutex_t countMutex;

/** @brief Mutex para acesso das linhas da matrix */
pthread_mutex_t rowMutex;

/**
 * @brief Inicializa vetor para acesso das linhas da matriz
 */
void initIndexes() {
  for (int i = 0; i < ROWS; i++) {
    indexes[i] = false;
  }
}

/**
 * @brief Acessa uma linha a ser calculada da matriz
 *
 * @return Linha que deve ser calculada, ou um vetor vazio
 */
std::vector<int> getRow() {
  pthread_mutex_lock(&rowMutex);

  std::vector<int> res = {};
  for (int i = 0; i < ROWS; i++) {
    if (!indexes[i]) {
      res = matrix.get_row(i);
      indexes[i] = true;
      break;
    }
  }

  pthread_mutex_unlock(&rowMutex);
  return res;
}

/**
 * @brief Soma uma linha da matriz e salva no count global
 *
 * @param _arg Variável não utilizada
 *
 * @return Retorna sempre um nullptr
 */
void *accumulator(void *_arg) {
  while (true) {
    std::vector<int> vec = getRow();
    if (vec.empty()) {
      break;
    }

    int acc = 0;

    for (int value : vec) {
      acc += value;
    }

    pthread_mutex_lock(&countMutex);
    count += acc;
    pthread_mutex_unlock(&countMutex);
  }

  return nullptr;
}

int main() {
  std::vector<pthread_t> tid;
  tid.resize(THREAD_NUM);

  initIndexes();

  pthread_mutex_init(&countMutex, nullptr);
  pthread_mutex_init(&rowMutex, nullptr);

  for (int i = 0; i < THREAD_NUM; i++) {
    int res = pthread_create(&(tid[i]), nullptr, accumulator, nullptr);
    if (res < 0) {
      std::cout << "Could not create thread\n";
    }
  }

  for (int i = 0; i < THREAD_NUM; i++) {
    pthread_join(tid[i], nullptr);
  }

  std::cout << count << std::endl;

  return 0;
}
