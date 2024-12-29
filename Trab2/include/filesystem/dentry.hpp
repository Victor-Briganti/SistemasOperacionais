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

#include "filesystem/dir.hpp"
#include "utils/types.hpp"

#include <array>
#include <string>
#include <vector>


class Dentry
{
  /* Nome longo da entrada */
  std::string longName;

  /* Nome curto da entrada */
  std::array<char, 11> shortName;

  /* Define se a entrada está escondida */
  bool hidden;

  /* Define se a entrada é um diretório */
  bool directory;

  /* Define se a entrada é somente leitura */
  bool readOnly;

  /* Cluster em que este arquivo/diretório se encontra */
  DWORD cluster;

  /* Tempo de criação do arquivo em ms */
  BYTE crtTimeTenth;

  /* Tempo que o arquivo foi criado */
  WORD crtTime;

  /* Data em que o arquivo foi criado */
  WORD crtDate;

  /* Última data de acesso */
  WORD lstAccDate;

  /* Bits mais significativos do cluster */
  WORD fstClusHI;

  /* Tempo em que o arquivo foi escrito */
  WORD wrtTime;

  /* Data em que o arquivo foi escrito */
  WORD wrtDate;

  /* Tamanho do arquivo. 0 no caso de diretórios */
  DWORD fileSize;

  /* Posição inicial no buffer do diretório */
  DWORD initPos;

  /* Posição final no buffer do diretório */
  DWORD endPos;

  /**
   * @brief Retorna o dia com base em um datestamp
   *
   * @param date Data para ser extraida
   *
   * @return O dia armazenado no datestamp
   */
  static inline WORD day(WORD date) { return date & 0x1F; }

  /**
   * @brief Retorna o mês com base em um datestamp
   *
   * @param date Data para ser extraida
   *
   * @return O mês armazenado no datestamp
   */
  static inline WORD month(WORD date) { return (date >> 5) & 0x0F; }

  /**
   * @brief Retorna o ano com base em um datestamp
   *
   * @param date Data para ser extraida
   *
   * @return O ano armazenado no datestamp
   */
  static inline WORD year(WORD date) { return ((date >> 9) & 0xFF) + 1980; }

  /**
   * @brief Cria um datestamp com base no dia, mês e ano atual
   *
   * @param day Dia atual
   * @param month Mês atual
   * @param year Ano atual
   *
   * @return O datestamp criado
   */
  static inline DWORD dateStamp(DWORD day, DWORD month, DWORD year)
  {
    return day | (month << 5) | ((year - 1980) << 9);
  }

  /**
   * @brief Retorna a hora com base em um timestamp
   *
   * @param time Tempo para ser extraida
   *
   * @return A hora armazenada no datestamp
   */
  static inline WORD hour(WORD time) { return (time >> 11) & 0xFF; }

  /**
   * @brief Retorna os minutos com base em um timestamp
   *
   * @param time Tempo para ser extraida
   *
   * @return Os minutos armazenado no timestamp
   */
  static inline WORD minutes(WORD time) { return (time >> 5) & 0x3F; }

  /**
   * @brief Retorna os segundos com base em um timestamp
   *
   * @param time Data para ser extraida
   *
   * @return Os segundos armazenados no timestamp
   */
  static inline WORD seconds(WORD time) { return ((time >> 9) & 0xFF) + 1980; }

  /**
   * @brief Cria um timestamp com base no hora, minutos e segundos
   *
   * @param hour Hora
   * @param minutes Minutos
   * @param seconds Segundos
   *
   * @return O timestamp criado
   */
  static inline DWORD timeStamp(DWORD hour, DWORD minutes, DWORD seconds)
  {
    return (hour << 11) | (minutes << 5) | (seconds / 2);
  }

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
  explicit Dentry(Dir dir,
    std::vector<LongDir> &ldir,
    DWORD initPos,
    DWORD endPos);

  /**
   * @brief Nome longo da entrada
   *
   * @return retorna o nome longo da entrada
   */
  [[nodiscard]] inline std::string getLongName() const { return longName; }

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
  [[nodiscard]] inline DWORD getCluster() const { return cluster; }

  /**
   * @brief Tamanho do arquivo
   *
   * @return retorna o tamanho do arquivo
   */
  [[nodiscard]] inline DWORD getFileSize() const { return fileSize; }

  /**
   * @brief Retorna verdadeiro se é um diretório
   *
   * @return retorna o valor de directory
   */
  [[nodiscard]] inline bool isDirectory() const { return directory; }

  /**
   * @brief Retorna verdadeiro se é uma entrada escondida
   *
   * @return retorna o valor de hidden
   */
  [[nodiscard]] inline bool isHidden() const { return hidden; }

  /**
   * @brief Retorna verdadeiro se é somente leitura
   *
   * @return retorna o valor de readOnly
   */
  [[nodiscard]] inline bool isReadOnly() const { return readOnly; }

  /**
   * @brief Retorna o dia de criação do arquivo
   *
   * @return retorna o dia do datestamp crtDate
   */
  [[nodiscard]] inline WORD getCreationDay() const { return day(crtDate); }

  /**
   * @brief Retorna o mês de criação do arquivo
   *
   * @return retorna o mês do datestamp crtDate
   */
  [[nodiscard]] inline WORD getCreationMonth() const { return month(crtDate); }

  /**
   * @brief Retorna o ano de criação do arquivo
   *
   * @return retorna o ano do datestamp crtDate
   */
  [[nodiscard]] inline WORD getCreationYear() const { return year(crtDate); }

  /**
   * @brief Retorna a hora de criação do arquivo
   *
   * @return retorna a hora do timestamp crtTime
   */
  [[nodiscard]] inline WORD getCreationHour() const { return hour(crtTime); }

  /**
   * @brief Retorna o mês de criação do arquivo
   *
   * @return retorna o mês do timestamp crtTime
   */
  [[nodiscard]] inline WORD getCreationMinute() const
  {
    return minutes(crtTime);
  }

  /**
   * @brief Retorna o ano de criação do arquivo
   *
   * @return retorna o ano do timestamp crtTime
   */
  [[nodiscard]] inline WORD getCreationSeconds() const
  {
    return seconds(crtTime);
  }

  /**
   * @brief Retorna o dia de criação do arquivo
   *
   * @return retorna o dia do datestamp wrtDate
   */
  [[nodiscard]] inline WORD getWriteDay() const { return day(wrtDate); }

  /**
   * @brief Retorna o mês de criação do arquivo
   *
   * @return retorna o mês do datestamp wrtDate
   */
  [[nodiscard]] inline WORD getWriteMonth() const { return month(wrtDate); }

  /**
   * @brief Retorna o ano de criação do arquivo
   *
   * @return retorna o ano do datestamp wrtDate
   */
  [[nodiscard]] inline WORD getWriteYear() const { return year(wrtDate); }

  /**
   * @brief Retorna a hora de criação do arquivo
   *
   * @return retorna a hora do timestamp wrtTime
   */
  [[nodiscard]] inline WORD getWriteHour() const { return hour(wrtTime); }

  /**
   * @brief Retorna o mês de criação do arquivo
   *
   * @return retorna o mês do timestamp wrtTime
   */
  [[nodiscard]] inline WORD getWriteMinute() const { return minutes(wrtTime); }

  /**
   * @brief Retorna o ano de criação do arquivo
   *
   * @return retorna o ano do timestamp wrtTime
   */
  [[nodiscard]] inline WORD getWriteSeconds() const { return seconds(wrtTime); }

  /**
   * @brief Imprime as informações sobre a entrada
   */
  void printInfo() const;
};

#endif// DENTRY_HPP
