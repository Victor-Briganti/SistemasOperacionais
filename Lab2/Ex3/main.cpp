/*
 * Busca um valor em um vetor
 * Descrição: Realiza a busca em um vetor dividindo a mesma entre vários
 * processos.
 *
 * Autores: Hendrick Felipe Scheifer, João Victor Briganti, Luiz Takeda
 * Licença: BSD 2
 *
 * Data: 22/10/2024
 */

#include "process_vector.hpp" // VectorProcess
#include <cstdio>             // printf()
#include <sys/wait.h>         // wait()
#include <unistd.h>           // fork(), _exit(), sysconf(), getpid()
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
    _exit(EXIT_FAILURE);
  }

  _exit(EXIT_SUCCESS);
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
  // Calcula o tamanho do vetor que cada processo irá receber
  int chunk = static_cast<int>(vectorProcess->vector.size()) / numProcess;

  // Calcula a quantidade de elementos que restaram após a divisão do vetor
  int excess = static_cast<int>(vectorProcess->vector.size()) % numProcess;

  pid_t pid = 0;

  // Realiza o fork e passa o tamanho da busca que cada vetor terá que realizar
  for (int i = 0; i < numProcess; i++) {
    pid = fork();
    if (pid < 0) {
      std::cout << "Processo(" << getpid() << "): falhou " << pid << "\n";
      return;
    }

    if (pid == 0) {
      // Calcula o inicio da busca. min() é usado para verificar se este vetor
      // irá receber um pedaço em "excesso" ou não
      int start = i * chunk + std::min(i, excess);

      // Calcula o fim das busca no vetor.
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
      std::printf("Processo(%d): falhou %d", getpid(), pid);
      return;
    }

    if (pid == 0) {
      // Como o vetor é pequeno, não há necessidade de criar vários processos,
      // apenas use a quantidade necessária para alcançar o objetivo da busca.
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
      std::printf("Process(%d) encontrou %d\n", wpid, num);
      found = true;
    }
  }

  return static_cast<bool>(found);
}

int main(int argc, char **argv) {
  if (argc < 4) {
    std::printf("%s <num_processes> <vector_size> <num_searched>\n", argv[0]);
    return 1;
  }

  int numProcess = atoi(argv[1]);

  // Verifica se é um número válido de processos no sistema.
  // Para isso ele deve ser maior que 1 e menor que o limite de filhos que um
  // processo pode ter.
  if (numProcess < 1 || sysconf(_SC_CHILD_MAX) < (numProcess * 2 + 1)) {
    std::printf("Número de processos inválido: %d\n", numProcess);
    return 1;
  }

  int vectorSize = atoi(argv[2]);

  // O vetor deve ter um tamanho inteiro positivo.
  if (vectorSize < 0) {
    std::printf("Tamanho de vetor inválido: %d\n", vectorSize);
    return 1;
  }

  int num = atoi(argv[3]);
  vectorProcess = new VectorProcess(vectorSize);

  // Dividi o vetor e a quantidade de processos conforme os valores passados
  if (vectorSize > numProcess) {
    big_vector(numProcess, num);
  } else {
    small_vector(num);
  }

  vectorProcess->print();

  if (!wait_processes(num)) {
    std::cout << "Não foi possível encontrar " << num << std::endl;
  }

  delete vectorProcess;
  return 0;
}
