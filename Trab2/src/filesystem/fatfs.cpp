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
#include "filesystem/entry/long_entry.hpp"
#include "filesystem/entry/short_entry.hpp"
#include "filesystem/structure/bpb.hpp"
#include "filesystem/structure/fsinfo.hpp"
#include "path/path_parser.hpp"
#include "utils/logger.hpp"

#include <cstdio>
#include <cstring>
#include <exception>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

//===------------------------------------------------------------------------===
// PRIVATE
//===------------------------------------------------------------------------===

std::vector<Dentry> FatFS::getDirEntries(DWORD num)
{
  // Aloca o buffer do diretório
  auto buffer = std::make_unique<BYTE[]>(bios->totClusByts());
  if (buffer == nullptr) {
    logger::logError("Não foi possível alocar buffer");
    return {};
  }

  // Lista com diretórios de nomes longos
  std::vector<LongEntry> longDirs;

  // Vetor com todas as entradas
  std::vector<Dentry> dentries;

  // Cadeia com os clusters
  std::vector<DWORD> chain = fatTable->listChain(num);

  for (auto cluster : chain) {
    // Lê o cluster no qual o diretório se encontra
    if (!clusterIO->readCluster(buffer.get(), cluster)) {
      logger::logError("Não foi possível ler cluster");
      return {};
    }

    // Cast para facilitar manuseio do buffer
    const ShortEntry *bufferEntry =
      reinterpret_cast<ShortEntry *>(buffer.get());

    // Define a posição da entrada no buffer
    DWORD bufferPos = 0;

    // Caminha por todo o buffer até encontrar o EOD_ENTRY ou alcançar o limite
    // de tamanho do cluster.
    for (size_t i = 0; i < (bios->totClusByts() / sizeof(ShortEntry)); i++) {
      // Final do diretório
      if (bufferEntry[i].name[0] == EOD_ENTRY) {
        break;
      }

      // Diretório vazio
      if (bufferEntry[i].name[0] == FREE_ENTRY) {
        continue;
      }

      if (bufferEntry[i].attr == ATTR_LONG_NAME) {
        if (bufferPos == 0) {
          bufferPos = static_cast<DWORD>(i);
        }

        LongEntry lentry;
        memcpy(&lentry, &bufferEntry[i], sizeof(lentry));
        longDirs.push_back(lentry);
      } else {
        Dentry entry(bufferEntry[i],
          longDirs,
          static_cast<DWORD>(bufferPos),
          static_cast<DWORD>(i));

        dentries.push_back(entry);
        longDirs.clear();
        bufferPos = 0;
      }
    }
  }

  return dentries;
}

bool FatFS::setDirEntries(DWORD num,
  DWORD startPos,
  DWORD endPos,
  const ShortEntry &entry,
  const std::vector<LongEntry> &lentry)
{
  // Aloca o buffer do diretório
  auto buffer = std::make_unique<BYTE[]>(bios->totClusByts());
  if (buffer == nullptr) {
    logger::logError("Não foi possível alocar buffer");
    return {};
  }

  // Lê o cluster no qual o diretório se encontra
  if (!clusterIO->readCluster(buffer.get(), num)) {
    logger::logError("Não foi possível ler cluster");
    return {};
  }

  // Cast para facilitar manuseio do buffer
  ShortEntry *bufferEntry = reinterpret_cast<ShortEntry *>(buffer.get());

  // Caminha pelo buffer escrevendo os novos valores.
  for (size_t i = startPos, j = 0; i <= endPos; i++, j++) {
    if (j < lentry.size()) {
      memcpy(&bufferEntry[i], &lentry[j], sizeof(entry));
    } else {
      memcpy(&bufferEntry[i], &entry, sizeof(entry));
    }
  }

  return clusterIO->writeCluster(buffer.get(), num);
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
    entry.getShortEntry(),
    entry.getLongEntries());

  // Salva a quantidade de clusters removidos no FSInfo
  bool result = (fatRm >= 0) && entryRm;
  if (result) {
    DWORD freeClusters = fsInfo->getFreeCount() + static_cast<DWORD>(fatRm);
    return fsInfo->setFreeCount(freeClusters);
  }

  return false;
}

