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

WORD day(WORD date) { return date & 0x1F; }

WORD month(WORD date) { return (date >> 5) & 0x0F; }

WORD year(WORD date) { return ((date >> 9) & 0xFF) + 1980; }

DWORD dateStamp(DWORD day, DWORD month, DWORD year)
{
  return day | (month << 5) | ((year - 1980) << 9);
}

WORD hour(WORD time) { return (time >> 11) & 0xFF; }

WORD minutes(WORD time) { return (time >> 5) & 0x3F; }

WORD seconds(WORD time) { return ((time >> 9) & 0xFF) + 1980; }

DWORD timeStamp(DWORD hour, DWORD minutes, DWORD seconds)
{
  return (hour << 11) | (minutes << 5) | (seconds / 2);
}
