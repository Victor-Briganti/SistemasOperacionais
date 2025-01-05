/**
 * Descrição: Definição para funções de log
 *
 * Autores: João Victor Briganti, Luiz Takeda
 * Licença: BSD 2
 *
 * Data: 05/12/2024
 */

#include "utils/logger.hpp"

#include <iostream>
#include <sstream>

static const std::string ERROR = "[\033[31mERROR\033[0m] ";
static const std::string WARNING = "[\033[33mWARNING\033[0m] ";
static const std::string INFO = "[\033[32mINFO\033[0m] ";

void logError(std::string error) { std::cout << ERROR << error << '\n'; }

void logWarning(std::string warning)
{
  std::cout << WARNING << warning << '\n';
}

void logInfo(std::string info) { std::cout << INFO << info << '\n'; }

void logError(const char *error) { std::cout << ERROR << error << '\n'; }

void logWarning(const char *warning)
{
  std::cout << WARNING << warning << '\n';
}

void logInfo(const char *info) { std::cout << INFO << info << '\n'; }

std::string to_hex(uint64_t value)
{
  std::stringstream ss;
  ss << std::hex << std::uppercase << value;
  return ss.str();
}
