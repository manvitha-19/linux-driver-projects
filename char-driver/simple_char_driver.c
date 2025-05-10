// SPDX-License-Identifier: GPL-2.0
/*
 * simple_char_driver.c - A basic Linux character device driver
 * 
 * Author: Kariveda Manvitha
 * GitHub: https://github.com/<your-github-username>/<your-repo-name>
 * License: GPL
 *
 * Description:
 * This is a simple Linux character driver that demonstrates basic
 * device file operations including open, read, write, and release.
 */

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include<linux/slab.h>                 
#include<linux/uaccess.h>              
#include <linux/err.h>

#define mem_size        1024  

dev_t dev = 0;
static struct class *dev_class;
static struct cdev simple_char_cdev;
uint8_t *kernel_buff;


static int __init simple_char_driver_init(void);
static void __exit simple_char_driver_exit(void);

static int simple_char_open(struct inode *inode, struct file *file); 
static int simple_char_release(struct inode *inode, struct file *file);
static ssize_t simple_char_read(struct file *filp, char __user *buf, size_t len, loff_t *off);
static ssize_t simple_char_write(struct file *filp, const char __user *buf, size_t len, loff_t *off);

static struct file_operations fops = {
	.owner = THIS_MODULE,
	.read = simple_char_read,
	.write = simple_char_write,
	.open = simple_char_open,
	.release = simple_char_release,
};

static int simple_char_open(struct inode *inode, struct file *file)
{
	pr_info("Device file opened\n");
	return 0;
}

static int simple_char_release(struct inode *inode, struct file *file)
{
        pr_info("Device file closed\n");
        return 0;
}
static ssize_t simple_char_read(struct file *filp, char __user *buf, size_t len, loff_t *off)
{

	if(*off >= mem_size)
	{
		return 0; //No more data
	}

	if( len > (mem_size - *off) )
	{
		len = mem_size - *off;
	}

	if ( copy_to_user (buf, kernel_buff + *off, len))
	{
		pr_err("Data read : error!\n");
		return -EFAULT;
	}
	pr_info("Data read : Done!\n");
	return len;
}

static ssize_t simple_char_write(struct file *filp, const char __user *buf, size_t len, loff_t *off)
{
	if( copy_from_user (kernel_buff, buf, len))
	{
		pr_err("Data write: error!\n");
	}
	pr_info("Data write: Done\n");
	return len;
}

static int __init simple_char_driver_init(void)
{
	if((alloc_chrdev_region(&dev, 0, 1, "simple_char_dev"))<0)
	{
		pr_info("Cannot allocate Major number\n");
		return -1;
	}
	pr_info("Major = %d, Minor = %d\n", MAJOR(dev), MINOR(dev));

	cdev_init(&simple_char_cdev, &fops);

	if((cdev_add(&simple_char_cdev, dev, 1)) < 0)
	{
		pr_info("Cannot add the device\n");
		goto r_class;
	}
	
	if(IS_ERR(dev_class = class_create("simple_char_class")))
	{
		pr_info("Cannot create the class\n");
		goto r_class;
	}
	if(IS_ERR(device_create(dev_class,NULL,dev,NULL,"simple_char_device"))){
            pr_info("Cannot create the Device\n");
            goto r_device;
        }
        
        if((kernel_buff = kmalloc(mem_size , GFP_KERNEL)) == 0){
            pr_info("Cannot allocate memory in kernel\n");
            goto r_device;
        }
	memset(kernel_buff, 0, mem_size);	
	strcpy(kernel_buff, "Hello_World");
        
        pr_info("Device Driver Insert...Done!!!\n");
        return 0;
 
	r_device:
        	class_destroy(dev_class);
	r_class:
        	unregister_chrdev_region(dev,1);

        return -1;
}
static void __exit simple_char_driver_exit(void)
{
 	kfree(kernel_buff);
        device_destroy(dev_class,dev);
        class_destroy(dev_class);
        cdev_del(&simple_char_cdev);
        unregister_chrdev_region(dev, 1);
        pr_info("Device Driver Remove...Done!!!\n");
}
 
module_init(simple_char_driver_init);
module_exit(simple_char_driver_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Kariveda Manvitha");
MODULE_DESCRIPTION("A simple Linux char device driver");
MODULE_VERSION("1.0");




