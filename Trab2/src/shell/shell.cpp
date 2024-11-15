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

#define is_end(c) ((c) == ' ' || (c) == '\0')

#define path_error(name)                               \
  std::cout << "[" << RED("ERROR") << "]: '" << (name) \
            << "' precisa de um caminho valido" << "\n"

//===----------------------------------------------------------------------===//
// PRIVATE
//===----------------------------------------------------------------------===//

bool Shell::attr(const std::vector<std::string> &path)
{
  std::cout << "attr ";

  for (const auto &str : path) {
    std::cout << "/" << str;
  }
  std::cout << "\n";
  return true;
}

bool Shell::cd(const std::vector<std::string> &path)
{
  std::cout << "cd ";

  for (const auto &str : path) {
    std::cout << "/" << str;
  }
  std::cout << "\n";
  return true;
}

Shell::Command Shell::parse_command(const std::string &input, size_t &pos)
{
  switch (input[0]) {
  case 'a':// attr
    if (input[1] == 't' && input[2] == 't' && input[3] == 'r'
        && is_end(input[4])) {
      pos = 5;
      return ATTR;
    }
    return UNKNOW;
  case 'c':// cd, cluster, cp
    switch (input[1]) {
    case 'd':// cd
      if (is_end(input[2])) {
        pos = 3;
        return CD;
      }
      return UNKNOW;
    case 'l':// cluster
      if (input[2] == 'u' && input[3] == 's' && input[4] == 't'
          && input[5] == 'e' && input[6] == 'r' && is_end(input[7])) {
        pos = 8;
        return CLUSTER;
      }
      return UNKNOW;
    case 'p':// cp
      if (is_end(input[2])) {
        pos = 3;
        return CP;
      }
      return UNKNOW;
    default:
      return UNKNOW;
    }
  case 'i':// info
    if (input[1] == 'n' && input[2] == 'f' && input[3] == 'o'
        && is_end(input[4])) {
      pos = 5;
      return INFO;
    }
    return UNKNOW;
  case 'l':// ls
    if (input[1] == 's' && is_end(input[2])) {
      pos = 3;
      return LS;
    }
    return UNKNOW;
  case 'm':// mkdir, mv
    switch (input[1]) {
    case 'k':// mkdir
      if (input[2] == 'd' && input[3] == 'i' && input[4] == 'r'
          && is_end(input[5])) {
        pos = 6;
        return MKDIR;
      }
      return UNKNOW;
    case 'v':// mv
      if (is_end(input[2])) {
        pos = 3;
        return MV;
      }
      return UNKNOW;
    default:
      return UNKNOW;
    }
  case 'p':// pwd
    if (input[1] == 'w' && input[2] == 'd' && is_end(input[3])) {
      pos = 4;
      return PWD;
    }
    return UNKNOW;
  case 'r':// rename, rm, rmdir
    switch (input[1]) {
    case 'e':// rename
      if (input[2] == 'n' && input[3] == 'a' && input[4] == 'm'
          && input[5] == 'e' && is_end(input[6])) {
        pos = 7;
        return RENAME;
      }
      return UNKNOW;
    case 'm':// rm, rmdir
      if (is_end(input[2])) {
        pos = 3;
        return RM;
      } else if (input[2] == 'd' && input[3] == 'i' && input[4] == 'r'
                 && is_end(input[5])) {
        pos = 6;
        return RMDIR;
      }
      return UNKNOW;
    default:
      return UNKNOW;
    }
  case 't':// touch
    if (input[1] == 'o' && input[2] == 'u' && input[3] == 'c' && input[4] == 'h'
        && is_end(input[5])) {
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
      if (!curPath.empty()) {
        fullPath.push_back(curPath);
      }

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
      if (curPath.length() > 0) {
        fullPath.push_back(curPath);
      }
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

bool Shell::execution(const Shell::Command command,
  const std::string &input,
  size_t pos)
{
  const std::string arguments = input.substr(pos, input.length());
  std::vector<std::string> path;
  switch (command) {
  case ATTR:
    path = parse_path(arguments);
    if (path.empty()) {
      path_error("attr");
      return false;
    }

    return Shell::attr(path);
  case CD:
    path = parse_path(arguments);
    if (path.empty()) {
      path_error("cd");
      return false;
    }

    return Shell::cd(path);
  case CLUSTER:
    path = parse_path(arguments);
    if (path.empty()) {
      std::cout << "[" << RED("ERROR")
                << "]: 'cluster' precisa de um caminho valido" << "\n";
      break;
    }
    std::cout << "CLUSTER\n";
    return true;
  case CP:
    path = parse_path(arguments);
    if (path.empty()) {
      std::cout << "[" << RED("ERROR") << "]: 'cp' precisa de um caminho valido"
                << "\n";
      break;
    }
    std::cout << "CP\n";
    return true;
  case INFO:
    path = parse_path(arguments);
    if (path.empty()) {
      std::cout << "[" << RED("ERROR")
                << "]: 'info' precisa de um caminho valido" << "\n";
      break;
    }
    std::cout << "INFO\n";
    return true;
  case LS:
    std::cout << "LS\n";
    return true;
  case MKDIR:
    path = parse_path(arguments);
    if (path.empty()) {
      std::cout << "[" << RED("ERROR")
                << "]: 'mkdir' precisa de um caminho valido" << "\n";
      break;
    }
    std::cout << "MKDIR\n";
    return true;
  case MV:
    path = parse_path(arguments);
    if (path.empty()) {
      std::cout << "[" << RED("ERROR") << "]: 'mv' precisa de um caminho valido"
                << "\n";
      break;
    }
    std::cout << "MV\n";
    return true;
  case PWD:
    std::cout << "PWD\n";
    return true;
  case RENAME:
    path = parse_path(arguments);
    if (path.empty()) {
      std::cout << "[" << RED("ERROR")
                << "]: 'rename' precisa de um caminho valido" << "\n";
      break;
    }
    std::cout << "RENAME\n";
    return true;
  case RM:
    path = parse_path(arguments);
    if (path.empty()) {
      std::cout << "[" << RED("ERROR") << "]: 'rm' precisa de um caminho valido"
                << "\n";
      break;
    }
    std::cout << "RM\n";
    return true;
  case RMDIR:
    path = parse_path(arguments);
    if (path.empty()) {
      std::cout << "[" << RED("ERROR")
                << "]: 'rmdir' precisa de um caminho valido" << "\n";
      break;
    }
    std::cout << "RMDIR\n";
    return true;
  case TOUCH:
    path = parse_path(arguments);
    if (path.empty()) {
      std::cout << "[" << RED("ERROR")
                << "]: 'touch' precisa de um caminho valido" << "\n";
      break;
    }
    std::cout << "TOUCH\n";
    return true;
  case UNKNOW:
    std::cout << "[" << RED("UNKNOW") << "] :" << input << "\n";
    break;
  }
  return false;
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
    Shell::Command command = Shell::parse_command(input, pos);
    Shell::execution(command, input, pos);
  }
}