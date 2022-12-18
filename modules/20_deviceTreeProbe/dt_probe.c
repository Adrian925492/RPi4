#include <linux/module.h>
#include <linux/init.h>
#include <linux/mod_devicetable.h>  //Includes to devicetree parse
#include <linux/property.h>
#include <linux/platform_device.h>
#include <linux/of_device.h>

/* Meta Informations */
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Adrian Podsiadlowski");
MODULE_DESCRIPTION("An example, of using and parsing a device tree in lkm driver");

/** Devclare the probe and remove functions */

/**
 * @brief This function is called then loading the driver
 */
static int dt_probe(struct platform_device *pdev) { //pdev - struct contains all necessary info from a device tree
    struct device *dev = &pdev->dev;
    const char *label;
    int my_value, ret;

    printk("dtbrobe - probe function!\n");

    /* Check the device properties */
    if(!device_property_present(dev, "label")) {
        printk("dtprobe error - device property label not found!\n");
        return -1;
    }

    if(!device_property_present(dev, "my_value")) {
        printk("dtprobe error - device property my_value not found!\n");
        return -1;
    }

    /* Read the properties */
    ret = device_property_read_string(dev, "label", &label);
    if (ret) {
        printk("dtprobe error - device property label read error!\n");
        return -1;
    }
    printk("dr_probe - label is %s\n", label);

    ret = device_property_read_u32(dev, "my_value", my_value);
    if (ret) {
        printk("dtprobe error - device property my_value read error!\n");
        return -1;
    }
    printk("dr_probe - my_value is %d\n", my_value);

    return 0;
}


/**
 * @brief This function is called then removing the driver
 */
static int dt_remove(struct platform_device *pdev) {
    printk("dtbrobe - remove function!\n");
    return 0;
}

//From https://elinux.org/images/f/f9/Petazzoni-device-tree-dummies_0.pdf
static struct of_device_id my_driver_ids[] = {   //List of comatible devices
    {
        .compatible = "brightlight,mydev",  //Which device is compatible with that driver
    }, { /* sentinel */ }
};

MODULE_DEVICE_TABLE(of, my_driver_ids);     //That macro assigns compatible drivers devices

static struct platform_driver my_driver = { //That struct describes our driver
    .probe = dt_probe,      //Pointer to probe function - called when registring the driver
    .remove = dt_remove,    //Pointer to remove function - called on removing the driver
    .driver = {
        .name = "my_device_driver",         //The driver name
        .of_match_table = my_driver_ids,    //List of compatible devices assigned to
    },
};

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
