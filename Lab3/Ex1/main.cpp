/*
 * Busca um valor em um vetor
 * Descrição: Localiza um valor em um vetor usando N threads.
 *
 * Author: Victor Briganti, Luiz Takeda
 * License: BSD 2
 */
#include "process_vector.hpp"
#include <cstdlib>
#include <iostream>
#include <limits.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <vector>

/**
 * @brief Estrutura de limites da busca
 *
 * Estrutura usada como arg na função pthread_create(). Tem como objetivo
 * delimitar o espaço de busca dentro do vetor e passar o valor sendo buscado.
 */
struct Bound {
  Bound(int start, int end, int num) : start(start), end(end), num(num) {}
  int start;
  int end;
  int num;
};

VectorProcess *vectorProcess;

/**
 * @brief Busca um valor dentro do vectorProcess
 *
 * A partir de valores passados pela estrutura Bound, realiza a busca de um
 * valor dentro do vectorProcess.
 *
 * @param arg void* que vai passar de um cast para Bound*
 *
 * @return nullptr se nada foi encontrado, um inteiro alocado caso contrário.
 */
void *search(void *arg) {
  void *res = nullptr;
  Bound *bound = static_cast<Bound *>(arg);

  if (vectorProcess->search(bound->start, bound->end, bound->num)) {
    res = static_cast<void *>(new int(1));
  }

  delete bound;
  return res;
}

/**
 * @brief Dividi a busca em um grande vetor
 *
 * Vetores com tamanho maior que a quantidade de threads, são divididos por
 * meio dessa função.
 *
 * @param tid Vetor das threads alocadas
 * @param num Valor sendo buscado
 *
 * @return true se encontrado, false caso contrário.
 */
bool big_vector(std::vector<pthread_t> &tid, int num) {
  bool found = false;
  int chunkSize = static_cast<int>(vectorProcess->vector.size() / tid.size());
  int excess = static_cast<int>(vectorProcess->vector.size() % tid.size());

  int end, start = 0;
  for (int i = 0; i < static_cast<int>(tid.size()); i++) {
    end = start + chunkSize + (i < excess ? 1 : 0);

    Bound *bound = new Bound(start, end, num);
    int err =
        pthread_create(&(tid[i]), nullptr, search, static_cast<void *>(bound));
    if (err) {
      printf("Could not create thread %d\n", i);
    }

    start = end;
  }

  for (int i = 0; i < static_cast<int>(tid.size()); i++) {
    void *x = nullptr;
    pthread_join(tid[i], &x);

    if (x != nullptr) {
      std::cout << "Found\n";
      found = true;

      delete static_cast<int *>(x);
      x = nullptr;
    }
  }

  return found;
}

/**
 * @brief Dividi a busca em um pequeno vetor
 *
 * Vetores com tamanho menor que a quantidade de threads, são divididos por
 * meio dessa função.
 *
 * @param tid Vetor das threads alocadas
 * @param num Valor sendo buscado
 *
 * @return true se encontrado, false caso contrário
 */
bool small_vector(std::vector<pthread_t> &tid, int num) {
  bool found = false;

  for (int i = 0; i < static_cast<int>(vectorProcess->vector.size()); i++) {
    Bound *bound = new Bound(i, i + 1, num);
    int err =
        pthread_create(&(tid[i]), nullptr, search, static_cast<void *>(bound));
    if (err) {
      printf("Could not create thread %d\n", i);
    }
  }

  for (int i = 0; i < static_cast<int>(vectorProcess->vector.size()); i++) {
    void *x = nullptr;
    pthread_join(tid[i], &x);

    if (x != nullptr) {
      std::cout << "Found\n";
      found = true;

      delete static_cast<int *>(x);
      x = nullptr;
    }
  }

  return found;
}

int main(int argc, char **argv) {
  if (argc < 3) {
    std::cout << argv[0] << " <num_threads> <vector_size> <num> \n";
    exit(1);
  }

  int num_threads = std::atoi(argv[1]);
  if (num_threads <= 0) {
    std::cout << num_threads << " invalid number of threads\n";
    exit(1);
  }

  int vector_size = std::atoi(argv[2]);
  if (vector_size < 0) {
    std::cout << vector_size << " invalid size vector\n";
    exit(1);
  }
  vectorProcess = new VectorProcess(vector_size);

  int num = std::atoi(argv[3]);
  std::vector<pthread_t> tid(num_threads, 0);

  if (vector_size > num_threads) {
    if (!big_vector(tid, num)) {
      std::cout << "Not found!\n";
    }
  } else {
    if (!small_vector(tid, num)) {
      std::cout << "Not found!\n";
    }
  }

  vectorProcess->print();

  delete vectorProcess;
  return 0;
}