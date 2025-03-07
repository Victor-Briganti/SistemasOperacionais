/*
 * Descrição: Definição da estrutura FSInfo
 *
 * Autores: João Victor Briganti, Luiz Gustavo Takeda, Matheus Floriano Saito
 * Licença: BSD 2
 *
 * Data: 31/12/2024
 */

#ifndef FSINFO_HPP
#define FSINFO_HPP

#include "filesystem/structure/fat_table.hpp"
#include "io/image.hpp"
#include "utils/types.hpp"

// Classe para lidar com a estrutura FSInfo do FAT filesystem
class FileSysInfo
{
  /* Definição de valor quando não se sabe a quantidade de clusters livres */
  static constexpr DWORD UNKNOWN_FREECLUS = 0xFFFFFFFF;

  /* Valor da assinatura do LeadSig */
  static constexpr DWORD LEADISG = 0x41615252;

  /* Valor da assinatura do StrucSig */
  static constexpr DWORD STRUCSIG = 0x61417272;

  /* Valor da assinatura do TrailSig*/
  static constexpr DWORD TRAILSIG = 0xAA550000;

  struct __attribute__((packed)) FSInfo
  {
    DWORD LeadSig; /* Assinatura para validar o FSInfo */
    BYTE Reserved1[480]; /* Estrutura que não deve ser usada */
    DWORD StrucSig; /* Outra assinatura para validar o FSInfo */
    DWORD FreeCount; /* Contém a quantidade de clusters livre do volume */
    DWORD NextFree; /* Próximo cluster livre */
    BYTE Reserved2[12]; /* Estrutura que não deve ser usada */
    DWORD TrailSig; /* Mais uma assinatura para validar a estrutura */
  };

  /* Estrutura do FSInfo */
  FSInfo fsinfo;

  /* Interface usada para ler e escrever a imagem */
  std::shared_ptr<Image> image;

  /* Offset da estrutra no sistema de arquivos */
  const DWORD offset;

  /* Flag para definir se a estrutura foi ou não alterada */
  bool dirty = false;

  /* Total de clusters no sistema */
  DWORD totalClus;

public:
  /**
   * @brief Inicia a estrutura FSInfo
   *
   * Lê a estrutura FSInfo em memória e inicializa informações necessárias para
   * o funcionamento dos sistema de arquivos.
   *
   * @param image Interface para fazer a leitura do sistema.
   * @param offset Offset do FSInfo no sistema de arquivos.
   * @param fatTable Tabela FAT usada para realizar algumas verificações.
   *
   * @exception Gera uma exceção se não for possível ler a estrutura FSInfo ou
   * houve algum erro durante sua verificação.
   */
  explicit FileSysInfo(std::shared_ptr<Image> image,
    DWORD offset,
    const std::shared_ptr<FatTable> &fatTable);

  /**
   * @brief Finaliza a estrutura
   *
   * Verifica se a flag dirty está setada, e se estiver escreve o FSInfo na
   * imagem original.
   */
  ~FileSysInfo();

  /**
   * @brief Quantidade de clusters livres no sistema
   *
   * @return Retorna o valor de fsinfo.FreeCount
   */
  [[nodiscard]] inline DWORD getFreeCount() const { return fsinfo.FreeCount; }

  /**
   * @brief Último cluste livre conhecido
   *
   * @return Retorna o valor de fsinfo.NextFree
   */
  [[nodiscard]] inline DWORD getNextFree() const { return fsinfo.NextFree; }

  /**
   * @brief Coloca um novo valor na quantidade de clusters livres
   *
   * @return true se foi possível alterar, false caso contrário
   */
  bool setFreeCount(DWORD freeCount);

  /**
   * @brief Coloca um novo valor no último cluster livre conhecido
   *
   * @return true se foi possível alterar, false caso contrário
   */
  bool setNextFree(DWORD nextFree);
};

#endif// FSINFO_HPP
