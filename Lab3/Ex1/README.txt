# Busca em um Vetor

Este programa implementa a busca de um valor em um vetor utilizando múltiplas threads. O objetivo é demonstra a utilização de threads para dividir a carga de trabalho na busca de um elemento específico em um vetor.

## Bibliotecas

As bibliotecas usadas neste programa foram:

- algorithm, biblioteca de C++ que apresenta algumas funções comuns. Desta biblioteca foram utilizados as seguintes funções:
  - find() para busca em um vetor.
  - swap() para realizar troca de valores.
- pthread.h, biblioteca para manipulação de threads POSIX. Foram utilizado as seguintes funções:
  - pthread_create(), usada para criar novas threads. Recebe como parametro o ID da thread, alguns atributos, um ponteiro para a função que será executada e um argumento.
  - pthred_join(), usada para aguardar a conclusão de uma thread. Está função recebe o ID da thread, e um void\*\* que recebe o valor retornado pela thread.

## Compilação

Para compilar está biblioteca o seguinte comando pode ser utilizado:

```sh
make
```

## Execução

Para executar este basta realizar o seguinte:

```sh
./main <num_threads> <vector_size> <num>
```

Onde num_threads é a quantidade de threads que serão criadas, vector_size é o tamanho do vetor e num é o número sendo buscado no vetor.

## Testes

Alguns testes realizados com este programa foram:

```sh
$ ./main 10 50 4
Found
[ 33 36 31 15 43 35 37 42 0 13 12 38 17 14 21 46 10 23 9 19 8 39 24 41 47 30 40 29 18 16 5 27 3 49 44 28 4 45 20 7 25 11 22 34 6 1 48 32 26 2 ]
```

```sh
$ ./main 4 23 -1
Not found!
[ 5 9 22 6 2 1 12 11 18 4 15 21 19 10 7 8 20 17 0 14 13 16 3 ]
```

```sh
$ ./main 1 30 0
Found
[ 15 8 26 25 23 3 17 28 21 14 27 7 20 18 4 6 0 1 22 13 5 9 2 16 10 29 24 19 11 12 ]
```
