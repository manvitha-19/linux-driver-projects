// SPDX-License-Identifier: GPL-2.0
/*
 * simple_i2c_driver.c - Full-feature I2C client driver with char device
 * Author: Kariveda Manvitha
 */

#include <linux/module.h>
#include <linux/i2c.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>

#define DRIVER_NAME "simple_i2c_driver"
#define DEVICE_NAME "simple_i2c_dev"
#define CLASS_NAME  "simple_i2c_class"

static struct i2c_client *i2c_demo_client;
static struct class *i2c_class;
static struct cdev i2c_cdev;
static dev_t dev_number;

/* open callback */
static int simple_i2c_open(struct inode *inode, struct file *file)
{
    pr_info("%s: device opened\n", DRIVER_NAME);
    return 0;
}

/* release callback */
static int simple_i2c_release(struct inode *inode, struct file *file)
{
    pr_info("%s: device closed\n", DRIVER_NAME);
    return 0;
}

/* read from device via char interface */
static ssize_t simple_i2c_read(struct file *file, char __user *buf,
                               size_t count, loff_t *offset)
{
    int ret;
    u8 data;

    if (!i2c_demo_client)
        return -ENODEV;

    /* Read from register 0x00 for demonstration */
    ret = i2c_smbus_read_byte_data(i2c_demo_client, 0x00);
    if (ret < 0) {
        pr_err("%s: I2C read failed\n", DRIVER_NAME);
        return ret;
    }

    data = ret;
    if (copy_to_user(buf, &data, 1))
        return -EFAULT;

    pr_info("%s: Read 0x%02x from reg 0x00\n", DRIVER_NAME, data);
    return 1;
}

/* write to device via char interface */
static ssize_t simple_i2c_write(struct file *file, const char __user *buf,
                                size_t count, loff_t *offset)
{
    int ret;
    u8 data;

    if (!i2c_demo_client)
        return -ENODEV;

    if (copy_from_user(&data, buf, 1))
        return -EFAULT;

    /* Write to register 0x00 for demonstration */
    ret = i2c_smbus_write_byte_data(i2c_demo_client, 0x00, data);
    if (ret < 0) {
        pr_err("%s: I2C write failed\n", DRIVER_NAME);
        return ret;
    }

    pr_info("%s: Wrote 0x%02x to reg 0x00\n", DRIVER_NAME, data);
    return 1;
}

/* file operations */
static struct file_operations fops = {
    .owner   = THIS_MODULE,
    .open    = simple_i2c_open,
    .release = simple_i2c_release,
    .read    = simple_i2c_read,
    .write   = simple_i2c_write,
};

/* probe: device matched */
static int simple_i2c_probe(struct i2c_client *client)
{
    int ret;
    pr_info("%s: Probed device at address 0x%02x\n", DRIVER_NAME, client->addr);
    i2c_demo_client = client;

    /* register char device */
    ret = alloc_chrdev_region(&dev_number, 0, 1, DEVICE_NAME);
    if (ret < 0) {
        pr_err("%s: alloc_chrdev_region failed\n", DRIVER_NAME);
        return ret;
    }

    cdev_init(&i2c_cdev, &fops);
    if (cdev_add(&i2c_cdev, dev_number, 1) < 0) {
        pr_err("%s: cdev_add failed\n", DRIVER_NAME);
        unregister_chrdev_region(dev_number, 1);
        return -1;
    }

    i2c_class = class_create(CLASS_NAME);
    if (IS_ERR(i2c_class)) {
        pr_err("%s: class_create failed\n", DRIVER_NAME);
        cdev_del(&i2c_cdev);
        unregister_chrdev_region(dev_number, 1);
        return PTR_ERR(i2c_class);
    }

    if (IS_ERR(device_create(i2c_class, NULL, dev_number, NULL, DEVICE_NAME))) {
        pr_err("%s: device_create failed\n", DRIVER_NAME);
        class_destroy(i2c_class);
        cdev_del(&i2c_cdev);
        unregister_chrdev_region(dev_number, 1);
        return -1;
    }

    pr_info("%s: char device registered successfully\n", DRIVER_NAME);
    return 0;
}

/* remove: device removed */
static void simple_i2c_remove(struct i2c_client *client)
{
    pr_info("%s: Removing device\n", DRIVER_NAME);

    device_destroy(i2c_class, dev_number);
    class_destroy(i2c_class);
    cdev_del(&i2c_cdev);
    unregister_chrdev_region(dev_number, 1);
}

/* match device tree (optional) */
static const struct of_device_id simple_i2c_of_match[] = {
    { .compatible = "manvitha,simple-i2c-device", },
    { }
};
MODULE_DEVICE_TABLE(of, simple_i2c_of_match);

/* driver struct */
static struct i2c_driver simple_i2c_driver = {
    .driver = {
        .name = DRIVER_NAME,
        .of_match_table = of_match_ptr(simple_i2c_of_match),
    },
    .probe    = simple_i2c_probe,
    .remove   = simple_i2c_remove,
};

/* module init/exit */
static int __init simple_i2c_init(void)
{
    int ret;

    pr_info("%s: Init\n", DRIVER_NAME);

    ret = i2c_add_driver(&simple_i2c_driver);
    if (ret) {
        pr_err("%s: failed to add driver\n", DRIVER_NAME);
        return ret;
    }

    pr_info("%s: driver registered successfully\n", DRIVER_NAME);
    return 0;
}

static void __exit simple_i2c_exit(void)
{
    i2c_del_driver(&simple_i2c_driver);
    pr_info("%s: driver unregistered\n", DRIVER_NAME);
}

module_init(simple_i2c_init);
module_exit(simple_i2c_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Kariveda Manvitha");
MODULE_DESCRIPTION("Full-feature I2C client driver with char device interface");
MODULE_VERSION("1.0");

