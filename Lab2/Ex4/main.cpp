/*
 * Shell Simples
 * Descrição: Shell simples usado para execução de processos.
 *
 * Author: Victor Briganti, Luiz Takeda
 * License: BSD 2
 */
#include <cctype>
#include <cstring>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <sys/wait.h>
#include <unistd.h>
#include <vector>

/**
 * @brief Realiza o parse dos argumentos
 *
 * Dividi os argumentos em um vetor de caracteres para serem usados no execvp().
 *
 * @param args String de argumentos
 *
 * @return vetor dos argumentos.
 */
std::vector<char *> parse_string(std::string args) {
  std::vector<char *> vector;

  if (!args.empty() && args[args.size() - 1] == '&') {
    args.pop_back();
  }

  size_t pos = 0;
  while (true) {
    size_t newPos = args.find(" ", pos);
    std::string token = args.substr(pos, newPos - pos);

    if (!token.empty()) {
      vector.push_back(strdup(token.c_str()));
    }

    if (newPos == std::string::npos) {
      break;
    }

    pos = newPos + 1;
  }

  vector.push_back(nullptr);
  return vector;
}

/**
 * @brief Verifica os caracteres na string
 *
 * Verifica se os caracteres na string podem ser colocados na tela.
 *
 * @param str String para ser verificada
 *
 * @return true se a string só tem caracteres válidos, false caso contrário.
 */
bool verify_string(std::string str) {
  for (char a : str) {
    if (!std::isprint(a)) {
      return false;
    }
  }
  return true;
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
int execute_command_wait(std::string args) {
  std::vector<char *> vector = parse_string(args);

  pid_t pid = fork();
  if (pid < 0) {
    return -1;
  }

  if (pid == 0) {
    execvp(vector[0], vector.data());
    exit(1);
  }

  wait(NULL);
  for (char *arg : vector) {
    delete[] arg;
  }

  return 0;
}

/**
 * @brief Executa o comando sem esperar
 *
 * @param args Argumentos usados para executar outro processo.
 *
 * @return 0 se tudo funcionou, -1 se houve algum problema.
 */
int execute_command(std::string args) {
  std::vector<char *> vector = parse_string(args);

  pid_t pid = fork();
  if (pid < 0) {
    return -1;
  }

  if (pid == 0) {
    execvp(vector[0], vector.data());
    exit(1);
  }

  for (char *arg : vector) {
    delete[] arg;
  }

  return 0;
}

int main() {
  std::string args;

  // Loop principal. Realiza iterações até que o usuário termine a execução com
  // <C-c> ou <C-d>.
  while (true) {
    std::cout << "$ ";

    // Obtém a linha passada pelo usuário
    if (!std::getline(std::cin, args)) {
      if (std::cin.eof()) {
        return 0;
      } else {
        std::cerr << "Could not read input\n";
        continue;
      }
    }
    std::cout << "\n";

    if (args.empty()) {
      continue;
    }

    // Verifica se os caracteres podem ser colocados na tela.
    if (!verify_string(args)) {
      std::cout << "Invalid characters passed\n";
      continue;
    }

    // Se o último character for '&' execute o programa sem esperar. Caso
    // contrário execute e espere.
    if (args[args.size() - 1] == '&') {
      if (execute_command(args) < 0) {
        std::cout << "Could not load the program\n";
        continue;
      }
    } else {
      if (execute_command_wait(args) < 0) {
        std::cout << "Could not load the program\n";
        continue;
      }
    }
  }

  return 0;
}