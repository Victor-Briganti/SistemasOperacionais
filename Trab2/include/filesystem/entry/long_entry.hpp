/**
 * Descrição: Definição da entrada de nome longo
 *
 * Autores: João Victor Briganti, Luiz Gustavo Takeda, Matheus Floriano Saito
 * Licença: BSD 2
 *
 * Data: 26/12/2024
 */

#ifndef LONG_ENTRY_HPP
#define LONG_ENTRY_HPP

#include "filesystem/entry/short_entry.hpp"
#include "utils/types.hpp"

#include <string>
#include <vector>

//===------------------------------------------------------------------------===
// ESTRUTURAS
//===------------------------------------------------------------------------===

// Estrutura de diretórios/arquivos para nomes longos
struct __attribute__((packed)) LongEntry
{
  BYTE ord; /* A ordem dessa entrada na sequência de nomes longos */
  BYTE name1[10]; /* Caracteres de 1-5 do nome longo */
  BYTE attr; /* Deve ser igual a ATTR_LONG_NAME */
  BYTE type; /* 0 é um diretório, outros valores rementem a arquivos. */
  BYTE chckSum; /* Checksum usado para verificar esta entrada */
  BYTE name2[12]; /* Caracteres 6-11 do nome longo */
  WORD fstClusLO; /* Deve ser 0. Está aqui só por compatibilidade */
  BYTE name3[4]; /* Caracteres 12-13 do nome longo */
};

//===------------------------------------------------------------------------===
// FUNÇÕES
//===------------------------------------------------------------------------===

/**
 * @brief Cria uma lista com as estruturas de nomes longos
 *
 * @param dir Estrutura de nome curto, relacionada estas entradas
 * @param name Nome do arquivo
 *
 * @exception Gera uma exceção caso algo não seja válido
 *
 * @return Uma lista com os nomes longos
 */
std::vector<LongEntry> createLongEntries(const ShortEntry &entry,
  const std::string &name);

/**
 * @brief Gera um checksum com base em um nome curto
 *
 * @param shortName Nome curto no qual o checksum será calculado
 *
 * @return O valor do checksum já calculado
 */
BYTE shortCheckSum(const char *shortName);

#endif// LONG_ENTRY_HPP
