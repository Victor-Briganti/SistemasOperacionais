/*
 * Descrição: Classe para manipulação de matrizes
 *
 * Author: Victor Briganti, Luiz Takeda
 * License: BSD 2
 */

#include <cmath>
#include <cstdio>
#include <iostream>
#include <vector>

class Matrix {
  std::vector<std::vector<int>> matrix;

public:
  Matrix(int numRows, int numColumns) {
    matrix =
        std::vector<std::vector<int>>(numRows, std::vector<int>(numColumns));

    for (int i = 0; i < numRows; i++) {
      for (int j = 0; j < numRows; j++) {
        matrix[i][j] = j;
      }
    }
  }

  void print() {
    for (auto row : matrix) {
      std::cout << "[ ";
      for (auto value : row) {
        std::cout << value << " ";
      }
      std::cout << "]\n";
    }
  }

  std::vector<int> get_row(int line) {
    if (line < 0 || static_cast<int>(matrix.size()) <= line) {
      return {};
    }

    return matrix[line];
  }

  int row_size() {
    if (matrix.empty()) {
      return -1;
    }

    return static_cast<int>(matrix.size()) - 1;
  }

  int column_size() {
    if (matrix.empty()) {
      return -1;
    }

    return static_cast<int>(matrix[0].size()) - 1;
  }
};
