/**
 * Descrição: Definição das funções para diretório/arquivos
 *
 * Autores: João Victor Briganti, Luiz Takeda
 * Licença: BSD 2
 *
 * Data: 01/01/2024
 */

#include "filesystem/dir.hpp"

#include <cctype>
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

char *generateShortName(const std::string &longName)
{
  char *shortName = new char[NAME_MAIN_SIZE + NAME_EXT_SIZE];
  if (shortName == nullptr) {
    return nullptr;
  }

  // Indica o local em que o '.' foi salvo
  int period = -1;

  std::string treatedName;
  for (size_t i = 0; i < longName.size(); i++) {
    // Se a primeira coisa na string for um ponto ou um espaço apenas saia.
    if ((i == 0 && longName[i] == '.') || std::isspace(longName[i])) {
      continue;
    }

    if (longName[i] == '.') {
      period = treatedName.size();
      continue;
    }

    // Se o caracter encontrado não for válido, troca por um '_'
    if (!validShortDirName(longName[i])) {
      treatedName += '_';
      continue;
    }

    // Salva o caracter em caixa alta
    treatedName += static_cast<char>(std::toupper(longName[i]));
  }

  // Se o nome longo for menor do que o necessário no nome curto realizamos o
  // padding
  if (treatedName.size() < NAME_MAIN_SIZE + NAME_EXT_SIZE) {
    // Realiza o padding na parte principal do nome
    for (int i = period; period != -1 && i < NAME_MAIN_SIZE; i++) {
      treatedName = treatedName.substr(0, i) + " "
                    + treatedName.substr(i, treatedName.size());
    }
  }

  // Adiciona o tail do nome
  treatedName[6] = '~';
  treatedName[7] = '1';

  // Copia a parte principal do nome
  for (int i = 0; i < NAME_MAIN_SIZE; i++) {
    shortName[i] = treatedName[i];
  }

  // Copia a extensão do nome
  for (int i = period, j = NAME_MAIN_SIZE; i < period + NAME_EXT_SIZE;
    i++, j++) {
    if (i > treatedName.size()) {
      shortName[j] = ' ';
    } else {
      shortName[j] = treatedName[i];
    }
  }

  return shortName;
}
