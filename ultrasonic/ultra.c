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
#include "sonic.h"
#include "sonic_ioctl.h"
MODULE_LICENSE("GPL");

static struct cdev* cd_cdev;
static dev_t dev_num;


int Trig_pin;
int Echo_pin;
int irq_num;
spinlock_t my_lock;

static struct timeval t0, t1;
int pulseDone;
DECLARE_WAIT_QUEUE_HEAD(waitqueue_t);

static struct gpio sonic_gpios[]={
	{TRIG,GPIOF_OUT_INIT_LOW , "TRIG"},
	{ECHO,GPIOF_IN , "ECHO"},
};


void init_ultrasonic(void)
{
	if(gpio_request_array(sonic_gpios,ARRAY_SIZE(sonic_gpios)))
		printk("gpio err\n");
	Trig_pin = TRIG;
	Echo_pin = ECHO;
}

//문제점 : micro 초 단위.
//while문 사용... -->그 조건 안 되면 잠재우는거 쓰자. 웨잇큐였나.
//wait event
int pulseIn(int pin, int level)
{
	long micros;
	//pulseDone=0;///last work
	do_gettimeofday(&t0);
	micros = 0;
	printk("wait Echo event\n");
	wait_event(waitqueue_t,pulseDone==1);
	spin_lock(&my_lock);
	pulseDone=0;
	spin_unlock(&my_lock);
	micros = (t1.tv_usec - t0.tv_usec);

	return micros;
}
static irqreturn_t sonar_isr(int irq,void* data){
//rising!
	do_gettimeofday(&t1);
	printk("FALLING Echo\n");
	spin_lock(&my_lock);
	pulseDone=1;
	spin_unlock(&my_lock);
	wake_up(&waitqueue_t);
	return IRQ_HANDLED;
}
/*
int pulseIn(int pin, int level, int timeout)
{
	struct timeval tn, t0, t1;
	long micros;
	do_gettimeofday(&t0);
	micros = 0;
	while (gpio_get_value(pin) != level)
	{
		printk("in the low level\n");
		do_gettimeofday(&tn);
		if (tn.tv_sec > t0.tv_sec) micros = 1000000L; else micros = 0;
		micros += (tn.tv_usec - t0.tv_usec);
		if (micros > timeout) return 0;
	}
	do_gettimeofday(&t1);
	while (gpio_get_value(pin) == level)
	{	
		printk("in the High level\n");
		do_gettimeofday(&tn);
		if (tn.tv_sec > t0.tv_sec) micros = 1000000L; else micros = 0;
		micros = micros + (tn.tv_usec - t0.tv_usec);

		if (micros > timeout) return 0;
	}

	if (tn.tv_sec > t1.tv_sec) micros = 1000000L; else micros = 0;
	micros = micros + (tn.tv_usec - t1.tv_usec);

	return micros;
}
*/
long Timing(void)
{
	//gpio_set_value(Trig_pin,LOW);
	//mdelay(2);
	gpio_set_value(Trig_pin,HIGH);
	mdelay(10);
	gpio_set_value(Trig_pin,LOW);
	return pulseIn(Echo_pin,HIGH);
}

long Ranging(int sys)
{
	long duration;
	long distance_cm, distance_inc;
	duration=Timing();
	distance_cm = duration /29 / 2;
	distance_inc = duration / 74/2;
	if(sys)
		return distance_cm;
	else
		return distance_inc;
}


static int sonic_open(struct inode* inode, struct file* file)
{
	printk("sonic open\n");
	return 0;
}
static int sonic_release(struct inode* inode, struct file* file)
{
	printk("sonic release\n");
	return 0;
}
static long sonic_ioctl(struct file* file, unsigned int cmd, unsigned long arg)
{

	switch(cmd){
		case IOCTL_NUM1:

			break;

		default:
			return -1;
	}
	return 0;
}

struct file_operations sonic_fops=
{
	.open	=	sonic_open,
	.release=	sonic_release,
	.unlocked_ioctl=sonic_ioctl,
};

static struct timer_list my_timer;
static void  my_timer_func(unsigned long data)
{
	my_timer.data = data+1;
	my_timer.expires = jiffies + (2*HZ);
	printk("distance : %ld cm\n",Ranging(CM));
	add_timer(&my_timer);
}
static int __init sonic_init(void)
{
	int ret;
	printk("Init Module\n");

	init_ultrasonic();
	alloc_chrdev_region(&dev_num,0,1,DEV_NAME);
	cd_cdev = cdev_alloc();
	cdev_init(cd_cdev,&sonic_fops);
	cdev_add(cd_cdev,dev_num,1);
	
	spin_lock_init(&my_lock);

	irq_num = gpio_to_irq(Echo_pin);
	ret=request_irq(irq_num,sonar_isr,IRQF_TRIGGER_FALLING,"sensor_irq",NULL);
	if(ret){
		printk(KERN_ERR "unable to request IRQ: %d\n", ret);
		free_irq(irq_num,NULL);
	}
	init_timer(&my_timer);
	my_timer.function = my_timer_func;
	my_timer.data = 1L;
	my_timer.expires=jiffies + (2*HZ);

	add_timer(&my_timer);
	return 0;	
}

static void __exit sonic_exit(void)
{
	printk("Exit Module\n");
	del_timer(&my_timer);
	gpio_free_array(sonic_gpios,ARRAY_SIZE(sonic_gpios));
	free_irq(irq_num,NULL);
	cdev_del(cd_cdev);
	unregister_chrdev_region(dev_num,1);
}

module_init(sonic_init);
module_exit(sonic_exit);
