/**
 * Descrição: Definição da classe que define o Shell iterativo do sistema.
 *
 * Autores: João Victor Briganti, Luiz Takeda
 * Licença: BSD 2
 *
 * Data: 10/01/2025
 */

#ifndef SHELL_HPP
#define SHELL_HPP

#include "filesystem/fatfs.hpp"

enum FSApi {
  INFO,
  CLUSTER,
  LS,
  RM,
  RMDIR,
  PWD,
  CD,
  ATTR,
  TOUCH,
  MKDIR,
  RENAME,
  MV,
  CP,

  FAIL,
};

class Shell
{
  /* Ponteiro para a interface do sistema de arquivos */
  std::unique_ptr<FatFS> fatFS;

  /**
   * @brief Dividi o comando passado em diferentes partes
   *
   * @param command Comando inserido pelo usuário
   *
   * @return Uma lista com todas as partes do comando
   */
  std::vector<std::string> parser(const std::string &cmd) const;

  /**
   * @brief Analisa a string e retorna o comando
   *
   * @param command Comando inserido pelo usuário
   *
   * @return O FSApi referente ao comando
   */
  FSApi command(const std::string &cmd) const;

  /**
   * @brief Executa o loop do shell iterativo
   */
  void exec(FSApi cmd, std::vector<std::string> params);

public:
  /**
   * @brief Inicia a estrutura do shell
   *
   * @param fatFS Interface do sistema de arquivos.
   */
  explicit Shell(std::unique_ptr<FatFS> fatFS);

  /**
   * @brief Executa o loop do shell iterativo
   */
  void interpreter();
};

#endif// SHELL_HPP
