/**
 * Descrição: Definição da interface para lidar com diretórios e arquivos do FAT
 *
 * Autores: João Victor Briganti, Luiz Takeda
 * Licença: BSD 2
 *
 * Data: 26/12/2024
 */

#ifndef DENTRY_HPP
#define DENTRY_HPP

#include "filesystem/cluster/cluster_index.hpp"
#include "filesystem/default.hpp"
#include "filesystem/entry/long_entry.hpp"
#include "filesystem/entry/short_entry.hpp"
#include "utils/time.hpp"
#include "utils/types.hpp"

#include <array>
#include <string>
#include <vector>

/* Define os tipos de nomes que a entrada pode ter */
enum DentryNameType {
  DOT_NAME = 0,
  DOTDOT_NAME = 1,
  LONG_NAME = 2,
  SHORT_NAME = 3,
};

class Dentry
{
  /* Estrutura de metadados */
  ShortEntry entry;

  /* Lista de nomes longos */
  std::vector<LongEntry> longEntries;

  /* Nome curto da entrada */
  std::array<char, NAME_FULL_SIZE + 1> shortName;

  /* Nome longo da entrada */
  std::string longName;

  /* Define qual o tipo de nome que a entrada possui */
  DentryNameType nameType;

  /* Define qual o tipo da entrada */
  EntryType type;

  /* Lista de clusters e posições que está entrada pode estar */
  std::vector<ClusterIndex> clusterIndexes;

public:
  /**
   * @brief Inicializa a entrada
   *
   * @param entry Entrada curta, que possui os atributos do arquivo em si
   * @param lentry Vetor que contém todos as partes do nome longo
   * @param initPost Posição inicial na entrada do buffer
   * @param endPos Posição final na entrada do buffer
   *
   * @exception Gera uma exceção no caso de haver algum erro no entry ou lentry.
   */
  explicit Dentry(const ShortEntry &entry,
    const std::vector<LongEntry> &lentry,
    const std::vector<ClusterIndex> &clusterIndexes);

  /**
   * @brief Nome longo da entrada
   *
   * @return retorna o nome longo da entrada
   */
  [[nodiscard]] inline std::string getLongName() const
  {
    switch (nameType) {
    case DOT_NAME:
      return ".";
    case DOTDOT_NAME:
      return "..";
    case SHORT_NAME:
      return shortName.data();
    default:
      return longName;
    }
  }

  /**
   * @brief Nome curto da entrada
   *
   * @return retorna o nome curto da entrada
   */
  [[nodiscard]] inline const char *getShortName() const
  {
    return shortName.data();
  }

  /**
   * @brief Cluster onde os dados da entrada estão armazenados
   *
   * @return retorna o cluster referente aos dados dessa entrada
   */
  [[nodiscard]] inline DWORD getDataCluster() const
  {
    return static_cast<DWORD>(entry.fstClusHI << 16) | entry.fstClusLO;
  }

  /**
   * @brief Defini o cluster dessa entrada
   *
   * @param cluster Número do custer que esta entrada está relacionada
   */
  void setDataCluster(DWORD cluster);

  /**
   * @brief Defini o tamanho do arquivo
   *
   * @param size Tamanho dessa entrada dentro do sistema
   */
  void setFileSize(DWORD fileSize);

  /**
   * @brief Cluster onde está entrada está armazenada
   *
   * @return retorna o cluster referente ao cluster que armazena está entrada
   */
  [[nodiscard]] inline DWORD getEntryCluster() const
  {
    return clusterIndexes[0].clusterNum;
  }

  /**
   * @brief Tamanho do arquivo
   *
   * @return retorna o tamanho do arquivo
   */
  [[nodiscard]] inline DWORD getFileSize() const { return entry.fileSize; }

  /**
   * @brief Retorna verdadeiro se é um diretório
   *
   * @return retorna o valor de directory
   */
  [[nodiscard]] inline bool isDirectory() const
  {
    return ((entry.attr & ATTR_DIRECTORY) != 0);
  }

  /**
   * @brief Retorna verdadeiro se é uma entrada escondida
   *
   * @return retorna o valor de hidden
   */
  [[nodiscard]] inline bool isHidden() const
  {
    return ((entry.attr & ATTR_HIDDEN) != 0);
  }

  /**
   * @brief Retorna verdadeiro se é somente leitura
   *
   * @return retorna o valor de readOnly
   */
  [[nodiscard]] inline bool isReadOnly() const
  {
    return ((entry.attr & ATTR_READ_ONLY) != 0);
  }

  /**
   * @brief Retorna o dia de criação do arquivo
   *
   * @return retorna o dia do datestamp crtDate
   */
  [[nodiscard]] inline WORD getCreationDay() const
  {
    return timefs::day(entry.crtDate);
  }

  /**
   * @brief Retorna o mês de criação do arquivo
   *
   * @return retorna o mês do datestamp crtDate
   */
  [[nodiscard]] inline WORD getCreationMonth() const
  {
    return timefs::month(entry.crtDate);
  }

