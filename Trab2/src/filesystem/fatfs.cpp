/**
 * Descrição: Implementação da interface FAT
 *
 * Autores: João Victor Briganti, Luiz Takeda
 * Licença: BSD 2
 *
 * Data: 26/12/2024
 */

#include "filesystem/fatfs.hpp"
#include "filesystem/default.hpp"
#include "filesystem/entry/dentry.hpp"
#include "filesystem/structure/bpb.hpp"
#include "filesystem/structure/fsinfo.hpp"
#include "io/fs_adapter.hpp"
#include "path/pathname.hpp"
#include "utils/logger.hpp"

#include <cstdio>
#include <cstring>
#include <exception>
#include <memory>
#include <optional>
#include <stdexcept>
#include <string>
#include <vector>

//===------------------------------------------------------------------------===
// PRIVATE
//===------------------------------------------------------------------------===

void FatFS::listClusterDir(DWORD num)
{
  // Lista de entradas
  std::vector<Dentry> dentries = clusterIO->getListEntries(num);

  // Mostra todas as entradas
  for (const auto &dtr : dentries) {
    dtr.printInfo();
  }
}

// TODO: Verificar se é necessário atualizar o DOT e o DOTDOT
int FatFS::updateParentTimestamp(std::string path)
{
  std::string _ = pathName->getLastNameFromPath(path);

  // Lista de nomes nos caminhos
  std::vector<std::string> listPath;
  auto entry = clusterIO->searchEntryByPath(path, listPath, DIRECTORY);
  if (!entry.has_value()) {
    return 1;
  }

  entry->updatedWrtTimestamp();
  if (clusterIO->updateEntry(entry.value())) {
    return 0;
  }

  return -1;
}

bool FatFS::copyInternalData(const std::string &from, const std::string &to)
{
  std::vector<std::string> listPath;
  try {
    auto dentry = clusterIO->searchEntryByPath(from, listPath, ARCHIVE);

    // Diretório raiz
    if (!dentry.has_value()) {
      logger::logError("Operação não permitida");
      return false;
    }

    // Arquivo que vai ser escrito
    FileSystemAdapter fsAdapter(to, WRITE);

    // Tamanho e posição do arquivo
    DWORD fileSize = dentry->getFileSize();
    DWORD offset = 0;

    // Cadeia de clusters e buffer para leitura
    auto chain = fatTable->listChain(dentry->getDataCluster());
    auto buffer = std::make_unique<BYTE[]>(bios->totClusByts());

    for (auto &clt : chain) {
      if (!clusterIO->readCluster(buffer.get(), clt)) {
        logger::logError("Não foi possível ler cluster");
        return false;
      }

      if (fileSize < bios->totClusByts()) {
        if (!fsAdapter.write(offset, buffer.get(), fileSize)) {
          logger::logError("Não foi possível escrever no arquivo");
          return false;
        }
      } else {
        if (!fsAdapter.write(offset, buffer.get(), bios->totClusByts())) {
          logger::logError("Não foi possível escrever no arquivo");
          return false;
        }
      }

      offset += bios->totClusByts();
      fileSize -= bios->totClusByts();
    }

    return true;
  } catch (const std::runtime_error &error) {
    logger::logError(error.what());
    return false;
  }
}

