#include <linux/module.h>
#include <linux/init.h>
#include <linux/gpio.h>
#include <linux/interrupt.h>

/* Meta Informations */
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Adrian Podsiadlowski");
MODULE_DESCRIPTION("An example, gpio with interrupt example");

/** variable contains pin number of interrupt controller to which gpio21(btn) is mapped to */
unsigned int irq_number;

/**
 * @brief This is interrupt service routine callback function
 */
static irq_handler_t gpio_irq_handler(unsigned int irq, void *dev_id, struct pt_regs* regs) {
    printk("An interrupt was triggered!!");
    return (irq_handler_t) IRQ_HANDLED;
}

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

    /* Set up an interrupt */
    // All possible interrupt sources in rpi are routed to interrupt controller. 
    irq_number = gpio_to_irq(21);

    if (request_irq(irq_number, (irq_handler_t)gpio_irq_handler, IRQF_TRIGGER_RISING, "my_gpio_irq", NULL) != 0) {
        printk("ERROR: Cannot request interrupt nr %d", irq_number);
        gpio_free(21);
        return -1;
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
 }

module_init(ModuleInit);
module_exit(ModuleExit);
