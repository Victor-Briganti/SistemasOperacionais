/**
 * Descrição: Definição das funções para diretório/arquivos
 *
 * Autores: João Victor Briganti, Luiz Takeda
 * Licença: BSD 2
 *
 * Data: 01/01/2024
 */

#include "filesystem/dir.hpp"
#include "filesystem/default.hpp"
#include "filesystem/entry/short_entry.hpp"

#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <cstring>
#include <stdexcept>
#include <sys/time.h>


/**
 * @brief Verifica se o nome longo é válido
 *
 * @return true se for válido, false caso contrário
 */
static inline bool validLongName(const std::string &name)
{
  bool valid = true;

  for (const auto &a : name) {
    valid = validLongDirName(a);
  }

  return valid;
}

/**
 * @brief Gera vetor de nomes longos
 *
 * @return Uma lista de caracteres prontos para serem escritos
 */
static std::vector<char> generateLongName(const std::string &name)
{
  std::vector<char> nameVec;

  for (const auto &chr : name) {
    nameVec.push_back(chr);
    nameVec.push_back('\0');
  }

  // O nome precisa ser finalizado com '\0'
  nameVec.push_back('\0');
  nameVec.push_back('\0');

  // Na prática uma entrada de nome longo armazena o dobro de caracteres
  while (nameVec.size() % (LONG_NAME_SIZE * 2) != 0) {
    nameVec.push_back(0xFF);
  }

  return nameVec;
}

//===------------------------------------------------------------------------===
// PUBLIC
//===------------------------------------------------------------------------===

std::vector<LongDir> createLongDir(const ShortEntry &entry,
  const std::string &name)
{
  if (!validLongName(name)) {
    std::string error = "Nome " + name + " inválido\n";
    throw std::runtime_error(error);
  }

  std::vector<LongDir> longDirs;
  BYTE checksum = shortCheckSum(reinterpret_cast<const char *>(entry.name));
  std::vector<char> longChars = generateLongName(name);

  size_t i = 0;
  while (i < longChars.size()) {
    LongDir ldir;
    ldir.chckSum = checksum;
    ldir.fstClusLO = 0;
    ldir.attr = ATTR_LONG_NAME;

    // O único valor especificado aqui é o do diretório que deve ser 0.
    if ((entry.attr & ATTR_DIRECTORY) == 0) {
      ldir.type = 0;
    } else {
      ldir.type = 1;
    }

    for (int j = 0; j < 10; j++, i++) {
      ldir.name1[j] = static_cast<BYTE>(longChars[i]);
    }

    for (int j = 0; j < 12; j++, i++) {
      ldir.name2[j] = static_cast<BYTE>(longChars[i]);
    }

    for (int j = 0; j < 4; j++, i++) {
      ldir.name3[j] = static_cast<BYTE>(longChars[i]);
    }

    longDirs.push_back(ldir);
  }

  std::reverse(longDirs.begin(), longDirs.end());
  for (int i = static_cast<int>(longDirs.size() - 1), j = 0; i >= 0; i--, j++) {
    if (i == static_cast<int>(longDirs.size() - 1)) {
      longDirs[j].ord = static_cast<BYTE>(LAST_LONG_ENTRY | (i + 1));
    } else {
      longDirs[j].ord = static_cast<BYTE>(i + 1);
    }
  }
  return longDirs;
}

BYTE shortCheckSum(const char *shortName)
{
  BYTE Sum = 0;

  for (size_t i = 0; i < NAME_FULL_SIZE; i++) {
    Sum = static_cast<BYTE>(((Sum & 1) ? 0x80 : 0) + (Sum >> 1) + shortName[i]);
  }

  return Sum;
}
