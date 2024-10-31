# Calculo de Matrizes

Este programa tem como objetivo realizar uma execução paralela com threads do cálculo da média geométrica e aritmética de uma matriz.

## Bibliotecas

As bibliotecas usadas neste programa foram:

- cmath, biblioteca usada para cálculos matemáticos. Deste biblioteca foram utilizados os seguinte:
  - nan() retorna que o o cálculo realizado foi um Not a Number(não é um número válido), como é o caso de uma divisão por zero.
  - pow() usada para calcular a potência de um número. math.h, usada para realizar o cálculo da quantidade de processos que podem ser criados no sistema.
- pthread.h, biblioteca para manipulação de threads posix. foram utilizado as seguintes funções:
  - pthread_create(), usada para criar novas threads. recebe como parametro o id da thread, alguns atributos, um ponteiro para a função que será executada e um argumento.
  - pthred_join(), usada para aguardar a conclusão de uma thread. está função recebe o id da thread, e um void\*\* que recebe o valor retornado pela thread.

## Compilação

Para compilar está biblioteca o seguinte comando pode ser utilizado:

```sh
make
```

## Execução

Para executar este basta realizar o seguinte:

```sh
./main <input_file> <output_file> <num_threads>
```

Onde input_file é o arquivo de entrada(neste caso é a matriz), output_file é o arquivo de saída e por fim num_threads é a quantidade de threads que será criada para realizar o cálculo.

## Testes

Alguns testes realizados com este programa foram:

```sh
$ ./main data_matriz_100_200.in out 1
$ ./main data_matriz_100_200.in out 2
$ ./main data_matriz_100_200.in out 4
$ ./main data_matriz_100_200.in out 8
$ ./main data_matriz_100_200.in out 16
```
