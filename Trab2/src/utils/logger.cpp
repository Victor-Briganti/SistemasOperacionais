/**
 * Descrição: Definição para funções de log
 *
 * Autores: João Victor Briganti, Luiz Gustavo Takeda, Matheus Floriano Saito
 * Licença: BSD 2
 *
 * Data: 05/12/2024
 */

#include "utils/logger.hpp"

#include <iostream>
#include <sstream>

namespace {
constexpr char ERROR[] = "[\033[31mERROR\033[0m] ";
constexpr char WARNING[] = "[\033[33mWARNING\033[0m] ";
constexpr char INFO[] = "[\033[32mINFO\033[0m] ";
}// namespace


void logger::logError(std::string error)
{
  std::cout << ERROR << error << '\n';
}

void logger::logWarning(std::string warning)
{
  std::cout << WARNING << warning << '\n';
}

void logger::logInfo(std::string info) { std::cout << INFO << info << '\n'; }

void logger::logError(const char *error)
{
  std::cout << ERROR << error << '\n';
}

void logger::logWarning(const char *warning)
{
  std::cout << WARNING << warning << '\n';
}

void logger::logInfo(const char *info) { std::cout << INFO << info << '\n'; }

std::string logger::to_hex(uint64_t value)
{
  std::stringstream ss;
  ss << std::hex << std::uppercase << value;
  return ss.str();
}

std::string logger::to_green(std::string str)
{
  return "\033[32m" + str + "\033[0m";
}
