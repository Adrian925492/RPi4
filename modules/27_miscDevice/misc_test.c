#include <linux/module.h>
#include <linux/init.h>
#include <linux/miscdevice.h>
#include <linux/fs.h>

/* Misc device - is something like char device (can be foun in /dev folder), and provides a standard read/write char
    operation like a standard character device, but:
        * Has always majon device number 10
        * Is much more easier to setup than char device.

    In general it is a special case of character device.
*/

/* Meta Informations */
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Adrian Podsiadlowski");
MODULE_DESCRIPTION("An example, misc device linux kernel module");

//Device file read/write related globals
#define MAX_SIZE 256
static char data[MAX_SIZE];
static size_t data_len;

static int my_open(struct inode *inode, struct file *file) {
    printk("isc device - open called\n");
    printk("misc device = Device number: %d %d\n", imajor(inode), iminor(inode));

    //Check device read permissions
    if(file->f_mode & FMODE_READ) {
        printk ("misc device - opened with read permissions!\n");
    }
    if(file->f_mode & FMODE_WRITE) {
        printk ("misc device - opened with write permissions!\n");
    }
    return 0;
}

static int my_close(struct inode *inode, struct file* file) {
    printk("misc device - close called\n");
    return 0;
}

//__user means that the variable concerns to user space!
static ssize_t my_write(struct file* file, const char __user *user_buffer, size_t user_len, loff_t *ppos) { 
    int status;
    if(user_len > data_len) {
        data_len = MAX_SIZE;
    } else {
        data_len = user_len;
    }

    status = copy_from_user(data, user_buffer, data_len);

    printk("misc device - write called!\n");

    if(status) {
        printk("misc device - error copying data from user!\n");
        return -status;
    }
    return data_len;
}

static ssize_t my_read(struct file* file, char __user *user_buffer, size_t user_len, loff_t *ppos) { 
    int status;
    size_t len;
    if(user_len < data_len) {
        len = data_len;
    } else {
        len = user_len;
    }

    status = copy_to_user(user_buffer, data, len);

    printk("misc device - read called!\n");

    if(status) {
        printk("misc device - error copying data to user!\n");
        return -status;
    }
    return len;
}

static const struct file_operations fops = {
    .owner = THIS_MODULE,
    .read = my_read,
    .write = my_write,
    .open = my_open,
    .release = my_close
};

static struct miscdevice my_device = {
    .name = "testdevice",               //This will show up in /dev
    .minor = MISC_DYNAMIC_MINOR,        //The device major numner (here will be set automatically, but an choose own)
    .fops = &fops,
};

/**
 * @brief This function is called when module is loaded into a kernel
 */
static int __init ModuleInit(void) {
    int status;
    printk("Hello World, misc device init!\n");      //Will print to linux kernel logs!

    status = misc_register(&my_device);     //Register driver

    if (status) { 
        printk("Error while instantiating misc device!\n");
        return -1;
    }
    
    
    return 0;
}

/**
 * @brief This function is called when module is unloaded into a kernel
 */
 static void __exit ModuleExit(void) {
     printk("Goodby World, misc device exit!\n");
     misc_deregister(&my_device);       //Unregister driver
 }

module_init(ModuleInit);
module_exit(ModuleExit);
