#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/list.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/types.h>

struct birthday {
  int day;
  int month;
  int year;
  struct list_head list;
};

static LIST_HEAD(birthday_list);

static int birthday_init(void) {
  printk(KERN_DEBUG "Loading Module\n");

  int day = 1;
  int month = 1;
  int year = 2000;

  for (int i = 0; i < 5; i++) {
    struct birthday *person = kmalloc(sizeof(*person), GFP_KERNEL);
    if (!person) {
      printk(KERN_ERR "Memory allocation failed for person\n");

      struct birthday *tmp, *person_removed;
      list_for_each_entry_safe(person_removed, tmp, &birthday_list, list) {
        list_del(&person_removed->list);
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

    INIT_LIST_HEAD(&person->list);
    list_add(&person->list, &birthday_list);
  }

  return 0;
}

static void birthday_exit(void) {
  struct birthday *person, *tmp;

  list_for_each_entry_safe(person, tmp, &birthday_list, list) {
    printk(KERN_DEBUG "Day: %d, Month: %d, Year: %d\n", person->day,
           person->month, person->year);
    list_del(&person->list);
    kfree(person);
  }

  printk(KERN_DEBUG "Removing Module\n");
}

/* Macros for registering module entry and exit points */
module_init(birthday_init);
module_exit(birthday_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Birthday Module");
MODULE_AUTHOR("SGG");