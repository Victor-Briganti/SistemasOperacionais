/*
 * Descrição: Classe para manipulação de matrizes
 *
 * Author: Hendrick Felipe Scheifer, Victor Briganti, Luiz Takeda
 * License: BSD 2
 */
#include <iostream> // cout
#include <vector>   // vector

class Matrix {
  std::vector<std::vector<int>> matrix;

public:
  /**
   * Inicializa a classe matriz.
   *
   * @param numRows Número de linhas na matriz
   * @param numColumns Número de colunas na matriz
   */
  Matrix(int numRows, int numColumns) {
    // Inicializa a matriz
    matrix =
        std::vector<std::vector<int>>(numRows, std::vector<int>(numColumns));

    // Colocar um valor em cada posição da matriz
    for (int i = 0; i < numRows; i++) {
      for (int j = 0; j < numRows; j++) {
        matrix[i][j] = j;
      }
    }
  }

  /** Imprime a matriz */
  void print() {
    for (auto row : matrix) {
      std::cout << "[ ";
      for (auto value : row) {
        std::cout << value << " ";
      }
      std::cout << "]\n";
    }
  }

  /**
   * Retorna uma linha da matriz
   *
   * @param line Linha a ser retornada
   * @return Retorna um vetor que simboliza a linha da matriz
   */
  std::vector<int> get_row(int line) {
    if (line < 0 || static_cast<int>(matrix.size()) <= line) {
      return {};
    }

    return matrix[line];
  }

  /**
   * Retorna a quantidade de linhas na matriz
   *
   * @return Retorna o tamanho das linhas na matriz
   */
  int row_size() {
    if (matrix.empty()) {
      return -1;
    }

    return static_cast<int>(matrix.size()) - 1;
  }

  /**
   * Retorna a quantidade de colunas na matriz
   *
   * @return Retorna o tamanho das colunas na matriz
   */
  int column_size() {
    if (matrix.empty()) {
      return -1;
    }

    return static_cast<int>(matrix[0].size()) - 1;
  }
};
