/*
 * Programa de manipulação de PIPEs
 * Descrição: Realiza a manipulação de PIPEs para receber strings e passar
 * informações entre processos.
 *
 * Author: Victor Briganti, Luiz Takeda
 * License: BSD 2
 */
#include <cctype>
#include <csignal>
#include <cstring>
#include <iostream>
#include <string>
#include <sys/types.h>
#include <unistd.h>

/** @brief Define o tamanho máximo da string usada como buffer. */
#define MAX_STRING_SIZE 4096

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
  int fd[2];
  pid_t pid;
  std::string inputBuffer;
  std::string outputBuffer;
  outputBuffer.resize(MAX_STRING_SIZE);

  pipe(fd);

  pid = fork();
  if (pid < 0) {
    std::cout << "Fork failed\n";
    return -1;
  }

  if (pid) {
    // Processo pai que irá ler o que o usuário escreveu e enviar para uma das
    // pontas no pipe
    while (true) {
      std::cout << "Escreva uma frase: \n";

      // Obtém a linha passada pelo usuário
      if (!std::getline(std::cin, inputBuffer)) {
        if (std::cin.eof()) {
          // Recebe um EOF (<C-d>) e mata o processo filho.
          kill(pid, SIGKILL);
          return 0;
        } else {
          std::cerr << "Could not read input\n";
          continue;
        }
      }

      std::string output;

      output += "Frase: " + inputBuffer + "\n";
      output += "Tamanho: " + std::to_string(inputBuffer.length()) + "\n";
      output +=
          "Número de Vogais: " + std::to_string(count_vowels(inputBuffer)) +
          "\n";
      output += "Número de Consoantes: " +
                std::to_string(count_consonants(inputBuffer)) + "\n";
      output += "Número de Espaços: " +
                std::to_string(count_white_spaces(inputBuffer)) + "\n";

      // Escreve no pipe que será lido pelo processo em outra ponta
      write(fd[1], output.c_str(), output.length() + 1);
    }
  } else {
    // Processo filho que irá ler o que está sendo escrito no pipe
    while (true) {
      // Lê o que está sendo escrito no pipe pelo outro processo
      size_t nbytes = read(fd[0], outputBuffer.data(), MAX_STRING_SIZE);
      outputBuffer.push_back('\0');
      std::cout << outputBuffer << "\n";
    }
  }
}
