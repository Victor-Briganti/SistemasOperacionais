/*
 * Programa de manipulação de FIFOs
 * Descrição: Realiza a manipulação de FIFOs para receber strings e passar
 * informações entre programas em execução.
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

/** @brief Macro para verificar se o caracter é uma vogal. */
#define isvowel(c)                                                             \
  ((((c) == 'A') || ((c) == 'E') || ((c) == 'I') || ((c) == 'O') ||            \
    ((c) == 'U')) ||                                                           \
   (((c) == 'a') || ((c) == 'e') || ((c) == 'i') || ((c) == 'o') ||            \
    ((c) == 'u')))

/** @brief Macro para verificar se o caracter é uma consoante. */
#define isconsonant(c) ((isalpha(c)) && !isvowel(c))

/**
 * @brief Conta a quantidade de consoantes na string
 *
 * @param str String que será avaliada
 *
 * @return Total de consoantes
 */
int count_consonants(std::string &str) {
  int count = 0;
  for (char c : str) {
    if (isconsonant(c)) {
      count++;
    }
  }

  return count;
}

/**
 * @brief Conta a quantidade de vogais na string
 *
 * @param str String que será avaliada
 *
 * @return Total de vogais
 */
int count_vowels(std::string &str) {
  int count = 0;
  for (char c : str) {
    if (isvowel(c)) {
      count++;
    }
  }

  return count;
}

/**
 * @brief Conta a quantidade de espaços em branco na string
 *
 * @param str String que será avaliada
 *
 * @return Total de espaços em branco
 */
int count_white_spaces(std::string &str) {
  int count = 0;
  for (char c : str) {
    if (isspace(c)) {
      count++;
    }
  }

  return count;
}

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

  while (true) {
    std::string input;
    std::cout << "Escreva uma frase: \n";

    // Obtém a linha passada pelo usuário
    if (!std::getline(std::cin, input)) {
      if (std::cin.eof()) {
        break;
      } else {
        std::cerr << "Could not read input\n";
        continue;
      }
    }

    std::string output;

    output += "Frase: " + input + "\n";
    output += "Tamanho: " + std::to_string(input.length()) + "\n";
    output += "Número de Vogais: " + std::to_string(count_vowels(input)) + "\n";
    output +=
        "Número de Consoantes: " + std::to_string(count_consonants(input)) +
        "\n";
    output +=
        "Número de Espaços: " + std::to_string(count_white_spaces(input)) +
        "\n";

    // Escreve no fifo que será lido por outro programa em outra ponta
    write(fd, output.c_str(), output.length() + 1);
  }

  close(fd);
  return 0;
}
