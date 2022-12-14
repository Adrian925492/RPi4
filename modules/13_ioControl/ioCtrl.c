#include <linux/module.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/ioctl.h>		//Ioctrl defs
#include <linux/uaccess.h>		//Defs for copy to and copy from user functions

#include "ioctrl_test.h"

/* IO ctrl is a mechanism that allows user space app to interract with kernem module app - by sending commnds between them.

To do so, we need a command API, defined in ioctrl_test.h. 

That api is included in bots user and kernel space code, like kernel headers.

The kernel part recognizes what command has been requested and does an action on it.

The user part sends the commands to kernel part and gets an answer */

/* Meta Informations */
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Adrian Podsiadlowski");
MODULE_DESCRIPTION("Simple example of ioctl for linux kernel module");

/**
 * @brief This function is called when device file is opened
 */
static int driver_open(struct inode *device_file, struct file *instance) {
	printk("ioctl_example - open was called!\n");
	return 0;
}

/*
 * @brief This function is called when the device file is closed
 */
static int driver_close(struct inode *device_file, struct file *instance) {
	printk("ioctl_example - close was called!\n");
	return 0;
}

// Global variable - we will change that value by interracting with ioctl from user space
int32_t variable = 12;

/*
 * @brief A function that handhelds ioctl
 */
static long int my_ioctl(struct file *dev_file, unsigned cmd, unsigned long arg) {
	struct mystruct test;
	
	switch(cmd){
		case WRITE_VALUE:
			if (copy_from_user(&variable, (int32_t*) arg, sizeof(variable))) {
				printk("ioctl_example - Error copying data from user space to module!");
			}
			printk("ioctl_example - Update the variable to %d", variable);
			break;

		case READ_VALUE:
			if (copy_to_user((int32_t*) arg, &variable, sizeof(variable))) {
				printk("ioctl_example - Error copying data to user space from module!");
			}
			printk("ioctl_example - Variable was copied to user space: %d", variable);
			break;

		case GREETER:
			if (copy_from_user(&test, (struct mystruct*) arg, sizeof(test))) {
				printk("ioctl_example - Error copying data from user space struct to module!");
			}
			printk("ioctl_example - Update the struct mystruct to kernel module: %d %s", test.repeat, test.name);
			break;
	}
	return 0;
}

static struct file_operations fops = {
	.owner = THIS_MODULE,
	.open = driver_open,
	.release = driver_close,
	.unlocked_ioctl = my_ioctl
};

#define MAJOR_DEVICE_NR 64

/**
 * @brief This function is called when module is loaded into a kernel
 */
static int __init ModuleInit(void) {
	int retval;
    	printk("Hello World, Device number!\n");      //Will print to linux kernel logs!
    	retval = register_chrdev(MAJOR_DEVICE_NR, "my_device_nr", &fops);	//Register the callbacks to device driver. Callbacks will be called on event .open and .release defined in struct when the defice file fill be opened or released. The device driver list can be find in /proc/devices file, and full list of available devices in /dev directory. ls -al /dev shows device numbers as well (in 3rd column). The number given here is a major device number. Under each major device number can be registered many minor device numbers.

	if (retval == 0)
	{
		printk ("ioctl_example - registered device number Major: %d, Minor %d", MAJOR_DEVICE_NR, 0);
	} else if (retval > 0) {
		printk ("ioctl_example - registered device number Major: %d, Minor %d", retval>>20, retval&0xfffff);
	} else {
		printk("Could not register device number!\n");
		return -1;
	}
	return 0;
}

/**
 * @brief This function is called when module is unloaded into a kernel
 */
 static void __exit ModuleExit(void) {
	unregister_chrdev(MAJOR_DEVICE_NR, "my_device_nr");
     printk("Goodby World, Device number!\n");
 }

module_init(ModuleInit);
module_exit(ModuleExit);
