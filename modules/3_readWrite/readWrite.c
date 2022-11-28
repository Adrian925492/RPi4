#include <linux/module.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>

/* Meta Informations */
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Adrian Podsiadlowski");
MODULE_DESCRIPTION("Rgisters device number and implement some callback function");

/* Buffer for data */
static char buffer[255];
static int buffer_pointer;

/* Variables for device and device class */
static dev_t my_device_nr;
static struct class *my_class;
static struct cdev my_device;


/**
 * @brief Read data out of the buffer
 * @note That function will read data from file (device) to the kernel space given buffer
 */
static ssize_t driver_read(struct file* File, char* user_buffer, size_t count, loff_t * offs) {
	int to_copy, not_copied, delta;

	/* Get amount data to copy */
	to_copy = min(count, buffer_pointer);

	/* Copy data to user */
	not_copied = copy_to_user(user_buffer, buffer, to_copy);

	/* As the kernel module resides in diferent memory region that data that wil be copied from, we cannot use memcopy! */

	/* Calculate delta */
	delta = to_copy - not_copied;

	return delta;
}

/*
 * @brief Write data from kernel buffer to file
 */
static ssize_t driver_write(struct file* File, const char* user_buffer, size_t count, loff_t* offs) {
	int to_copy, not_copied, delta;

	/* Get amount data to copy */
	to_copy = min(count, sizeof(buffer));

	/*Copy data to user */
	not_copied = copy_from_user(buffer, user_buffer, to_copy);
	buffer_pointer = to_copy;

	/* Calculate delta */
	delta = to_copy - not_copied;

	return delta;
}


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
	.release = driver_close,
	.read = driver_read,
	.write = driver_write
};

#define DRIVER_NAME "dummydriver"
#define DRIVER_CLASS "myModuleClass"
/**
 * @brief This function is called when module is loaded into a kernel
 */
static int __init ModuleInit(void) {
	int retval;
    	printk("Hello World,  Read-write device!\n");      //Will print to linux kernel logs!

	/* Allocatea device number */
	if( alloc_chrdev_region(&my_device_nr, 0, 1, DRIVER_NAME) < 0) {
		printk("Device nr. could not be allocated \n");
		return -1;
	}

	printk("read_write - device number: major: %d, Minor %d was registered!", my_device_nr>>20, my_device_nr && 0xfffff);

	/* Create a device class */
	if((my_class = class_create(THIS_MODULE, DRIVER_CLASS)) == NULL) {
			printk("Device class cannot be created!\n");
			goto ClassError;
	}

	/* Create device file */
	if(device_create(my_class, NULL, my_device_nr, NULL, DRIVER_NAME) == NULL) {
		printk("Can not create device file!\n");
		goto FileError;
	}

	/* Initialize device file */
	cdev_init(&my_device, &fops);

	/* Register device to kernel */
	if(cdev_add(&my_device, my_device_nr, 1) == -1)
	{
		printk("Register of device to kernel failed!]n");
		goto AddError;
	}
	return 0;

AddError:
	device_destroy(my_class, my_device_nr);	

FileError:
	class_destroy(my_class);

ClassError:
	unregister_chrdev(my_device_nr, DRIVER_NAME);
	return -1;

}

/**
 * @brief This function is called when module is unloaded into a kernel
 */
 static void __exit ModuleExit(void) {
	cdev_del(&my_device);
 	device_destroy(my_class, my_device_nr);	
	class_destroy(my_class);
	unregister_chrdev(my_device_nr, DRIVER_NAME);
     	printk("Goodby World, Read write device!\n");
 }

module_init(ModuleInit);
module_exit(ModuleExit);
