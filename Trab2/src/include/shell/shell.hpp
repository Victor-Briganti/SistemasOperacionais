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
#include <cstdint>
#include <string>
#include <vector>

typedef std::vector<std::string> path_fs;

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
  static Command parse_command(const std::string &input, size_t &pos);

  /*
   * @brief Realiza o parse do caminho
   *
   * @param input Caminho passado pelo usuário
   * @param pos Posição da string no argumento
   *
   * @return Retorna um vetor com a lista da hierarquia dos caminhos em ordem.
   * Se o vetor for vazio houve algum erro durante o parser
   */
  static path_fs parse_path(const std::string &input, size_t &pos);

  /*
   * @brief Extrai os atributos de um arquivo/diretório e imprime na tela
   *
   * @param path Caminho para o diretório ou arquivo
   *
   * @return true se a execução ocorreu, false caso contrário.
   */
  static bool attr(const path_fs &path);

  /*
   * @brief Altera o diretório atual
   *
   * @param path Caminho para o novo diretório
   *
   * @return true se a execução ocorreu, false caso contrário.
   */
  static bool cd(const path_fs &path);

  /*
   * @brief Exibe o conteudo de um bloco no formato texto
   *
   * @param num Número do bloco que queremos ler
   *
   * @return true se a execução ocorreu, false caso contrário.
   */
  static bool cluster(uint64_t num);

  /*
   * @brief Copia um arquivo de origem para o de destino
   *
   * @param src Caminho de origem
   * @param dest Caminho de destino
   *
   * @return true se a execução ocorreu, false caso contrário.
   */
  static bool cp(const path_fs &src, const path_fs &dest);

  /*
   * @brief Mostra informações sobre o sistema de arquivos
   *
   * @return true se a execução ocorreu, false caso contrário.
   */
  static bool info();

  /*
   * @brief Mostra arquivos e diretórios do diretório atual
   *
   * @return true se a execução ocorreu, false caso contrário.
   */
  static bool ls();

  /*
   * @brief Executa o comando especificado pelo usuário
   *
   * @param command Comando a ser executado
   * @param input Argumentos necessários para que o comando execute
   * @param pos Posição em que os argumentos se iniciam na entrada
   *
   * @return true se a execução ocorreu, false caso contrário.
   */
  static bool
    execution(const Command command, const std::string &input, size_t pos);

public:
  /*
   * @brief Interface publica para iniciar o shell
   */
  static void interpreter();
};

#endif// SHELL_HPP
