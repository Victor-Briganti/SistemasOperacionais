/**
 * Descrição: Definição da interface FAT
 *
 * Autores: João Victor Briganti, Luiz Takeda
 * Licença: BSD 2
 *
 * Data: 26/12/2024
 */

#ifndef FAT_FS_HPP
#define FAT_FS_HPP

#include "filesystem/cluster/cluster.hpp"
#include "filesystem/entry/dentry.hpp"
#include "filesystem/entry/long_entry.hpp"
#include "filesystem/entry/short_entry.hpp"
#include "filesystem/structure/bpb.hpp"
#include "filesystem/structure/fat_table.hpp"
#include "filesystem/structure/fsinfo.hpp"
#include "io/image.hpp"
#include "path/pathname.hpp"

#include <memory>
#include <string>

/* Definições para os tipos de arquivos */
#define DIR_ENTRY 1
#define ARCH_ENTRY 0
#define ALL_ENTRY -1

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
   * @brief Realiza o parser de um caminho
   *
   * @param path Caminho a ser dividido e verificado
   * @param expectDir Diz ao parser  qual o tipo de arquivo sendo buscado.
   *
   * @exception Gera um exceção se houver algum problema durante a verificação
   * do caminho.
   * @return Um vetor com o caminho devidamente dividido.
   */
  std::vector<std::string> parser(const std::string &path, int expectDir);

  /**
   * @brief Busca a entrada e o cluster do diretório onde essa entrada se
   * encontra
   *
   * @param path Caminho para buscar a entrada
   * @param expectDir Diz ao parser  qual o tipo de arquivo sendo buscado.
   *
   * @exception Gera um exceção se houver algum problema durante a busca da
   * entrada do caminho.
   * @return Um par com a entrada e o número do cluster
   */
  std::pair<Dentry, DWORD> searchEntry(const std::vector<std::string> &listPath,
    int expectDir);

  /**
   * @brief Insere as entradas no espaço do diretório
   *
   * @param num Número do cluster onde ocorrerá a busca
   * @param dir Entrada curta
   * @param lentry Entradas longas
   *
   * @exception Gera um exceção se houver algum problema durante a busca da
   * entrada do caminho.
   * @return true se houver espaço false caso contrário
   */
  bool insertDirEntries(DWORD num,
    const ShortEntry &entry,
    const std::vector<LongEntry> &lentry);

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
   */
  bool updateParentTimestamp(std::string path);

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
   */
  bool cluster(DWORD num);

  /**
   * @brief Lista todos os diretórios/arquivos de um caminho especificado
   *
   * @param path Caminho a ser listado
   */
  bool ls(const std::string &path);

  /**
   * @brief Remove um arquivo do sistema
   *
   * @param path Caminho do arquivo a ser removido
   */
  bool rm(const std::string &path);

  /**
   * @brief Remove um diretório vazio do sistema
   *
   * @param path Caminho do diretório vazio a ser removido
   */
  bool rmdir(const std::string &path);

  /**
   * @brief Retorna o caminho atual do sistema
   */
  bool pwd();

  /**
   * @brief Altera o caminho atual
   *
   * @param path Novo caminho base do sistema
   */
  bool cd(const std::string &path);

  /**
   * @brief Imprime os atributos de uma entrada
   *
   * @param path Caminho para a entrada a ser apresentada
   */
  bool attr(const std::string &path);

  /**
   * @brief Cria um arquivo vazio
   *
   * @param path Caminho para a entrada a ser criada
   */
  bool touch(const std::string &path);
};

#endif// FAT_FS_HPP
