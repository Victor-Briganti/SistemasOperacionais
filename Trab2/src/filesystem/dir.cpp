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
  if (treatedName.size() < NAME_MAIN_SIZE + NAME_EXT_SIZE) {
    // Realiza o padding na parte principal do nome
    for (int i = period; period != -1 && i < NAME_MAIN_SIZE; i++, period++) {
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
