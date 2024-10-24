/*
 * Wrapper da estrutura vector
 * Descrição: Wrapper da estrutura vector de C++.
 *
 * Autores: Hendrick Felipe Scheifer, João Victor Briganti, Luiz Takeda
 * Licença: BSD 2
 *
 * Data: 22/10/2024
 */
#pragma once

#include <algorithm> // find()
#include <cstdlib>   // rand(), srand()
#include <iostream>  // cout
#include <vector>    // vector

#define SEED 1

class VectorProcess {
public:
  std::vector<int> vector;

  VectorProcess(size_t size) {
    vector.reserve(size);
    this->fill_and_shuffle(size);
  }

  /**
   * @brief Preenche o vetor com valores de 0 até size
   *
   * @param size Define a quantidade de valores que será inserido no vetor.
   */
  void fill(size_t size) {
    for (int i = 0; i < static_cast<int>(size); i++) {
      vector.push_back(i);
    }
  }

  /**
   * @brief Embaralha o vetor
   *
   * Está função deve ser chamada somente depois do vetor ter sido preenchido.
   */
  void shuffle() {
    srand(SEED);
    for (size_t i = 0; i < vector.size(); i++) {
      size_t pos = std::rand() % vector.size();
      std::swap(this->vector[i], this->vector[pos]);
    }
  }

  /**
   * @brief Preenche e embaralha o vetor
   */
  void fill_and_shuffle(size_t size) {
    this->fill(size);
    this->shuffle();
  }

  /**
   * @brief Busca um elemento no vetor
   *
   * Realiza a busca de um elemento no vetor.
   *
   * @param start Posição inicial de busca no vetor
   * @param end Posição final de busca no vetor
   * @param num Elemento que está sendo buscado
   * @return true se encontrado, false caso contrário.
   */
  bool search(size_t start, size_t end, int num) {
    auto startPos = vector.begin() + start;
    auto endPos = vector.begin() + end;

    auto it = std::find(startPos, endPos, num);

    return (it != endPos);
  }

  /**
   * @brief Print do vetor
   *
   * Realiza o print de todos os elementos do vetor. Está função é usada
   * principalmente para debug.
   */
  void print() {
    std::cout << "[ ";
    for (auto a : vector) {
      std::cout << a << " ";
    }
    std::cout << "]\n";
  }
};
