/*
 * Descrição: Cabeçalho do shell
 *
 * Autores: Hendrick Felipe Scheifer, João Victor Briganti, Luiz Takeda
 * Licença: BSD 2
 *
 * Data: 15/11/2024
 */

#ifndef SHELL_HPP
#define SHELL_HPP

#include <cstddef>
#include <string>
#include <vector>

class Shell
{
  // Enum para todos os comandos que são aceitos pelo shell
  enum Command {
    ATTR,
    CD,
    CLUSTER,
    CP,
    INFO,
    LS,
    MKDIR,
    MV,
    PWD,
    RENAME,
    RM,
    RMDIR,
    TOUCH,

    UNKNOW
  };

  /*
   * @brief Realiza o parse da entrada e determina o comando
   *
   * @param input Entrada passada pelo usuário
   * @param pos Última posição do comando
   *
   * @return Retorna o enum que equivale ao comando
   */
  Command parse_command(const std::string &input, size_t &pos);

  /*
   * @brief Realiza o parse do caminho
   *
   * @param input Caminho passado pelo usuário
   *
   * @return Retorna um vetor com a lista da hierarquia dos caminhos em ordem.
   * Se o vetor for vazio houve algum erro durante o parser
   */
  std::vector<std::string> parse_path(const std::string &input);

public:
  /*
   * @brief Interface publica para iniciar o shell
   */
  void interpreter();
};

#endif// SHELL_HPP
