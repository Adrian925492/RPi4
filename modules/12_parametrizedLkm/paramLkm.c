#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>

/* Meta Informations */
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Adrian Podsiadlowski");
MODULE_DESCRIPTION("An example of a parametrized linux kernel odule");

/** Kernel module's parameters */
static unsigned int gpio_nr = 12;           //A default led gpio nr
static char* device_name = "test_device";   //A default device name

module_param(gpio_nr, uint, S_IRUGO);       //Request a param (param value, param type, access right s indicator (here - all access, like 666 in chmod))
module_param(device_name, charp, S_IRUGO);

MODULE_PARM_DESC(gpio_nr, "Nr of gpio for LKM use");                       //A macro for giving description of a param usage
MODULE_PARM_DESC(device_name, "Device name used in parametrized LKM");     

/*
With such param the module can be run with params like:
    sudo insmod <modname> gpio_nr=X device_name=Y
*/

/**
 * @brief This function is called when module is loaded into a kernel
 */
static int __init ModuleInit(void) {
    printk("Hello World, My parametrized module!\n");      //Will print to linux kernel logs!

    printk("gpio_nr=%d", gpio_nr);
    printk("device_name=%s", device_name);

    return 0;
}

/**
 * @brief This function is called when module is unloaded into a kernel
 */
 static void __exit ModuleExit(void) {
    printk("Goodby World, My parametrized module!\n");
 }

module_init(ModuleInit);
module_exit(ModuleExit);
