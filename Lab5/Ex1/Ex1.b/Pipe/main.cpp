/*
 * Programa de manipulação de PIPEs
 * Descrição: Realiza a manipulação de PIPEs para executar uma expressão
 * matemática simples entre processos.
 *
 * Author: Victor Briganti, Luiz Takeda
 * License: BSD 2
 */
#include <algorithm>
#include <cctype>
#include <cmath>
#include <csignal>
#include <cstring>
#include <iostream>
#include <string>
#include <sys/types.h>
#include <unistd.h>

/** @brief Define o tamanho máximo da string usada como buffer. */
#define MAX_STRING_SIZE 4096

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
  int fd[2];
  pid_t pid;
  std::string inputBuffer;

  pipe(fd);

  pid = fork();
  if (pid < 0) {
    std::cout << "Fork failed\n";
    return -1;
  }

  if (pid) {
    std::cout << "Programa para leitura de expressões simples.\n";
    std::cout << "As equações aceitas são: (+, -, *, /).\n";
    std::cout << "Devem ser escritas sem espaço. Exemplo: 10+5\n";

    // Processo pai que irá ler o que o usuário escreveu e enviar para uma das
    // pontas no pipe
    while (true) {
      // Obtém a linha passada pelo usuário
      if (!std::getline(std::cin, inputBuffer)) {
        if (std::cin.eof()) {
          // Recebe um EOF (<C-d>) e mata o processo filho.
          //   kill(pid, SIGKILL);
          return 0;
        } else {
          std::cerr << "Could not read input\n";
          continue;
        }
      }

      // Escreve no pipe que será lido pelo processo em outra ponta
      write(fd[1], inputBuffer.c_str(), inputBuffer.length() + 1);
    }
  } else {
    // Processo filho que irá ler o que está sendo escrito no pipe
    while (true) {
      std::string outputBuffer;
      outputBuffer.resize(MAX_STRING_SIZE);

      // Lê o que está sendo escrito no pipe pelo outro processo
      size_t nbytes = read(fd[0], outputBuffer.data(), MAX_STRING_SIZE);
      outputBuffer.push_back('\0');
      std::cout << arithmetic(outputBuffer) << "\n";
    }
  }
}