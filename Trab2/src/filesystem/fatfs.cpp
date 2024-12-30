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
#include "filesystem/dentry.hpp"
#include "filesystem/dir.hpp"
#include "path/parser.hpp"
#include "utils/color.hpp"

#include <cstring>
#include <stdexcept>
#include <string>
#include <vector>

//===------------------------------------------------------------------------===
// PRIVATE
//===------------------------------------------------------------------------===

bool FatFS::readCluster(void *buffer, DWORD num)
{
  if (num >= bios->getCountOfClusters()) {
    std::fprintf(stderr, "[" ERROR "] %d número inváldo de cluster\n", num);
    return false;
  }

  const DWORD offset = bios->firstSectorOfCluster(num) * bios->getBytesPerSec();
  if (!image->read(offset, buffer, bios->totClusByts())) {
    std::fprintf(stderr, "[" ERROR "] Não foi possível ler cluster\n");
    return false;
  }
  return true;
}

bool FatFS::writeCluster(const void *buffer, DWORD num)
{
  if (num >= bios->getCountOfClusters()) {
    std::fprintf(stderr, "[" ERROR "] %d número inváldo de cluster\n", num);
    return false;
  }

  DWORD offset = bios->firstSectorOfCluster(num) * bios->getBytesPerSec();
  if (!image->write(offset, buffer, bios->totClusByts())) {
    std::fprintf(stderr, "[" ERROR "] Não foi possível escrever cluster\n");
    return false;
  }
  return true;
}

DWORD FatFS::getEntryClus(const Dentry &entry)
{
  if (entry.getNameType() == DOTDOT_NAME && entry.getCluster() == 0) {
    return bios->getRootClus();
  }

  return entry.getCluster();
}

std::vector<Dentry> FatFS::getDirEntries(DWORD num)
{
  // Aloca o buffer do diretório
  void *buffer = calloc(sizeof(char), bios->totClusByts());
  if (buffer == nullptr) {
    std::fprintf(stderr, "[" ERROR "] Não foi possível alocar buffer\n");
    return {};
  }

  // Lê o cluster no qual o diretório se encontra
  if (!readCluster(buffer, num)) {
    std::fprintf(stderr, "[" ERROR "] Não foi possível ler cluster\n");
    delete[] static_cast<char *>(buffer);
    return {};
  }

  // Cast para facilitar manuseio do buffer
  Dir *bufferDir = static_cast<Dir *>(buffer);

  // Lista com diretórios de nomes longos
  std::vector<LongDir> longDirs;

  // Vetor com todas as entradas
  std::vector<Dentry> dentries;

  // Define a posição da entrada no buffer
  DWORD bufferPos = 0;

  // Caminha por todo o buffer até encontrar o FREE_ENTRY ou alcançar o limite
  // de tamanho do cluster.
  for (size_t i = 0; i < (bios->totClusByts() / sizeof(Dir)); i++) {
    // Final do diretório
    if (bufferDir[i].name[0] == EOD_ENTRY) {
      break;
    }

    // Diretório vazio
    if (bufferDir[i].name[0] == FREE_ENTRY) {
      continue;
    }

    if (bufferDir[i].attr == ATTR_LONG_NAME) {
      if (bufferPos == 0) {
        bufferPos = static_cast<DWORD>(i);
      }

      LongDir ldir;
      memcpy(&ldir, &bufferDir[i], sizeof(ldir));
      longDirs.push_back(ldir);
    } else {
      Dentry entry(bufferDir[i],
        longDirs,
        static_cast<DWORD>(bufferPos),
        static_cast<DWORD>(i));

      dentries.push_back(entry);
      longDirs.clear();
      bufferPos = 0;
    }
  }

  delete[] static_cast<char *>(buffer);
  return dentries;
}

