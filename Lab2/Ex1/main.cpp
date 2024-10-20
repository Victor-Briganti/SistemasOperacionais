/*
 * Hierarquia de Processos
 * Descrição: Cria processos em N níveis e mostra a árvore de visualização.
 *
 * Author: Victor Briganti, Luiz Takeda
 * License: BSD 2
 */
#include <cstdio>
#include <cstdlib>
#include <math.h>
#include <string>
#include <sys/wait.h>
#include <unistd.h>

/**
 * @brief Mostra mostra o ID do pai e do processo em execução
 *
 * @param level Nível do processo
 */
void pstree(int level) {
  std::string out = "(Nível:" + std::to_string(level) + ")";
  for (int i = 0; i < level - 1; i++) {
    out += "    ";
  }
  out += "PPID[" + std::to_string(getppid()) + "]:";
  out += "PID[" + std::to_string(getpid()) + "]";
  std::printf("%s\n", out.c_str());
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
  if (level > max_level) {
    return;
  }

  pstree(level);

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
}

int main(int argc, char **argv) {
  if (argc < 2) {
    printf("%s <num_process>\n", argv[0]);
    return 1;
  }

  const int N = atoi(argv[1]);

  // Verifica se é um número válido de processos no sistema
  if (N < 1 || sysconf(_SC_CHILD_MAX) < static_cast<long int>(pow(2, N))) {
    printf("%d é um número inválido de processos", N);
    return 1;
  }

  process_tree(1, N);

  // Espera os processos terminarem
  while (wait(NULL) >= 0) {
  }

  return 0;
}
