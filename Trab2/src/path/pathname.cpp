/**
 * Descrição: Implementação da classe que cuida do caminho do sistema
 * arquivos.
 *
 * Autores: João Victor Briganti, Luiz Gustavo Takeda, Matheus Floriano Saito
 * Licença: BSD 2
 *
 * Data: 05/01/2025
 */

#include "path/pathname.hpp"

#include <sstream>

//===------------------------------------------------------------------------===
// PUBLIC
//===------------------------------------------------------------------------===

PathName::PathName() { curPath = ROOT_DIR; }

std::string PathName::getCurPath() const { return curPath; }

void PathName::setCurPath(const std::string &path) { curPath = path; }

std::string PathName::getRootDir() const { return ROOT_DIR; }

std::vector<std::string> PathName::split(const std::string &path,
  char delim) const
{
  std::stringstream stream(path);

  std::vector<std::string> paths;
  std::string entry;

  while (getline(stream, entry, delim)) {
    paths.push_back(entry);
  }

  if (path.size() >= 4 && path.substr(0, 4) == ROOT_DIR) {
    paths[0] = ROOT_DIR;
  }

  return paths;
}

std::string PathName::merge(const std::vector<std::string> &path) const
{
  std::string pathName;

  for (size_t i = 0; i < path.size(); i++) {
    pathName += path[i];
    if (i < path.size() - 1 && (path[i] != ROOT_DIR)) {
      pathName += "/";
    }
  }

  return pathName;
}

std::vector<std::string> PathName::generateFullPath(
  const std::string &path) const
{
  // Lista de nomes nos caminhos
  std::vector<std::string> listPath = split(path, '/');

  if (path[0] == EXTERNAL_PATH) {
    return {};
  }

  // Se necessário monta o caminho completo
  if (listPath.empty() || listPath[0] != getRootDir()) {
    std::vector<std::string> fullPath = split(getCurPath(), '/');
    fullPath.insert(fullPath.end(), listPath.begin(), listPath.end());
    listPath = fullPath;
  }

  return listPath;
}

bool PathName::isRootDir(const std::vector<std::string> &listPath)
{
  return (listPath.size() == 1 && listPath[0] == ROOT_DIR);
}

std::string PathName::getLastNameFromPath(std::string &path)
{
  if (path == "") {
    throw std::runtime_error("Caminho não pode ser vazio");
  }

  // Divide o caminho
  std::vector<std::string> listPath = split(path, '/');

  if (isRootDir(listPath)) {
    throw std::runtime_error("Não é possível remover caminho do " + ROOT_DIR);
  }

  std::string name = listPath.back();
  listPath.pop_back();
  path = merge(listPath);
  return name;
}

bool PathName::isExternalPath(const std::string &path) const
{
  return path[0] == EXTERNAL_PATH;
}
