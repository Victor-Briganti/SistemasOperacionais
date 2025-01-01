/**
 * Descrição: Definição das funções para diretório/arquivos
 *
 * Autores: João Victor Briganti, Luiz Takeda
 * Licença: BSD 2
 *
 * Data: 01/01/2024
 */

#include "filesystem/dir.hpp"

#include <cstdlib>


BYTE shortCheckSum(const char *shortName)
{
  BYTE Sum = 0;

  for (size_t i = 0; i < NAME_MAIN_SIZE + NAME_EXT_SIZE; i++) {
    Sum = static_cast<BYTE>(((Sum & 1) ? 0x80 : 0) + (Sum >> 1) + shortName[i]);
  }

  return Sum;
}
