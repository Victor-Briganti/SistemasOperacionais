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
#include "filesystem/fsinfo.hpp"
#include "path/parser.hpp"
#include "utils/color.hpp"

#include <cstring>
#include <exception>
#include <iostream>
#include <list>
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

  // Lista com diretórios de nomes longos
  std::vector<LongDir> longDirs;

  // Vetor com todas as entradas
  std::vector<Dentry> dentries;

  // Cadeia com os clusters
  std::vector<DWORD> chain = fatTable->listChain(num);

  for (auto cluster : chain) {
    // Lê o cluster no qual o diretório se encontra
    if (!readCluster(buffer, cluster)) {
      std::fprintf(stderr, "[" ERROR "] Não foi possível ler cluster\n");
      delete[] static_cast<char *>(buffer);
      return {};
    }

    // Cast para facilitar manuseio do buffer
    Dir *bufferDir = static_cast<Dir *>(buffer);

    // Define a posição da entrada no buffer
    DWORD bufferPos = 0;

    // Caminha por todo o buffer até encontrar o EOD_ENTRY ou alcançar o limite
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

  // Remove as entradas da tabela FAT
  int fatRm = fatTable->removeChain(entry.getCluster());
  if (fatRm < 0) {
    return false;
  }

  // Remove a entrada do diretório
  bool entryRm = setDirEntries(num,
    entry.getInitPos(),
    entry.getEndPos(),
    entry.getDirectory(),
    entry.getLongDirectories());

  // Salva a quantidade de clusters removidos no FSInfo
  bool result = (fatRm >= 0) && entryRm;
  if (result) {
    DWORD freeClusters = fsInfo->getFreeCount() + static_cast<DWORD>(fatRm);
    return fsInfo->setFreeCount(freeClusters);
  }

  return false;
}

std::vector<std::string> FatFS::pathParser(const std::string &path,
  int expectDir)
{
  // Lista de nomes nos caminhos
  std::vector<std::string> listPath = split(path, '/');

  if (path[0] == '/') {
    std::string error =
      "[" ERROR "] " + merge(listPath) + " caminho inválido\n";
    throw std::runtime_error(error);
  }

  // Se necessário monta o caminho completo
  if (listPath.empty() || listPath[0] != ROOT_DIR) {
    std::vector<std::string> fullPath = split(curPath, '/');
    fullPath.insert(fullPath.end(), listPath.begin(), listPath.end());
    listPath = fullPath;
  }

  // Se o caminho é o root, não há o que verificar
  if (listPath.size() == 1 && listPath[0] == ROOT_DIR) {
    return listPath;
  }

  // Caminho que será criado
  std::vector<std::string> newPath;
  newPath.emplace_back(ROOT_DIR);

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
        if (i == listPath.size() - 1 && expectDir != ALL_ENTRY
            && expectDir != static_cast<int>(a.isDirectory())) {
          std::string error =
            "[" ERROR "] " + merge(listPath) + " esperava um ";

          if (DIR_ENTRY) {
            error += "diretório\n";
          } else {
            error += "arquivo\n";
          }

          throw std::runtime_error(error);
        }

        if ((i != listPath.size() - 1) && !a.isDirectory()) {
          std::string error =
            "[" ERROR "]" ROOT_DIR + merge(listPath) + " caminho inválido\n";
          throw std::runtime_error(error);
        }

        if (a.getNameType() == DOT_NAME) {
          found = true;
          break;
        }

        if (a.getNameType() == DOTDOT_NAME && newPath.size() == 1) {
          std::string error =
            "[" ERROR "]" ROOT_DIR + merge(listPath) + " caminho inválido\n";
          throw std::runtime_error(error);
        }

        if (a.getNameType() == DOTDOT_NAME && newPath.back() != ROOT_DIR) {
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
      std::string error =
        "[" ERROR "]" ROOT_DIR + merge(listPath) + " caminho inválido\n";
      throw std::runtime_error(error);
    }

    // Atualiza a lista de entradas
    dentries = getDirEntries(clt);
  }

  return newPath;
}

