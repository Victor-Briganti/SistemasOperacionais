/**
 * Descrição: Implementações relacionadas a BPB
 *
 * Autores: João Victor Briganti, Luiz Takeda
 * Licença: BSD 2
 *
 * Data: 26/12/2024
 */

#include "filesystem/structure/bpb.hpp"
#include "io/image.hpp"

#include <cstdio>
#include <stdexcept>


//===------------------------------------------------------------------------===
// PRIVATE
//===------------------------------------------------------------------------===

int BiosBlock::fatType() const
{
  if (this->countOfClusters < 4085) {
    return 12;
  }

  if (this->countOfClusters < 65525) {
    return 16;
  }

  return 32;
}


int BiosBlock::bpbInit()
{
  // Determina o tamanho do FAT
  if (bpb.FATSz16 != 0) {
    this->fatSz = bpb.FATSz16;
  } else {
    this->fatSz = bpb.FATSz32;
  }

  // Determina o total de setores
  if (bpb.TotSec16 != 0) {
    this->totSec = bpb.TotSec16;
  } else {
    this->totSec = bpb.TotSec32;
  }

  // Determina a quantidade de setores no diretório root
  DWORD countOfDirEntries = bpb.RootEntCnt * 32;
  this->rootDirSectors =
    (countOfDirEntries + (bpb.BytsPerSec - 1)) / bpb.BytsPerSec;

  // Determina a quantidade de setores na região de dados
  DWORD fatTotSz = bpb.NumFATs * this->fatSz;
  this->dataSecTotal =
    this->totSec - (bpb.RsvdSecCnt + fatTotSz + this->rootDirSectors);

  // Determina a quantidade de clusters no sistema
  this->countOfClusters = this->dataSecTotal / bpb.SecPerClus;

  // Determina o primeiro setor de dados
  this->firstDataSector =
    bpb.RsvdSecCnt + (bpb.NumFATs * this->fatSz) + this->rootDirSectors;

  return 0;
}

//===------------------------------------------------------------------------===
// PUBLIC
//===------------------------------------------------------------------------===

BiosBlock::BiosBlock(const std::shared_ptr<Image> &image)
{
  if (!image->read(0, static_cast<void *>(&bpb), sizeof(bpb))) {
    std::string error = "Não foi possível ler o BPB\n";
    throw std::runtime_error(error);
  }

  bpbInit();

  if (fatType() != 32) {
    std::string error = "Somente o FAT32 é suportado\n";
    throw std::runtime_error(error);
  }
}

void BiosBlock::bpbPrint() const
{
  std::fprintf(stdout, "Informações do FAT\n");
  std::fprintf(stdout, "Filesystem type: FAT32\n");
  std::fprintf(stdout, "OEMName: %s\n", bpb.OEMName);
  std::fprintf(stdout, "Total sectors: %d\n", bpb.TotSec32);
  std::fprintf(
    stdout, "Total data clusters: %d\n", dataSecTotal / bpb.SecPerClus);
  std::fprintf(stdout, "Data size: %u\n", dataSecTotal * bpb.BytsPerSec);
  std::fprintf(stdout, "Disk size: %u\n", totSec * bpb.BytsPerSec);
  std::fprintf(stdout, "Bytes per sector: %d\n", bpb.BytsPerSec);
  std::fprintf(stdout, "Sectors per cluster: %d\n", bpb.SecPerClus);
  std::fprintf(
    stdout, "Bytes per cluster: %d\n", bpb.SecPerClus * bpb.BytsPerSec);
  std::fprintf(stdout, "Reserved sectors: %d\n", bpb.RsvdSecCnt);
  std::fprintf(stdout, "Sectors per FAT: %d\n", fatSz);
  std::fprintf(stdout, "Fat size: %d\n", fatSz * bpb.BytsPerSec);
  std::fprintf(stdout, "Number of FATs: %d\n", bpb.NumFATs);
  std::fprintf(stdout, "Root cluster: %d\n", bpb.RootClus);

  for (int i = 0; i < bpb.NumFATs; i++) {
    std::fprintf(stdout,
      "FAT%d start address: 0x%08x\n",
      i + 1,
      fatSector(i) * bpb.BytsPerSec);
  }

  std::fprintf(
    stdout, "Data start address: 0x%08x\n", firstDataSector * bpb.BytsPerSec);

  std::fprintf(stdout, "Volume label: ");
  for (int i = 0; i < 11; i++) {
    std::fprintf(stdout, "%c", bpb.VolLab[i]);
  }
  std::fprintf(stdout, "\n");
}

DWORD BiosBlock::fatSector(int num) const
{
  if (num >= bpb.NumFATs || num < 0) {
    std::string error = std::to_string(num) + " é uma FAT inválida\n";
    throw std::runtime_error(error);
  }

  return (bpb.RsvdSecCnt + this->fatSz * static_cast<DWORD>(num));
}
