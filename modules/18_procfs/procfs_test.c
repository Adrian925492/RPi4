#include <linux/module.h>
#include <linux/init.h>
#include <linux/proc_fs.h> //Procfs related stuffs

/* Meta Informations */
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Adrian Podsiadlowski");
MODULE_DESCRIPTION("Linux procfs usage exmple");

/*
Procfs is a process related filestystem in linux. It resides in /proc directory. The content is available only when kernel is running, and is refreshed every reboot.
The filesystem contains directories for each process (named as proces ID), and some files with information about hardware, memory et. Each folder contains
informations about process it reresents.
*/

/* Global variables */
static struct proc_dir_entry *proc_folder;
static struct proc_dir_entry *proc_file;

/**
 * @brief Read data out of the buffer
 * @note That function will read data from file (device) to the kernel space given buffer
 */
static ssize_t driver_read(struct file* File, char* user_buffer, size_t count, loff_t * offs) {
	char text[] = "Hello from a procfs file!\n";
    int to_copy, not_copied, delta;

	/* Get amount data to copy */
	to_copy = min(count, sizeof(text));

	/* Copy data to user */
	not_copied = copy_to_user(user_buffer, text, to_copy);

	/* As the kernel module resides in diferent memory region that data that wil be copied from, we cannot use memcopy! */

	/* Calculate delta */
	delta = to_copy - not_copied;

	return delta;
}

/*
 * @brief Write data from kernel buffer to file
 */
static ssize_t driver_write(struct file* File, const char* user_buffer, size_t count, loff_t* offs) {
	char text[256];
    int to_copy, not_copied, delta;

    /* Clear text */
    memset(text, 0, sizeof(text));

	/* Get amount data to copy */
	to_copy = min(count, sizeof(text));

	/*Copy data to user */
	not_copied = copy_from_user(text, user_buffer, to_copy);
	printk("procfs text - You have written %s to me\n", text);

	/* Calculate delta */
	delta = to_copy - not_copied;

	return delta;
}

/** Proc file operation structore (similar to devicefile structure) */
static struct proc_ops fops = {
    .proc_read = driver_read,
    .proc_write = driver_write
};

/**
 * @brief This function is called when module is loaded into a kernel
 */
static int __init ModuleInit(void) {
    printk("Hello World,  procfs text!\n");      //Will print to linux kernel logs!

    /* Goal: create /proc/hello/dummy file! */
    proc_folder = proc_mkdir("hello", NULL);    //NULL - means we want to create the dir inside standard dir location - which is /proc
    if(proc_folder == NULL) {
        printk("procfs_test - error creating proc hello!");
        return -ENOMEM;
    }

    proc_file = proc_create("dummy", 0666, proc_folder, &fops); //create file named dummy, with access rights 0666, in proc_folder (NUL means in /proc)
    if(proc_file == NULL) {
        printk("procfs_test - error creating proc file!");
        proc_remove(proc_folder);
        return -ENOMEM;
    }
    printk("procfs - created /proc/hello/dummy!\n");
	return 0;
}

/**
 * @brief This function is called when module is unloaded into a kernel
 */
 static void __exit ModuleExit(void) {
    printk("Goodby World, procfs text!\n");

    proc_remove(proc_file);
    proc_remove(proc_folder);

    printk("procfs - removed /proc/hello/dummy!\n");
 }

module_init(ModuleInit);
module_exit(ModuleExit);
