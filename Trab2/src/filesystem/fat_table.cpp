/**
 * Descrição: Implementação da estrutura FAT Table
 *
 * Autores: João Victor Briganti, Luiz Takeda
 * Licença: BSD 2
 *
 * Data: 26/12/2024
 */

#include "filesystem/fat_table.hpp"
#include "filesystem/bpb.hpp"
#include "utils/color.hpp"

#include <cstdlib>
#include <stdexcept>
#include <vector>

//===------------------------------------------------------------------------===
// PRIVATE
//===------------------------------------------------------------------------===

bool FatTable::readFatTable(const int num)
{
  if (num < 0 || num >= bios->getNumFATs()) {
    return false;
  }

  const DWORD fatOffset = bios->fatSector(num) * bios->getBytesPerSec();
  const DWORD fatSize = bios->getFATSz() * bios->getBytesPerSec();

  return this->image->read(fatOffset, table, fatSize);
}

bool FatTable::writeFatTable(const int num)
{
  if (num < 0 || num >= bios->getNumFATs()) {
    return false;
  }

  const DWORD fatOffset = bios->fatSector(num) * bios->getBytesPerSec();
  const DWORD fatSize = bios->getFATSz() * bios->getBytesPerSec();

  return this->image->write(fatOffset, table, fatSize);
}

DWORD FatTable::readFromTable(const DWORD offset) const
{
  return static_cast<DWORD *>(table)[offset] & LSB_MASK;
}

void FatTable::writeInTable(const DWORD offset, const DWORD value)
{
  static_cast<DWORD *>(table)[offset] &= MSB_MASK;
  static_cast<DWORD *>(table)[offset] |= (value & LSB_MASK);
}

//===------------------------------------------------------------------------===
// PUBLIC
//===------------------------------------------------------------------------===


FatTable::FatTable(Image *image, BiosBlock *bios) : image(image), bios(bios)
{
  table = new char[sizeof(void *) * bios->getFATSz() * bios->getBytesPerSec()];
  if (table == nullptr) {
    std::string error = "[" ERROR "] Não foi possível alocar a FAT table\n";
    throw std::runtime_error(error);
  }

  if (!readFatTable(0)) {
    std::string error = "[" ERROR "] Não foi possível ler a FAT table\n";
    throw std::runtime_error(error);
  }
}

FatTable::~FatTable() { delete[] static_cast<char *>(this->table); }

void FatTable::printTable() const
{
  const BYTE *byteTable = static_cast<BYTE *>(table);
  for (DWORD i = 0; i < bios->getFATSz(); i++) {

    std::fprintf(stdout, "%02x", byteTable[i]);

    if (i % 2 == 1 && (i + 1) % 16 != 0 && i != 0) {
      std::fprintf(stdout, " ");
    }

    if ((i + 1) % 16 == 0) {
      std::fprintf(stdout, "\n");
    }
  }

  std::fprintf(stdout, "\n");
}

void FatTable::printInfo() const
{
  const DWORD totalClus = bios->getDataSecTotal() / bios->getSecPerCluster();

  std::fprintf(stdout,
    "Free clusters: %u/%u\n",
    totalClus - static_cast<DWORD>(usedClusters()),
    totalClus);

  std::fprintf(stdout,
    "Free space: %u\n",
    (totalClus - static_cast<DWORD>(usedClusters())) * bios->getBytesPerSec());

  std::fprintf(stdout,
    "Used space: %u\n",
    static_cast<DWORD>(usedClusters()) * bios->getBytesPerSec());
}

std::vector<DWORD> FatTable::listChain(DWORD start)
{
  // Arquivo recém criado que não tem nada escrito
  if (start == 0) {
    return {};
  }

  std::vector<DWORD> chain;
  chain.push_back(start);

  DWORD value = readFromTable(start);
  while (value < EOC) {
    chain.push_back(value);
    value = readFromTable(value);
  }

  return chain;
}

int FatTable::removeChain(DWORD start)
{
  std::vector<DWORD> chain = listChain(start);
  if (chain.empty()) {
    return 0;
  }

  for (const auto &a : chain) {
    writeInTable(a, FREE_CLUSTER);
  }

  for (int i = 0; i < bios->getNumFATs(); i++) {
    if (!writeFatTable(i)) {
      return -1;
    }
  }

  return static_cast<int>(chain.size());
}

DWORD FatTable::usedClusters() const
{
  const auto *dwordTable = static_cast<DWORD *>(table);

  DWORD totalClus = bios->getDataSecTotal() / bios->getSecPerCluster();
  DWORD count = 0;

  for (DWORD i = 0; i < totalClus; i++) {
    if (dwordTable[i] != 0) {
      count++;
    }
  }

  // Cluster 1 e 2 sempre estão alocados
  return count - 2;
}

DWORD FatTable::freeClusters() const
{
  DWORD totalClus = bios->getDataSecTotal() / bios->getSecPerCluster();
  return totalClus - usedClusters();
}
