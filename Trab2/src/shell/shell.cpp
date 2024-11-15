/*
 * Descrição: Arquivo principal do shell
 *
 * Autores: Hendrick Felipe Scheifer, João Victor Briganti, Luiz Takeda
 * Licença: BSD 2
 *
 * Data: 15/11/2024
 */

#include "shell/shell.hpp"
#include "shell/color.hpp"

#include <iostream>

//===----------------------------------------------------------------------===//
// PRIVATE
//===----------------------------------------------------------------------===//

Shell::Command Shell::parse_command(const std::string &input, size_t &pos)
{
  switch (input[0]) {
  case 'a':// attr
    if (input[1] == 't' && input[2] == 't' && input[3] == 'r'
        && input[4] == ' ') {
      pos = 5;
      return ATTR;
    }
    return UNKNOW;
  case 'c':// cd, cluster, cp
    switch (input[1]) {
    case 'd':// cd
      if (input[2] == ' ') {
        pos = 3;
        return CD;
      }
      return UNKNOW;
    case 'l':// cluster
      if (input[2] == 'u' && input[3] == 's' && input[4] == 't'
          && input[5] == 'e' && input[6] == 'r' && input[7] == ' ') {
        pos = 8;
        return CLUSTER;
      }
      return UNKNOW;
    case 'p':// cp
      if (input[2] == ' ') {
        pos = 3;
        return CP;
      }
      return UNKNOW;
    default:
      return UNKNOW;
    }
  case 'i':// info
    if (input[1] == 'n' && input[2] == 'f' && input[3] == 'o'
        && input[4] == ' ') {
      pos = 5;
      return INFO;
    }
    return UNKNOW;
  case 'l':// ls
    if (input[1] == 's' && (input[2] == '\0' || input[2] == ' ')) {
      pos = 3;
      return LS;
    }
    return UNKNOW;
  case 'm':// mkdir, mv
    switch (input[1]) {
    case 'k':// mkdir
      if (input[2] == 'd' && input[3] == 'i' && input[4] == 'r'
          && input[5] == ' ') {
        pos = 6;
        return MKDIR;
      }
      return UNKNOW;
    case 'v':// mv
      if (input[2] == '\0' || input[2] == ' ') {
        pos = 3;
        return MV;
      }
      return UNKNOW;
    default:
      return UNKNOW;
    }
  case 'p':// pwd
    if (input[1] == 'w' && input[2] == 'd'
        && (input[3] == '\0' || input[3] == ' ')) {
      pos = 4;
      return PWD;
    }
    return UNKNOW;
  case 'r':// rename, rm, rmdir
    switch (input[1]) {
    case 'e':// rename
      if (input[2] == 'n' && input[3] == 'a' && input[4] == 'm'
          && input[5] == 'e' && input[6] == ' ') {
        pos = 7;
        return RENAME;
      }
      return UNKNOW;
    case 'm':// rm, rmdir
      if (input[2] == ' ') {
        pos = 3;
        return RM;
      } else if (input[2] == 'd' && input[3] == 'i' && input[4] == 'r'
                 && input[5] == ' ') {
        pos = 6;
        return RMDIR;
      }
      return UNKNOW;
    }
  case 't':// touch
    if (input[1] == 'o' && input[2] == 'u' && input[3] == 'c' && input[4] == 'h'
        && input[5] == ' ') {
      pos = 6;
      return TOUCH;
    }
    return UNKNOW;
  default:
    return UNKNOW;
  }
}

std::vector<std::string> Shell::parse_path(const std::string &input)
{
  std::vector<std::string> fullPath;// Armazena todo o caminho
  std::string curPath;// Gerencia o caminho atual

  size_t pos = 0;
  bool slash = false;
  while (true) {
    // Se a posição for maior do que a string chegamos no fim
    if (input.size() < pos) {
      if (!curPath.empty()) { fullPath.push_back(curPath); }

      break;
    }

    // A posição inicial deve ser começar com um '/'
    if (pos == 0 && input[0] != '/') {
      break;
    } else if (pos == 0) {
      pos++;
      slash = true;
      continue;
    }

    // Se eu encontrar um / seguido de outro / houve um erro.
    // Caso contrário se uma / foi encontrada salva o caminho atual e continua.
    if (slash && input[pos] == '/') {
      fullPath.clear();
      break;
    } else if (!slash && input[pos] == '/') {
      slash = true;
      pos++;
      if (curPath.length() > 0) { fullPath.push_back(curPath); }
      curPath.clear();
      continue;
    }

    if (input[pos] == '\0') {
      pos++;
      continue;
    }

    curPath += input[pos];
    pos++;
    slash = false;
  }

  return fullPath;
}

//===----------------------------------------------------------------------===//
// PUBLIC
//===----------------------------------------------------------------------===//

void Shell::interpreter()
{
  std::string input;

  while (true) {
    std::cout << GREEN("fatshell") << ":" << YELLOW("/") << " ";
    std::getline(std::cin, input);

    if (std::cin.eof()) {
      std::cout << "\n";
      return;
    }

    size_t pos = 0;
    Shell::Command command = this->parse_command(input, pos);
    switch (command) {
    case ATTR: {
      std::vector<std::string> path =
        parse_path(input.substr(pos, input.length()));
      if (path.empty()) {
        std::cout << "Caminho vazio\n";
        break;
      }
      for (const auto &str : path) { std::cout << str << "\n"; }
      break;
    }
    case CD:
      std::cout << "CD\n";
      break;
    case CLUSTER:
      std::cout << "CLUSTER\n";
      break;
    case CP:
      std::cout << "CP\n";
      break;
    case INFO:
      std::cout << "INFO\n";
      break;
    case LS:
      std::cout << "LS\n";
      break;
    case MKDIR:
      std::cout << "MKDIR\n";
      break;
    case MV:
      std::cout << "MV\n";
      break;
    case PWD:
      std::cout << "PWD\n";
      break;
    case RENAME:
      std::cout << "RENAME\n";
      break;
    case RM:
      std::cout << "RM\n";
      break;
    case RMDIR:
      std::cout << "RMDIR\n";
      break;
    case TOUCH:
      std::cout << "TOUCH\n";
      break;
    case UNKNOW:
      std::cout << "UNKNOW\n";
      break;
    }
  }
}
