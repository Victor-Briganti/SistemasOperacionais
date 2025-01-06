/**
 * Descrição: Implementações da entrada com nome curto
 *
 * Autores: João Victor Briganti, Luiz Takeda
 * Licença: BSD 2
 *
 * Data: 05/01/2025
 */

#include "filesystem/entry/short_entry.hpp"
#include "filesystem/default.hpp"
#include "utils/time.hpp"

#include <cstring>
#include <stdexcept>

//===------------------------------------------------------------------------===
// PRIVATE
//===------------------------------------------------------------------------===

/**
 * @brief Dividi o cluster na parte menos significativa
 *
 * @param cluster Cluster que terá o seu valor dividido
 *
 * @return Valor menos significativo do cluster
 */
static inline WORD lowCluster(DWORD cluster)
{
  return static_cast<WORD>(cluster);
}

/**
 * @brief Dividi o cluster na parte mais significativa
 *
 * @param cluster Cluster que terá o seu valor dividido
 *
 * @return Valor mais significativo do cluster
 */
static inline WORD highCluster(DWORD cluster)
{
  return static_cast<WORD>(cluster >> 16);
}

//===------------------------------------------------------------------------===
// PUBLIC
//===------------------------------------------------------------------------===

ShortEntry createShortEntry(const std::string &name,
  DWORD fileSize,
  DWORD cluster,
  BYTE attrs)
{
  ShortEntry entry;
  if (attrs & ~ATTR_LONG_NAME_MASK) {
    throw std::runtime_error("Atributo inválido");
  }

  std::unique_ptr<BYTE[]> shortName = generateShortName(name);
  if (shortName == nullptr) {
    throw std::runtime_error("Não foi possível alocar estrutura");
  }
  memcpy(entry.name, shortName.get(), NAME_FULL_SIZE);

  entry.fileSize = fileSize;
  entry.fstClusLO = lowCluster(cluster);
  entry.fstClusHI = highCluster(cluster);
  entry.attr = attrs;
  entry.crtTime = timefs::currentTime();
  entry.crtDate = timefs::currentDate();
  entry.wrtTime = timefs::currentTime();
  entry.wrtDate = timefs::currentDate();
  entry.crtTimeTenth = timefs::currentMilliseconds();
  entry.lstAccDate = timefs::currentDate();

  return entry;
}

std::unique_ptr<BYTE[]> generateShortName(const std::string &longName)
{
  auto shortName = std::make_unique<BYTE[]>(NAME_FULL_SIZE);
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

    // Salva sempre o último ponto visto no nome do arquivo
    if (longName[i] == '.') {
      period = static_cast<int>(treatedName.size());
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
  if (period != -1) {
    // Realiza o padding na parte principal do nome
    for (auto i = static_cast<size_t>(period); i < NAME_MAIN_SIZE;
         i++, period++) {
      treatedName = treatedName.substr(0, i) + " "
                    + treatedName.substr(i, treatedName.size());
    }
  } else {
    // Realiza o padding no final do nome
    while (treatedName.size() < NAME_FULL_SIZE) {
      treatedName.push_back(' ');
    }
  }

  // Adiciona o tail do nome
  if (treatedName.size() > NAME_FULL_SIZE) {
    treatedName[NUM_TAIL_POS] = '~';
    treatedName[NUM_TAIL_POS + 1] = '1';
  }

  // Copia a parte principal do nome
  for (size_t i = 0; i < NAME_MAIN_SIZE; i++) {
    shortName[i] = static_cast<BYTE>(treatedName[i]);
  }

  // Copia a extensão do nome. Se existir uma
  if (period != -1) {
    for (size_t i = static_cast<size_t>(period), j = NAME_MAIN_SIZE;
         i < static_cast<size_t>(period) + NAME_EXT_SIZE;
         i++, j++) {
      if (i > treatedName.size()) {
        shortName[j] = ' ';
      } else {
        shortName[j] = static_cast<BYTE>(treatedName[i]);
      }
    }
  } else {
    for (size_t i = NAME_MAIN_SIZE; i < NAME_FULL_SIZE; i++) {
      shortName[i] = ' ';
    }
  }

  return shortName;
}

bool randomizeShortname(char *shortName)
{
  if (shortName == nullptr) {
    return false;
  }

  srand(static_cast<unsigned int>(time(NULL)));

  for (int i = 0; i < NUM_TAIL_POS; i++) {
    if (rand() % NUM_TAIL_POS) {
      shortName[i] = static_cast<char>(rand() % ('Z' - 'A') + 'A');
    } else {
      shortName[i] = static_cast<char>(rand() % ('9' - '0') + '0');
    }
  }

  return true;
}
