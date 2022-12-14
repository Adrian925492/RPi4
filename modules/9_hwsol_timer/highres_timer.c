#include <linux/module.h>
#include <linux/init.h>
#include <linux/hrtimer.h>
#include <linux/jiffies.h>

/* Meta Informations */
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Adrian Podsiadlowski");
MODULE_DESCRIPTION("An example, using high resolution timer in linux kernel module");

/* hight resolution timer variables */
static struct hrtimer my_hrtimer;
u64 start_t;

static enum hrtimer_restart test_hrtimer_handler(struct hrtimer *timer) {
    /* Het current time */
    u64 now_t = jiffies;
    printk("start_t - now_t = %u\n", jiffies_to_msecs(now_t - start_t));

    return HRTIMER_NORESTART;
}

/**
 * @brief This function is called when module is loaded into a kernel
 */
static int __init ModuleInit(void) {
    printk("Hello World, My Mudule!\n");      //Will print to linux kernel logs!

    /* Init of hrtimer */
    hrtimer_init(&my_hrtimer, CLOCK_MONOTONIC, HRTIMER_MODE_REL);   //Init timer
    my_hrtimer.function = &test_hrtimer_handler;                    //Assign a callback
    start_t = jiffies;
    hrtimer_start(&my_hrtimer, ms_to_ktime(200), HRTIMER_MODE_REL);     //Start timer and set wait (ms_t_ktime => miliseconds to kernel time)
    
    return 0;
}

/**
 * @brief This function is called when module is unloaded into a kernel
 */
 static void __exit ModuleExit(void) {
     printk("Goodby World, My Module!\n");
     hrtimer_cancel(&my_hrtimer);
 }

module_init(ModuleInit);
module_exit(ModuleExit);