  /**
   * @brief Retorna o ano de criação do arquivo
   *
   * @return retorna o ano do datestamp crtDate
   */
  [[nodiscard]] inline WORD getCreationYear() const
  {
    return timefs::year(entry.crtDate);
  }

  /**
   * @brief Retorna a hora de criação do arquivo
   *
   * @return retorna a hora do timestamp crtTime
   */
  [[nodiscard]] inline WORD getCreationHour() const
  {
    return timefs::hour(entry.crtTime);
  }

  /**
   * @brief Retorna o mês de criação do arquivo
   *
   * @return retorna o mês do timestamp crtTime
   */
  [[nodiscard]] inline WORD getCreationMinute() const
  {
    return timefs::minutes(entry.crtTime);
  }

  /**
   * @brief Retorna o ano de criação do arquivo
   *
   * @return retorna o ano do timestamp crtTime
   */
  [[nodiscard]] inline WORD getCreationSeconds() const
  {
    return timefs::seconds(entry.crtTime);
  }

  /**
   * @brief Retorna o dia de criação do arquivo
   *
   * @return retorna o dia do datestamp wrtDate
   */
  [[nodiscard]] inline WORD getWriteDay() const
  {
    return timefs::day(entry.wrtDate);
  }

  /**
   * @brief Retorna o mês de criação do arquivo
   *
   * @return retorna o mês do datestamp wrtDate
   */
  [[nodiscard]] inline WORD getWriteMonth() const
  {
    return timefs::month(entry.wrtDate);
  }

  /**
   * @brief Retorna o ano de criação do arquivo
   *
   * @return retorna o ano do datestamp wrtDate
   */
  [[nodiscard]] inline WORD getWriteYear() const
  {
    return timefs::year(entry.wrtDate);
  }

  /**
   * @brief Retorna o dia de criação do arquivo
   *
   * @return retorna o dia do datestamp lstAccDate
   */
  [[nodiscard]] inline WORD getLstAccDay() const
  {
    return timefs::day(entry.lstAccDate);
  }

  /**
   * @brief Retorna o mês de criação do arquivo
   *
   * @return retorna o mês do datestamp lstAccDate
   */
  [[nodiscard]] inline WORD getLstAccMonth() const
  {
    return timefs::month(entry.lstAccDate);
  }

  /**
   * @brief Retorna o ano de criação do arquivo
   *
   * @return retorna o ano do datestamp lstAccDate
   */
  [[nodiscard]] inline WORD getLstAccYear() const
  {
    return timefs::year(entry.lstAccDate);
  }

  /**
   * @brief Retorna a hora de criação do arquivo
   *
   * @return retorna a hora do timestamp wrtTime
   */
  [[nodiscard]] inline WORD getWriteHour() const
  {
    return timefs::hour(entry.wrtTime);
  }

  /**
   * @brief Retorna o mês de criação do arquivo
   *
   * @return retorna o mês do timestamp wrtTime
   */
  [[nodiscard]] inline WORD getWriteMinute() const
  {
    return timefs::minutes(entry.wrtTime);
  }

  /**
   * @brief Retorna o ano de criação do arquivo
   *
   * @return retorna o ano do timestamp wrtTime
   */
  [[nodiscard]] inline WORD getWriteSeconds() const
  {
    return timefs::seconds(entry.wrtTime);
  }

  /**
   * @brief Retorna a entrada curta relacionada a está entrada
   *
   * @return retorna uma cópia de dir
   */
  [[nodiscard]] inline ShortEntry getShortEntry() const { return entry; }

  /**
   * @brief Retorna todas entradas longas relacionada a está entrada
   *
   * @return retorna uma cópia da lista de entrada longas
   */
  [[nodiscard]] inline std::vector<LongEntry> getLongEntries() const
  {
    return longEntries;
  }

  /**
   * @brief Retorna a lista de indexação desta entrada nos clusters
   *
   * @return retorna a lista de indexação
   */
  [[nodiscard]] inline std::vector<ClusterIndex> getClusterIndexes() const
  {
    return clusterIndexes;
  }

  /**
   * @brief Tipo do arquivo
   *
   * Função usada para identificar se o arquivo é do tipo DOT ou um nome longo
   * comum.
   *
   * @return retorna o valor de nameType
   */
  [[nodiscard]] inline int getNameType() const { return nameType; }

  /**
   * @brief Retorna o tipo da entrada
   *
   * @return retorna o valor de type
   */
  [[nodiscard]] inline EntryType getEntryType() const { return type; }

  /**
   * @brief Imprime as informações sobre a entrada
   */
  void printInfo() const;

  /**
   * @brief Marca todas as entradas que fazem parte deste arquivo/diretório como
   * livres
   */
  void markFree();

  /**
   * @brief Atualiza a timestamp e a datestamp relacionadas a escrita
   */
  void updatedWrtTimestamp();
};

#endif// DENTRY_HPP
