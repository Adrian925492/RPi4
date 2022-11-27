#include <linux/module.h>
#include <linux/fs.h>
#include <linux/init.h>

/* Meta Informations */
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Adrian Podsiadlowski");
MODULE_DESCRIPTION("Rgisters device number and implement some callback function");

/**
 * @brief This function is called when device file is opened
 */
static int driver_open(struct inode *device_file, struct file *instance) {
	printk("dev_nr - open was called!\n");
	return 0;
}

/*
 * @brief This function is called when the device file is closed
 */
static int driver_close(struct inode *device_file, struct file *instance) {
	printk("dev nr - close was called!\n");
	return 0;
}

static struct file_operations fops = {
	.owner = THIS_MODULE,
	.open = driver_open,
	.release = driver_close
};

#define MAJOR_DEVICE_NR 90

/**
 * @brief This function is called when module is loaded into a kernel
 */
static int __init ModuleInit(void) {
	int retval;
    	printk("Hello World, Device number!\n");      //Will print to linux kernel logs!
    	retval = register_chrdev(MAJOR_DEVICE_NR, "my_device_nr", &fops);	//Register the callbacks to device driver. Callbacks will be called on event .open and .release defined in struct when the defice file fill be opened or released. The device driver list can be find in /proc/devices file, and full list of available devices in /dev directory. ls -al /dev shows device numbers as well (in 3rd column). The number given here is a major device number. Under each major device number can be registered many minor device numbers.

	if (retval == 0)
	{
		printk ("dev_nr - registered device number Major: %d, Minor %d", MAJOR_DEVICE_NR, 0);
	} else if (retval > 0) {
		printk ("dev_nr - registered device number Major: %d, Minor %d", retval>>20, retval&0xfffff);
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
