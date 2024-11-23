# Sinais

Este programa tem como objetivo realizar o tratamento de sinais.

## Bibliotecas

As bibliotecas utilizadas foram:

- csignal, está biblioteca é o wrapper da sinal.h para C++, que tras as estruturas e funções para tratamento de sinais. No código foi utilizado o sig_atomic_t um tipo usado para flags globais no tratamento de sinais, struct sigaction estrutura que armazena informações sobre a ação a ser tomada durante o tratamento do sinal e por fim sigaction() que é a função que registra o tratador do sinal.
- fstream, desta biblioteca foram utilizados as classes ifstream e ofstream usadas para arquivos de entrada e saída respectivamente
- iostream, desta biblioteca foram utilizados as classes cout e cerr para imprimir informações na tela.
- string, biblioteca padrão de C++ para tratamento de strings
- unistd, desta biblioteca foram utilizados a função sleep() para colocar o processo em um estado de espera após a escrita e o getpid() para obter o PID do processo.

## Compilação

Para compilar este programa o seguinte comando pode ser utilizado:

```sh
make
```

## Execução

Para executar este basta realizar o seguinte:

```sh
./main input.txt output.txt
```