std::vector<std::string> FatFS::parser(const std::string &path, int expectDir)
{
  // Lista de nomes nos caminhos
  std::vector<std::string> listPath = pathParser->split(path, '/');

  if (path[0] == '/') {
    throw std::runtime_error(pathParser->merge(listPath) + " caminho inválido");
  }

  // Se necessário monta o caminho completo
  if (listPath.empty() || listPath[0] != ROOT_DIR) {
    std::vector<std::string> fullPath =
      pathParser->split(pathParser->getCurPath(), '/');
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
          std::string error = pathParser->merge(listPath) + " esperava um ";

          if (DIR_ENTRY) {
            error += "diretório";
          } else {
            error += "arquivo";
          }

          throw std::runtime_error(error);
        }

        if ((i != listPath.size() - 1) && !a.isDirectory()) {
          std::string error =
            pathParser->merge(listPath) + " caminho inválido\n";
          throw std::runtime_error(error);
        }

        if (a.getNameType() == DOT_NAME) {
          found = true;
          break;
        }

        if (a.getNameType() == DOTDOT_NAME && newPath.size() == 1) {
          throw std::runtime_error(
            pathParser->merge(listPath) + " caminho inválido");
        }

        if (a.getNameType() == DOTDOT_NAME && newPath.back() != ROOT_DIR) {
          newPath.pop_back();
        } else {
          newPath.push_back(a.getLongName());
        }

        // Altera o cluster em que ocorre a busca
        clt = clusterIO->getEntryClus(a);
        found = true;
        break;
      }
    }

    // Se não foi encontrado, gera um erro
    if (!found) {
      throw std::runtime_error(
        pathParser->merge(listPath) + " caminho inválido");
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
          std::string error = "Esperava um ";
          error += a.isDirectory() ? "diretório" : "arquivo";
          throw std::runtime_error(error);
        }

        // Se no meio do caminho temos um arquivo gera um erro
        if ((i != listPath.size() - 1) && !a.isDirectory()) {
          throw std::runtime_error(
            pathParser->merge(listPath) + " caminho inválido");
        }

        // Atualiza a entrada
        entry.first = a;
        entry.second = clt;

        // Altera o cluster em que ocorre a busca
        clt = clusterIO->getEntryClus(a);
        found = true;
        break;
      }
    }

    // Se não foi encontrado gera um erro
    if (!found) {
      throw std::runtime_error(pathParser->merge(listPath) + " não encontrado");
    }

    // Atualiza a lista de entradas
    dentries = getDirEntries(clt);
  }

  return entry;
}

