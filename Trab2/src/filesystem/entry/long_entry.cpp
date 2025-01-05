/**
 * Descrição: Implementações da entrada de nome longo
 *
 * Autores: João Victor Briganti, Luiz Takeda
 * Licença: BSD 2
 *
 * Data: 26/12/2024
 */

#include "filesystem/entry/long_entry.hpp"
#include "utils/types.hpp"

#include <algorithm>
#include <stdexcept>
#include <string>
#include <vector>

//===------------------------------------------------------------------------===
// PRIVATE
//===------------------------------------------------------------------------===

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

std::vector<LongEntry> createLongEntries(const ShortEntry &entry,
  const std::string &name)
{
  if (!validLongName(name)) {
    std::string error = "Nome " + name + " inválido\n";
    throw std::runtime_error(error);
  }

  std::vector<LongEntry> longEntries;
  BYTE checksum = shortCheckSum(reinterpret_cast<const char *>(entry.name));
  std::vector<char> longChars = generateLongName(name);

  size_t i = 0;
  while (i < longChars.size()) {
    LongEntry lentry;
    lentry.chckSum = checksum;
    lentry.fstClusLO = 0;
    lentry.attr = ATTR_LONG_NAME;

    // O único valor especificado aqui é o do diretório que deve ser 0.
    if ((entry.attr & ATTR_DIRECTORY) == 0) {
      lentry.type = 0;
    } else {
      lentry.type = 1;
    }

    for (int j = 0; j < 10; j++, i++) {
      lentry.name1[j] = static_cast<BYTE>(longChars[i]);
    }

    for (int j = 0; j < 12; j++, i++) {
      lentry.name2[j] = static_cast<BYTE>(longChars[i]);
    }

    for (int j = 0; j < 4; j++, i++) {
      lentry.name3[j] = static_cast<BYTE>(longChars[i]);
    }

    longEntries.push_back(lentry);
  }

  std::reverse(longEntries.begin(), longEntries.end());
  for (size_t i = longEntries.size() - 1, j = 0; i >= 0; i--, j++) {
    if (i == longEntries.size() - 1) {
      longEntries[j].ord = static_cast<BYTE>(LAST_LONG_ENTRY | (i + 1));
    } else {
      longEntries[j].ord = static_cast<BYTE>(i + 1);
    }
  }
  return longEntries;
}

BYTE shortCheckSum(const char *shortName)
{
  BYTE Sum = 0;
  for (size_t i = 0; i < NAME_FULL_SIZE; i++) {
    Sum = static_cast<BYTE>(((Sum & 1) ? 0x80 : 0) + (Sum >> 1) + shortName[i]);
  }

  return Sum;
}