bool FatFS::copyExternalData(const std::string &from, const std::string &to)
{
  std::vector<std::string> listPath;
  try {
    // Arquivo que vai ser escrito
    FileSystemAdapter fsAdapter(from, READ);

    size_t fileSize = 0;
    if (!fsAdapter.getFileSize(fileSize)) {
      return false;
    }

    // Soma-se 2 na verificação para garantir que vá haver espaço ao alocar a
    // entrada
    if (fileSize > MAX_FILE_SZ
        || (fileSize + 2) > fatTable->maxFreeClusByts()) {
      logger::logError("Arquivo é maior do que o suportado pelo sistema");
      return false;
    }

    if (!touch(to)) {
      return false;
    }

    auto dentry = clusterIO->searchEntryByPath(to, listPath, ARCHIVE);
    if (!dentry.has_value()) {
      return false;
    }

    // Aloca todos os clusters necessários
    DWORD cluster = 0;
    for (size_t i = 0; i * bios->totClusByts() < fileSize; i++) {
      if (!clusterIO->allocNewCluster(cluster)) {
        return false;
      }

      if (i == 0) {
        dentry->setDataCluster(cluster);
      }
    }

    dentry->setFileSize(static_cast<DWORD>(fileSize));
    if (!clusterIO->updateEntry(dentry.value())) {
      logger::logError("Não foi possível atualizar entrada");
      return false;
    }

    // Posição no arquivo de escrita
    DWORD offset = 0;

    // Cadeia de clusters e buffer para leitura
    auto chain = fatTable->listChain(dentry->getDataCluster());
    auto buffer = std::make_unique<BYTE[]>(bios->totClusByts());

    for (auto &clt : chain) {
      size_t size = 0;
      if (fileSize < bios->totClusByts()) {
        size = fileSize;
      } else {
        size = bios->totClusByts();
      }

      if (!fsAdapter.read(offset, buffer.get(), size)) {
        logger::logError("Não foi possível ler arquivo");
        return false;
      }

      if (!clusterIO->writeCluster(buffer.get(), clt)) {
        logger::logError("Não foi possível escrever no cluster");
        return false;
      }

      memset(buffer.get(), 0, bios->totClusByts());
      offset += bios->totClusByts();
      fileSize -= bios->totClusByts();
    }

    return true;
  } catch (std::runtime_error &error) {
    logger::logError(error.what());
    return false;
  }
}

bool FatFS::copyInSystemData(const std::string &from, const std::string &to)
{
  std::vector<std::string> listPath;
  try {
    auto fromDentry = clusterIO->searchEntryByPath(from, listPath, ARCHIVE);

    // Diretório raiz
    if (!fromDentry.has_value()) {
      logger::logError("Operação não permitida");
      return false;
    }

    if ((fromDentry->getFileSize() + 2) > fatTable->maxFreeClusByts()) {
      logger::logError("Não há espaço no sistema");
      return false;
    }

    if (!touch(to)) {
      return false;
    }

    auto toDentry = clusterIO->searchEntryByPath(to, listPath, ARCHIVE);
    if (!toDentry.has_value()) {
      return false;
    }

    // Aloca todos os clusters necessários
    DWORD cluster = 0;
    for (size_t i = 0; i * bios->totClusByts() < fromDentry->getFileSize();
         i++) {
      if (!clusterIO->allocNewCluster(cluster)) {
        return false;
      }

      if (i == 0) {
        toDentry->setDataCluster(cluster);
      }
    }

    toDentry->setFileSize(static_cast<DWORD>(fromDentry->getFileSize()));
    if (!clusterIO->updateEntry(toDentry.value())) {
      logger::logError("Não foi possível atualizar entrada");
      return false;
    }

    // Cadeia de clusters e buffer para leitura
    auto toChain = fatTable->listChain(toDentry->getDataCluster());
    auto fromChain = fatTable->listChain(fromDentry->getDataCluster());
    auto buffer = std::make_unique<BYTE[]>(bios->totClusByts());

    // Posição no arquivo de escrita
    DWORD fileSize = fromDentry->getFileSize();

    for (size_t i = 0; i < toChain.size(); i++) {
      size_t size = 0;
      if (fileSize < bios->totClusByts()) {
        size = fileSize;
      } else {
        size = bios->totClusByts();
      }

      if (!clusterIO->readCluster(buffer.get(), fromChain[i])) {
        logger::logError("Não foi possível ler arquivo");
        return false;
      }

      if (!clusterIO->writeCluster(buffer.get(), toChain[i])) {
        logger::logError("Não foi possível escrever no cluster");
        return false;
      }

      memset(buffer.get(), 0, bios->totClusByts());
      fileSize -= bios->totClusByts();
    }

    return true;
  } catch (std::runtime_error &error) {
    logger::logError(error.what());
    return false;
  }
}

