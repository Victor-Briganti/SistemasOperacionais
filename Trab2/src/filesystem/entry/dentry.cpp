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

  if (!isDot) {
    type = DOT_NAME;
    return;
  }

  if (!isDotDot) {
    type = DOTDOT_NAME;
    return;
  }

  type = LONG_NAME;

  BYTE checkSum = shortCheckSum(shortName.data());
  for (auto a : lentry) {
    if (a.chckSum != checkSum) {
      logger::logWarning("Checksum não condiz com a entrada");
      longEntries.clear();
      break;
    }

    std::string name;

    for (int i = 0; i < 10; i++) {
      char chr = static_cast<char>(a.name1[i]);
      if (std::isprint(chr)) {
        name += chr;
      }
    }

    for (int i = 0; i < 12; i++) {
      char chr = static_cast<char>(a.name2[i]);
      if (std::isprint(chr)) {
        name += chr;
      }
    }

    for (int i = 0; i < 4; i++) {
      char chr = static_cast<char>(a.name3[i]);
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
  std::fprintf(stdout, "cluster=%-4d  ", getCluster());
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
