#include <linux/module.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/string.h>

/* Meta Informations */
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Adrian Podsiadlowski");
MODULE_DESCRIPTION("An example of malloc usage in linux kernel module");

struct driver_data {
    u8 version;
    char text[64];
};

u32* ptr1;
struct driver_data *ptr2;

/**
 * @brief This function is called when module is loaded into a kernel
 */
static int __init ModuleInit(void) {
    ptr1 = kmalloc(sizeof(u32), GFP_KERNEL);        //Allocate memory and set flags
    if (ptr1 == NULL) {
        printk("alloc test - out of memory\n");
        return -1;
    }

    printk(" alloc test - ptr1: 0x%x\n", *ptr1);
    *ptr1 = 0x001c0de;
    printk(" alloc test - ptr1: 0x%x\n", *ptr1);

    kfree(ptr1);    //Free the memory


    ptr1 = kzalloc(sizeof(u32), GFP_KERNEL);        //Allocate memory and set flags and set it initially to zeros
    if (ptr1 == NULL) {
        printk("alloc test - out of memory\n");
        return -1;
    }

    printk(" alloc test - ptr1: 0x%x\n", *ptr1);
    *ptr1 = 0x001c0de;
    printk(" alloc test - ptr1: 0x%x\n", *ptr1);

    kfree(ptr1);    //Free the memory

    ptr2 = kzalloc(sizeof(struct driver_data), GFP_KERNEL);
    if (ptr2 == NULL) {
        printk("alloc test - out of memory ptr2\n");
        return -1;
    }

    ptr2->version = 123;
    strcpy(ptr2->text, "This is a test string!");

    printk("alloc test - ptr2->version is %d, ptr2->data is %s", ptr2->version, ptr2->text);


    printk("Hello World, My Mudule!\n");      //Will print to linux kernel logs!
    return 0;
}

/**
 * @brief This function is called when module is unloaded into a kernel
 */
 static void __exit ModuleExit(void) {
     printk("Goodby World, My Module!\n");

     kfree(ptr2);
 }

module_init(ModuleInit);
module_exit(ModuleExit);
