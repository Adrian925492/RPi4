#include <linux/module.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/pwm.h>

/* Meta Informations */
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Adrian Podsiadlowski");
MODULE_DESCRIPTION("A simple to steer pwm pin");

/* RPI4 has a hardware PWM available on pins  GPIO12, GPIO13, GPIO18, and GPIO19. On that example only GPIO18 will be used */

/* Buffer for data */
static int buffer_pointer;

/* Variables for device and device class */
static dev_t my_device_nr;
static struct class *my_class;
static struct cdev my_device;

/* Variables for PWM */
struct pwm_device *pwm0 = NULL;
u32 pwm_on_time = 500000000;
/*
 * @brief Write data from kernel buffer to file to steer pwm duty
 */
static ssize_t driver_write(struct file* File, const char* user_buffer, size_t count, loff_t* offs) {
	int to_copy, not_copied, delta;
	char value;

	/* Get amount of data to copy */
	to_copy = min(count, sizeof(value));

	/* Copy data to user */
	not_copied = copy_from_user(&value, user_buffer, to_copy);

	/* Set PWM on time */
	if (value < 'a' || value > 'j') {
		printk("Invalid value %c\n", value);
	} else {
		pwm_config(pwm0, 100000000 * (value - 'a'), 1000000000);
	}

	/* Calculate delta */
	delta = to_copy - not_copied;

	return delta;
}

/**
 * @brief This function is called when device file is opened
 */
static int driver_open(struct inode *device_file, struct file *instance) {
	printk("pwm - open was called!\n");
	return 0;
}

/*
 * @brief This function is called when the device file is closed
 */
static int driver_close(struct inode *device_file, struct file *instance) {
	printk("pwm - close was called!\n");
	return 0;
}

static struct file_operations fops = {
	.owner = THIS_MODULE,
	.open = driver_open,
	.release = driver_close,
	.write = driver_write
};

#define DRIVER_NAME "my_pwm_driver"
#define DRIVER_CLASS "myPwmModuleClass"
/**
 * @brief This function is called when module is loaded into a kernel
 */
static int __init ModuleInit(void) {
	printk("Hello World,  Pwm driver device!\n");      //Will print to linux kernel logs!

	/* Allocatea device number */
	if( alloc_chrdev_region(&my_device_nr, 0, 1, DRIVER_NAME) < 0) {
		printk("Device nr. could not be allocated \n");
		return -1;
	}

	printk("pwm_write - device number: major: %d, Minor %d was registered!", my_device_nr>>20, my_device_nr && 0xfffff);

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

	pwm0 = pwm_request(0, "my_pwm");
	if (pwm0 == NULL) {
		printk("Could not get PWM0\n");
		goto AddError;
	}

	pwm_config(pwm0, pwm_on_time, 100000000);
	pwm_enable(pwm0);

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
	pwm_disable(pwm0);
	pwm_free(pwm0);

	cdev_del(&my_device);
 	device_destroy(my_class, my_device_nr);	
	class_destroy(my_class);
	unregister_chrdev(my_device_nr, DRIVER_NAME);

    printk("Goodby World, Pwm device!\n");
 }

module_init(ModuleInit);
module_exit(ModuleExit);
