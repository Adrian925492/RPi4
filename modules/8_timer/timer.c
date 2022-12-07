#include <linux/module.h>
#include <linux/init.h>
#include <linux/gpio.h>
#include <linux/jiffies.h>
#include <linux/timer.h>

/* Meta Informations */
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Adrian Podsiadlowski");
MODULE_DESCRIPTION("An example, timer usage linux kernel module");

/** variables for timer */
static struct timer_list my_timer;

/**
 @brief Timer callback function
 */
void timer_callback(struct timer_list* data) {
    gpio_set_value(18, 0);  //Turn off led
}

/**
 * @brief This function is called when module is loaded into a kernel
 */
static int __init ModuleInit(void) {
    printk("Hello World, My Module!\n");      //Will print to linux kernel logs!

	/* GPIO LED INIT */
	if(gpio_request(18, "rpi-gpio-18")) {
		printk("Can not allocate GPIO 18\n!");
		return -1;
	}

	/* Set gpio direction */
	if(gpio_direction_output(18, 0)) {
		printk("Can not set GPIO 20 to output\n!");
		goto Gpio18Error;
	}

    /* set led */
    gpio_set_value(18,1);

    /* Initialize the timer */
    timer_setup(&my_timer, timer_callback, 0);
    mod_timer(&my_timer, jiffies + msecs_to_jiffies(1000));      //jiffies - is a variable containing nr of systicks from startup the os. Defined in <linux/jiffies.h>

	return 0;

Gpio18Error:
	gpio_free(20);
	return -1;
}

/**
 * @brief This function is called when module is unloaded into a kernel
 */
 static void __exit ModuleExit(void) {
     printk("Goodby World, My Module!\n");

	gpio_free(18);

    del_timer(&my_timer);
 }

module_init(ModuleInit);
module_exit(ModuleExit);