//===------------------------------------------------------------------------===
// PUBLIC
//===------------------------------------------------------------------------===

FatFS::FatFS(const std::string &path)
{
  image = std::make_shared<Image>(path);
  if (image == nullptr) {
    throw std::runtime_error("Não foi possível ler imagem");
  }

  bios = std::make_shared<BiosBlock>(image);
  if (bios == nullptr) {
    throw std::runtime_error("Não foi possível iniciar BPB");
  }

  fatTable = std::make_shared<FatTable>(image, bios);
  if (fatTable == nullptr) {
    throw std::runtime_error("Não foi possível iniciar tabela FAT");
  }

  fsInfo = std::make_shared<FileSysInfo>(
    image, bios->getFSInfo() * bios->getBytesPerSec(), fatTable);
  if (fsInfo == nullptr) {
    throw std::runtime_error("Não foi possível iniciar FSInfo");
  }

  pathName = std::make_shared<PathName>();
  if (fsInfo == nullptr) {
    throw std::runtime_error("Não foi possível iniciar PathName");
  }

  clusterIO =
    std::make_shared<ClusterIO>(image, bios, fatTable, fsInfo, pathName);
  if (fsInfo == nullptr) {
    throw std::runtime_error("Não foi possível iniciar o cluster");
  }
}

void FatFS::info()
{
  bios->bpbPrint();
  fatTable->printInfo();
}

bool FatFS::cluster(DWORD num)
{
  auto buffer = std::make_unique<BYTE[]>(bios->totClusByts());
  if (buffer == nullptr) {
    logger::logError("Não foi possível alocar buffer");
    return false;
  }

  if (!clusterIO->readCluster(buffer.get(), num)) {
    logger::logError("Não foi possível ler cluster");
    return false;
  }

  char *bufferChar = reinterpret_cast<char *>(buffer.get());
  for (size_t i = 0; i < bios->totClusByts() * sizeof(char); i++) {
    std::fprintf(stdout, "%c", bufferChar[i]);
    if (i == bios->totClusByts() * sizeof(char) - 1 && bufferChar[i] != '\n') {
      std::fprintf(stdout, "\n");
    } 
  }

  return true;
}

bool FatFS::ls(const std::string &path)
{
  try {
    // Lista de nomes nos caminhos
    std::vector<std::string> listPath;
    std::optional<Dentry> entry;
    if (path == "") {
      entry = clusterIO->searchEntryByPath(pwd(), listPath, DIRECTORY);
    } else {
      entry = clusterIO->searchEntryByPath(path, listPath, DIRECTORY);
    }

    if (!entry.has_value()) {
      listClusterDir(bios->getRootClus());
    } else {
      listClusterDir(entry->getDataCluster());
    }

    return true;
  } catch (const std::exception &error) {
    logger::logError(error.what());
    return false;
  }
}

bool FatFS::rm(const std::string &path)
{
  try {
    // Lista de nomes nos caminhos
    std::vector<std::string> listPath;
    auto entry = clusterIO->searchEntryByPath(path, listPath, ARCHIVE);

    // Diretório raiz
    if (!entry.has_value()) {
      logger::logError("rm espera um arquivo");
      return false;
    }

    if (!clusterIO->deleteEntry(entry.value())) {
      logger::logError("rm " + path + " operação falhou");
      return false;
    }

    if (updateParentTimestamp(path) < 0) {
      logger::logWarning(
        "Não foi possível atualizar as datas do diretório pai");
    }

    return true;
  } catch (const std::exception &error) {
    logger::logError(error.what());
    return false;
  }
}

