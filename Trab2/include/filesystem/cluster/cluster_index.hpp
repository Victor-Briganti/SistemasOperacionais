/**
 * Descrição: Definição da estrutura que indexa o cluster
 *
 * Autores: João Victor Briganti, Luiz Takeda
 * Licença: BSD 2
 *
 * Data: 06/01/2025
 */

#ifndef CLUSTER_INDEX_HPP
#define CLUSTER_INDEX_HPP

#include "utils/types.hpp"

struct ClusterIndex
{
  /* Número do cluster */
  DWORD clusterNum;

  /* Posição inicial no cluster. (Relativa ao tamanho das entradas) */
  DWORD initPos;

  /* Posição final no cluster. (Relativa ao tamanho das entradas) */
  DWORD endPos;
};

#endif// CLUSTER_INDEX_HPP
