# Sockets

Este programa tem como objetivo realizar a manipulação de sockets.

## Bibliotecas

As bibliotecas utilizadas foram:

- fstream, desta biblioteca foi utilizado a classe ifstream para a leitura dos dicionários
- iostream, desta biblioteca foi utilizado a classe cout e cerr para imprimir informações na tela e a cin para obter os valores passados pelo usuário
- map, desta biblioteca foi utilizado a classe map 
- string, biblioteca padrão para manipulação de strings em C++
- sys/socket.h, desta biblioteca foram utilizados os seguintes: 
    - struct sockaddr, estrutura que define o socket no sistema
    - AF_UNIX, macro usada para criar um socket do tipo UNIX
    - SOCK_STREAM, macro usada para definir o como o envio das mensagens irá ocorrer. Neste caso o envio das mensagens é sequêncial e confiável.
    - socket(), a função que cria o socket em si, retorna um descritor que referencia o socket criado.
    - bind(), atribui um endereço ao socket criado
    - listen(), deixa o socket criado em modo passivo, isso significa que ele só aceita receber conexões
    - accept(), retira a primeira conexão da fila de conexões, cria um novo socket para comunicação e retorna o descritor deste socket criado.
    - connect(), conecta o socket em um endereço especificado.
- sys/un.h, desta biblioteca foi utilizado a estrutura sockaddr_un que define o um socket do tipo UNIX.
- unistd.h, desta biblioteca foram utilizados as seguintes funções:
    - close(), usada para fechar um descritor no caso o socket
    - read(), usada para ler as informações passadas de um socket a outro
    - write(), usada para escrever informações de um socket a outro
    - unlink(), usado para remover um arquivo do sistema, neste caso o arquivo usado pelo socket para comunicação
- vector, biblioteca padrão do C++ que introduz a tipo vector.

## Compilação

Para compilar este programa o seguinte comando pode ser utilizado:

```sh
make
```

## Execução

Para executar este programa é necessário realizar a execução de cada processo na seguinte ordem:

```sh
./server
./client
```