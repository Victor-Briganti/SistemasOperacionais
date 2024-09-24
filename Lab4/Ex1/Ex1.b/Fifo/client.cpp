/*
 * Cliente Fifo
 * Descrição: Realiza a manipulação de FIFOs para receber strings e realizar
 * execução de expressões aritméticas.
 *
 * Author: Victor Briganti, Luiz Takeda
 * License: BSD 2
 */
#include <algorithm>
#include <cmath>
#include <fcntl.h>
#include <iostream>
#include <sys/stat.h>
#include <unistd.h>

/** @brief Define o tamanho máximo da string usada como buffer. */
#define MAX_STRING_SIZE 4096

/** @brief Define o arquivo que será usado como FIFO. */
#define FIFO "ffifo"

/**
 * @brief Realiza o cálculo de uma expressão.
 *
 * Cálcula a expressão passada por meio de uma string. As expressões aceitas são
 * (+, *, -, /).
 *
 * @param str String que armazena a expressão.
 *
 * @return o valor da expressão avaliada, ou NaN caso não seja possível avaliar.
 */
float arithmetic(std::string &str) {
  float x, y;

  if (std::find(str.begin(), str.end(), '*') != str.end()) {
    int n = sscanf(str.c_str(), "%f*%f", &x, &y);
    if (n < 0) {
      std::cout << "Não foi possível ler expressão\n";
      return std::nan("");
    }
    return x * y;
  } else if (std::find(str.begin(), str.end(), '+') != str.end()) {
    int n = sscanf(str.c_str(), "%f+%f", &x, &y);
    if (n < 0) {
      std::cout << "Não foi possível ler expressão\n";
      return std::nan("");
    }
    return x + y;
  } else if (std::find(str.begin(), str.end(), '-') != str.end()) {
    int n = sscanf(str.c_str(), "%f-%f", &x, &y);
    if (n < 0) {
      std::cout << "Não foi possível ler expressão\n";
      return std::nan("");
    }
    return x - y;
  } else if (std::find(str.begin(), str.end(), '/') != str.end()) {
    int n = sscanf(str.c_str(), "%f/%f", &x, &y);
    if (n < 0) {
      std::cout << "Não foi possível ler expressão\n";
      return std::nan("");
    }
    return x / y;
  }

  return std::nan("");
}

int main() {
  // Abre o arquivo FIFO para ler
  int fd = open(FIFO, O_RDONLY);
  if (fd == -1) {
    std::cerr << "Error opening FIFO for reading" << std::endl;
    return 1;
  }

  while (true) {

    std::string receiveBuffer;
    receiveBuffer.resize(MAX_STRING_SIZE);

    // Lê as entradas passadas pelo fifo
    ssize_t bytesRead = read(fd, receiveBuffer.data(), MAX_STRING_SIZE);
    if (bytesRead > 0) {
      std::cout << arithmetic(receiveBuffer) << std::endl;
    } else {
      std::cout << "Exiting..." << std::endl;
      break;
    }
  }

  close(fd);
  return 0;
}