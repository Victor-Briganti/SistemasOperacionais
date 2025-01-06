/**
 * Descrição: Implementação da classe que cuida do caminho do sistema
 * arquivos.
 *
 * Autores: João Victor Briganti, Luiz Takeda
 * Licença: BSD 2
 *
 * Data: 05/01/2025
 */

#include "path/path_parser.hpp"

#include <sstream>

//===------------------------------------------------------------------------===
// PUBLIC
//===------------------------------------------------------------------------===

PathParser::PathParser() { curPath = ROOT_DIR; }

std::string PathParser::getCurPath() const { return curPath; }

void PathParser::setCurPath(const std::string &path) { curPath = path; }

std::string PathParser::getRootDir() { return ROOT_DIR; }

std::vector<std::string> PathParser::split(const std::string &path,
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

std::string PathParser::merge(const std::vector<std::string> &path) const
{
  std::string pathName;

  for (size_t i = 0; i < path.size(); i++) {
    pathName += path[i];
    if (i < path.size() - 1 && (i != 0 && path[i] != ROOT_DIR)) {
      pathName += "/";
    }
  }

  return pathName;
}

std::vector<std::string> PathParser::generateFullPath(const std::string &path)
{
  // Lista de nomes nos caminhos
  std::vector<std::string> listPath = split(path, '/');

  if (path[0] == '/') {
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
