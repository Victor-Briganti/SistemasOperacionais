//===---- birthday.cpp - Arquivo principal do Módulo ----------------------===//
//
// Autores: Hendrick Felipe Scheifer, João Victor Briganti, Luiz Gustavo Takeda
// Data: 15/12/2024
//
//===----------------------------------------------------------------------===//
//
// Módulo do núcleo que tem como objetivo demonstrar o uso de estruturas e
// funções comuns na programação do núcleo.
//
//===----------------------------------------------------------------------===//

#include <linux/list.h> // INIT_LIST_HEAD(), LIST_HEAD(), list_add(), list_del(), list_for_each_entry_safe()
#include <linux/module.h> // MODULE_AUTHOR(), MODULE_DESCRIPTION(), MODULE_LICENSE(), module_exit(), module_init()
#include <linux/slab.h>   // kmalloc(), kfree()
#include <linux/types.h>  // struct list_head

// Estrutura da data de um aniversário
struct birthday {
  int day;
  int month;
  int year;
  struct list_head list;
};

// Inicializa a lista que irá conter as estruturas da data de aniversário
static LIST_HEAD(birthday_list);

/**
 * @brief Inicializa a lista encadeada com as datas de aniversário
 *
 * Percorre a lista encadeada alocando estruturas do tipo birthday, e então
 * inserindo na lista.
 *
 * @return 0 se tudo ocorreu bem, ENOMEM se houve falta de memória
 */
static int birthday_init(void) {
  // Similar a um printf porém usado pelo núcleo
  printk(KERN_DEBUG "Loading Birthday Module\n");

  int day = 1;
  int month = 1;
  int year = 2000;

  for (int i = 0; i < 5; i++) {
    // Aloca a estrutura dentro do espaço de memória do núcleo
    struct birthday *person = kmalloc(sizeof(*person), GFP_KERNEL);

    // Não foi possível alocar uma pessoa
    if (!person) {
      printk(KERN_ERR "Memory allocation failed for person\n");

      // Realiza a interação sobre a lista removendo cada pessoa que foi
      // previamente alocada. A macro list_for_each_entry_safe é usada com o
      // propósito de remover valores da lista sem quebrar a iteração.
      struct birthday *tmp, *person_removed;
      list_for_each_entry_safe(person_removed, tmp, &birthday_list, list) {
        // Remove da lista
        list_del(&person_removed->list);

        // Desaloca a estrutura do espaço de memória do núcleo
        kfree(person_removed);
      }

      return -ENOMEM;
    }

    person->day = day;
    person->month = month;
    person->year = year;
    day++;
    month++;
    year++;

    // Inicializa a estrutura da lista da estrutura criada e então insere na
    // lista.
    INIT_LIST_HEAD(&person->list);
    list_add(&person->list, &birthday_list);
  }

  return 0;
}

/**
 * @brief Finaliza a lista de aniversário
 *
 * Percorre a lista encadead mostrando a data de aniversários e então, desaloca
 * a estrutura do espaço do núcleo.
 */
static void birthday_exit(void) {
  struct birthday *person, *tmp;

  // Itera sobre cada elemento da lista, mostrando a estrutura e removendo o
  // elemento
  list_for_each_entry_safe(person, tmp, &birthday_list, list) {
    printk(KERN_DEBUG "Day: %d, Month: %d, Year: %d\n", person->day,
           person->month, person->year);

    // Remove o elemento da lista
    list_del(&person->list);

    // Desaloca a estrutura do espaço de kernel
    kfree(person);
  }

  printk(KERN_DEBUG "Removing Birthday Module\n");
}

// Macros que registram as funções de entrada e saída do Módulo. São chamadas ao
// montar e desmontar o módulo, respectivamente.
module_init(birthday_init);
module_exit(birthday_exit);

// Macros que definem alguns metadados relacionados com o módulo
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Birthday Module");
MODULE_AUTHOR("João B.");
MODULE_AUTHOR("Takeda");
MODULE_AUTHOR("Hendrick");
