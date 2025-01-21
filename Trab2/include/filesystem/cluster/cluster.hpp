/**
 * Descrição: Definição da classe que abstrai os clusters do sistema
 *
 * Autores: João Victor Briganti, Luiz Takeda
 * Licença: BSD 2
 *
 * Data: 06/01/2025
 */

#ifndef CLUSTER_HPP
#define CLUSTER_HPP

#include "filesystem/entry/dentry.hpp"
#include "filesystem/structure/fsinfo.hpp"
#include "path/pathname.hpp"

#include <memory>
#include <optional>
#include <vector>

class ClusterIO
{
  /* Interface usada para ler e escrever a imagem */
  std::shared_ptr<Image> image;

  /* Interface usada lidar com a estrutura BPB */
  std::shared_ptr<BiosBlock> bios;

  /* Estrutura que armazena informações sobre os clusters */
  std::shared_ptr<FileSysInfo> fsInfo;

  /* Interface para lidar com a tabela FAT */
  std::shared_ptr<FatTable> fatTable;

  /* Caminho atual no sistema de arquivos */
  std::shared_ptr<PathName> pathName;

  /* Define a quantidade de entradas por cluster */
  size_t NUM_ENTRIES;

  /**
   * @brief Gera as entradas relacionadas ao nome especificado
   *
   * @param num Número do cluster onde a entrada será criada
   * @param name Nome da entrada a ser criada
   * @param sentry Referência a entrada curta a ser criada
   * @param lentry Referência a entrada longa a ser criada
   * @param attrs Atributos da entrada a ser criada
   *
   * @exception Gera um exceção se houver algum problema durante a busca do
   * cluster.
   * @return true se foi possível criar, false caso contrário
   */
  bool generateEntries(DWORD num,
    const std::string &name,
    ShortEntry &sentry,
    std::vector<LongEntry> &lentry,
    BYTE attrs);

  /**
   * @brief Busca entradas vazias e continuas
   *
   * @param cluster Número do cluster onde ocorrerá a busca
   * @param numEntries Quantidade de entradas a serem alocadas
   * @param clusterIndexes Lista com as posições para escrever a entrada
   *
   * @exception Gera um exceção se houver algum problema durante a busca do
   * cluster.
   * @return 0 se o espaço vazio suporta todas as entradas, 0< se o espaço não é
   * suficiente.
   */
  int searchEmptyEntries(DWORD cluster,
    DWORD numEntries,
    std::vector<ClusterIndex> &clusterIndexes);

  /**
   * @brief Inicializa e insere as entradas DOT e DOTDOT nod diretório criado
   *
   * @param parentClt Número do cluster onde a entrada será criada
   * @param dentry Entrada que representa o diretório alocado
   *
   * @return true se foi possível alocar, false caso contrário.
   */
  bool insertDotEntries(DWORD parentClt, Dentry &dentry);

  /**
   * @brief Cria uma entrada do tipo diretório e insere no buffer de entrada
   *
   * Além da criação da entrada, no diretório também se faz necessário alocar um
   * buffer para receber as demais entradas.
   *
   * @param num Número do cluster onde a entrada será criada
   * @param name Nome do arquivo a ser criado
   *
   * @return true se foi possível criar, false caso contrário.
   */
  bool createDirectoryEntry(DWORD num, const std::string &name);

  /**
   * @brief Cria uma entrada do tipo arquivo e insere no buffer de entrada
   *
   * @param num Número do cluster onde a entrada será criada
   * @param name Nome do arquivo a ser criado
   *
   * @return true se foi possível criar, false caso contrário.
   */
  bool createArchiveEntry(DWORD num, const std::string &name);

public:
  explicit ClusterIO(std::shared_ptr<Image> image,
    std::shared_ptr<BiosBlock> bios,
    std::shared_ptr<FatTable> fatTable,
    std::shared_ptr<FileSysInfo> fsInfo,
    std::shared_ptr<PathName> pathName);

  /**
   * @brief Lê um cluster da memória em um buffer
   *
   * @param buffer Ponteiro que irá receber as informações a serem lidas
   * @param num Número do cluster que será lido
   *
   * @return true se foi possível ler, false caso contrário
   */
  bool readCluster(void *buffer, DWORD num);

  /**
   * @brief Escreve um cluster na imagem
   *
   * @param buffer Ponteiro com as informações que serão escritas
   * @param num Número do cluster que será escrito
   *
   * @return true se foi possível escrever, false caso contrário
   */
  bool writeCluster(const void *buffer, DWORD num);

  /**
   * @brief Aloca um novo cluster
   *
   * @param cluster Número do cluster que será estendido. Se o valor for 0, uma
   * nova cadeia será criada
   *
   * @exception Gera um exceção se houver algum problema durante a busca do
   * cluster.
   * @return true se foi possível alocar o cluster, false caso contrário.
   */
  bool allocNewCluster(DWORD &cluster);

  /**
   * @brief Retorna o cluster da entrada corrente
   *
   * No caso da entrada que é um "..", ao tentar retornar seu cluster o valor 0
   * será retornado. Para evitar problemas aqui retornamos o valor do
   * RootClus.
   *
   * @param entry Entrada que terá seu cluster extraído.
   *
   * @return O valor do cluster da entrada
   */
  DWORD getEntryClus(const Dentry &entry);

  /**
   * @brief Retorna uma lista com todas as entradas de um diretório
   *
   * @param num Número do cluster do diretório que será listado
   *
   * @exception Exceções podem ocorrer durante a leitura das informações das
   * entradas.
   *
   * @return Um vetor com todos as entradas se foi possível ler.
   */
  std::vector<Dentry> getListEntries(DWORD num);

  /**
   * @brief Busca uma entrada por meio do caminho passado
   *
   * @param path Caminho que está sendo buscado
   * @param listPath Referência a lista com todas as partes do caminho. Este
   * caminho é alterado ao longo da busca.
   * @param searchType Especifica o tipo de entrada sendo buscado.
   *
   * @exception Gera um exceção se houver algum problema durante a verificação
   * do caminho.
   * @return Um vetor com o caminho propriamente dividido e o cluster no qual o
   * último arquivo se encontra
   */
  std::optional<Dentry> searchEntryByPath(const std::string &path,
    std::vector<std::string> &listPath,
    EntryType searchType);

  /**
   * @brief Atualiza uma entrada dentro do cluster
   *
   * Reescreve a entrada passada nos clusters em que suas entradas são
   * localizadas.
   *
   * @param entry Entrada a ser atualizada
   *
   * @return true se foi possível atualizar, false caso contrário.
   */
  bool updateEntry(Dentry &entry);

  /**
   * @brief Remove uma entrada do cluster
   *
   * @param entry Entrada a ser removida
   *
   * @return true se foi possível remover, false caso contrário.
   */
  bool removeEntry(Dentry &entry);

  /**
   * @brief Deleta a entrada do sistema de arquivo
   *
   * A entrada é removida tanto do cluster quanto do FAT.
   *
   * @param entry Entrada a ser removida
   *
   * @return true se foi possível deletar, false caso contrário.
   */
  bool deleteEntry(Dentry &entry);

  /**
   * @brief Aloca a entrada e insere no buffer
   *
   * @param name Nome do arquivo a ser criado
   * @param num Número do cluster onde a entrada será criada
   *
   * @return true se foi possível criar, false caso contrário.
   */
  bool createEntry(DWORD num, const std::string &name, EntryType type);
};

#endif// CLUSTER_HPP
