/*
 * Descrição: Implementação para print de cores em logs
 *
 * Autores: João Victor Briganti, Luiz Takeda
 * Licença: BSD 2
 *
 * Data: 15/11/2024
 */

#include "utils/color.hpp"
#include <string>


std::string red_str(const std::string &string)
{
  return "\033[31m" + (string) + "\033[0m";
}

std::string green_str(const std::string &string)
{
  return "\033[32m" + (string) + "\033[0m";
}

std::string yellow_str(const std::string &string)
{
  return "\033[33m" + (string) + "\033[0m";
}

std::string blue_str(const std::string &string)
{
  return "\033[34m" + (string) + "\033[0m";
}

std::string magenta_str(const std::string &string)
{
  return "\033[35m" + (string) + "\033[0m";
}

std::string black_str(const std::string &string)
{
  return "\033[1;30m" + (string) + "\033[0m";
}
