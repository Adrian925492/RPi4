#include <linux/module.h>
#include <linux/init.h>
#include <linux/string.h>
#include <linux/kobject.h>      //Kernel objects include - the kobjects stuffs are resp for creating entries in sysfs

/* Meta Informations */
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Adrian Podsiadlowski");
MODULE_DESCRIPTION("Linux sysfs usage exmple");

/*
Sysfs is a filesystem that allows to control hardware and buses from a linux perspective. It is stored in /sys directory, and provides an interface to control
hardware and get information about hardware. For example, we can use sys fs to control gpio by /sys/class/gpio/ ...

It also provides valuable debug inforamtion about the hrdware, includeing buses it is connected etc. in /sys/kernel/debug directory.
*/


/**
 * @brief Read callback for sysfs file
 */
static ssize_t driver_read(struct kobject* kobj, struct kobj_attribute *attr, char* buffer) {   //kobj - a folder from which it is called, and attr - represents a file from which it is called
    return sprintf(buffer, "YOu have read from /sts/kernel/%s/%s\n", kobj->name, attr->attr.name);
}

/*
 * @brief Write callback for sysfs file
 */
static ssize_t driver_write(struct kobject* kobj, struct kobj_attribute *attr, const char* buffer, size_t count) {   //kobj - a folder from which it is called, and attr - represents a file from which it is called, count - nr ob bytes to write to kernel module
    printk("Sysfs test - you wrote %s to /sys/kernel/%s%s\n", buffer, kobj->name, attr->attr.name);
    return count;
}

/* Global variables */
static struct kobject *dummy_kobj;
static struct kobj_attribute dummy_kfile = __ATTR(dummy, 0660, driver_read, driver_write);      //Create sysfs file - named dummy, 0660 is a access right, read callback, write callback

/**
 * @brief This function is called when module is loaded into a kernel
 */
static int __init ModuleInit(void) {
    printk("Hello World,  sysfs text!\n");      //Will print to linux kernel logs!

    /* Goal: create /sys/hello/dummy file! */
    dummy_kobj = kobject_create_and_add("hello", kernel_kobj);    //hello file which will be stored in given location - which is /sys/kernel
    if(dummy_kobj == NULL) {
        printk("sysfs - error creating sys hello!");
        return -ENOMEM;
    }

    if(sysfs_create_file(dummy_kobj, &dummy_kfile.attr) > 0) {
        printk("sysfs - error creating sys hello/dummy!");
        kobject_put(dummy_kobj);        //Delete the folder
        return -ENOMEM;
    }

    printk("sysfs - created /sys/kernel/hello/dummy!\n");
	return 0;
}

/**
 * @brief This function is called when module is unloaded into a kernel
 */
 static void __exit ModuleExit(void) {
    printk("Goodby World, sysfs text!\n");

    sysfs_remove_file(dummy_kobj, &dummy_kfile.attr);    //Delete file
    kobject_put(dummy_kobj);        //Delete folder

    printk("procfs - removed /proc/hello/dummy!\n");
 }

module_init(ModuleInit);
module_exit(ModuleExit);
