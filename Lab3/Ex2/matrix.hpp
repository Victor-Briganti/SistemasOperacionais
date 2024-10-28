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
  Matrix(const char *filename) {
    FILE *fd;
    int row, column;

    fd = fopen(filename, "r");
    fscanf(fd, "%dx%d\n", &row, &column);

    matrix = std::vector<std::vector<int>>(row, std::vector<int>(column));

    int value = 0;
    for (int i = 0; i < row; i++) {
      for (int j = 0; j < column; j++) {
        fscanf(fd, "%d", &value);
        matrix[i][j] = value;
      }
    }

    fclose(fd);
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

  double geometric_mean(int column) {
    double acc = 1;

    for (auto vec : matrix) {
      if (vec[column] <= 0) {
        return std::nan("");
      }
      acc *= vec[column];
    }

    double e = static_cast<double>(1) / static_cast<double>(matrix.size());
    return pow(acc, e);
  }

  double arithmetic_mean(int row) {
    double acc = 0;

    if (matrix.empty() || matrix[row].empty()) {
      return std::nan("");
    }

    for (auto value : matrix[row]) {
      acc += value;
    }

    return acc / static_cast<double>(matrix[row].size());
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
