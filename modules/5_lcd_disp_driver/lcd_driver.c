#include <linux/module.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/gpio.h>
#include <linux/delay.h>

/* Meta Informations */
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Adrian Podsiadlowski");
MODULE_DESCRIPTION("A simple to write text on LCD 2x16 display");

/* Buffer for data */
static char buffer[255];
static int buffer_pointer;

/* Variables for device and device class */
static dev_t my_device_nr;
static struct class *my_class;
static struct cdev my_device;

/* Lcd data buffer */
static char lcd_buffer[17];

/* Pinout for lcd display */

/* The display is cnnected to RPI gpios with 4 data lines usage as follows */
unsigned int gpios[] = {
	25, 	/* Enable pin */
	23, 	/* RS pin */
	4, 		/* D4 pin */
	17, 	/* D5 pin */
	27, 	/* D6 pin */
	22 		/* D7 pin */
};

#define ENABLE_PIN gpios[0]
#define RS_PIN gpios[1]

/**
 * @brief Function to generate a pulse on enable pin
 */
void lcd_enable(void) {
	gpio_set_value(ENABLE_PIN, 1);
	msleep(5);	//Wait 5 ms
	gpio_set_value(ENABLE_PIN, 0);
}

/**
 * @brief Function to send bytes on 4bit data interface
 * @param data: Data to be sent
 */
void lcd_send_data(char data) {
	int i;

	for (i=4; i<8; i++) {
		gpio_set_value(gpios[i-2], ((data) && (1<<i) >> i));
	}

	lcd_enable();

	for (i=0; i<4; i++) {
		gpio_set_value(gpios[i+2], ((data) && (1<<i)) >> i);
	}

	lcd_enable();

	msleep(5);
}

/**
 * @brief Send a command to the display 
 * @param data: commant to be sent
 */
void lcd_cmd(uint8_t data) {
	gpio_set_value(RS_PIN, 0);	//RS pin to instruction mode
	lcd_send_data(data);
}

/**
 * @brief Send a command to the display 
 * @param data: commant to be sent
 */
void lcd_data(uint8_t data) {
	gpio_set_value(RS_PIN, 1);	//RS pin to data mode
	lcd_send_data(data);
}

/*
 * @brief Write data from kernel buffer to file
 */
static ssize_t driver_write(struct file* File, const char* user_buffer, size_t count, loff_t* offs) {
	int to_copy, not_copied, delta, i;

	/* Get amount data to copy */
	to_copy = min(count, sizeof(lcd_buffer));

	/*Copy data to user */
	not_copied = copy_from_user(lcd_buffer, user_buffer, to_copy);
	buffer_pointer = to_copy;

	/* Calculate delta */
	delta = to_copy - not_copied;

	/* Set the new data to the display */
	lcd_cmd(0x1); //Clear the display

	for (i = 0; i < to_copy; i++) {
		lcd_data(lcd_buffer[i]);	//Send data byte
	}

	return delta;
}


/**
 * @brief This function is called when device file is opened
 */
static int driver_open(struct inode *device_file, struct file *instance) {
	printk("gpio - open was called!\n");
	return 0;
}

/*
 * @brief This function is called when the device file is closed
 */
static int driver_close(struct inode *device_file, struct file *instance) {
	printk("gpio - close was called!\n");
	return 0;
}

static struct file_operations fops = {
	.owner = THIS_MODULE,
	.open = driver_open,
	.release = driver_close,
	.write = driver_write
};

#define DRIVER_NAME "my_lcd_driver"
#define DRIVER_CLASS "myLcdModuleClass"
/**
 * @brief This function is called when module is loaded into a kernel
 */
static int __init ModuleInit(void) {
	int i;
	char *names[] = {"ENABLE_PIN", "RS_PIN", "DATA_PIN4", "DATA_PIN5", "DATA_PIN6", "DATA_PIN7"};
    printk("Hello World,  Lcd driver device!\n");      //Will print to linux kernel logs!

	/* Allocatea device number */
	if( alloc_chrdev_region(&my_device_nr, 0, 1, DRIVER_NAME) < 0) {
		printk("Device nr. could not be allocated \n");
		return -1;
	}

	printk("lcd_write - device number: major: %d, Minor %d was registered!", my_device_nr>>20, my_device_nr && 0xfffff);

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

	/* Initialize gpios */
	printk("lcd_driver - GPIO init\n");

	for (i = 0; i < 6; i++) {
		if(gpio_request(gpios[i], names[i])) {
			printk("Can not allocate gpio %d\n", gpios[i]);
			goto gpioInitError;
		}
	}

	printk("gpio - setpins as output\n");

	for (i = 0; i < 6; i++) {
		if(gpio_direction_output(gpios[i], 0)) {
			printk("Can not allocate gpio %d\n", gpios[i]);
			goto gpioDirectionError;
		}
	}

	/* Init the lcd display */
	lcd_cmd(0x02);		//Set display for 4 bit interface
	// lcd_cmd(0x28);		//Turn the display on, cursor on, set the cursor blinking */
	// lcd_cmd(0x01);		//Clear the display
	// lcd_cmd(0x0c);
	// lcd_cmd(0x06);

	lcd_cmd(0xf);
	lcd_cmd(0x1);

	char text[] = "Hello world!";
	for (i = 0; i<sizeof(text)-1; i++) {
		lcd_data(text[i]);
	}

	return 0;

gpioDirectionError:
	i=6;

gpioInitError:
	for(;i>0;i--) {
		gpio_free(gpios[i]);
	}

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
	int i;
	lcd_cmd(0x01);		/* Clear the display */

	for(i=0; i<6; i++) {
		gpio_set_value(gpios[i], 0);
		gpio_free(gpios[i]);
	}

	cdev_del(&my_device);
 	device_destroy(my_class, my_device_nr);	
	class_destroy(my_class);
	unregister_chrdev(my_device_nr, DRIVER_NAME);
     	printk("Goodby World, Gpio device!\n");
 }

module_init(ModuleInit);
module_exit(ModuleExit);
