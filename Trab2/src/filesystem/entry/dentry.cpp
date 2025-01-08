/**
 * Descrição: Implementação da interface de manipulação das entradas do FAT
 *
 * Autores: João Victor Briganti, Luiz Takeda
 * Licença: BSD 2
 *
 * Data: 26/12/2024
 */

#include "filesystem/entry/dentry.hpp"
#include "filesystem/entry/long_entry.hpp"
#include "filesystem/entry/short_entry.hpp"
#include "utils/logger.hpp"

#include <cctype>
#include <cstring>

Dentry::Dentry(const ShortEntry &entry,
  const std::vector<LongEntry> &lentry,
  const std::vector<ClusterIndex> &clusterIndexes)
  : entry(entry), longEntries(lentry), clusterIndexes(clusterIndexes)
{

  for (size_t i = 0; i < NAME_FULL_SIZE; i++) {
    shortName[i] = static_cast<char>(entry.name[i]);
  }
  shortName[NAME_FULL_SIZE] = '\0';

  int isDot = strncmp(shortName.data(), DOT, NAME_FULL_SIZE);
  int isDotDot = strncmp(shortName.data(), DOTDOT, NAME_FULL_SIZE);

  if ((!isDot || !isDotDot) && !lentry.empty()) {
    logger::logWarning("Entrada sem nome longo");
  }

  if (isDirectory()) {
    type = DIRECTORY;
  } else {
    type = ARCHIVE;
  }

  if (!isDot) {
    nameType = DOT_NAME;
    return;
  }

  if (!isDotDot) {
    nameType = DOTDOT_NAME;
    return;
  }

  nameType = LONG_NAME;

  BYTE checkSum = shortCheckSum(shortName.data());
  for (size_t i = 0; i < lentry.size(); i++) {

    if (i == 0 && !(lentry[i].ord & LAST_LONG_ENTRY)) {
      logger::logWarning("Entrada não está em ordem");
      nameType = SHORT_NAME;
      break;
    }

    if (lentry[i].chckSum != checkSum) {
      logger::logWarning("Checksum não condiz com a entrada");
      nameType = SHORT_NAME;
      break;
    }

    std::string name;

    for (int j = 0; j < 10; j++) {
      char chr = static_cast<char>(lentry[i].name1[j]);
      if (std::isprint(chr)) {
        name += chr;
      }
    }

    for (int j = 0; j < 12; j++) {
      char chr = static_cast<char>(lentry[i].name2[j]);
      if (std::isprint(chr)) {
        name += chr;
      }
    }

    for (int j = 0; j < 4; j++) {
      char chr = static_cast<char>(lentry[i].name3[j]);
      if (std::isprint(chr)) {
        name += chr;
      }
    }

    longName = name + longName;
  }
}

void Dentry::printInfo() const
{
  // Informações sobre tipo e permissão da entrada
  std::fprintf(stdout, "%-2c ", isDirectory() ? 'd' : 'f');
  std::fprintf(stdout, "%-2c ", isReadOnly() ? 'r' : '-');
  std::fprintf(stdout, "%-2c ", isHidden() ? '-' : 'H');

  // Informações sobre do cluster e tamanho
  std::fprintf(stdout, "cluster=%-4d  ", getDataCluster());
  std::fprintf(stdout, "size=%-8d  ", getFileSize());

  // Informações sobre data de escrita
  std::fprintf(
    stdout, "%02d-%02d-%02d  ", getWriteDay(), getWriteMonth(), getWriteYear());

  // Informações sobre hora de escrita
  std::fprintf(stdout,
    "%02d:%02d:%02d  ",
    getWriteHour(),
    getWriteMinute(),
    getWriteSeconds());

  // Nome do arquivo
  std::fprintf(stdout, "%-30s  ", getLongName().c_str());
  std::fprintf(stdout, "(%-11s)\n", getShortName());
}

void Dentry::markFree()
{
  entry.name[0] = FREE_ENTRY;

  for (auto &a : longEntries) {
    a.ord = FREE_ENTRY;
  }
}

void Dentry::updatedWrtTimestamp()
{
  entry.wrtTime = timefs::currentTime();
  entry.wrtDate = timefs::currentDate();
  entry.lstAccDate = timefs::currentDate();
}

void Dentry::setDataCluster(DWORD cluster)
{
  entry.fstClusHI = highCluster(cluster);
  entry.fstClusLO = lowCluster(cluster);
}
