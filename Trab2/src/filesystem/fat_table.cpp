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

FatTable::FatTable(Image *image)
  : image(image), numFATs(num_fats()), tableSize(fatSz() * bytesPerSec())
{
  table = calloc(this->tableSize, sizeof(void *));
  if (table == nullptr) {
    throw std::runtime_error(
      "[" ERROR "] Não foi possível alocar a FAT table\n");
  }

  if (!read_table(1)) {
    throw std::runtime_error("[" ERROR "] Não foi possível ler a FAT table\n");
  }
}

FatTable::~FatTable() { free(table); }

void FatTable::print_table()
{
  BYTE *byteTable = static_cast<BYTE *>(table);
  for (int i = 0; i < tableSize; i++) {

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

void FatTable::print_info()
{
  DWORD totalSec = dataSecTotal() / secPerCluster();
  std::fprintf(stdout, "Free clusters: %u/%u\n", totalSec - used(), totalSec);
  std::fprintf(stdout, "Free space: %u\n", (totalSec - used()) * bytesPerSec());
  std::fprintf(stdout, "Used space: %u\n", used() * bytesPerSec());
}

bool FatTable::read_table(int num)
{
  if (num < 0 || num >= numFATs) {
    return false;
  }

  return this->image->read(fat_sector(1) * bytesPerSec(), table, tableSize);
}

DWORD FatTable::used()
{
  DWORD *dwordTable = static_cast<DWORD *>(table);

  DWORD count = 0;
  for (int i = 0; i < tableSize; i++) {
    if (dwordTable[i] != 0) {
      count++;
    }
  }

  return count;
}