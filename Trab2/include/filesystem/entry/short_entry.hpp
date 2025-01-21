/**
 * Descrição: Definição de uma entrada com nome curto
 *
 * Autores: João Victor Briganti, Luiz Takeda
 * Licença: BSD 2
 *
 * Data: 05/01/2025
 */

#ifndef SHORT_ENTRY_HPP
#define SHORT_ENTRY_HPP

#include "filesystem/default.hpp"
#include "utils/types.hpp"

#include <memory>
#include <string>

//===------------------------------------------------------------------------===
// ESTRUTURA
//===------------------------------------------------------------------------===

// Estrutura de diretórios/arquivos
struct __attribute__((packed)) ShortEntry
{
  BYTE name[NAME_FULL_SIZE]; /* Nome curto */
  BYTE attr; /* Atributos que o arquivo pode ter */
  BYTE ntres; /* Reservado */
  BYTE crtTimeTenth; /* Tempo que o arquivo foi criado em ms */
  WORD crtTime; /* Tempo em que o arquivo foi criado */
  WORD crtDate; /* Data em que o arquivo foi criado */
  WORD lstAccDate; /* Última data de acesso */
  WORD fstClusHI; /* Bits mais significativos do cluster */
  WORD wrtTime; /* Tempo em que o arquivo foi escrito */
  WORD wrtDate; /* Data em que o arquivo foi escrito */
  WORD fstClusLO; /* Bits menos significativos do cluster */
  DWORD fileSize; /* Tamanho do arquivo. 0 no caso de diretórios */
};

//===------------------------------------------------------------------------===
// FUNÇÕES
//===------------------------------------------------------------------------===

/**
 * @brief Cria uma estrutura de entrada curta
 *
 * @param name Nome do diretório a ser criado
 * @param fileSize Tamanho do arquivo
 * @param cluster Número do cluster
 *
 * @exception Gera uma exceção caso algo não seja válido
 *
 * @return Um novo diretório
 */
ShortEntry createShortEntry(const std::string &name,
  DWORD fileSize,
  DWORD cluster,
  BYTE attrs);

/**
 * @brief Gera o nome curto a partir do nome longo
 *
 * @param longName Nome longo que será tratado
 *
 * @return O novo nome curto gerado
 */
std::unique_ptr<BYTE[]> generateShortName(const std::string &longName);

/**
 * @brief Gera um nome curto aleatório para evitar problemas
 *
 * @param shortName Ponteiro para nome curto que será alterado
 *
 * @return true se foi possível alterar o nome, false caso contrário
 */
bool randomizeShortname(char *shortName);

/**
 * @brief Dividi o cluster na parte menos significativa
 *
 * @param cluster Cluster que terá o seu valor dividido
 *
 * @return Valor menos significativo do cluster
 */
WORD lowCluster(DWORD cluster);

/**
 * @brief Dividi o cluster na parte mais significativa
 *
 * @param cluster Cluster que terá o seu valor dividido
 *
 * @return Valor mais significativo do cluster
 */
WORD highCluster(DWORD cluster);

#endif// SHORT_ENTRY_HPP
