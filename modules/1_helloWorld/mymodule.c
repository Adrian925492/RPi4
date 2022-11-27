#include <linux/module.h>
#include <linux/init.h>

/* Meta Informations */
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Adrian Podsiadlowski");
MODULE_DESCRIPTION("An example, hello world linux kernel module");

/**
 * @brief This function is called when module is loaded into a kernel
 */
static int __init ModuleInit(void) {
    printk("Hello World, My Mudule!\n");      //Will print to linux kernel logs!
    return 0;
}

/**
 * @brief This function is called when module is unloaded into a kernel
 */
 static void __exit ModuleExit(void) {
     printk("Goodby World, My Module!\n");
 }

module_init(ModuleInit);
module_exit(ModuleExit);
