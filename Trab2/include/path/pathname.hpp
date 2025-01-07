/**
 * Descrição: Definição da classe que cuida do caminho do sistema
 * arquivos.
 *
 * Autores: João Victor Briganti, Luiz Takeda
 * Licença: BSD 2
 *
 * Data: 05/01/2025
 */

#ifndef PATHNAME_HPP
#define PATHNAME_HPP

#include <string>
#include <vector>

class PathName
{
  /* Define o diretório raiz do sistema */
  const std::string ROOT_DIR = "img/";

  /* Define o caminho atual do sistema */
  std::string curPath;

public:
  PathName();

  /** @brief Retorna o caminho atual do sistema */
  std::string getCurPath() const;

  /**
   * @brief Atualiza o caminho atual do sistema
   *
   * @param path Novo caminho para o sistema
   */
  void setCurPath(const std::string &path);

  /** @brief Retorna o diretório raiz atual */
  std::string getRootDir();

  /**
   * @brief Dividi um caminho em suas partes
   *
   * @param path Caminho a ser dividido em partes
   * @param delim Caracter que delimita a divisão do caminho
   *
   * @return Um vetor contendo o nome de cada parte do caminho
   */
  std::vector<std::string> split(const std::string &path, char delim) const;

  /**
   * @brief Junta todas as partes de um caminho
   *
   * @param path Vetor com cada parte do caminho a ser unido
   *
   * @return Uma string com o caminho
   */
  std::string merge(const std::vector<std::string> &path) const;

  /**
   * @brief Gera o caminho completo a partir de um caminho qualquer
   *
   * @param path Caminho a ser tratado
   *
   * @return Uma lista com o caminho completo
   */
  std::vector<std::string> generateFullPath(const std::string &path);
};

#endif// PATH_PARSER_HPP