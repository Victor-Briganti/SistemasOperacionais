/*
 * Trata o SIGINT com uma contagem regressiva
 * Descrição:
 *
 * Author: Victor Briganti, Luiz Takeda
 * License: BSD 2
 */
#include <csignal>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>
#include <unistd.h>

bool finished = false;

void handleInt(int signo, siginfo_t *info, void *context) {
  std::cout << "Terminando de escrever\n";
  finished = true;
}

int main(int argc, char **argv) {
  if (argc < 3) {
    std::cout << argv[0] << " <input_file> <output_file>\n";
    return EXIT_FAILURE;
  }

  std::ifstream input(argv[1]);
  if (!input || !input.is_open()) {
    std::cerr << "Não foi possível ler arquivo de entrada\n";
    return EXIT_FAILURE;
  }

  std::ofstream output(argv[2]);
  if (!output || !output.is_open()) {
    std::cerr << "Não foi possível ler arquivo de saída\n";
    input.close();
    return EXIT_FAILURE;
  }

  struct sigaction actInt;
  actInt.sa_sigaction = &handleInt;

  struct sigaction actTerm;
  actInt.sa_sigaction = &handleInt;

  if (sigaction(SIGINT, &actInt, NULL) < 0) {
    perror("sigaction SIGINT");
    input.close();
    output.close();
    return EXIT_FAILURE;
  }

  if (sigaction(SIGTERM, &actInt, NULL) < 0) {
    perror("sigaction SIGTERM");
    input.close();
    output.close();
    return EXIT_FAILURE;
  }

  std::string line;
  while (std::getline(input, line)) {
    output << line << "\n";
    if (!finished) {
      usleep(500);
    }
  }

  output.close();
  input.close();
  return EXIT_SUCCESS;
}
