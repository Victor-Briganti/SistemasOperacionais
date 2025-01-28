/**
 * Descrição: Definição da interface FAT
 *
 * Autores: João Victor Briganti, Luiz Gustavo Takeda, Matheus Floriano Saito
 * Licença: BSD 2
 *
 * Data: 26/12/2024
 */

#ifndef FAT_FS_HPP
#define FAT_FS_HPP

#include "filesystem/cluster/cluster.hpp"
#include "filesystem/structure/bpb.hpp"
#include "filesystem/structure/fat_table.hpp"
#include "filesystem/structure/fsinfo.hpp"
#include "io/image.hpp"
#include "path/pathname.hpp"

#include <memory>
#include <string>

class FatFS
{
private:
  /* Interface usada para ler e escrever a imagem */
  std::shared_ptr<Image> image;

  /* Interface usada lidar com a estrutura BPB */
  std::shared_ptr<BiosBlock> bios;

  /* Interface para lidar com a tabela FAT */
  std::shared_ptr<FatTable> fatTable;

  /* Interface para lidar com a estrutura FSInfo */
  std::shared_ptr<FileSysInfo> fsInfo;

  /* Caminho atual no sistema de arquivos */
  std::shared_ptr<PathName> pathName;

  /* Classe para acesso aos clusters */
  std::shared_ptr<ClusterIO> clusterIO;

  /**
   * @brief Helper para a listagem de diretórios
   *
   * @param num Número do cluster onde ocorrerá a listagem
   *
   * @exception Gera um exceção se houver algum problema durante a busca do
   * cluster.
   */
  void listClusterDir(DWORD num);

  /**
   * @brief Atualiza as timestamps do diretório pai
   *
   * @param path Caminho para o diretório que precisa ter o seu tempo atualizado
   *
   * @exception Gera um exceção se houver algum problema durante a busca do
   * cluster.
   * @return 0 se foi possível atualizar, 1 se foi tentado atualizar o pai e -1 caso tenha algum erro.
   */
  int updateParentTimestamp(std::string path);

  /**
   * @brief Cópia dados internos para uma localização externa
   *
   * @param from Caminho do arquivo interno
   * @param to Caminho para o arquivo externo
   *
   * @exception Gera um exceção se houver algum problema durante a busca do
   * cluster.
   * @return true se foi possível copiar, false caso contrário
   */
  bool copyInternalData(const std::string &from, const std::string &to);

  /**
   * @brief Cópia dados externos para uma localização interna
   *
   * @param from Caminho do arquivo externo
   * @param to Caminho para o arquivo interno
   *
   * @exception Gera um exceção se houver algum problema durante a busca do
   * cluster.
   * @return true se foi possível copiar, false caso contrário
   */
  bool copyExternalData(const std::string &from, const std::string &to);

  /**
   * @brief Cópia dados de um caminho interno para outro interno
   *
   * @param from Caminho do arquivo interno
   * @param to Caminho para o arquivo interno
   *
   * @exception Gera um exceção se houver algum problema durante a busca do
   * cluster.
   * @return true se foi possível copiar, false caso contrário
   */
  bool copyInSystemData(const std::string &from, const std::string &to);

public:
  /**
   * @brief Inicia a estrutura do sistema de arquivo
   *
   * Aloca cada uma das estruturas e as inicializa.
   *
   * @param path Caminho para a imagem FAT.
   *
   * @exception Gera uma exceção se não for possível alocar ou inicializar as
   * estruturas.
   */
  explicit FatFS(const std::string &path);

  /**
   * @brief Mostra as informações sobre o sistema de arquivos
   */
  void info();

  /**
   * @brief Mostra as informações de um cluster no formato texto
   *
   * @param num Número do cluster no qual as informações serão mostradas
   *
   * @return true se foi possível ler, false caso contrário
   */
  bool cluster(DWORD num);

  /**
   * @brief Lista todos os diretórios/arquivos de um caminho especificado
   *
   * @param path Caminho a ser listado
   *
   * @return true se foi possível listar diretório, false caso contrário
   */
  bool ls(const std::string &path);

  /**
   * @brief Remove um arquivo do sistema
   *
   * @param path Caminho do arquivo a ser removido
   *
   * @return true se foi possível remover arquivo, false caso contrário
   */
  bool rm(const std::string &path);

  /**
   * @brief Remove um diretório vazio do sistema
   *
   * @param path Caminho do diretório vazio a ser removido
   *
   * @return true se foi possível remover diretório, false caso contrário
   */
  bool rmdir(const std::string &path);

  /**
   * @brief Retorna o caminho atual do sistema
   */
  std::string pwd();

  /**
   * @brief Altera o caminho atual
   *
   * @param path Novo caminho base do sistema
   *
   * @return true se foi possível alterar caminho atual, false caso contrário
   */
  bool cd(const std::string &path);

  /**
   * @brief Imprime os atributos de uma entrada
   *
   * @param path Caminho para a entrada a ser apresentada
   *
   * @return true se foi possível listar atributos, false caso contrário
   */
  bool attr(const std::string &path);

  /**
   * @brief Cria um arquivo vazio
   *
   * @param path Caminho para a entrada a ser criada
   *
   * @return true se foi possível criar arquivo, false caso contrário
   */
  bool touch(const std::string &path);

  /**
   * @brief Cria um diretório
   *
   * @param path Caminho para o diretório a ser criado
   *
   * @return true se foi possível criar diretório, false caso contrário
   */
  bool mkdir(const std::string &path);

  /**
   * @brief Renomeia um arquivo
   *
   * @param from Caminho do arquivo original
   * @param to Caminho do destino
   *
   * @return true se foi possível renomear, false caso contrário
   */
  bool rename(const std::string &from, const std::string &to);

  /**
   * @brief Move um arquivo de um caminho a outro
   *
   * @param from Caminho do arquivo original
   * @param to Caminho do destino
   *
   * @return true se foi possível mover, false caso contrário
   */
  bool mv(const std::string &from, const std::string &to);

  /**
   * @brief Copia um arquivo de um caminho a outro
   *
   * @param from Caminho do arquivo original
   * @param to Caminho do destino
   *
   * @return true se foi possível copiar, false caso contrário
   */
  bool cp(const std::string &from, const std::string &to);
};

#endif// FAT_FS_HPP
