# Módulo do Núcleo

## Compilação

A compilação pode ser feita seguindo o seguinte comando:

```sh
make
```

Em caso de erros relacionados a assinatura do módulo, se faz necessário compilar
da seguinte maneira:

```sh
make CONFIG_MODULE_SIG=n
```

## Execução

Para carregar o módulo se faz necessário o uso do comando `insmod`, que serve
para inserir um módulo, que neste caso é o binário gerado ao fim da compilação 
com a extensão `.ko`. O comando completo para isso é o seguinte:

```sh
sudo insmod birthday.ko
```

Para descarregar o módulo o comando `rmmod` é utilizado, assim como com o 
`insmod` se faz necessário é necessário saber o nome do módulo que se deseja 
remover. O comando completor para isso é o seguinte:

```sh
sudo rmmod birthday.ko
```

A saídas desses módulos não ocorre nas saídas padrões, logo para obter a saída
se faz necessário o uso da ferramenta `dmesg` que mostra as saídas do núcleo.
Com o comando abaixo será possível determinar se o módulo está sendo carregado
corretamente:

```sh
dmesg
```

## Bibliotecas
- <linux/list.h> Estrutura de lista duplamente encadeada genérica 
    
    - INIT_LIST_HEAD(): Inicializa uma lista vazia
    
    - LIST_HEAD(): Declara e inicializa uma lista
    
    - list_add(): Adiciona um elemento no início da lista
    
    - list_del(): Remove um elemento da lista
    
    - list_for_each_entry_safe(): Itera de forma segura sobre uma lista, mesmo
      removendo elementos durante a iteração.

- <linux/module.h> Define funções de carregamento do módulo

    - MODULE_AUTHOR(): Define o nome do autor do módulo.

    - MODULE_DESCRIPTION(): Descreve o que o módulo faz.
    
    - MODULE_LICENSE(): Define a licença do módulo.
    
    - module_exit(): Define a função que será executada ao remover o módulo.
    
    - module_init(): define a função de inicialização ao carregar o módulo.

- <linux/slab.h> 
    
    - kmalloc(): Realiza uma alocação dentro do espaço de memória do núcleo.
    
    - kfree(): Desaloca um espaço de memória alocado no núcleo.

- <linux/types.h>  

    - struct list_head: Estrutura de lista encadeada.
