#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>

#include <linux/fs.h>
#include <linux/delay.h>
#include <linux/timer.h>
#include <linux/gpio.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/fs.h>
#include <linux/interrupt.h>
#include "light.h"
#define AFTER_SUNSET	2
//#include "light_ioctl.h"
MODULE_LICENSE("GPL");
static int light_pin;
//static int led_pin;
static struct cdev* cd_cdev;
static dev_t dev_num;
/*
static struct gpio sonic_gpios[]={
	{TRIG,GPIOF_IN , "TRIG"},
	{ECHO,GPIOF_INIT_LOW , "ECHO"},
};

 */
int irq_num;
static struct timer_list my_timer;
static void my_timer_func(unsigned long data)
{
	//my_timer.data = data+1;
	//my_timer.expires = jiffies + (AFTER_SUNSET*HZ);
	printk("After sunset setting\n");
	printk("Send signal\n");
	///app에 signal 보내기
	if(gpio_get_value(light_pin)==0)
	{
		printk("light off\n");
	}else
		printk("light on\n");
	add_timer(&my_timer);
}
static irqreturn_t light_isr(int irq,void* data){
	printk("irq called\n");
	return IRQ_HANDLED;
}
void init_light(void)
{
	printk("init_light\n");	
	light_pin = LIGHT_PIN;
	if(gpio_request_one(light_pin,GPIOF_IN,DEV_NAME))
		printk("gpio err\n");
	irq_num = gpio_to_irq(LIGHT_PIN);
}
/*
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
*/



static int light_open(struct inode* inode, struct file* file)
{
	printk("light open\n");
	return 0;
}
static int light_release(struct inode* inode, struct file* file)
{
	printk("light release\n");
	return 0;
}

ssize_t (*read) (struct file *, char __user *, size_t, loff_t *);
static ssize_t light_read(struct file* fd, char* buf, int size,loff_t* f_pos)
{
	int cur_val;
	cur_val = gpio_get_value(light_pin);
	return copy_to_user((void*)buf,(void*)cur_val,sizeof(cur_val));
	//no 0 ->err
}
struct file_operations light_fops=
{
	.open	=	light_open,
	.read	=	light_read,
	.release=	light_release,
//	.unlocked_ioctl	=	lihgt_ioctl,
};


static int __init light_init(void)
{
	int ret;
	printk("Init Module\n");
	alloc_chrdev_region(&dev_num,0,1,DEV_NAME);
	cd_cdev = cdev_alloc();
	cdev_init(cd_cdev,&light_fops);
	cdev_add(cd_cdev,dev_num,1);

	init_light();
	printk("req_irq\n");
	ret = request_irq(irq_num,light_isr,IRQF_TRIGGER_FALLING,"light_sensor_irq",NULL);
	if(ret){
		printk(KERN_ERR "unalbe to request IRQ: %d\n",ret);
		free_irq(irq_num,NULL);
	}
	init_timer(&my_timer);
	my_timer.function = my_timer_func;
	my_timer.data = 1L;
	my_timer.expires=jiffies + (2*HZ);
	add_timer(&my_timer);
	
	return 0;	
}

static void __exit light_exit(void)
{
	printk("Exit Module\n");
	del_timer(&my_timer);
	free_irq(irq_num,NULL);
	gpio_free(light_pin);
	cdev_del(cd_cdev);
	unregister_chrdev_region(dev_num,1);
}

module_init(light_init);
module_exit(light_exit);
