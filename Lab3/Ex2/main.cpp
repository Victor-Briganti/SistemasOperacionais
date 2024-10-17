/*
 * Calcula média na coluna e no vetor de uma matrix
 * Descrição: Calcula média aritmética das linhas da matrix. Calcula média
 * geométrica das linhas da matrix.
 *
 * Author: Victor Briganti, Luiz Takeda
 * License: BSD 2
 */

#include "matrix.hpp"
#include <cstdlib>
#include <fstream>
#include <iostream>

/**
 * @brief Estrutura de limites da execução
 *
 * Estrutura usada como arg na função pthread_create(). Tem como objetivo
 * delimitar a(s) coluna(s) ou linha(s) que está thread tem responsabilidade de
 * calcular.
 */
struct Bound {
  Bound(int start, int end) : start(start), end(end) {}
  int start;                  ///< Delimita a linha/coluna inicial
  int end;                    ///< Delimita a linha/coluna final
  std::vector<double> values; ///< Armazena os valores calculados
};

/** @brief Estrutura global da matrix */
Matrix *matrix = nullptr;

/**
 * @brief Realiza média aritmética nas linhas da matrix
 *
 * @param arg void* que sofre um cast para Bound*. Armazena as informações das
 * linhas que devem ser calculadas
 *
 * @return estrutura bound com os valores calculados, ou nullptr se algo de
 * errado acontecer.
 */
void *rows(void *arg) {
  if (arg == nullptr) {
    std::cout << "arg is (nil)\n";
    return nullptr;
  }

  Bound *bound = static_cast<Bound *>(arg);

  for (int i = bound->start; i < bound->end; i++) {
    double value = matrix->arithmetic_mean(i);
    if (std::isnan(value)) {
      std::cout << "Value is nan\n";
      delete bound;
      return nullptr;
    }

    bound->values.push_back(value);
  }

  return static_cast<void *>(bound);
}

/**
 * @brief Realiza média geométrica nas colunas da matrix
 *
 * @param arg void* que sofre um cast para Bound*. Armazena as informações das
 * colunas que devem ser calculadas
 *
 * @return estrutura bound com os valores calculados, ou nullptr se algo de
 * errado acontecer.
 */
void *columns(void *arg) {
  if (arg == nullptr) {
    std::cout << "arg is (nil)\n";
    return nullptr;
  }

  Bound *bound = static_cast<Bound *>(arg);

  for (int i = bound->start; i < bound->end; i++) {
    double value = matrix->geometric_mean(i);
    if (std::isnan(value)) {
      std::cout << "Value is nan\n";
      delete bound;
      return nullptr;
    }

    bound->values.push_back(value);
  }

  return static_cast<void *>(bound);
}

/**
 * @brief Dividi o trabalho de entre as threads, e escreve as saídas no arquivo.
 *
 * Dividi o trabalho das threads entre os cálculos de linhas e colunas.
 *
 * @param tid Vetor das threads a serem criadas
 * @param file Arquivo de saída
 */
void thread_manager(std::vector<pthread_t> &tid, std::ofstream &file) {
  int numRows = matrix->row_size();
  int numCols = matrix->column_size();

  int numThreads = tid.size();
  int halfThreads = numThreads / 2;

  int chunkSizeRow = numRows / halfThreads;
  int excessRow = numRows % halfThreads;

  int start = 0;
  for (int i = 0; i < halfThreads; i++) {
    int end = start + chunkSizeRow + (i < excessRow ? 1 : 0);

    Bound *bound = new Bound(start, end);
    int err =
        pthread_create(&(tid[i]), nullptr, rows, static_cast<void *>(bound));
    if (err) {
      std::cerr << "Could not create thread for row " << i << std::endl;
    }

    start = end;
  }

  int chunkSizeCol = numCols / (numThreads - halfThreads);
  int excessCol = numCols % (numThreads - halfThreads);

  start = 0;
  for (int i = halfThreads; i < numThreads; i++) {
    int end = start + chunkSizeCol + (i - halfThreads < excessCol ? 1 : 0);

    Bound *bound = new Bound(start, end);
    int err =
        pthread_create(&(tid[i]), nullptr, columns, static_cast<void *>(bound));
    if (err) {
      std::cerr << "Could not create thread for column " << i << std::endl;
    }

    start = end;
  }

  std::vector<double> rowResults(numRows);
  for (int i = 0; i < halfThreads; i++) {
    void *res = nullptr;
    pthread_join(tid[i], &res);

    if (res == nullptr) {
      std::cerr << "Something went terribly wrong in row thread " << i
                << std::endl;
      continue;
    }

    Bound *bound = static_cast<Bound *>(res);
    for (int j = bound->start, k = 0; j < bound->end; j++, k++) {
      rowResults[j] = bound->values[k];
    }
    delete bound;
  }

  file << "Linhas:\n[ ";
  for (auto a : rowResults) {
    file << a << " ";
  }
  file << "]\n\n";

  std::vector<double> colResults(numCols);
  for (int i = halfThreads; i < numThreads; i++) {
    void *res = nullptr;
    pthread_join(tid[i], &res);

    if (res == nullptr) {
      std::cerr << "Something went terribly wrong in column thread " << i
                << std::endl;
      continue;
    }

    Bound *bound = static_cast<Bound *>(res);
    for (int j = bound->start, k = 0; j < bound->end; j++, k++) {
      colResults[j] = bound->values[k];
    }
    delete bound;
  }

  file << "Colunas:\n[ ";
  for (auto a : colResults) {
    file << a << " ";
  }
  file << "]\n";
}

int main(int argc, char **argv) {
  if (argc < 3) {
    std::cout << argv[0] << " <input_file> <output_file> <num_threads>\n";
    return 1;
  }

  matrix = new Matrix(argv[1]);
  if (!matrix) {
    std::cout << "Could not allocate the matrix\n";
    return 1;
  }

  std::ofstream file(argv[2]);
  if (!file) {
    std::cout << "Could not open the file " << argv[2] << "\n";
    return 1;
  }

  int numThreads = std::atoi(argv[3]);
  if (numThreads <= 0) {
    std::cout << numThreads << " invalid number of threads\n";
    return 1;
  }

  std::vector<pthread_t> tid(numThreads);
  thread_manager(tid, file);

  delete matrix;
  return 0;
}
