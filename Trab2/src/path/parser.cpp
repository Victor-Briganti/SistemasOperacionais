/**
 * Descrição: Tipos descritos pelo white paper da microsoft
 *
 * Autores: João Victor Briganti, Luiz Takeda
 * Licença: BSD 2
 *
 * Data: 27/12/2024
 */

#include "path/parser.hpp"
#include <cstdio>
#include <sstream>

std::vector<std::string> split(const std::string &path, char delim)
{
  std::stringstream stream(path);

  std::vector<std::string> paths;
  std::string entry;

  while (getline(stream, entry, delim)) {
    paths.push_back(entry);
  }

  if (path.size() >= 4 && path.substr(0, 4) == "img/") {
    paths[0] = "img/";
  }

  return paths;
}

std::string merge(const std::vector<std::string> &path)
{
  std::string pathName;

  for (size_t i = 1; i < path.size(); i++) {
    pathName += path[i];
    if (i < path.size() - 1) {
      pathName += "/";
    }
  }

  return pathName;
}
