/*
 * Hierarquia de Processos
 * Descrição: Cria processos em N níveis e mostra a árvore de visualização.
 *
 * Author: Victor Briganti, Luiz Takeda
 * License: BSD 2
 */
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <sys/wait.h>
#include <unistd.h>

#define SLEEP_SEC 1

/**
 * @brief Mostra a árvore de processos do programa
 */
void pstree() {
  std::string str("pstree -c -p ");
  str += std::to_string(getppid());
  sleep(SLEEP_SEC);
  system(str.c_str());
}

/**
 * @brief Cria uma árvore de processos
 *
 * Cada processo cria um nó filho a esquerda e a direita, até o nível máximo ser
 * atingido.
 *
 * @param level Nível atual do processo
 * @param max_level Nível máximo que os processos podem chegar
 */
void process_tree(int level, const int max_level) {
  if (level == max_level) {
    return;
  }

  // Cria o processo filho a esquerda
  pid_t left_child = fork();

  if (left_child < 0) {
    printf("Process(%d): fail %d\n", getpid(), left_child);
    exit(1);
  }

  if (left_child == 0) {
    // Chama recusivamente a função para criar sues processos filhos
    process_tree(level + 1, max_level);
    exit(0);
  }

  // Cria o processo filho a direita
  pid_t right_child = fork();

  if (right_child < 0) {
    printf("Process(%d): fail %d\n", getpid(), right_child);
    exit(1);
  }

  if (right_child == 0) {
    process_tree(level + 1, max_level);
    exit(0);
  }

  // Dorme por alguns segundos e então mostra a árvore de processos.
  sleep(SLEEP_SEC);
  pstree();

  // Espera pelo processo a direita e a esquerda terminarem
  wait(NULL);
  wait(NULL);
}

int main(int argc, char **argv) {
  if (argc < 2) {
    printf("%s <num_process>\n", argv[0]);
    return 1;
  }

  const int N = atoi(argv[1]);
  if (N < 1) {
    printf("%d é um número inválido de processos", N);
    return 1;
  }

  process_tree(1, N);
  return 0;
}