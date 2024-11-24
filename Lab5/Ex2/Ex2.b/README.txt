# Sinais

Program que realiza a leitura de um arquivo e cópia seus conteúdos para outro arquivo. Ao receber um sinal de interrupção, realiza o seu tratamento e finaliza a operação de maneira a não corromper o sistema.

## Compilação

Para compilar este programa o seguinte comando pode ser utilizado:

```sh
make
```

## Execução

Para executar este basta realizar o seguinte:

```sh
./main <input> <output>
```
input -> Nome do arquivo de entrada
output -> Nome do arquivo de saída

## Bibliotecas

  - <csignal>:
    Biblioteca é o wrapper da sinal.h para C++, usda para capturar e manipular sinais do sistema: que tras as estruturas e funções para tratamento de sinais. No código foi utilizado o sig_atomic_t um tipo usado para flags globais no tratamento de sinais, struct sigaction estrutura que armazena informações sobre a ação a ser tomada durante o tratamento do sinal e por fim sigaction() que é a função que registra o tratador do sinal.
    sig_atomic_t, tipo comumente usado em flags durante o tratamento de sinais.
    struct sigaction, estrutra que armazena informações sobre a ação a ser tomada durante o tratamento do sinal
    sigaction(), função registra o tratador do sinal.
    SIGINT, macro que define um sinal de interrupção, comumente usado quando o usuário pressiona Ctrl+C.
    SIGTERM, macro que define um sinal de interrupção, comumente enviado por outros processos para solicitar a terminação do programa.

  - <fstream>, desta biblioteca foram utilizados as classes ifstream e ofstream usadas para arquivos de entrada e saída respectivamente
    Biblioteca padrão C++ para manipulação de arquivos.
    ifstream, classe usada para abrir um arquivo de entrada
    ofstream, classe usada para abrir um arquivo de saída

  - <iostream>, desta biblioteca foram utilizados as classes cout e cerr para imprimir informações na tela.
    Biblioteca padrão C++ para entrada e saída.
    cout e cerr, classes usadas para imprimir mensagens no console (saída padrão e saída de erros)
    cin, classe padrão para ler entradas de usuário.

  - <string>, 
    Biblioteca padrão de C++ para manipulação de strings.
    string, classe padrão string de C++

  - <unistd.h>:
    Biblioteca de chamada de sistema para sistemas do tipo UNIX: 
    sleep(), usado para colocar o processo em um estado bloqueado por x segundos
    getpid(), usado para retornar o PID do processo atual