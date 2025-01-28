# Sistema de Arquivos FAT32

## Compilar

Para compilar basta usar o comando

```sh
make
```

## Executando

Após a compilação para executar o programa basta realizar o seguinte:

```sh
./main fat32.img
```

Os comandos aceitos são:

- info
  Mostra informações sobre o FAT

- ls [caminho]
  Lista os arquivos e diretórios do caminho atual ou do caminho passado por parâmetro.

- cluster <número>
  Lê as informações do cluster especificado.

- cp <origem> <destino>
  Cópia um arquivo de um local a outro. Este comando pode ser usado para interagir com o sistema de arquivos do host executando o programa.

- attr <arquivo|diretório>
  Mostra informações sobre o arquivo ou diretório especificado.

- touch <arquivo>
  Cria um arquivo vazio com o nome especificado.

- mkdir <diretório>
  Cria um diretório com o nome especificado.

- cd <caminho>
  Altera o diretório atual para o especificado via parâmetro.

- rename <origem> <destino>
  Altera o nome arquivo de origem para o especificado no destino.

- mv <origem> <destino>
  Move um arquivo de origem para o local de destino. Assim como o cp pode ser usado para interagir com o sistema de arquivos do host executando o programa.

- rm <arquivo>
  Exclui o arquivo do sistema.

- rmdir <diretório>
  Exclui um diretório vazio do sistema.

- pwd
  Mostra o caminho completo atual.

## Estrutura de Arquivos

A estrutura do código de implementação, segue a seguinte hierarquia:

Arquivos .h
```bash
include/
├── filesystem
│   ├── cluster
│   │   ├── cluster.hpp
│   │   └── cluster_index.hpp
│   ├── default.hpp
│   ├── entry
│   │   ├── dentry.hpp
│   │   ├── long_entry.hpp
│   │   └── short_entry.hpp
│   ├── fatfs.hpp
│   └── structure
│       ├── bpb.hpp
│       ├── fat_table.hpp
│       └── fsinfo.hpp
├── io
│   ├── fs_adapter.hpp
│   └── image.hpp
├── path
│   └── pathname.hpp
├── shell
│   └── shell.hpp
└── utils
    ├── logger.hpp
    ├── time.hpp
    └── types.hpp
```

Arquivos .cpp
``` bash
src/
├── filesystem
│   ├── cluster
│   │   └── cluster.cpp
│   ├── entry
│   │   ├── dentry.cpp
│   │   ├── long_entry.cpp
│   │   └── short_entry.cpp
│   ├── fatfs.cpp
│   └── structure
│       ├── bpb.cpp
│       ├── fat_table.cpp
│       └── fsinfo.cpp
├── io
│   ├── fs_adapter.cpp
│   └── image.cpp
├── main.cpp
├── path
│   └── pathname.cpp
├── shell
│   └── shell.cpp
└── utils
    ├── logger.cpp
    └── time.cpp
```

O diretório `filesystem` contém as implementações relacionadas ao FAT32 em si, dentro dele há ainda o diretório `cluster` usado para a leitura dos dados em blocos, `entry` que contém as classes relacionadas as entradas tanto a longa quanto a curta, o diretório `structure` que mantém as estruturas principais que gerenciam o sistema (BPB, FSInfo e a própria FAT table) e por fim temos o arquivo `fatfs.cpp` que implementa a classe que gerencia todas essas outras estruturas.
No diretório `io` está implementado as classes que lidam com a leitura e escrita no baixo nível, são usados para leitura e escrita tanto na imagem quanto em arquivos do sistema de arquivos do host.
No `path` está implementado a classe responsável por lidar com os caminhos do sistema de arquivos.
`shell` é onde se encontra a classe que implementa o shell interativo que o usuário utiliza para acessar o sistema de arquivos em si.
`utils` são utilitários do sistema que são usados por quase todas as classes, aqui temos o `logger.cpp` usado para depuração do sistema e o `timer.cpp` para lidar com questoes de data e hora.

## Bibliotecas Utilizadas

#include <algorithm> 
  Biblioteca que possui algoritmos comumente usados. A função std::reverse é utilizada para inverter a ordem das entradas de nome longo

#include <stdexcept> 
  Inclui classes de exceção padrãopara C++, usado com o std::runtime_error para lançar uma exceção quando algum erro acontece.

#include <string> 
  A biblioteca oferece a classe std::string para manipulação de strings

#include <vector>
  A biblioteca define a classe std::vector, estrutura similar a um array que tem tamanho variado.

#include <cctype>
  Biblioteca C para uso em códigos C++ usada para verificação de tipos, usada para verificar os caracteres dos nomes.

#include <cstring>
 Biblioteca C para uso em códigos C++  que fornece funções para manipulação de strings e gerenciamento de memória, com funções como `memcpy` e `memset`.

#include <array>
  A biblioteca  que fornece uma estrutura de dados que encapsula um array fixo de tamanho definido em tempo de compilação.

#include <cstdio>
  Biblioteca para operações de entrada e saída, como std::fprintf.

#include <climits>
  Biblioteca que define limites nos tipos comuns ao sistema.

#include <cstdint>
  Biblioteca que define tipos de números que são padrões a todas as arquiteturas.

#include <cstdlib>
  Biblioteca padrão de C para uso em códigos C++, possui funções como conversão de strings para números e geração de números aleatórios.

#include <ctime>
  Biblioteca com definições para manipulação de data e hora.

#include <filesystem>
  Biblioteca que tras operações para facilitar operações no sistema de arquivos do sistema operacional do host.

#include <fstream>
   Biblioteca para operações de entrada e saída com arquivos

#include <iostream>
  Biblioteca para operações de entrada e saída.

#include <memory>
  Define utilitários para manipulação de memória, com ponteiros inteligentes, como é o caso do unique_ptr e shared_ptr.

#include <sstream>
  Biblioteca para manipular uma string assim como um arquivo. Similar ao fstring em C.


## Autores

  João Victor Briganti
  Luiz Gustavo Takeda
  Matheus Floriano Saito