bool FatFS::setDirEntries(DWORD num,
  DWORD startPos,
  DWORD endPos,
  const Dir &dir,
  const std::vector<LongDir> &ldir)
{
  // Aloca o buffer do diretório
  void *buffer = calloc(sizeof(char), bios->totClusByts());
  if (buffer == nullptr) {
    std::fprintf(stderr, "[" ERROR "] Não foi possível alocar buffer\n");
    return {};
  }

  // Lê o cluster no qual o diretório se encontra
  if (!readCluster(buffer, num)) {
    std::fprintf(stderr, "[" ERROR "] Não foi possível ler cluster\n");
    delete[] static_cast<char *>(buffer);
    return {};
  }

  // Cast para facilitar manuseio do buffer
  Dir *bufferDir = static_cast<Dir *>(buffer);

  // Caminha pelo buffer escrevendo os novos valores.
  for (size_t i = startPos, j = 0; i <= endPos; i++, j++) {
    if (j < ldir.size()) {
      memcpy(&bufferDir[i], &ldir[j], sizeof(dir));
    } else {
      memcpy(&bufferDir[i], &dir, sizeof(dir));
    }
  }

  bool result = writeCluster(buffer, num);
  delete[] static_cast<char *>(buffer);
  return result;
}

//===------------------------------------------------------------------------===
// PUBLIC
//===------------------------------------------------------------------------===

FatFS::FatFS(const std::string &path)
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

// TODO: Ajustar está função para aceitar o raiz como se fosse um dir normal
void FatFS::ls(const std::string &path)
{
  std::vector<std::string> listPath = split(path, '/');
  std::vector<Dentry> dentries;

  // Lista caminhos longos
  if (listPath[0] == "img") {
    DWORD clt = bios->getRootClus();
    bool found = false;

    dentries = getDirEntries(clt);
    for (size_t i = 1; i < listPath.size(); i++, found = false) {
      // Caminha por todas as entradas até encontrar o diretório
      for (const auto &a : dentries) {
        // Se a última parte do caminho não for um diretório gera um erro.
        if (listPath[i] == a.getLongName()) {
          if (!a.isDirectory()) {
            goto notDir;
          }

          clt = getEntryClus(a);
          found = true;
          break;
        }
      }

      if (!found) {
        goto notFound;
      }

      dentries = getDirEntries(clt);
    }


    for (const auto &a : dentries) {
      a.printInfo();
    }

    return;
  }

  // TODO: Fazer caminho relativo

notFound:
  std::fprintf(stderr, "[" ERROR "] %s não encontrado\n", path.c_str());
  return;
notDir:
  std::fprintf(stderr, "[" ERROR "] %s não é um diretório\n", path.c_str());
}

void FatFS::rm(const std::string &path)
{
  std::vector<std::string> listPath = split(path, '/');
  if (listPath.size() == 1) {
    goto notArchive;
  }

  // Caminho completo
  if (listPath.size() != 1 && listPath[0] == "img") {
    bool found = false;
    DWORD clt = bios->getRootClus();
    std::vector<Dentry> dentries = getDirEntries(clt);

    for (size_t i = 1; i < listPath.size(); i++, found = false) {
      for (auto &a : dentries) {
        if (listPath[i] == a.getLongName()) {
          // Se a última parte do caminho for um diretório gera um erro
          if (i == listPath.size() - 1 && a.isDirectory()) {
            goto notArchive;
          }

          // Se o arquivo foi encontrado deleta suas entradas.
          if (i == listPath.size() - 1 && !a.isDirectory()) {
            a.markFree();
            if (!fatTable->removeChain(a.getCluster())
                || !setDirEntries(clt,
                  a.initPos,
                  a.endPos,
                  a.getDirectory(),
                  a.getLongDirectories())) {
              goto fail;
            }
            return;
          }

          if (!a.isDirectory()) {
            goto invalidPath;
          }

          clt = getEntryClus(a);
          found = true;
          break;
        }
      }

      if (!found) {
        goto notFound;
      }

      dentries = getDirEntries(clt);
    }

    return;
  }

  // TODO: Fazer caminho relativo

invalidPath:
  std::fprintf(stderr, "[" ERROR "] %s caminho inválido\n", path.c_str());
  return;
notFound:
  std::fprintf(stderr, "[" ERROR "] %s não encontrado\n", path.c_str());
  return;
notArchive:
  std::fprintf(stderr, "[" ERROR "] %s não é um arquivo\n", path.c_str());
  return;
fail:
  std::fprintf(stderr, "[" ERROR "] rm %s operação falhou\n", path.c_str());
}
