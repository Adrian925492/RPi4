#include <linux/module.h>
#include <linux/init.h>
#include <linux/kthread.h>
#include <linux/sched.h>
#include <linux/delay.h>
#include <linux/fs.h>
#include <linux/wait.h>
#include <linux/jiffies.h>

#define MYMAJOR 64

/* Meta Informations */
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Adrian Podsiadlowski");
MODULE_DESCRIPTION("An example, module sends a signal to an userspace app when gpio with button connected has a rising edge");

/** Global variables for threads */
static struct task_struct *kthread_1;
static struct task_struct *kthread_2;
static int t1 = 1, t2 = 2;

/** Watch wariable to monitor the qaitqueues */
static long int watch_var = 0;

/* Static method to declare a waitqueue */
DECLARE_WAIT_QUEUE_HEAD(wq1);       //Already initialized here

/* Dynamic method to declare a waitqueue */
static wait_queue_head_t wq2;       //Has to be initialized somewhere in a code

/**
 * @brief This thread function
 * @param Any data passed to thread - in that cxase number of thread
 */
int thread_func(void* wait_sel) {
    int selection = *(int*) wait_sel;
    printk("waitqueue - thread monitoring wq%d finished execution", selection);

    switch(selection) {
        case 1:
            wait_event(wq1, watch_var == 11);  //Will check if var == 11, if not will go to sleep untill wq1 is notified. Than, it checks variable again and so on.
            printk("waitqueue - watch_var is now 11");
        break;
        case 2:
            while(wait_event_timeout(wq2, watch_var == 22, msecs_to_jiffies(5000) == 0));      //The last arg is a timeout - in that case 5 sec. It will ret a timeout untill notified. If notified and timeout wil not elapse - it will ret zero. 
                printk("waitqueue - watch_var is still not 22 but timeout elapsed!");
            printk("waitqueue - watch_var is now 22");
        break;
        default:
        break;
    }

    return 0;
}

/*
 * @brief Write data from kernel buffer to file
 */
static ssize_t driver_write(struct file* File, const char* user_buffer, size_t count, loff_t* offs) {
	int to_copy, not_copied, delta;
    char buffer[16];
    printk("waitqueue - write callback called");

    memset(buffer, 0, sizeof(buffer));

	/* Get amount data to copy */
	to_copy = min(count, sizeof(buffer));

	/*Copy data to user */
	not_copied = copy_from_user(buffer, user_buffer, to_copy);

	/* Calculate delta */
	delta = to_copy - not_copied;

    /* Convert string to long int */
    if (kstrtol(buffer, 10, &watch_var) == -EINVAL) {  //10 is a base - 10 means we want to obtain devcimal values
        printk("Error converting input!");
    } else {
        printk("Watchvar is now %d", watch_var);
    }

    wake_up(&wq1);
    wake_up(&wq2);

	return delta;
}

static struct file_operations fops = {
	.owner = THIS_MODULE,
	.write = driver_write
};

/**
 * @brief This function is called when module is loaded into a kernel
 */
static int __init ModuleInit(void) {
    int t1 = 1, t2 = 2;
    printk("Hello World, My waitqueue example!\n");      //Will print to linux kernel logs!

    /* Initi dynamic waitqueue */
    init_waitqueue_head(&wq2);

    /* Register device number */
    if (register_chrdev(MYMAJOR, "my_dev_nr", &fops) != 0) {
        printk("waitqueue - Could not register device number!");
        return -1;
    }

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
        watch_var = 11;
        wake_up(&wq1);
        mdelay(10);
        watch_var = 22;
        wake_up(&wq2);
        mdelay(10);
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
    watch_var = 11;
    wake_up(&wq1);
    mdelay(10);
    watch_var = 22;
    wake_up(&wq2);
    mdelay(10);
    unregister_chrdev(MYMAJOR, "my_dev_nr");
 }

module_init(ModuleInit);
module_exit(ModuleExit);