std::pair<Dentry, DWORD>
  FatFS::searchEntry(const std::vector<std::string> &listPath, int expectDir)
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
        if (i == listPath.size() - 1 && expectDir != ALL_ENTRY
            && expectDir != static_cast<int>(a.isDirectory())) {
          std::string error = "[" ERROR "] esperava um ";
          error += a.isDirectory() ? "diretório\n" : "arquivo\n";
          throw std::runtime_error(error);
        }

        // Se no meio do caminho temos um arquivo gera um erro
        if ((i != listPath.size() - 1) && !a.isDirectory()) {
          std::string error =
            "[" ERROR "]" ROOT_DIR + merge(listPath) + " caminho inválido5\n";
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
        "[" ERROR "]" ROOT_DIR + merge(listPath) + " não encontrado\n";
      throw std::runtime_error(error);
    }

    // Atualiza a lista de entradas
    dentries = getDirEntries(clt);
  }

  return entry;
}

bool FatFS::insertDirEntries(DWORD num,
  const Dir &dir,
  const std::vector<LongDir> &ldir)
{
  // Aloca o buffer do diretório
  void *buffer = new char[bios->totClusByts()];
  if (buffer == nullptr) {
    std::fprintf(stderr, "[" ERROR "] Não foi possível alocar buffer\n");
    return false;
  }

  // Define a posição da entrada no buffer
  DWORD bufferPos = 0;

  // Quantidade de entradas que precisam ser salvas
  size_t requiredEntries = ldir.size() + 1;

  // Cadeia com os clusters
  std::vector<DWORD> chain = fatTable->listChain(num);

  // Quantidade de entradas por cluster
  size_t numDirs = bios->totClusByts() / sizeof(Dir);

  for (size_t i = 0, countFreeEntries = 0; i < chain.size(); i++) {
    // Lê o cluster no qual o diretório se encontra
    if (!readCluster(buffer, chain[i])) {
      std::fprintf(stderr, "[" ERROR "] Não foi possível ler cluster\n");
      delete[] static_cast<char *>(buffer);
      return {};
    }

    // Cast para facilitar manuseio do buffer
    Dir *bufferDir = static_cast<Dir *>(buffer);

    // Caminha por todo o buffer até encontrar uma quantidade suficiente de
    // entradas livres ou alcançar o limite de tamanho do cluster.
    for (size_t j = 0; j < numDirs; j++) {
      // Final do diretório
      if (bufferDir[i].name[0] == EOD_ENTRY) {
        bufferPos = i;
        break;
      }

      // Diretório vazio
      if (bufferDir[i].name[0] == FREE_ENTRY) {
        countFreeEntries++;

        // Se for a primeira parte da contagem salva sua posição
        if (countFreeEntries == 1) {
          bufferPos = i;
        }

        // Se a quantidade de diretórios vazios bate com o que precisamos use
        if (countFreeEntries == requiredEntries) {
          size_t k = bufferPos;
          for (auto a : ldir) {
            memcpy(&bufferDir[k], &a, sizeof(a));
            k++;
          }
          memcpy(&bufferDir[k], &dir, sizeof(dir));

          bool result = writeCluster(bufferDir, chain[i]);
          delete[] static_cast<char *>(buffer);
          return result;
        }

        continue;
      }

      countFreeEntries = 0;
    }
  }

  return false;
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

  fsInfo = new FileSysInfo(
    image, bios->getFSInfo() * bios->getBytesPerSec(), fatTable);
  if (fsInfo == nullptr) {
    std::string error = "[" ERROR "] Não foi alocar FSInfo";
    throw std::runtime_error(error);
  }

  curPath = ROOT_DIR;
}

FatFS::~FatFS()
{
  delete fsInfo;
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
    listPath = pathParser(path, DIR_ENTRY);
  } catch (const std::exception &error) {
    std::cout << error.what();
    return;
  }

  // Diretório raiz
  if (listPath.back() == ROOT_DIR) {
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
    auto [entry, _] = searchEntry(listPath, DIR_ENTRY);

    // Lista de entradas
    std::vector<Dentry> dentries = getDirEntries(entry.getCluster());

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
    listPath = pathParser(path, ARCH_ENTRY);
  } catch (const std::exception &error) {
    std::cout << error.what();
    return;
  }

  // Diretório raiz
  if (listPath.back() == ROOT_DIR) {
    std::fprintf(stderr, "[" ERROR "] rm espera um arquivo\n");
    return;
  }

  try {
    // Busca pela entrada
    std::pair<Dentry, DWORD> entry = searchEntry(listPath, ARCH_ENTRY);

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
    listPath = pathParser(path, DIR_ENTRY);
  } catch (const std::exception &error) {
    std::cout << error.what();
    return;
  }

  // Diretório raiz
  if (listPath.back() == ROOT_DIR) {
    std::fprintf(stderr, "[" ERROR "] operação não permitida\n");
    return;
  }

  try {
    // Busca pela entrada
    auto [entry, cluster] = searchEntry(listPath, DIR_ENTRY);

    if (!removeEntry(entry, cluster)) {
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
    curPath = merge(pathParser(path, DIR_ENTRY));
  } catch (const std::exception &error) {
    std::cout << error.what();
  }
}

