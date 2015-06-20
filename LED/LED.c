#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>

#include <linux/fs.h>
#include <linux/gpio.h>
#include <linux/timer.h>
#include <linux/delay.h>
#include <linux/timekeeping.h>
#include <linux/spinlock.h>

#include <linux/wait.h>
#include <linux/sched.h>
#include <linux/interrupt.h>
#include <linux/cdev.h>
#include "LED.h"
#include "LED_ioctl.h"
MODULE_LICENSE("GPL");

static struct cdev* cd_cdev;
static dev_t dev_num;


int button_pin;
int red_pin;
int yellow_pin;
int green_pin;

int irq_num;
spinlock_t my_lock;

//int pulseDone;
//DECLARE_WAIT_QUEUE_HEAD(waitqueue_t);

static struct gpio led_gpios[]={
	{RED,GPIOF_OUT_INIT_LOW , "RED"},
	{YELLOW,GPIOF_OUT_INIT_LOW , "YELLOW"},
	{GREEN,GPIOF_OUT_INIT_LOW , "GREEN"},
};


void init_led(void)
{
	if(gpio_request_array(led_gpios,ARRAY_SIZE(led_gpios)))
		printk("gpio err\n");
	red_pin = RED;
	yellow_pin = YELLOW;
	green_pin = GREEN;
}
static irqreturn_t button_isr(int irq,void* data){
//rising!
	printk("button pressed!\n");
	return IRQ_HANDLED;
}
void init_button(void)
{
	int ret;
	button_pin = BUTTON;
	if(gpio_request_one(button_pin,GPIOF_IN,"BUTTON"))
		printk("button gpio err\n");
	irq_num = gpio_to_irq(button_pin);
	ret=request_irq(irq_num,button_isr,IRQF_TRIGGER_RISING,"button_irq",NULL);
	if(ret){
		printk(KERN_ERR "unable to request IRQ: %d\n", ret);
		free_irq(irq_num,NULL);
	}
}

static int led_open(struct inode* inode, struct file* file)
{
	printk("led open\n");
	return 0;
}
static int led_release(struct inode* inode, struct file* file)
{
	printk("led release\n");
	return 0;
}
static long led_ioctl(struct file* file, unsigned int cmd, unsigned long arg)
{

	switch(cmd){
		case IOCTL_NUM1:

			break;

		default:
			return -1;
	}
	return 0;
}

struct file_operations led_fops=
{
	.open	=	led_open,
	.release=	led_release,
	.unlocked_ioctl=led_ioctl,
};

static struct timer_list my_timer;
static void rule_one(unsigned long data)
{
	my_timer.data = data+1;
	my_timer.expires = jiffies + (7*HZ);
	add_timer(&my_timer);
	POWER_ON(red_pin);
	mdelay(4000);	
	POWER_OFF(red_pin);
	POWER_ON(green_pin);
	mdelay(2000);
	POWER_OFF(green_pin);
}
static void rule_two(unsigned long data)
{
	my_timer.data = data+1;
	my_timer.expires = jiffies + (2*HZ);
	add_timer(&my_timer);
}
static int __init led_init(void)
{
	printk("Init Module\n");

	init_led();
	init_button();
	alloc_chrdev_region(&dev_num,0,1,DEV_NAME);
	cd_cdev = cdev_alloc();
	cdev_init(cd_cdev,&led_fops);
	cdev_add(cd_cdev,dev_num,1);
	
//	spin_lock_init(&my_lock);

	init_timer(&my_timer);
	my_timer.function = rule_one;
	my_timer.data = 1L;
	my_timer.expires=jiffies + (2*HZ);

	add_timer(&my_timer);
	return 0;	
}

static void __exit led_exit(void)
{
	printk("Exit Module\n");
	del_timer(&my_timer);
	gpio_free_array(led_gpios,ARRAY_SIZE(led_gpios));
	free_irq(irq_num,NULL);
	cdev_del(cd_cdev);
	unregister_chrdev_region(dev_num,1);
}

module_init(led_init);
module_exit(led_exit);
