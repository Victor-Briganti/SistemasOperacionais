/*
 * Classe para manipulação de matrizes
 *
 * Autores: Hendrick Felipe Scheifer, João Victor Briganti, Luiz Takeda
 * Licença: BSD 2
 *
 * Data: 30/10/2024
 */
#include <cmath>    // nan(), pow()
#include <iostream> // fopen(), fscanf(), cout
#include <vector>   // vector

class Matrix {
  std::vector<std::vector<int>> matrix;

public:
  /*
   * @brief Construtor do wrapper de matrizes
   *
   * @param filename Nome do arquivo que será lido para inicializar a matriz
   */
  Matrix(const char *filename) {
    FILE *fd;
    int row, column;

    // Abre o arquivo  e verifica o tamanho da matriz
    fd = fopen(filename, "r");
    fscanf(fd, "%dx%d\n", &row, &column);

    matrix = std::vector<std::vector<int>>(row, std::vector<int>(column));

    // Salva os valores do arquivo na matriz
    int value = 0;
    for (int i = 0; i < row; i++) {
      for (int j = 0; j < column; j++) {
        fscanf(fd, "%d", &value);
        matrix[i][j] = value;
      }
    }

    fclose(fd);
  }

  /*
   * @brief Imprime a matriz
   */
  void print() {
    for (auto row : matrix) {
      std::cout << "[ ";
      for (auto value : row) {
        std::cout << value << " ";
      }
      std::cout << "]\n";
    }
  }

  /*
   * @brief Realiza o cálculo da média geométrica na matriz
   *
   * @param column Coluna que será usada para realizar este cálculo
   * @return A média geométrica da coluna especificada
   */
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

  /*
   * @brief Realiza o cálculo da média aritmética na matriz
   *
   * @param row Linha que será usada para realizar este cálculo
   * @return A média aritmética da coluna especificada
   */
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

  /* @brief Retorna o tamanho das colunas */
  int column_size() {
    if (matrix.empty()) {
      return -1;
    }

    return static_cast<int>(matrix[0].size()) - 1;
  }

  /* @brief Retorna o tamanho das linhas */
  int row_size() {
    if (matrix.empty()) {
      return -1;
    }

    return static_cast<int>(matrix.size()) - 1;
  }
};
