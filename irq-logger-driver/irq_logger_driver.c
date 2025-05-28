#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/interrupt.h>
#include <linux/uaccess.h>

#define IRQ_NUM 1

dev_t dev;
struct cdev irq_cdev;
struct class *irq_class;
char irq_msg[100] = "IRQ Not Triggered Yet\n";

static irqreturn_t irq_logger_handler(int irq, void *dev_id)
{
    snprintf(irq_msg, sizeof(irq_msg), "IRQ %d Triggered\n", irq);
    pr_info("IRQ Logger: %s", irq_msg);
    return IRQ_HANDLED;
}

static int irq_open(struct inode *inode, struct file *file)
{
    pr_info("IRQ Logger: Device opened\n");
    return 0;
}

static int irq_release(struct inode *inode, struct file *file)
{
    pr_info("IRQ Logger: Device closed\n");
    return 0;
}

static ssize_t irq_read(struct file *filp, char __user *buf, size_t len, loff_t *off)
{
    size_t msg_len = strlen(irq_msg);
    if (*off >= msg_len)
        return 0;
    if (copy_to_user(buf, irq_msg, msg_len))
        return -EFAULT;
    *off += msg_len;
    return msg_len;
}

static struct file_operations fops = {
    .owner = THIS_MODULE,
    .open = irq_open,
    .release = irq_release,
    .read = irq_read,
};

static int __init irq_logger_init(void)
{
    int ret;

    // Allocate device number
    if ((ret = alloc_chrdev_region(&dev, 0, 1, "irq_logger")) < 0) {
        pr_info("Failed to allocate major number\n");
        return ret;
    }

    // Create device class
    irq_class = class_create("irq_class");
    if (IS_ERR(irq_class)) {
        pr_info("Failed to create class\n");
        unregister_chrdev_region(dev, 1);
        return PTR_ERR(irq_class);
    }

    // Create device
    if (IS_ERR(device_create(irq_class, NULL, dev, NULL, "irq_logger_device"))) {
        pr_info("Failed to create device\n");
        class_destroy(irq_class);
        unregister_chrdev_region(dev, 1);
        return -1;
    }

    // Init and add cdev
    cdev_init(&irq_cdev, &fops);
    if ((cdev_add(&irq_cdev, dev, 1)) < 0) {
        pr_info("Failed to add cdev\n");
        device_destroy(irq_class, dev);
        class_destroy(irq_class);
        unregister_chrdev_region(dev, 1);
        return -1;
    }

    // Request IRQ
    ret = request_irq(IRQ_NUM, irq_logger_handler, IRQF_SHARED, "irq_logger_device", (void *)(irq_logger_handler));
    if (ret) {
        pr_err("Failed to request IRQ %d\n", IRQ_NUM);
        cdev_del(&irq_cdev);
        device_destroy(irq_class, dev);
        class_destroy(irq_class);
        unregister_chrdev_region(dev, 1);
        return ret;
    }

    pr_info("IRQ Logger Driver Loaded. Major = %d\n", MAJOR(dev));
    return 0;
}

static void __exit irq_logger_exit(void)
{
    free_irq(IRQ_NUM, (void *)(irq_logger_handler));
    cdev_del(&irq_cdev);
    device_destroy(irq_class, dev);
    class_destroy(irq_class);
    unregister_chrdev_region(dev, 1);
    pr_info("IRQ Logger Driver Unloaded\n");
}

module_init(irq_logger_init);
module_exit(irq_logger_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Manvitha");
MODULE_DESCRIPTION("IRQ Logger Driver with Device Interface");
MODULE_VERSION("1.0");

