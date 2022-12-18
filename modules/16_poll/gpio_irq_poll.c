#include <linux/module.h>
#include <linux/init.h>
#include <linux/gpio.h>
#include <linux/interrupt.h>
#include <linux/ioctl.h>		//Ioctrl defs
#include <linux/fs.h>           //Filesystem related stuffs
#include <linux/cdev.h>         //Chaaracter device related stuffs
#include <linux/poll.h>         //Polling functions
#include <linux/wait.h>

#define MAJOR_DEVICE_NR 64

/* Meta Informations */
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Adrian Podsiadlowski");
MODULE_DESCRIPTION("An example, module sends a signal to an userspace app when gpio with button connected has a rising edge");

/** variable contains pin number of interrupt controller to which gpio21(btn) is mapped to */
unsigned int irq_number;
static int irq_ready = 0;
static wait_queue_head_t waitqueue;


/**
 * @brief This is interrupt service routine callback function
 */
static irq_handler_t gpio_irq_handler(unsigned int irq, void *dev_id, struct pt_regs* regs) {
    printk("An interrupt was triggered!!");
    irq_ready = 1;
    wake_up(&waitqueue);        //Wake up the poll_wait - works similat to condition variable notify(). More qbout wait queues in next chapter.
    return (irq_handler_t) IRQ_HANDLED;
}

/*
 * @brief This function is called when the device file is closed
 */
static int driver_close(struct inode *device_file, struct file *instance) {
	printk("ioctl_example - close was called!\n");
	return 0;
}

/*
 * @brief Poll callback to allow user space app to poll for pressed button
 */
static unsigned int my_poll(struct file *file, poll_table* wait) {
    poll_wait(file, &waitqueue, wait);  //Wait for polling. The "wait" is a non blocking wait function. WOrks similar to wait on condition variable.
    
    if(irq_ready == 1) {    //Interrupt occured before polling
        irq_ready = 0;      //Clear irq indicator
        return POLLIN;          //Indicate that button was pressed! - send a poll event POLLIN
    }
    return 0;   //Nothing happens
}

static struct file_operations fops = {
	.owner = THIS_MODULE,
    .poll = my_poll             //Assign polling callback to the ioctl
};

/**
 * @brief This function is called when module is loaded into a kernel
 */
static int __init ModuleInit(void) {
    printk("Hello World, My poll gpio module!\n");      //Will print to linux kernel logs!

    /* Initialize wait queue */
    init_waitqueue_head(&waitqueue);


    /* Set up gpio */

	/* GPIO BTN INIT */
	if(gpio_request(21, "rpi-gpio-21")) {
		printk("Can not allocate GPIO 21\n!");
		return -1;
	}

	/* Set gpio direction */
	if(gpio_direction_input(21)) {
		printk("Can not set GPIO 21 to output\n!");
		goto Gpio21Error;
	}

    gpio_set_debounce(21,300);

    /* Set up an interrupt */
    // All possible interrupt sources in rpi are routed to interrupt controller. 
    irq_number = gpio_to_irq(21);

    if (request_irq(irq_number, (irq_handler_t)gpio_irq_handler, IRQF_TRIGGER_RISING, "my_gpio_irq", NULL) != 0) {
        printk("ERROR: Cannot request interrupt nr %d", irq_number);
        gpio_free(21);
        return -1;
    }

    if (register_chrdev(MAJOR_DEVICE_NR, "my_device_nr", &fops) < 0) {
        printk("Error registering char device number!");
        gpio_free(21);
        free_irq(irq_number, NULL);
    }

    printk("GPIO %d is mapped to irq nr %d", 21, irq_number);

    return 0;


Gpio21Error:
	gpio_free(21);
	return -1;
}

/**
 * @brief This function is called when module is unloaded into a kernel
 */
 static void __exit ModuleExit(void) {
    printk("Goodby World, My poll gpio module!\n");

    free_irq(irq_number, NULL);
    gpio_free(21);
    unregister_chrdev(MAJOR_DEVICE_NR, "gpio_irq_signal");
 }

module_init(ModuleInit);
module_exit(ModuleExit);
