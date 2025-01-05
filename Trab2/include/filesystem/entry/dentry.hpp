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

#include "filesystem/default.hpp"
#include "filesystem/dir.hpp"
#include "filesystem/entry/short_entry.hpp"
#include "utils/time.hpp"
#include "utils/types.hpp"

#include <array>
#include <string>
#include <vector>

// Define os tipos de nomes que a entrada pode ter
enum DentryType {
  DOT_NAME = 0,
  DOTDOT_NAME = 1,
  LONG_NAME = 2,
};

class Dentry
{
  /* Estrutura de metadados */
  ShortEntry entry;

  /* Lista de nomes longos */
  std::vector<LongDir> longDirs;

  /* Nome longo da entrada */
  std::string longName;

  /* Nome curto da entrada */
  std::array<char, NAME_FULL_SIZE + 1> shortName;

  /* Define qual o tipo de nome que a entrada possui */
  DentryType type;

  /* Posição inicial no buffer do diretório */
  DWORD initPos;

  /* Posição final no buffer do diretório */
  DWORD endPos;

public:
  /**
   * @brief Inicializa a entrada
   *
   * @param dir Entrada curta, que possui os atributos do arquivo em si
   * @param ldir Vetor que contém todos as partes do nome longo
   * @param initPost Posição inicial na entrada do buffer
   * @param endPos Posição final na entrada do buffer
   *
   * @exception Gera uma exceção no caso de haver algum erro no dir ou ldir.
   */
  explicit Dentry(const ShortEntry &entry,
    const std::vector<LongDir> &ldir,
    const DWORD initPos,
    const DWORD endPos);

  /**
   * @brief Nome longo da entrada
   *
   * @return retorna o nome longo da entrada
   */
  [[nodiscard]] inline std::string getLongName() const
  {
    switch (type) {
    case DOT_NAME:
      return ".";
    case DOTDOT_NAME:
      return "..";
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
   * @brief Cluster da entrada
   *
   * @return retorna o cluster da entrada
   */
  [[nodiscard]] inline DWORD getCluster() const
  {
    return static_cast<DWORD>(entry.fstClusHI << 16) | entry.fstClusLO;
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
    return day(entry.crtDate);
  }

  /**
   * @brief Retorna o mês de criação do arquivo
   *
   * @return retorna o mês do datestamp crtDate
   */
  [[nodiscard]] inline WORD getCreationMonth() const
  {
    return month(entry.crtDate);
  }

  /**
   * @brief Retorna o ano de criação do arquivo
   *
   * @return retorna o ano do datestamp crtDate
   */
  [[nodiscard]] inline WORD getCreationYear() const
  {
    return year(entry.crtDate);
  }

  /**
   * @brief Retorna a hora de criação do arquivo
   *
   * @return retorna a hora do timestamp crtTime
   */
  [[nodiscard]] inline WORD getCreationHour() const
  {
    return hour(entry.crtTime);
  }

  /**
   * @brief Retorna o mês de criação do arquivo
   *
   * @return retorna o mês do timestamp crtTime
   */
  [[nodiscard]] inline WORD getCreationMinute() const
  {
    return minutes(entry.crtTime);
  }

  /**
   * @brief Retorna o ano de criação do arquivo
   *
   * @return retorna o ano do timestamp crtTime
   */
  [[nodiscard]] inline WORD getCreationSeconds() const
  {
    return seconds(entry.crtTime);
  }

  /**
   * @brief Retorna o dia de criação do arquivo
   *
   * @return retorna o dia do datestamp wrtDate
   */
  [[nodiscard]] inline WORD getWriteDay() const { return day(entry.wrtDate); }

  /**
   * @brief Retorna o mês de criação do arquivo
   *
   * @return retorna o mês do datestamp wrtDate
   */
  [[nodiscard]] inline WORD getWriteMonth() const
  {
    return month(entry.wrtDate);
  }

  /**
   * @brief Retorna o ano de criação do arquivo
   *
   * @return retorna o ano do datestamp wrtDate
   */
  [[nodiscard]] inline WORD getWriteYear() const { return year(entry.wrtDate); }

  /**
   * @brief Retorna o dia de criação do arquivo
   *
   * @return retorna o dia do datestamp lstAccDate
   */
  [[nodiscard]] inline WORD getLstAccDay() const
  {
    return day(entry.lstAccDate);
  }

  /**
   * @brief Retorna o mês de criação do arquivo
   *
   * @return retorna o mês do datestamp lstAccDate
   */
  [[nodiscard]] inline WORD getLstAccMonth() const
  {
    return month(entry.lstAccDate);
  }

  /**
   * @brief Retorna o ano de criação do arquivo
   *
   * @return retorna o ano do datestamp lstAccDate
   */
  [[nodiscard]] inline WORD getLstAccYear() const
  {
    return year(entry.lstAccDate);
  }

  /**
   * @brief Retorna a hora de criação do arquivo
   *
   * @return retorna a hora do timestamp wrtTime
   */
  [[nodiscard]] inline WORD getWriteHour() const { return hour(entry.wrtTime); }

  /**
   * @brief Retorna o mês de criação do arquivo
   *
   * @return retorna o mês do timestamp wrtTime
   */
  [[nodiscard]] inline WORD getWriteMinute() const
  {
    return minutes(entry.wrtTime);
  }

  /**
   * @brief Retorna o ano de criação do arquivo
   *
   * @return retorna o ano do timestamp wrtTime
   */
  [[nodiscard]] inline WORD getWriteSeconds() const
  {
    return seconds(entry.wrtTime);
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
  [[nodiscard]] inline std::vector<LongDir> getLongDirectories() const
  {
    return longDirs;
  }

  /**
   * @brief Retorna a posição inicial dessa entrada no cluster
   *
   * @return retorna o valor de initPos
   */
  [[nodiscard]] inline DWORD getInitPos() const { return initPos; }

  /**
   * @brief Retorna a posição final dessa entrada no cluster
   *
   * @return retorna o valor de endPos
   */
  [[nodiscard]] inline DWORD getEndPos() const { return endPos; }

  /**
   * @brief Tipo do arquivo
   *
   * Função usada para identificar se o arquivo é do tipo DOT ou um nome longo
   * comum.
   *
   * @return retorna o valor de nameType
   */
  [[nodiscard]] inline int getNameType() const { return type; }

  /**
   * @brief Imprime as informações sobre a entrada
   */
  void printInfo() const;

  /**
   * @brief Marca todas as entradas que fazem parte deste arquivo/diretório como
   * livres
   */
  void markFree();
};

#endif// DENTRY_HPP
