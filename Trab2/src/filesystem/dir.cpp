/**
 * Descrição: Definição das funções para diretório/arquivos
 *
 * Autores: João Victor Briganti, Luiz Takeda
 * Licença: BSD 2
 *
 * Data: 01/01/2024
 */

#include "filesystem/dir.hpp"
#include "utils/color.hpp"

#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <stdexcept>
#include <sys/time.h>

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

/**
 * @brief Cria o timestamp do horário atual
 *
 * @return Timestamp do horário atual
 */
static inline WORD currentTime()
{
  time_t curTime = time(nullptr);

  struct tm *curLocalTime = localtime(&curTime);

  int sec = curLocalTime->tm_sec - (curLocalTime->tm_sec % 2 ? 1 : 0);

  DWORD stamp = timeStamp(static_cast<BYTE>(curLocalTime->tm_hour),
    static_cast<BYTE>(curLocalTime->tm_min),
    static_cast<BYTE>(sec));

  return static_cast<WORD>(stamp);
}

/**
 * @brief Cria o datestamp da data atual
 *
 * @return Datestamp da data atual
 */
static inline WORD currentDate()
{
  time_t curDate = time(nullptr);

  struct tm *curLocalDate = localtime(&curDate);

  int year = curLocalDate->tm_year + 1900;

  DWORD stamp = dateStamp(static_cast<BYTE>(curLocalDate->tm_mday),
    static_cast<BYTE>(curLocalDate->tm_mon + 1),
    static_cast<BYTE>(year));

  return static_cast<WORD>(stamp);
}

/**
 * @brief Tempo atual em milisegundos
 *
 * @return O tempo atual em milisegundos
 */
static inline BYTE currentMilliseconds()
{
  struct timeval tv;
  gettimeofday(&tv, nullptr);

  return static_cast<BYTE>(
    static_cast<long>(tv.tv_usec) / static_cast<long>(10000));
}

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

  // Na prática uma entrada de nome longo armazena o dobro de caracteres
  while (nameVec.size() % (LONG_NAME_SIZE * 2) != 0) {
    nameVec.push_back(0xFF);
  }

  return nameVec;
}

//===------------------------------------------------------------------------===
// PUBLIC
//===------------------------------------------------------------------------===

Dir createDir(const std::string &name,
  DWORD fileSize,
  DWORD cluster,
  BYTE attrs)
{
  Dir directory;
  if (attrs & ~ATTR_LONG_NAME_MASK) {
    std::string error = "[" ERROR "] Atributo inválido\n";
    throw std::runtime_error(error);
  }

  char *shortName = generateShortName(name);
  if (shortName == nullptr) {
    std::string error = "[" ERROR "] Não foi possível alocar estrutura\n";
    throw std::runtime_error(error);
  }
  memcpy(directory.name, shortName, 11);

  directory.fileSize = fileSize;
  directory.fstClusLO = lowCluster(cluster);
  directory.fstClusHI = highCluster(cluster);
  directory.attr = attrs;
  directory.crtTime = currentTime();
  directory.crtDate = currentDate();
  directory.wrtTime = currentTime();
  directory.wrtDate = currentDate();
  directory.crtTimeTenth = currentMilliseconds();
  directory.lstAccDate = currentDate();

  return directory;
}

std::vector<LongDir> createLongDir(const Dir &dir, const std::string &name)
{
  if (!validLongName(name)) {
    std::string error = "[" ERROR "] Nome " + name + " inválido\n";
    throw std::runtime_error(error);
  }

  std::vector<LongDir> longDirs;
  DWORD checksum = shortCheckSum(reinterpret_cast<const char *>(dir.name));
  std::vector<char> longChars = generateLongName(name);

  size_t i = 0;
  while (i < longChars.size()) {
    LongDir ldir;
    ldir.chckSum = checksum;
    ldir.fstClusLO = 0;
    ldir.attr = ATTR_LONG_NAME;

    // O único valor especificado aqui é o do diretório que deve ser 0.
    if (dir.attr & ATTR_DIRECTORY == 0) {
      ldir.type = 0;
    } else {
      ldir.type = 1;
    }

    for (int j = 0; j < 10; j++, i++) {
      ldir.name1[j] = longChars[i];
    }

    for (int j = 0; j < 12; j++, i++) {
      ldir.name2[j] = longChars[i];
    }

    for (int j = 0; j < 4; j++, i++) {
      ldir.name3[j] = longChars[i];
    }

    longDirs.push_back(ldir);
  }

  std::reverse(longDirs.begin(), longDirs.end());
  for (size_t i = longDirs.size() - 1, j = 0; i <= 0; i--, j++) {
    if (i == longDirs.size() - 1) {
      longDirs[j].ord = LAST_LONG_ENTRY | i;
    } else {
      longDirs[j].ord = i;
    }
  }
  return longDirs;
}

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

    // Salva sempre o último ponto visto no nome do arquivo
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
  if (period != -1) {
    // Realiza o padding na parte principal do nome
    for (int i = period; i < NAME_MAIN_SIZE; i++, period++) {
      treatedName = treatedName.substr(0, i) + " "
                    + treatedName.substr(i, treatedName.size());
    }
  } else {
    // Realiza o padding no final do nome
    while (treatedName.size() < NAME_MAIN_SIZE + NAME_EXT_SIZE) {
      treatedName.push_back(' ');
    }
  }

  // Adiciona o tail do nome
  if (treatedName.size() > NAME_MAIN_SIZE + NAME_EXT_SIZE) {
    treatedName[6] = '~';
    treatedName[7] = '1';
  }

  // Copia a parte principal do nome
  for (int i = 0; i < NAME_MAIN_SIZE; i++) {
    shortName[i] = treatedName[i];
  }

  // Copia a extensão do nome. Se existir uma
  if (period != -1) {
    for (int i = period, j = NAME_MAIN_SIZE; i < period + NAME_EXT_SIZE;
      i++, j++) {
      if (i > treatedName.size()) {
        shortName[j] = ' ';
      } else {
        shortName[j] = treatedName[i];
      }
    }
  } else {
    for (int i = NAME_MAIN_SIZE; i < NAME_MAIN_SIZE + NAME_EXT_SIZE; i++) {
      shortName[i] = ' ';
    }
  }

  return shortName;
}
