/*
 * Calcula média na coluna e no vetor de uma matrix
 * Descrição: Calcula média aritmética das linhas da matrix. Calcula média
 * geométrica das linhas da matrix.
 *
 * Autores: Hendrick Felipe Scheifer, João Victor Briganti, Luiz Takeda
 * Licença: BSD 2
 *
 * Data: 30/10/2024
 */
#include "matrix.hpp"
#include <fstream>   // ifstream, ofstream
#include <iostream>  // cout, cerr
#include <pthread.h> // phtread_join(), phtread_create()

// Arquivo do sistema que delimita a quantidade máxima de threads
#define MAX_THREADS_FILE "/proc/sys/kernel/threads-max"

/** @brief Estrutura global da matrix */
Matrix *matrix = nullptr;

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
 * @brief Cria uma thread para realizar o cálculo de uma matrix, e escreve as
 * saídas no arquivo.
 *
 * Está função realiza a criação de uma única thread para realizar todos os
 * calculos, aqui a thread principal não está sendo considerada.
 *
 * @param tid Vetor da thread a ser criada
 * @param file Arquivo de saída
 */
void thread_single(std::vector<pthread_t> &tid, std::ofstream &file) {
  // Cria os limites das linhas que serão calculados
  Bound *bound = new Bound(0, matrix->row_size());

  // Inicia a thread
  int err =
      pthread_create(&(tid[0]), nullptr, rows, static_cast<void *>(bound));
  if (err) {
    std::cerr << "Could not create thread for row" << std::endl;
    return;
  }

  // Realiza o join da thread e verifica se o seu resultado é válido
  void *res = nullptr;
  pthread_join(tid[0], &res);
  if (res == nullptr) {
    std::cerr << "Something went terribly wrong during the row calculation\n";
    return;
  }

  // Salva os resultados finais
  std::vector<double> results(matrix->row_size());
  for (int i = bound->start; i < bound->end; i++) {
    results[i] = bound->values[i];
  }
  delete bound;

  // Escreve em um arquivo os resultados salvos
  file << "Linhas:\n[ ";
  for (auto a : results) {
    file << a << " ";
  }
  file << "]\n\n";

  // Cria os limites das colunas que serão calculados
  bound = new Bound(0, matrix->column_size());
  err = pthread_create(&(tid[0]), nullptr, columns, static_cast<void *>(bound));
  if (err) {
    std::cerr << "Could not create thread for row" << std::endl;
    return;
  }

  // Realiza o join da thread e verifica se o seu resultado é válido
  pthread_join(tid[0], &res);
  if (res == nullptr) {
    std::cerr
        << "Something went terribly wrong during the column calculation\n";
    return;
  }

  // Salva os resultados finais
  results.resize(matrix->column_size());
  for (int i = bound->start; i < bound->end; i++) {
    results[i] = bound->values[i];
  }
  delete bound;

  // Escreve em um arquivo os resultados salvos
  file << "Colunas:\n[ ";
  for (auto a : results) {
    file << a << " ";
  }
  file << "]\n\n";
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
  // Verifica o tamanho total de linhas e colunas da matriz
  int numRows = matrix->row_size();
  int numCols = matrix->column_size();

  // Verifica o número total de threads
  int numThreads = static_cast<int>(tid.size());

  // Realiza a divisão do número total de threads. Uma parte será usada com
  // linhas e a outra com colunas
  int halfThreads = numThreads / 2;

  // Realiza a divisão dos "pedaços" matriz relacionada as linhas para cada
  // thread
  int chunkSizeRow = numRows / halfThreads;
  int excessRow = numRows % halfThreads;

  // Cria as threads que irão realizar o cálculo das linhas
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

  // Realiza a divisão dos "pedaços" matriz relacionada as linhas para cada
  // thread
  int chunkSizeCol = numCols / (numThreads - halfThreads);
  int excessCol = numCols % (numThreads - halfThreads);

  // Cria as threads que irão realizar o cálculo das colunas
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

  // Realiza o join de todas as threads relacionada as linhas
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

  // Salva os resultados das linhas
  file << "Linhas:\n[ ";
  for (auto a : rowResults) {
    file << a << " ";
  }
  file << "]\n\n";

  // Realiza o join de todas as threads relacionada as colunas
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

  // Salva os resultados das colunas
  file << "Colunas:\n[ ";
  for (auto a : colResults) {
    file << a << " ";
  }
  file << "]\n";
}

/**
 * @brief Verifica o número máximo de threads do sistema.
 *
 * @return Número total de threads que podem ser criadas no sistema
 */
int max_threads() {
  std::ifstream file(MAX_THREADS_FILE);
  std::string max;
  std::getline(file, max);
  return std::atoi(max.c_str());
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
  if (numThreads <= 0 || max_threads() < numThreads) {
    std::cout << numThreads << " invalid number of threads\n";
    return 1;
  }

  std::vector<pthread_t> tid(numThreads);

  // Verifica o número total de threads e realiza a chamada de função conforme a
  // necessidade
  if (numThreads == 1) {
    thread_single(tid, file);
  } else {
    thread_manager(tid, file);
  }

  delete matrix;
  return 0;
}
