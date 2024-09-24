/*
 * Busca um valor em um vetor
 * Descrição: Realiza a busca em um vetor dividindo a mesma entre vários
 * processos.
 *
 * Author: Victor Briganti, Luiz Takeda
 * License: BSD 2
 */
#include "process_vector.hpp"
#include <cstdio>
#include <cstdlib>
#include <sys/wait.h>
#include <unistd.h>

VectorProcess *vectorProcess;

/**
 * @brief Busca o valor dentro do vetor de processos
 *
 * Busca o valor dentro do wrapper do vetor.
 *
 * @param start Posição inicial da busca
 * @param end Posição final da busca
 * @param num Valor sendo buscado
 */
void search(int start, int end, int num) {
  if (!vectorProcess->search(start, end, num)) {
    exit(1);
  }

  exit(0);
}

/**
 * @brief Dividi a busca em um grande vetor
 *
 * Vetores com tamanho maior que a quantidade de processos, são divididos por
 * meio dessa função.
 *
 * @param numProcess Quantidade de processos a serem criados
 * @param num Valor sendo buscado
 */
void big_vector(int numProcess, int num) {
  int chunk = static_cast<int>(vectorProcess->vector.size()) / numProcess;
  int excess = static_cast<int>(vectorProcess->vector.size()) % numProcess;

  pid_t pid = 0;
  for (int i = 0; i < numProcess; i++) {
    pid = fork();
    if (pid < 0) {
      std::cout << "Process(" << getpid() << "): fail " << pid << "\n";
      return;
    }

    if (pid == 0) {
      int start = i * chunk + std::min(i, excess);
      int end = (i + 1) * chunk + std::min(i + 1, excess);
      search(start, end, num);
    }
  }
}

/**
 * @brief Dividi a busca em um pequeno vetor
 *
 * Vetores com tamanho menor ou igual a quantidade de processos, não precisam
 * ser divididos. Cria processos conforme a quantidade de valores no vetor.
 *
 * @param num Valor sendo buscado
 */
void small_vector(int num) {
  pid_t pid = 0;
  for (int i = 0; i < static_cast<int>(vectorProcess->vector.size()); i++) {
    pid = fork();
    if (pid < 0) {
      printf("Process(%d): fail %d", getpid(), pid);
      return;
    }

    if (pid == 0) {
      search(i, i + 1, num);
    }
  }
}

/**
 * @brief Espera os processos executando
 *
 * Espera os processos executando. Caso o vetor tenha encontrado o valor sendo
 * buscado, mostra o seu id.
 *
 * @param num Valor sendo buscado
 *
 * @return true se encontrado, false caso não encontrado
 */
bool wait_processes(int num) {
  int status = -1;
  bool found = 0;

  pid_t wpid = 0;
  while (wpid >= 0) {
    wpid = wait(&status);
    if (status == 0 && wpid != -1) {
      printf("Process(%d) found %d\n", wpid, num);
      found = true;
    }
  }

  return static_cast<bool>(found);
}

int main(int argc, char **argv) {
  if (argc < 4) {
    printf("%s <num_processes> <vector_size> <num_searched>\n", argv[0]);
    return 1;
  }

  int numProcess = atoi(argv[1]);
  if (numProcess < 0) {
    printf("Número de processos inválido: %d\n", numProcess);
    return 1;
  }

  int vectorSize = atoi(argv[2]);
  if (vectorSize < 0) {
    printf("Tamanho de vetor inválido: %d\n", vectorSize);
    return 1;
  }

  int num = atoi(argv[3]);
  vectorProcess = new VectorProcess(vectorSize);

  if (vectorSize > numProcess) {
    big_vector(numProcess, num);
  } else {
    small_vector(num);
  }

  vectorProcess->print();

  if (!wait_processes(num)) {
    std::cout << "Could not found " << num << std::endl;
  }

  delete vectorProcess;
  return 0;
}