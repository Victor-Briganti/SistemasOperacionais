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
 * @brief Dividi o trabalho de calcular linhas entre as threads.
 *
 * Realiza a divisão das partes das linhas da matrix que serão calculadas por
 * cada thread.
 *
 * @param tid vetor das threads a serem criadas
 */
void row_threads(std::vector<pthread_t> &tid, std::ofstream &file) {
  int chunkSize = static_cast<int>(matrix->row_size() / tid.size());
  int excess = static_cast<int>(matrix->row_size() % tid.size());

  int iteration = 0;
  if (matrix->row_size() > tid.size()) {
    iteration = static_cast<int>(tid.size());
  } else {
    iteration = static_cast<int>(matrix->row_size());
  }

  int end, start = 0;
  for (int i = 0; i < iteration; i++) {
    end = start + chunkSize + (i < excess ? 1 : 0);

    Bound *bound = new Bound(start, end);
    int err =
        pthread_create(&(tid[i]), nullptr, rows, static_cast<void *>(bound));
    if (err) {
      printf("Could not create thread %d\n", i);
    }

    start = end;
  }

  std::vector<double> results;
  results.resize(matrix->row_size());
  for (int i = 0; i < iteration; i++) {
    void *res = nullptr;
    pthread_join(tid[i], &res);

    if (res == nullptr) {
      std::cout << "Something went terrible wrong\n";
      continue;
    }

    Bound *bound = static_cast<Bound *>(res);

    for (int i = bound->start, j = 0; i < bound->end; i++, j++) {
      results[i] = bound->values[j];
    }
    delete static_cast<Bound *>(res);
  }

  file << "Linhas:\n";
  file << "[ ";
  for (auto a : results) {
    file << a << " ";
  }
  file << "]\n";
}

/**
 * @brief Dividi o trabalho de calcular coluna entre as threads.
 *
 * Realiza a divisão das partes das colunas da matrix que serão calculadas por
 * cada thread.
 * Se nada de errado tiver acontecido salva as informações no arquivo passado.
 *
 * @param tid vetor das threads a serem criadas
 */
void column_threads(std::vector<pthread_t> &tid, std::ofstream &file) {
  int chunkSize = static_cast<int>(matrix->column_size() / tid.size());
  int excess = static_cast<int>(matrix->column_size() % tid.size());

  int iteration = 0;
  if (matrix->column_size() > tid.size()) {
    iteration = static_cast<int>(tid.size());
  } else {
    iteration = static_cast<int>(matrix->column_size());
  }

  int end, start = 0;
  for (int i = 0; i < iteration; i++) {
    end = start + chunkSize + (i < excess ? 1 : 0);

    Bound *bound = new Bound(start, end);
    int err =
        pthread_create(&(tid[i]), nullptr, columns, static_cast<void *>(bound));
    if (err) {
      printf("Could not create thread %d\n", i);
    }

    start = end;
  }

  std::vector<double> results;
  results.resize(matrix->column_size());
  for (int i = 0; i < iteration; i++) {
    void *res = nullptr;
    pthread_join(tid[i], &res);

    if (res == nullptr) {
      std::cout << "Something went terrible wrong\n";
      continue;
    }

    Bound *bound = static_cast<Bound *>(res);

    for (int i = bound->start, j = 0; i < bound->end; i++, j++) {
      results[i] = bound->values[j];
    }
    delete static_cast<Bound *>(res);
  }

  file << "Coluna:\n";
  file << "[ ";
  for (auto a : results) {
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
  row_threads(tid, file);
  file << "\n\n";
  column_threads(tid, file);

  delete matrix;
  return 0;
}