// TODO: Criação de um diretório deve atualizar suas informações
bool FatFS::insertDirEntries(DWORD num,
  const ShortEntry &entry,
  const std::vector<LongEntry> &lentry)
{
  // Aloca o buffer do diretório
  auto buffer = std::make_unique<BYTE[]>(bios->totClusByts());
  if (buffer == nullptr) {
    logger::logError("Não foi possível alocar buffer");
    return false;
  }

  // Flag para definir que o EOD foi encontrado
  bool eodFound = false;

  // Define a posição da entrada no buffer
  DWORD bufferPos = 0;

  // Quantidade de entradas que precisam ser salvas
  size_t requiredEntries = lentry.size() + 1;

  // Cadeia com os clusters
  std::vector<DWORD> chain = fatTable->listChain(num);

  // Quantidade de entradas por cluster
  size_t numDirs = bios->totClusByts() / sizeof(ShortEntry);

  // Cast para facilitar manuseio do buffer
  ShortEntry *bufferEntry = nullptr;

  // Cluster atual
  DWORD curCluster = 0;

  for (size_t i = 0, countFreeEntries = 0; i < chain.size(); i++) {
    curCluster = chain[i];
    // Lê o cluster no qual o diretório se encontra
    if (!clusterIO->readCluster(buffer.get(), curCluster)) {
      logger::logError("Não foi possível ler cluster");
      return {};
    }

    bufferEntry = reinterpret_cast<ShortEntry *>(buffer.get());

    // Caminha por todo o buffer até encontrar uma quantidade suficiente de
    // entradas livres ou alcançar o limite de tamanho do cluster.
    for (size_t j = 0; j < numDirs; j++) {
      // Final do diretório
      if (bufferEntry[j].name[0] == EOD_ENTRY) {
        bufferPos = static_cast<DWORD>(j);
        eodFound = true;
        break;
      }

      // Diretório vazio
      if (bufferEntry[j].name[0] == FREE_ENTRY) {
        countFreeEntries++;

        // Se for a primeira parte da contagem salva sua posição
        if (countFreeEntries == 1) {
          bufferPos = static_cast<DWORD>(j);
        }

        // Se a quantidade de diretórios vazios bate com o que precisamos use
        if (countFreeEntries == requiredEntries) {
          size_t k = bufferPos;
          for (auto a : lentry) {
            memcpy(&bufferEntry[k], &a, sizeof(a));
            k++;
          }
          memcpy(&bufferEntry[k], &entry, sizeof(entry));

          return clusterIO->writeCluster(bufferEntry, curCluster);
        }

        continue;
      }

      countFreeEntries = 0;
    }
  }

  if (eodFound && bufferPos + requiredEntries < numDirs) {
    size_t k = bufferPos;
    for (auto a : lentry) {
      memcpy(&bufferEntry[k], &a, sizeof(a));
      k++;
    }
    memcpy(&bufferEntry[k], &entry, sizeof(entry));

    bool result = clusterIO->writeCluster(bufferEntry, curCluster);
    return result;
  }

  if (eodFound && bufferPos + requiredEntries >= numDirs) {
    // TODO: Ao alocar as tabelas é necessário atualizar o fsInfo
    DWORD cluster = fsInfo->getNextFree();
    if (fatTable->searchFreeEntry(cluster)) {
      if (!fatTable->allocClusters(curCluster, { cluster })) {
        logger::logError("Não foi possível alocar clusters");
        return false;
      }

      // Aloca buffer do novo diretório
      auto newBuffer = std::make_unique<BYTE[]>(bios->totClusByts());
      if (newBuffer == nullptr) {
        logger::logError("Não foi possível alocar buffer");
        return false;
      }

      // Lê o cluster no qual o diretório se encontra
      if (!clusterIO->readCluster(newBuffer.get(), cluster)) {
        logger::logError("Não foi possível ler cluster");
        return false;
      }

      // Zera a entrada para não haver problemas
      memset(newBuffer.get(), 0, bios->totClusByts());

      // Cast para facilitar manuseio do buffer
      auto newBufferDir = reinterpret_cast<ShortEntry *>(newBuffer.get());

      // Ponteiro que especifica qual o buffer que está sendo utilizado
      ShortEntry *currentBuffer = bufferEntry;

      size_t k = bufferPos;
      for (size_t j = 0; j < lentry.size(); j++) {
        if (currentBuffer != newBufferDir && k + j >= numDirs) {
          currentBuffer = newBufferDir;
          k = 0;
        }

        memcpy(&currentBuffer[k], &lentry[j], sizeof(lentry[j]));
        k++;
      }

      memcpy(&currentBuffer[k], &entry, sizeof(entry));

      return clusterIO->writeCluster(bufferEntry, curCluster)
             && clusterIO->writeCluster(newBufferDir, cluster);
    }
  }

  if (!eodFound) {
    // TODO: Ao alocar as tabelas é necessário atualizar o fsInfo
    DWORD cluster = fsInfo->getNextFree();
    if (fatTable->searchFreeEntry(cluster)) {
      if (!fatTable->allocClusters(curCluster, { cluster })) {
        logger::logError("Não foi possível alocar clusters");
        return false;
      }

      // Lê o cluster no qual o diretório se encontra
      if (!clusterIO->readCluster(buffer.get(), cluster)) {
        logger::logError("Não foi possível ler cluster");
        return false;
      }

      // Zera a entrada para não haver problemas
      memset(buffer.get(), 0, bios->totClusByts());

      // Cast para facilitar manuseio do buffer
      bufferEntry = reinterpret_cast<ShortEntry *>(buffer.get());

      size_t k = bufferPos;
      for (size_t j = 0; j < lentry.size(); j++) {
        memcpy(&bufferEntry[k], &lentry[j], sizeof(lentry[j]));
        k++;
      }

      memcpy(&bufferEntry[k], &entry, sizeof(entry));

      bool result = clusterIO->writeCluster(bufferEntry, cluster);

      return result;
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
    throw std::runtime_error("Não foi possível ler imagem");
  }

  bios = new BiosBlock(image);
  if (bios == nullptr) {
    throw std::runtime_error("Não foi possível iniciar BPB");
  }

  fatTable = new FatTable(image, bios);
  if (fatTable == nullptr) {
    throw std::runtime_error("Não foi possível iniciar tabela FAT");
  }

  fsInfo = new FileSysInfo(
    image, bios->getFSInfo() * bios->getBytesPerSec(), fatTable);
  if (fsInfo == nullptr) {
    throw std::runtime_error("Não foi possível iniciar FSInfo");
  }

  pathParser = new PathParser();
  if (fsInfo == nullptr) {
    throw std::runtime_error("Não foi possível iniciar PathParser");
  }

  clusterIO = new ClusterIO(image, bios, fatTable, fsInfo, pathParser);
  if (fsInfo == nullptr) {
    throw std::runtime_error("Não foi possível iniciar o cluster");
  }
}

FatFS::~FatFS()
{
  delete fsInfo;
  delete fatTable;
  delete bios;
  delete image;
  delete pathParser;
  delete clusterIO;
}

void FatFS::info()
{
  bios->bpbPrint();
  fatTable->printInfo();
}

void FatFS::cluster(DWORD num)
{
  auto buffer = std::make_unique<BYTE[]>(bios->totClusByts());
  if (buffer == nullptr) {
    logger::logError("Não foi possível alocar buffer");
    return;
  }

  if (!clusterIO->readCluster(buffer.get(), num)) {
    logger::logError("Não foi possível ler cluster");
    return;
  }

  char *bufferChar = reinterpret_cast<char *>(buffer.get());
  for (size_t i = 0; i < bios->totClusByts() * sizeof(char); i++) {
    std::fprintf(stdout, "%c", bufferChar[i]);
  }
}

void FatFS::ls(const std::string &path)
{
  // Lista de nomes nos caminhos
  std::vector<std::string> listPath;
  try {
    listPath = parser(path, DIR_ENTRY);
  } catch (const std::exception &error) {
    logger::logError(error.what());
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
    logger::logError(error.what());
    return;
  }
}

void FatFS::rm(const std::string &path)
{
  // Lista de nomes nos caminhos
  std::vector<std::string> listPath;
  try {
    listPath = parser(path, ARCH_ENTRY);
  } catch (const std::exception &error) {
    logger::logError(error.what());
    return;
  }

  // Diretório raiz
  if (listPath.back() == ROOT_DIR) {
    logger::logError("rm espera um arquivo");
    return;
  }

  try {
    // Busca pela entrada
    std::pair<Dentry, DWORD> entry = searchEntry(listPath, ARCH_ENTRY);

    if (!removeEntry(entry.first, entry.second)) {
      logger::logError("rm " + path + " operação falhou");
      return;
    }
  } catch (const std::exception &error) {
    logger::logError(error.what());
    return;
  }
}

void FatFS::rmdir(const std::string &path)
{
  // Lista de nomes nos caminhos
  std::vector<std::string> listPath;
  try {
    listPath = parser(path, DIR_ENTRY);
  } catch (const std::exception &error) {
    logger::logError(error.what());
    return;
  }

  // Diretório raiz
  if (listPath.back() == ROOT_DIR) {
    logger::logError("operação não permitida");
    return;
  }

  try {
    // Busca pela entrada
    auto [entry, cluster] = searchEntry(listPath, DIR_ENTRY);

    if (!removeEntry(entry, cluster)) {
      logger::logError("rmdir " + path + " operação não permitida");
      return;
    }
  } catch (const std::exception &error) {
    logger::logError(error.what());
    return;
  }
}

void FatFS::pwd()
{
  std::fprintf(stdout, "%s\n", pathParser->getCurPath().c_str());
}

void FatFS::cd(const std::string &path)
{
  try {
    auto x = parser(path, DIR_ENTRY);
    auto y = pathParser->merge(x);
    pathParser->setCurPath(y);
  } catch (const std::exception &error) {
    logger::logError(error.what());
  }
}

void FatFS::attr(const std::string &path)
{
  // Lista de nomes nos caminhos
  std::vector<std::string> listPath;
  try {
    listPath = parser(path, ALL_ENTRY);
  } catch (const std::exception &error) {
    logger::logError(error.what());
    return;
  }

  // Diretório raiz
  if (listPath.back() == ROOT_DIR) {
    logger::logError("operação não permitida");
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
    logger::logError(error.what());
    return;
  }
}

void FatFS::touch(const std::string &path)
{
  // Lista de nomes nos caminhos
  std::vector<std::string> listPath = pathParser->split(path, '/');
  if (listPath.size() == 1 && listPath[0] == ROOT_DIR) {
    logger::logError("operação inválida");
    return;
  }

  // Salva o nome do arquivo a ser criado
  std::string filename = listPath.back();
  listPath.pop_back();

  if (filename.size() > FILENAME_MAX) {
    logger::logError("nome muito longo");
    return;
  }

  try {
    std::string newPath = pathParser->merge(listPath);
    listPath = parser(newPath, DIR_ENTRY);
  } catch (const std::exception &error) {
    logger::logError(error.what());
    return;
  }

  // Diretório raiz
  if (listPath.back() == ROOT_DIR) {
    DWORD cluster = bios->getRootClus();

    std::vector<Dentry> dentries = getDirEntries(cluster);


    // Gera a entrada curta
    ShortEntry entry = createShortEntry(filename, 0, 0, ATTR_ARCHIVE);
    for (const auto &dtr : dentries) {
      if (dtr.getLongName() == filename) {
        logger::logError("Arquivo já existe");
        return;
      }

      while (!std::strncmp(dtr.getShortName(),
        reinterpret_cast<const char *>(entry.name),
        NAME_FULL_SIZE)) {
        randomizeShortname(reinterpret_cast<char *>(entry.name));
      }
    }

    // Gera as entradas longas
    std::vector<LongEntry> lentry = createLongEntries(entry, filename);

    if (!insertDirEntries(cluster, entry, lentry)) {
      logger::logError("operação falhou");
    }

    return;
  }

  // Busca pela entrada
  auto [dentry, _] = searchEntry(listPath, DIR_ENTRY);

  // Lista de entradas
  std::vector<Dentry> dentries = getDirEntries(dentry.getCluster());

  // Gera a entrada curta
  ShortEntry entry = createShortEntry(filename, 0, 0, ATTR_ARCHIVE);

  for (const auto &dtr : dentries) {
    if (dtr.getLongName() == filename) {
      logger::logError(filename + " arquivo já existe");
      return;
    }

    while (!std::strncmp(dtr.getShortName(),
      reinterpret_cast<const char *>(entry.name),
      NAME_FULL_SIZE)) {
      randomizeShortname(reinterpret_cast<char *>(entry.name));
    }
  }

  // Gera as entradas longas
  std::vector<LongEntry> lentry = createLongEntries(entry, filename);

  if (!insertDirEntries(dentry.getCluster(), entry, lentry)) {
    logger::logError("operação falhou");
    return;
  }
}
