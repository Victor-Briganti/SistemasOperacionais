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
#include <exception>
#include <iostream>
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
  void *buffer = new char[bios->totClusByts()];
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
  void *buffer = new char[bios->totClusByts()];
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

bool FatFS::removeEntry(Dentry &entry, DWORD num)
{
  entry.markFree();
  bool fatRm = fatTable->removeChain(entry.getCluster());
  bool entryRm = setDirEntries(num,
    entry.getInitPos(),
    entry.getEndPos(),
    entry.getDirectory(),
    entry.getLongDirectories());

  return fatRm && entryRm;
}

std::vector<std::string> FatFS::pathParser(const std::string &path,
  bool expectDir)
{
  // Lista de nomes nos caminhos
  std::vector<std::string> listPath = split(path, '/');

  if (path[0] == '/') {
    std::string error = "[" ERROR "] " + path + " caminho inválido\n";
    throw std::runtime_error(error);
  }

  // Se necessário monta o caminho completo
  if (listPath[0] != "img") {
    std::vector<std::string> fullPath = split(curPath, '/');
    fullPath.insert(fullPath.end(), listPath.begin(), listPath.end());
    listPath = fullPath;
  }

  // Caminho que será criado
  std::vector<std::string> newPath;
  newPath.emplace_back("img");

  // Começa a busca pelo "/"
  DWORD clt = bios->getRootClus();
  bool found = false;

  // Lista de entradas
  std::vector<Dentry> dentries = getDirEntries(clt);

  // Caminha pela lista de nomes
  for (size_t i = 1; i < listPath.size(); i++, found = false) {
    // Caminha por todas as entradas até encontrar o diretório
    for (const auto &a : dentries) {
      if (listPath[i] == a.getLongName()) {
        if (i == listPath.size() - 1 && (expectDir != a.isDirectory())) {
          std::string error = "[" ERROR "] " + path + " esperava um ";
          error += expectDir ? "diretório\n" : "arquivo\n";
          throw std::runtime_error(error);
        }

        if ((i != listPath.size() - 1) && !a.isDirectory()) {
          std::string error = "[" ERROR "] " + path + " caminho inválido\n";
          throw std::runtime_error(error);
        }

        if (a.getNameType() == DOT_NAME) {
          found = true;
          break;
        }

        if (a.getNameType() == DOTDOT_NAME && newPath.size() == 1) {
          std::string error = "[" ERROR "] " + path + " caminho inválido\n";
          throw std::runtime_error(error);
        }

        if (a.getNameType() == DOTDOT_NAME && newPath.back() != "img") {
          newPath.pop_back();
        } else {
          newPath.push_back(a.getLongName());
        }

        // Altera o cluster em que ocorre a busca
        clt = getEntryClus(a);
        found = true;
        break;
      }
    }

    // Se não foi encontrado, gera um erro
    if (!found) {
      std::string error = "[" ERROR "] " + path + " caminho inválido\n";
      throw std::runtime_error(error);
    }

    // Atualiza a lista de entradas
    dentries = getDirEntries(clt);
  }

  return newPath;
}

std::pair<Dentry, DWORD>
  FatFS::searchEntry(const std::vector<std::string> &listPath, bool expectDir)
{
  // Começa a busca pelo "/"
  DWORD clt = bios->getRootClus();
  bool found = false;

  // Lista de entradas
  std::vector<Dentry> dentries = getDirEntries(clt);

  // Entrada e cluster a ser retornado
  std::pair<Dentry, DWORD> entry = { dentries[0], 0 };

  // Caminha pela lista de nomes
  for (size_t i = 1; i < listPath.size(); i++, found = false) {
    for (auto &a : dentries) {
      if (listPath[i] == a.getLongName()) {

        // Última parte do caminho está de acordo com o tipo de arquivo
        if (i == listPath.size() - 1 && expectDir != a.isDirectory()) {
          std::string error = "[" ERROR "] esperava um ";
          error += a.isDirectory() ? "diretório\n" : "arquivo\n";
          throw std::runtime_error(error);
        }

        // Se no meio do caminho temos um arquivo gera um erro
        if ((i != listPath.size() - 1) && !a.isDirectory()) {
          std::string error =
            "[" ERROR "] " + merge(listPath) + " caminho inválido\n";
          throw std::runtime_error(error);
        }

        // Atualiza a entrada
        entry.first = a;
        entry.second = clt;

        // Altera o cluster em que ocorre a busca
        clt = getEntryClus(a);
        found = true;
        break;
      }
    }

    // Se não foi encontrado gera um erro
    if (!found) {
      std::string error =
        "[" ERROR "] " + merge(listPath) + " não encontrado\n";
      throw std::runtime_error(error);
    }

    // Atualiza a lista de entradas
    dentries = getDirEntries(clt);
  }

  return entry;
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

  curPath = "img/";
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

void FatFS::ls(const std::string &path)
{
  // Lista de nomes nos caminhos
  std::vector<std::string> listPath;
  try {
    listPath = pathParser(path, true);
  } catch (const std::exception &error) {
    std::cout << error.what();
    return;
  }

  // Diretório raiz
  if (listPath.back() == "img") {
    // Lista de entradas
    std::vector<Dentry> dentries = getDirEntries(bios->getRootClus());

    // Mostra todas as entradas
    for (const auto &a : dentries) {
      a.printInfo();
    }

    return;
  }

  try {
    // Busca pela entrada
    std::pair<Dentry, DWORD> entry = searchEntry(listPath, true);

    // Lista de entradas
    std::vector<Dentry> dentries = getDirEntries(entry.first.getCluster());

    // Mostra todas as entradas
    for (const auto &a : dentries) {
      a.printInfo();
    }
  } catch (const std::exception &error) {
    std::cout << error.what();
    return;
  }
}

void FatFS::rm(const std::string &path)
{
  // Lista de nomes nos caminhos
  std::vector<std::string> listPath;
  try {
    listPath = pathParser(path, false);
  } catch (const std::exception &error) {
    std::cout << error.what();
    return;
  }

  // Diretório raiz
  if (listPath.back() == "img") {
    std::fprintf(stderr, "[" ERROR "] rm espera um arquivo\n");
    return;
  }

  try {
    // Busca pela entrada
    std::pair<Dentry, DWORD> entry = searchEntry(listPath, false);

    if (!removeEntry(entry.first, entry.second)) {
      std::fprintf(stderr, "[" ERROR "] rm %s operação falhou\n", path.c_str());
      return;
    }
  } catch (const std::exception &error) {
    std::cout << error.what();
    return;
  }
}

void FatFS::rmdir(const std::string &path)
{
  // Lista de nomes nos caminhos
  std::vector<std::string> listPath;
  try {
    listPath = pathParser(path, true);
  } catch (const std::exception &error) {
    std::cout << error.what();
    return;
  }

  // Diretório raiz
  if (listPath.back() == "img") {
    std::fprintf(stderr, "[" ERROR "] operação não permitida\n");
    return;
  }

  try {
    // Busca pela entrada
    std::pair<Dentry, DWORD> entry = searchEntry(listPath, true);

    if (!removeEntry(entry.first, entry.second)) {
      std::fprintf(
        stderr, "[" ERROR "] rmdir %s operação falhou\n", path.c_str());
      return;
    }
  } catch (const std::exception &error) {
    std::cout << error.what();
    return;
  }
}

void FatFS::pwd() { std::fprintf(stdout, "%s\n", curPath.c_str()); }

void FatFS::cd(const std::string &path)
{
  try {
    curPath = merge(pathParser(path, true));
  } catch (const std::exception &error) {
    std::cout << error.what();
  }
}
