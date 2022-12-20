#include <linux/module.h>
#include <linux/init.h>
#include <linux/proc_fs.h> //Procfs related stuffs
#include <linux/i2c.h>

/* Meta Informations */
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Adrian Podsiadlowski");
MODULE_DESCRIPTION("An example, of using i2c device rtc");

/** I2c global variables */
static struct i2c_client *rtc_client;       //Over this pointer the device will be accessed

/*
 * @brief Write data from kernel buffer to file
 */
static ssize_t driver_write(struct file* File, const char* user_buffer, size_t count, loff_t* offs) {
    printk("Write callback call!\n");
	return 0;
}

/*
 * @brief Read data from rtc
 */
static ssize_t driver_read(struct file* File, char* user_buffer, size_t count, loff_t* offs) {
    u8 time;
    time = i2c_smbus_read_byte(rtc_client);
    return sprintf(user_buffer, "%d\n", time);
}

/** Proc file operation structore (similar to devicefile structure) */
static struct proc_ops fops = {
    .proc_write = driver_write,
    .proc_read = driver_read
};

static struct proc_dir_entry *proc_file;

/**
 * @brief This function is called then loading the driver
 */
static int my_rtc_probe(struct i2c_client *client, const struct i2c_device_id  *id) { //i2c_client - struct contains all necessary info from a device tree
    printk("my_rtc_probe - probe i2c function!\n");

    /* Check the device properties */
    if(client->addr != 0x68) {
        printk("my_rtc_probe error - device property address is not right!\n");
        return -1;
    }

    /* Initialize i2c client */
    rtc_client = client;

    /* Goal: create /proc/led file! */
    proc_file = proc_create("myrtc", 0666, NULL, &fops); //create file named dummy, with access rights 0666, in proc_folder (NUL means in /proc)
    if(proc_file == NULL) {
        printk("dtprobe - error creating proc file!");
        return -ENOMEM;
    }
    printk("procfs - created /proc/myrtc!\n");

    return 0;
}


/**
 * @brief This function is called then removing the driver
 */
static int my_rtc_remove(struct i2c_client *client){
    printk("my_rtc_remove - remove function!\n");

    proc_remove(proc_file);

    return 0;
}

//From https://elinux.org/images/f/f9/Petazzoni-device-tree-dummies_0.pdf
static struct of_device_id my_driver_ids[] = {   //List of comatible devices
    {
        .compatible = "brightlight,myrtc",  //Which device is compatible with that driver
    }, { /* sentinel */ }
};

MODULE_DEVICE_TABLE(of, my_driver_ids);     //That macro assigns compatible drivers devices

static struct i2c_device_id my_rtc[] = {     //Another compatble device declaration
    {"my_rtc", 0},
    { },
};

MODULE_DEVICE_TABLE(i2c, my_rtc);           //And its assignment macro

static struct i2c_driver my_driver = { //That struct describes our driver
    .probe = my_rtc_probe,      //Pointer to probe function - called when registring the driver
    .remove = my_rtc_remove,    //Pointer to remove function - called on removing the driver
    .id_table = my_rtc,
    .driver = {
        .name = "my_rtc",         //The driver name
        .of_match_table = my_driver_ids,    //List of compatible devices assigned to
    },
};

module_i2c_driver(my_driver);   //This will create the init nd exit function automatically!
#if 0 //Instead of that we can use the function above!
/**
 * @brief This function is called when module is loaded into a kernel
 */
static int __init ModuleInit(void) {
    printk("Hello World, My dt probe!\n");      //Will print to linux kernel logs!

    if (platform_driver_register(&my_driver)) {
        printk("dtprobe error - could not load the driver\n");
        return -1;
    }

    return 0;
}

/**
 * @brief This function is called when module is unloaded into a kernel
 */
 static void __exit ModuleExit(void) {
    printk("Goodby World, My dt probe!\n");

    platform_driver_unregister(&my_driver);
 }

module_init(ModuleInit);
module_exit(ModuleExit);

#endif