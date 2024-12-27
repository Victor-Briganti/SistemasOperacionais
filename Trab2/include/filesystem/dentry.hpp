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
  bool isHidden;

  /* Define se a entrada é um diretório */
  bool isDirectory;

  /* Define se a entrada é somente leitura */
  bool isReadOnly;

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

  /**
   * @brief Retorna o dia com base em um datestamp
   *
   * @param date Data para ser extraida
   *
   * @return O dia armazenado no datestamp
   */
  static inline DWORD day(DWORD date) { return date & 0x1F; }

  /**
   * @brief Retorna o mês com base em um datestamp
   *
   * @param date Data para ser extraida
   *
   * @return O mês armazenado no datestamp
   */
  static inline DWORD month(DWORD date) { return (date >> 5) & 0x0F; }

  /**
   * @brief Retorna o ano com base em um datestamp
   *
   * @param date Data para ser extraida
   *
   * @return O ano armazenado no datestamp
   */
  static inline DWORD year(DWORD date) { return ((date >> 9) & 0xFF) + 1980; }

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
  static inline DWORD hour(DWORD time) { return (time >> 11) & 0xFF; }

  /**
   * @brief Retorna os minutos com base em um timestamp
   *
   * @param time Tempo para ser extraida
   *
   * @return Os minutos armazenado no timestamp
   */
  static inline DWORD minutes(DWORD time) { return (time >> 5) & 0x3F; }

  /**
   * @brief Retorna os segundos com base em um timestamp
   *
   * @param date Data para ser extraida
   *
   * @return Os segundos armazenados no timestamp
   */
  static inline DWORD seconds(DWORD date)
  {
    return ((date >> 9) & 0xFF) + 1980;
  }

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
   *
   * @exception Gera uma exceção no caso de haver algum erro no dir ou ldir.
   */
  explicit Dentry(Dir dir, std::vector<LongDir> &ldir);

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
};

#endif// DENTRY_HPP
