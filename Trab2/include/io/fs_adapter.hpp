/**
 * Descrição: Classe para lidar com sistema de arquivos do OS
 *
 * Autores: João Victor Briganti, Luiz Takeda
 * Licença: BSD 2
 *
 * Data: 09/02/2025
 */

#ifndef FILESYSTEM_ADAPTER_HPP
#define FILESYSTEM_ADAPTER_HPP

#include "utils/types.hpp"

#include <fstream>
#include <string>

/* Opções disponíveis para a abertura do arquivo */
enum FileOption {
  WRITE,
  READ,
};

class FileSystemAdapter
{
  /* Caminho para o arquivo sendo manipulado */
  std::string filePath;

  /* Ponteiro para o arquivo que vai ser manipulado */
  std::fstream file;

  /* Define o como o arquivo está aberto */
  FileOption fileOption;

  /* Flag para dizer se o arquivo foi ou não excluído */
  bool isDeleted;

  /**
   * @brief Limpa o arquivo
   *
   * Está opção só deve ser usada quando o arquivo for aberto em maneira de
   * leitura.
   *
   * @param path Caminho para o arquivo que será limpo
   *
   * @return true se foi possível limpar o arquivo, false caso contrário
   */
  bool clean(const std::string &path);

  /**
   * @brief Abre o arquivo conforme a opção passada
   *
   * @param path Caminho para o arquivo que será aberto
   *
   * @return true se foi possível abrir o arquivo, false caso contrário
   */
  bool open(const std::string &path, FileOption option);

public:
  /**
   * @brief Construtor para a manipulação do arquivo
   *
   * Abre o arquivo para manipulação especificada pelo caminho.
   *
   * @param path Caminho para o arquivo
   * @param option Opção usada ao abrir o arquivo
   *
   * @exception Argumento inválido, quando o caminho não é válido
   * @exception Runtime error, quando não é possível abrir o arquivo
   */
  explicit FileSystemAdapter(const std::string &path, FileOption option);

  ~FileSystemAdapter();

  /**
   * @brief Escreve no arquivo
   *
   * @param offset Posição onde a escrita será iniciada
   * @param buffer Posição de memória que tem os dados a serem escritos
   * @param size Tamanho do dado a ser escrito
   *
   * @return true se foi possível escrever, false caso contrário
   */
  bool write(DWORD offset, const void *const buffer, size_t size);

  /**
   * @brief Lê o arquivo
   *
   * @param offset Posição onde a leitura será iniciada
   * @param buffer Posição de memória que irá receber o dado lido
   * @param size Tamanho do dado a ser lido
   *
   * @return true se foi possível ler, false caso contrário
   */
  bool read(DWORD offset, void *buffer, size_t size);

  /**
   * @brief Exclui o arquivo
   *
   * @return true se foi possível remover, false caso contrário
   */
  bool remove();
};

#endif// FILESYSTEM_ADAPTER_HPP