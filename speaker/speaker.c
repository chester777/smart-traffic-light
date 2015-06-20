#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>

#include <linux/fs.h>
#include <linux/timer.h>
#include <linux/delay.h>
#include <linux/gpio.h>
#include <linux/cdev.h>
#include <linux/fs.h>

#include "speaker.h"
#include "speaker.ioctl.h"
MODULE_LICENSE("GPL");
static int speaker_pin;
static int BassTab[]={1911,1702,1516,1431,1275,1275,1136,1012};
static struct timer_list my_timer;
static struct cdev* cd_cdev;
static dev_t dev_num;
/*
static struct gpio sonic_gpios[]={
	{TRIG,GPIOF_IN , "TRIG"},
	{ECHO,GPIOF_INIT_LOW , "ECHO"},
};

 */

void init_speaker(void)
{
	speaker_pin = GPIO_PIN;
	if(gpio_request_one(speaker_pin,GPIOF_OUT_INIT_LOW,DEV_NAME))
		printk("gpio err\n");
}
void sound(int note_index)
{
	int i=0;
	for(i=0; i<50; i++){
		gpio_set_value(speaker_pin,HIGH);
		mdelay(BassTab[note_index]);
		gpio_set_value(speaker_pin,LOW);
		mdelay(BassTab[note_index]);
	}
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




static int speaker_open(struct inode* inode, struct file* file)
{
	printk("speaker open\n");
	return 0;
}
static int speaker_release(struct inode* inode, struct file* file)
{
	printk("speaker release\n");
	return 0;
}

struct file_operations speaker_fops=
{
	.open	=	speaker_open,
	.release=	speaker_release,
	.unlocked_ioctl	=	speaker_ioctl,
};

static void  my_timer_func(unsigned long data)
{
	int note_index=0;
	my_timer.data = data+1;
	my_timer.expires = jiffies + (2*HZ);
	for(note_index=0; note_index<7; note_index++)
	{
		sound(note_index);
		mdelay(500);
	}
	add_timer(&my_timer);
}

static int __init speaker_init(void)
{
	printk("Init Module\n");
	alloc_chrdev_region(&dev_num,0,1,DEV_NAME);
	cd_cdev = cdev_alloc();
	cdev_init(cd_cdev,&speaker_fops);
	cdev_add(cd_cdev,dev_num,1);

	init_speaker();
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
	gpio_free(speaker_pin);
	cdev_del(cd_cdev);
	unregister_chrdev_region(dev_num,1);
}

module_init(speaker_init);
module_exit(speaker_exit);
