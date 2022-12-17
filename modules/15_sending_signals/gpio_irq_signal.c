#include <linux/module.h>
#include <linux/init.h>
#include <linux/gpio.h>
#include <linux/interrupt.h>
#include <linux/ioctl.h>		//Ioctrl defs
#include <linux/fs.h>           //Filesystem related stuffs
#include <linux/cdev.h>         //Chaaracter device related stuffs
#include <linux/sched/signal.h> //Singals definitions to send

/* Meta Informations */
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Adrian Podsiadlowski");
MODULE_DESCRIPTION("An example, module sends a signal to an userspace app when gpio with button connected has a rising edge");

/** variable contains pin number of interrupt controller to which gpio21(btn) is mapped to */
unsigned int irq_number;

#define MAJOR_DEVICE_NR 64

/** GLobal variables needed for us app registration */
#define REGISTER_UAPP _IO('R', 'g')
static struct task_struct *task = NULL; //Will store current US task data here when registered

/** Define for signal sending */
#define SIGNR 44

/**
 * @brief This is interrupt service routine callback function
 */
static irq_handler_t gpio_irq_handler(unsigned int irq, void *dev_id, struct pt_regs* regs) {
    siginfo_t info;
    printk("An interrupt was triggered!!");

    if(task != NULL) {
		memset(&info, 0, sizeof(info));
		info.si_signo = SIGNR;
		info.si_code = SI_QUEUE;


		/* Send the signal */
        unsigned int status = send_sig_info(info.si_signo, (struct kernel_siginfo *) &info, task);
        printk("gpio irq - signal will be sent! %d", status);

		if(status < 0) {
			printk("gpio_irq_signal: Error sending signal\n");
        }
    }
    
    return (irq_handler_t) IRQ_HANDLED;
}

/*
 * @brief This function is called when the device file is closed
 */
static int driver_close(struct inode *device_file, struct file *instance) {
	printk("ioctl_example - close was called!\n");
    if (task != NULL)
        task = NULL;
	return 0;
}

/*
 * @brief IOCTRL for registering the userspace app to the lkm
 */
static long int my_ioctl(struct file *file, unsigned cmd, unsigned long arg) {
    if(cmd == REGISTER_UAPP) {
        task = get_current();       //Captures the current task to the task structure
        printk("gpio_irq_signal: Userspace app with PID %d is registered", task->pid);
    }
    return 0;
}

static struct file_operations fops = {
	.owner = THIS_MODULE,
	.release = driver_close,
    .unlocked_ioctl = my_ioctl
};

/**
 * @brief This function is called when module is loaded into a kernel
 */
static int __init ModuleInit(void) {
    printk("Hello World, My int gpio module!\n");      //Will print to linux kernel logs!

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
    printk("Goodby World, My int gpio module!\n");

    free_irq(irq_number, NULL);
    gpio_free(21);
    unregister_chrdev(MAJOR_DEVICE_NR, "gpio_irq_signal");
 }

module_init(ModuleInit);
module_exit(ModuleExit);
