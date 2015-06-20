
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>

#include <linux/fs.h>
#include <linux/timer.h>
#include <linux/delay.h>
#include <linux/gpio.h>
#include <linux/cdev.h>
#include <linux/fs.h>

#include "light.h"
#include "light_ioctl.h"
MODULE_LICENSE("GPL");
static int light_pin;
static int led_pin;
static struct timer_list my_timer;
static struct cdev* cd_cdev;
static dev_t dev_num;
/*
static struct gpio sonic_gpios[]={
	{TRIG,GPIOF_IN , "TRIG"},
	{ECHO,GPIOF_INIT_LOW , "ECHO"},
};

 */

void init_light(void)
{
	
	light_pin = GPIO_PIN;
	led_pin = 23;
	if(gpio_request_one(led_pin,GPIOF_OUT_INIT_LOW,DEV_NAME))
		printk("gpio err\n");
}

static long speaker_ioctl(struct file* file, unsigned int cmd, unsigned long arg)
{

	switch(cmd){
		case IOCTL_NUM1:

			break;

		default:
			return -1;
	}
	return 0;
}




static int light_open(struct inode* inode, struct file* file)
{
	printk("speaker open\n");
	return 0;
}
static int light_release(struct inode* inode, struct file* file)
{
	printk("speaker release\n");
	return 0;
}

struct file_operations speaker_fops=
{
	.open	=	light_open,
	.release=	lihgt_release,
	.unlocked_ioctl	=	lihgt_ioctl,
};

static void  my_timer_func(unsigned long data)
{
	int note_index=0;
	my_timer.data = data+1;
	my_timer.expires = jiffies + (2*HZ);



	add_timer(&my_timer);
}

static int __init speaker_init(void)
{
	printk("Init Module\n");
	alloc_chrdev_region(&dev_num,0,1,DEV_NAME);
	cd_cdev = cdev_alloc();
	cdev_init(cd_cdev,&speaker_fops);
	cdev_add(cd_cdev,dev_num,1);

	init_lihgt();
	init_timer(&my_timer);
	my_timer.function = my_timer_func;
	my_timer.data = 1L;
	my_timer.expires=jiffies + (2*HZ);

	add_timer(&my_timer);
	
	
	return 0;	
}

static void __exit speaker_exit(void)
{
	printk("Exit Module\n");
	gpio_free(light_pin);
	cdev_del(cd_cdev);
	unregister_chrdev_region(dev_num,1);
}

module_init(speaker_init);
module_exit(speaker_exit);
