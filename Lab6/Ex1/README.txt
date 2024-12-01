# Matrix

Este programa tem como objetivo realizar a soma das linhas de uma matriz.
 
Descrição: 
Processa uma matriz realizando a soma de todos seus elementos.
Cada thread lê uma linha realiza a soma e armazena o resultado em uma váriavel global

## Compilação

Para compilar o seguinte comando pode ser utilizado:

```sh
make
```

## Execução

Para executar este basta realizar o seguinte:

```sh
./main
```

## Bibliotecas

As bibliotecas utilizadas foram:

  - <pthread.h>: 
    Biblioteca utilizada para utilizar e manipular threads e mutex.
    Funções utilizadas:
      pthread_create(); // Cria uma nova thread de execução, permitindo a execução concorrente de funções em diferentes threads.
      pthread_join(); // Aguarda a conclusão de uma thread específica, garantindo que a thread chamadora espere o término da execução da thread especificada.
      pthread_mutex_init(); // Inicializa um mutex, permitindo controle de acesso a recursos compartilhados em threads. 
      pthread_mutex_lock(); // Bloqueia um mutex para que somente uma thread possa acessar o recurso protegido por ele.
      pthread_mutex_unlock(); // Desbloqueia um mutex, permitindo que outras threads acessem o recurso protegido.

  - <iostream>: 
    Biblioteca padrão do c++.
     std::cout // Saída de texto 
     std::endl // Utilizada para mandar um endline para saída

  - <vector>: 
    Biblioteca padrão para utilização de vetores em c++.
     std::vector // Classe para manipular vetor