bool FatFS::rmdir(const std::string &path)
{
  try {
    // Lista de nomes nos caminhos
    std::vector<std::string> listPath;
    auto entry = clusterIO->searchEntryByPath(path, listPath, DIRECTORY);

    // Diretório raiz
    if (!entry.has_value()) {
      logger::logError("Operação não permitida");
      return false;
    }

    if (pathName->merge(listPath) == pwd()) {
      logger::logError("Diretório atual não pode ser excluido");
      return false;
    }

    // Verifica se o diretório está vazio
    std::vector<Dentry> dentries =
      clusterIO->getListEntries(entry->getDataCluster());

    if (dentries.size() != 2) {
      logger::logError("Diretório deve estar vazio");
      return false;
    }

    if (!clusterIO->deleteEntry(entry.value())) {
      logger::logError("rmdir " + path + " operação não permitida");
      return false;
    }

    if (updateParentTimestamp(path) < 0) {
      logger::logWarning(
        "Não foi possível atualizar as datas do diretório pai");
    }

    return true;
  } catch (const std::exception &error) {
    logger::logError(error.what());
    return false;
  }
}

std::string FatFS::pwd() { return pathName->getCurPath().c_str(); }

bool FatFS::cd(const std::string &path)
{
  try {
    std::vector<std::string> listPath;
    auto entry = clusterIO->searchEntryByPath(path, listPath, DIRECTORY);

    if (!entry.has_value()) {
      pathName->setCurPath(listPath[0]);
      return false;
    }

    pathName->setCurPath(pathName->merge(listPath));
    return true;
  } catch (const std::exception &error) {
    logger::logError(error.what());
    return false;
  }
}

bool FatFS::attr(const std::string &path)
{
  try {
    // Lista de nomes nos caminhos
    std::vector<std::string> listPath;
    auto entry = clusterIO->searchEntryByPath(path, listPath, ANY);

    // Diretório raiz
    if (!entry.has_value()) {
      logger::logError("operação não permitida");
      return false;
    }

    std::fprintf(stdout,
      "Entrada: %s (%s)\n",
      entry->getLongName().c_str(),
      entry->getShortName());

    std::fprintf(stdout,
      "Tamanho=%-8d Cluster=%-4d\n",
      entry->getFileSize(),
      entry->getDataCluster());

    std::fprintf(stdout,
      "Data Criação: %02d-%02d-%02d\n",
      entry->getCreationDay(),
      entry->getCreationMonth(),
      entry->getCreationYear());

    std::fprintf(stdout,
      "Hora Criação: %02d:%02d:%04d\n",
      entry->getCreationHour(),
      entry->getCreationMinute(),
      entry->getCreationSeconds());

    std::fprintf(stdout,
      "Data Escrita: %02d-%02d-%02d\n",
      entry->getWriteDay(),
      entry->getWriteMonth(),
      entry->getWriteYear());

    std::fprintf(stdout,
      "Hora Escrita: %02d:%02d:%04d\n",
      entry->getWriteHour(),
      entry->getWriteMinute(),
      entry->getWriteSeconds());

    std::fprintf(stdout,
      "Última Data de Acesso: %02d-%02d-%02d\n",
      entry->getLstAccDay(),
      entry->getLstAccMonth(),
      entry->getLstAccYear());

    std::fprintf(
      stdout, "Tipo: %s", entry->isDirectory() ? "diretório\n" : "arquivo\n");

    std::fprintf(
      stdout, "%s", entry->isReadOnly() ? "Atributo: Somente leitura\n" : "");
    std::fprintf(
      stdout, "%s", entry->isHidden() ? "Atributo: Escondido\n" : "");

    return true;
  } catch (const std::exception &error) {
    logger::logError(error.what());
    return false;
  }
}

bool FatFS::touch(const std::string &path)
{
  std::string filename = path;
  try {
    std::string name = pathName->getLastNameFromPath(filename);

    std::vector<std::string> listPath;
    auto entry = clusterIO->searchEntryByPath(filename, listPath, DIRECTORY);

    if (!entry.has_value()) {
      if (!clusterIO->createEntry(bios->getRootClus(), name, ARCHIVE)) {
        logger::logError("Não foi possível criar entrada");
        return false;
      }

      return true;
    }

    if (!clusterIO->createEntry(entry->getDataCluster(), name, ARCHIVE)) {
      logger::logError("Não foi possível criar entrada");
      return false;
    }

    if (updateParentTimestamp(path) < 0) {
      logger::logWarning(
        "Não foi possível atualizar as datas do diretório pai");
    }

    return true;
  } catch (const std::exception &error) {
    logger::logError(error.what());
    return false;
  }
}

