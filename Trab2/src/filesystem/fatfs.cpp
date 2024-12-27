/**
 * Descrição: Implementação da interface FAT
 *
 * Autores: João Victor Briganti, Luiz Takeda
 * Licença: BSD 2
 *
 * Data: 26/12/2024
 */

#include "filesystem/fatfs.hpp"
#include "filesystem/bpb.hpp"
#include "utils/color.hpp"
#include <stdexcept>
#include <string>

//===------------------------------------------------------------------------===
// PRIVATE
//===------------------------------------------------------------------------===

bool FatFS::readCluster(void *buffer, DWORD num)
{
  if (num >= bios->getCountOfClusters()) {
    std::fprintf(stderr, "[" ERROR "] %d número inváldo de cluster\n", num);
    return false;
  }

  DWORD offset = bios->firstSectorOfCluster(num) * bios->getBytesPerSec();
  if (!image->read(offset, buffer, bios->totClusByts())) {
    std::fprintf(stderr, "[" ERROR "] Não foi possível ler cluster\n");
    return false;
  }
  return true;
}

//===------------------------------------------------------------------------===
// PUBLIC
//===------------------------------------------------------------------------===

FatFS::FatFS(std::string &path)
{
  image = new Image(path);
  if (image == nullptr) {
    std::string error = "[" ERROR "] Não foi alocar imagem";
    throw std::runtime_error(error);
  }

  bios = new BiosBlock(image);
  if (bios == nullptr) {
    std::string error = "[" ERROR "] Não foi alocar BIOS";
    throw std::runtime_error(error);
  }

  fatTable = new FatTable(image, bios);
  if (fatTable == nullptr) {
    std::string error = "[" ERROR "] Não foi alocar tabela FAT";
    throw std::runtime_error(error);
  }
}

FatFS::~FatFS()
{
  delete fatTable;
  delete bios;
  delete image;
}

void FatFS::info()
{
  bios->bpbPrint();
  fatTable->printInfo();
}

void FatFS::cluster(DWORD num)
{
  void *buffer = new char[bios->totClusByts()];
  if (buffer == nullptr) {
    std::fprintf(stderr, "[" ERROR "] Não foi possível alocar buffer\n");
    return;
  }

  if (!readCluster(buffer, num)) {
    std::fprintf(stderr, "[" ERROR "] Não foi possível ler cluster\n");
    delete[] static_cast<char *>(buffer);
    return;
  }

  char *bufferChar = static_cast<char *>(buffer);
  for (size_t i = 0; i < bios->totClusByts() * sizeof(char); i++) {
    std::fprintf(stdout, "%c", bufferChar[i]);
  }

  delete[] static_cast<char *>(buffer);
}