void FatFS::attr(const std::string &path)
{
  // Lista de nomes nos caminhos
  std::vector<std::string> listPath;
  try {
    listPath = pathParser(path, ALL_ENTRY);
  } catch (const std::exception &error) {
    std::cout << error.what();
    return;
  }

  // Diretório raiz
  if (listPath.back() == ROOT_DIR) {
    std::fprintf(stderr, "[" ERROR "] operação não permitida\n");
    return;
  }

  try {
    // Busca pela entrada
    auto [entry, _] = searchEntry(listPath, ALL_ENTRY);

    std::fprintf(stdout,
      "Entrada: %s (%s)\n",
      entry.getLongName().c_str(),
      entry.getShortName());

    std::fprintf(stdout,
      "Tamanho=%-8d Cluster=%-4d\n",
      entry.getFileSize(),
      entry.getCluster());

    std::fprintf(stdout,
      "Data Criação: %02d-%02d-%02d\n",
      entry.getCreationDay(),
      entry.getCreationMonth(),
      entry.getCreationYear());

    std::fprintf(stdout,
      "Hora Criação: %02d:%02d:%04d\n",
      entry.getCreationHour(),
      entry.getCreationMinute(),
      entry.getCreationSeconds());

    std::fprintf(stdout,
      "Data Escrita: %02d-%02d-%02d\n",
      entry.getWriteDay(),
      entry.getWriteMonth(),
      entry.getWriteYear());

    std::fprintf(stdout,
      "Hora Escrita: %02d:%02d:%04d\n",
      entry.getWriteHour(),
      entry.getWriteMinute(),
      entry.getWriteSeconds());

    std::fprintf(stdout,
      "Última Data de Acesso: %02d-%02d-%02d\n",
      entry.getLstAccDay(),
      entry.getLstAccMonth(),
      entry.getLstAccYear());

    std::fprintf(
      stdout, "Tipo: %s", entry.isDirectory() ? "diretório\n" : "arquivo\n");

    std::fprintf(
      stdout, "%s", entry.isReadOnly() ? "Atributo: Somente leitura\n" : "");
    std::fprintf(stdout, "%s", entry.isHidden() ? "Atributo: Escondido\n" : "");

  } catch (const std::exception &error) {
    std::cout << error.what();
    return;
  }
}

void FatFS::touch(const std::string &path)
{
  // Lista de nomes nos caminhos
  std::vector<std::string> listPath = split(path, '/');
  if (listPath.size() == 1 && listPath[0] == ROOT_DIR) {
    std::fprintf(stderr, "[" ERROR "] operação inválida\n");
    return;
  }

  // Salva o nome do arquivo a ser criado
  std::string filename = listPath.back();
  listPath.pop_back();

  try {
    std::string newPath = merge(listPath);
    listPath = pathParser(newPath, DIR_ENTRY);
  } catch (const std::exception &error) {
    std::cout << error.what();
    return;
  }

  // Diretório raiz
  if (listPath.back() == ROOT_DIR) {
    DWORD cluster = bios->getRootClus();

    std::vector<Dentry> dentries = getDirEntries(cluster);

    for (const auto &dtr : dentries) {
      if (dtr.getLongName() == filename) {
        std::fprintf(stderr, "[" ERROR "] arquivo já existe\n");
        return;
      }
    }

    Dir dir = createDir(filename, 0, 0, ATTR_LONG_NAME | ATTR_ARCHIVE);
    std::vector<LongDir> ldir = createLongDir(dir, filename);
    if (!insertDirEntries(cluster, dir, ldir)) {
      std::fprintf(stderr, "[" ERROR "] operação falhou\n");
      return;
    }
  }
}
