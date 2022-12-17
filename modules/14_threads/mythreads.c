#include <linux/module.h>
#include <linux/init.h>
#include <linux/kthread.h>      //All functions regarding thread handling
#include <linux/sched.h>        //Scheduler - contains definitin f a task structure
#include <linux/delay.h>        //Delay functions in thread

/* Meta Informations */
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Adrian Podsiadlowski");
MODULE_DESCRIPTION("An example of lk module with threads usage.");

/** Global variables for threads */
static struct task_struct *kthread_1;
static struct task_struct *kthread_2;
static int t1 = 1, t2 = 2;

/**
 * @brief This thread function
 * @param Any data passed to thread - in that cxase number of thread
 */
int thread_func(void* thread_nr) {
    unsigned int i = 0;
    int t_nr = *(int*)thread_nr;

    /* Working loop */
    while(!kthread_should_stop()) {
        printk("kthread - thread %d is executed! cnt: %d", t_nr, i++);
        msleep(t_nr *1000);
    }
    printk("kthread - thread %d finished!", t_nr);

    return 0;
}

/**
 * @brief This function is called when module is loaded into a kernel
 */
static int __init ModuleInit(void) {
    printk("Hello World, My thread example!\n");      //Will print to linux kernel logs!

    kthread_1 = kthread_create(thread_func, &t1, "kthread1");       //Creates the thread (ptr to thread fcn, data passed to func, thread name)
    if(kthread_1 == NULL) {
        printk("Error creating thread 1!");
        return -1;
    } else {
        /* Start the thread */
        wake_up_process(kthread_1);     //Statrt thread
        printk("Thread 1 is running now!");
    }
    
    kthread_2 = kthread_run(thread_func, &t2, "kthread2");          //Creates a thread and runs it immidietly
    if(kthread_2 == NULL) {
        kthread_stop(kthread_1);
        printk("Error creating thread 2!");
        return -1;
    } else {
        printk("Thread 2 is running now!");
    }

    return 0;
}

/**
 * @brief This function is called when module is unloaded into a kernel
 */
 static void __exit ModuleExit(void) {
    printk("Goodby World, My thread example!\n");
    kthread_stop(kthread_1);
    kthread_stop(kthread_2);
 }

module_init(ModuleInit);
module_exit(ModuleExit);
