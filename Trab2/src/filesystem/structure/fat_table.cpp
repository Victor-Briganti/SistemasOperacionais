/**
 * Descrição: Implementação da estrutura FAT Table
 *
 * Autores: João Victor Briganti, Luiz Gustavo Takeda, Matheus Floriano Saito
 * Licença: BSD 2
 *
 * Data: 26/12/2024
 */

#include "filesystem/structure/fat_table.hpp"
#include "filesystem/structure/bpb.hpp"
#include "utils/logger.hpp"

#include <cstdlib>
#include <memory>
#include <stdexcept>
#include <vector>

//===------------------------------------------------------------------------===
// PRIVATE
//===------------------------------------------------------------------------===

bool FatTable::readFatTable(int num)
{
  if (num < 0 || num >= bios->getNumFATs()) {
    return false;
  }

  const DWORD fatOffset = bios->fatSector(num) * bios->getBytesPerSec();
  const DWORD fatSize = bios->getFATSz() * bios->getBytesPerSec();

  return this->image->read(fatOffset, table.get(), fatSize);
}

bool FatTable::writeFatTable(int num)
{
  if (num < 0 || num >= bios->getNumFATs()) {
    return false;
  }

  const DWORD fatOffset = bios->fatSector(num) * bios->getBytesPerSec();
  const DWORD fatSize = bios->getFATSz() * bios->getBytesPerSec();

  return this->image->write(fatOffset, table.get(), fatSize);
}

DWORD FatTable::readFromTable(DWORD offset) const
{
  return reinterpret_cast<DWORD *>(table.get())[offset] & LSB_MASK;
}

void FatTable::writeInTable(const DWORD offset, const DWORD value)
{
  reinterpret_cast<DWORD *>(table.get())[offset] &= MSB_MASK;
  reinterpret_cast<DWORD *>(table.get())[offset] |= (value & LSB_MASK);
}

//===------------------------------------------------------------------------===
// PUBLIC
//===------------------------------------------------------------------------===


FatTable::FatTable(std::shared_ptr<Image> image,
  std::shared_ptr<BiosBlock> bios)
  : image(image), bios(bios)
{
  table = std::make_unique<BYTE[]>(bios->getFATSz() * bios->getBytesPerSec());
  if (table == nullptr) {
    std::string error = "Não foi possível alocar a FAT table\n";
    throw std::runtime_error(error);
  }

  if (!readFatTable(0)) {
    std::string error = "Não foi possível ler a FAT table\n";
    throw std::runtime_error(error);
  }
}

void FatTable::printTable() const
{
  const BYTE *byteTable = static_cast<BYTE *>(table.get());
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
  const auto *dwordTable = reinterpret_cast<DWORD *>(table.get());

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

bool FatTable::searchFreeEntry(DWORD &num)
{
  // Total de clusters na tablea
  DWORD totalClus = bios->getDataSecTotal() / bios->getSecPerCluster();

  // Salva o cluster que está livre
  DWORD savedCluster = 0;

  // Busca por um cluster que esteja livre
  DWORD i = (num + 1) % totalClus;
  while (i != num) {
    if (i == 0 || i == 1) {
      i = 2;
    }

    if (readFromTable(i) == 0) {
      savedCluster = i;
      break;
    }

    i++;
  }

  if (savedCluster != 0) {
    num = savedCluster;
    return true;
  }

  return false;
}

bool FatTable::allocClusters(DWORD tail, const std::vector<DWORD> &clusters)
{
  // Sobreescreve o inicio do tail para apontar para a próxima cadeia
  writeInTable(tail, clusters[0]);

  // Escreve a nova cadeia de clusters
  for (size_t i = 0; i < clusters.size(); i++) {
    if (i < clusters.size() - 1) {
      writeInTable(clusters[i], clusters[i + 1]);
    } else {
      writeInTable(clusters[i], EOC);
    }
  }

  // Salva a tabela FAT
  for (int i = 0; i < bios->getNumFATs(); i++) {
    if (!writeFatTable(i)) {
      return false;
    }
  }

  return true;
}

size_t FatTable::maxFreeClusByts()
{
  return freeClusters() * bios->totClusByts();
}

std::vector<DWORD> FatTable::searchListFreeEntry(DWORD initPos, DWORD num)
{
  // Cadeia de clusters livres
  std::vector<DWORD> chain;

  // Total de clusters na tablea
  DWORD totalClus = bios->getDataSecTotal() / bios->getSecPerCluster();

  // Busca por um cluster que esteja livre
  DWORD i = (initPos + 1) % totalClus;
  while (i != initPos) {
    if (i == 0 || i == 1) {
      i = 2;
    }

    if (readFromTable(i) == 0) {
      chain.push_back(i);
      if (chain.size() == num) {
        break;
      }
    }

    i++;
  }

  if (chain.size() != num) {
    return {};
  }

  return chain;
}