bool FatFS::mkdir(const std::string &path)
{
  std::string filename = path;
  try {
    std::string name = pathName->getLastNameFromPath(filename);

    std::vector<std::string> listPath;
    auto entry = clusterIO->searchEntryByPath(filename, listPath, DIRECTORY);

    if (!entry.has_value()) {
      return clusterIO->createEntry(bios->getRootClus(), name, DIRECTORY);
    }

    if (!clusterIO->createEntry(entry->getDataCluster(), name, DIRECTORY)) {
      logger::logError("Não foi possível criar entrada");
      return false;
    }

    if (updateParentTimestamp(path) < 0) {
      logger::logWarning(
        "Não foi possível atualizar as datas do diretório pai");
    }

    return true;
  } catch (const std::exception &error) {
    logger::logError(error.what());
    return false;
  }
}

bool FatFS::rename(const std::string &from, const std::string &to)
{
  std::vector<std::string> listPath;
  try {
    auto oldEntry = clusterIO->searchEntryByPath(from, listPath, ARCHIVE);
    listPath.pop_back();

    // Diretório raiz
    if (!oldEntry.has_value()) {
      logger::logError("rename espera um arquivo");
      return false;
    }

    // Salva os valores antigos do diretório antes de excluir
    DWORD oldDataCluster = oldEntry->getDataCluster();
    DWORD oldFileSize = oldEntry->getFileSize();

    if (!clusterIO->removeEntry(oldEntry.value())) {
      logger::logError("rename " + from + " " + to + " operação falhou");
      return false;
    }

    if (!pathName->isRootDir(listPath) && !listPath.empty()
        && (updateParentTimestamp(from) < 0)) {
      logger::logWarning(
        "Não foi possível atualizar as datas do diretório pai");
    }

    if (!touch(to)) {
      return false;
    }

    auto newEntry = clusterIO->searchEntryByPath(to, listPath, ARCHIVE);
    if (!newEntry.has_value()) {
      logger::logError("rename não pode ser concluído");
      return false;
    }

    newEntry->setDataCluster(oldDataCluster);
    newEntry->setFileSize(oldFileSize);
    return clusterIO->updateEntry(newEntry.value());
  } catch (const std::exception &error) {
    logger::logError(error.what());
    return false;
  }
}

bool FatFS::mv(const std::string &from, const std::string &to)
{
  try {
    if (!pathName->isExternalPath(from) && !pathName->isExternalPath(to)) {
      return rename(from, to);
    }

    if (!pathName->isExternalPath(from) && pathName->isExternalPath(to)) {
      if (!copyInternalData(from, to)) {
        logger::logError("mv operação falhou");
        return false;
      }

      return rm(from);
    }

    if (pathName->isExternalPath(from) && !pathName->isExternalPath(to)) {
      if (!copyExternalData(from, to)) {
        logger::logError("mv operação falhou");
        return false;
      }

      FileSystemAdapter adapter(from, READ);
      return adapter.remove();
    }

    logger::logError("mv não suporta operações entre dois arquivos externos");
    return false;
  } catch (const std::exception &error) {
    logger::logError(error.what());
    return false;
  }
}

bool FatFS::cp(const std::string &from, const std::string &to)
{
  try {
    if (!pathName->isExternalPath(from) && !pathName->isExternalPath(to)) {
      return copyInSystemData(from, to);
    }

    if (!pathName->isExternalPath(from) && pathName->isExternalPath(to)) {
      return copyInternalData(from, to);
    }

    if (pathName->isExternalPath(from) && !pathName->isExternalPath(to)) {
      return copyExternalData(from, to);
    }

    logger::logError("cp não suporta operações entre dois arquivos externos");
    return false;
  } catch (const std::exception &error) {
    logger::logError(error.what());
    return false;
  }
}
