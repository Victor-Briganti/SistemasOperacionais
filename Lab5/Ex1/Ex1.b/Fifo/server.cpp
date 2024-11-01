/*
 * Server FIFO
 * Descrição: Realiza a manipulação de FIFOs para receber strings e realizar
 * execução de expressões aritméticas.
 *
 * Author: Victor Briganti, Luiz Takeda
 * License: BSD 2
 */
#include <cctype>
#include <csignal>
#include <cstring>
#include <fcntl.h>
#include <iostream>
#include <string>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

/** @brief Define o arquivo que será usado como FIFO. */
#define FIFO "ffifo"

int main() {
  if (mkfifo(FIFO, 0777) < 0) {
    std::cerr << "Error creating the FIFO\n";
    return 1;
  }

  int fd = open(FIFO, O_WRONLY);
  if (fd < 0) {
    std::cerr << "Error opening the FIFO for writing\n";
    return 1;
  }

  std::cout << "Programa para leitura de expressões simples.\n";
  std::cout << "As equações aceitas são: (+, -, *, /).\n";
  std::cout << "Devem ser escritas sem espaço. Exemplo: 10+5\n";

  while (true) {
    std::string input;

    // Obtém a linha passada pelo usuário
    if (!std::getline(std::cin, input)) {
      if (std::cin.eof()) {
        break;
      } else {
        std::cerr << "Could not read input\n";
        continue;
      }
    }

    // Escreve no fifo que será lido por outro programa em outra ponta
    write(fd, input.c_str(), input.length() + 1);
  }

  close(fd);
  return 0;
}
