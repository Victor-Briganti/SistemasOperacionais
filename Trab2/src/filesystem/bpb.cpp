/*
 * Descrição: Implementações relacionadas a BPB
 *
 * Autores: João Victor Briganti, Luiz Takeda
 * Licença: BSD 2
 *
 * Data: 26/12/2024
 */

#include "filesystem/bpb.hpp"
#include "utils/color.hpp"

#include <cstdio>
#include <stdexcept>

namespace {
BPB bpb;

/**
 * @brief Determinar a quantidade de setores no diretório raiz
 *
 * @return A quantidade de setores no dir raiz.
 */
inline DWORD rootDirSectors()
{
  DWORD countOfDirEntries = bpb.RootEntCnt * 32;
  return (countOfDirEntries + (bpb.BytsPerSec - 1)) / bpb.BytsPerSec;
}


/**
 * @brief Determinar o total de setores no FS
 *
 * @return O total de setores com base no tipo do FS.
 */
inline DWORD totSec()
{
  if (bpb.TotSec16 != 0) {
    return bpb.TotSec16;
  }

  return bpb.TotSec32;
}

/**
 * @brief Determina o tipo de FAT do sistema
 *
 * @return 12 se for um FAT12, 16 se for FAT16 e 32 se for FAT32.
 */
inline int fat_type()
{
  DWORD CountOfClusters = dataSecTotal() / bpb.SecPerClus;

  if (CountOfClusters < 4085) {
    return 12;
  } else if (CountOfClusters < 65525) {
    return 16;
  }

  return 32;
}

/**
 * @brief Primeiro setor dos dados
 *
 * @return Retorna o primeiro setor de dados
 */
inline DWORD firstDataSector()
{
  return bpb.RsvdSecCnt + (bpb.NumFATs * fatSz()) + rootDirSectors();
}


}// namespace


int bpb_init(Image &image)
{
  if (!image.read(0, static_cast<void *>(&bpb), 512)) {
    std::fprintf(stderr, "[" ERROR "] Não foi possível ler o BPB\n");
    return -1;
  }

  if (fat_type() != 32) {
    std::fprintf(stderr, "[" ERROR "] Somente FAT32 é suportado\n");
    return -1;
  }

  return 0;
}

int bpb_print()
{
  std::fprintf(stdout, "Informações do FAT\n");
  std::fprintf(stdout, "Filesystem type: FAT32\n");
  std::fprintf(stdout, "OEMName: %s\n", bpb.OEMName);
  std::fprintf(stdout, "Total sectors: %d\n", bpb.TotSec32);
  std::fprintf(
    stdout, "Total data clusters: %d\n", dataSecTotal() / bpb.SecPerClus);
  std::fprintf(stdout, "Data size: %u\n", dataSecTotal() * bpb.BytsPerSec);
  std::fprintf(stdout, "Disk size: %u\n", totSec() * bpb.BytsPerSec);
  std::fprintf(stdout, "Bytes per sector: %d\n", bpb.BytsPerSec);
  std::fprintf(stdout, "Sectors per cluster: %d\n", bpb.SecPerClus);
  std::fprintf(
    stdout, "Bytes per cluster: %d\n", bpb.SecPerClus * bpb.BytsPerSec);
  std::fprintf(stdout, "Reserved sectors: %d\n", bpb.RsvdSecCnt);
  std::fprintf(stdout, "Sectors per FAT: %d\n", fatSz());
  std::fprintf(stdout, "Fat size: %d\n", fatSz() * bpb.BytsPerSec);
  std::fprintf(stdout, "Number of FATs: %d\n", bpb.NumFATs);
  std::fprintf(stdout, "Root cluster: %d\n", bpb.RootClus);

  for (int i = 0; i < bpb.NumFATs; i++) {
    std::fprintf(stdout,
      "FAT%d start address: 0x%08x\n",
      i + 1,
      fat_sector(i) * bpb.BytsPerSec);
  }

  std::fprintf(
    stdout, "Data start address: 0x%08x\n", firstDataSector() * bpb.BytsPerSec);

  std::fprintf(stdout, "Volume label: ");
  for (int i = 0; i < 11; i++) {
    std::fprintf(stdout, "%c", bpb.VolLab[i]);
  }
  std::fprintf(stdout, "\n");

  return 0;
}

DWORD fat_sector(int num)
{
  if (num >= bpb.NumFATs || num < 0) {
    std::string error = std::string("[" ERROR "] ") + std::to_string(num)
                        + " é um número inválido para FAT\n";
    throw std::runtime_error(error);
  }

  return (bpb.RsvdSecCnt + fatSz() * static_cast<DWORD>(num));
}

BYTE num_fats() { return bpb.NumFATs; }

DWORD fatSz()
{
  if (bpb.FATSz16 != 0) {
    return bpb.FATSz16;
  }

  return bpb.FATSz32;
}

DWORD bytesPerSec() { return bpb.BytsPerSec; }

DWORD dataSecTotal()
{
  DWORD fatTotSz = bpb.NumFATs * fatSz();
  return totSec() - (bpb.RsvdSecCnt + fatTotSz + rootDirSectors());
}

DWORD secPerCluster() { return bpb.SecPerClus; }
