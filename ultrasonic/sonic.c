#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/cdev.h>
#include <linux/fs.h>

MODULE_LICENSE("GPL");


#include "Ultrasonic.h"
/*
int main(void)
{
	Ultrasonic(,);
	while(1)
	{
		printk("ultrasonic range : %ld \n",Ranging(CM));
		sleep(1000);
	}	
	return 0;
}
*/
static int sonar_open(int ch)
{
	printk("sonar open\n");
	return 0;
}
static int sonar_relrease(struct inode* inode,struct file* file)
{
	printk("sonar release\n");
	return 0;
}


static file_operations sonic_fops={
	.open=sonar_open,
	.release=sonar_release,
};

static int __init sonar_init(void)
{
	printk("Init sonar Module\n");
	Ultrasonic(Trig,Echo);
	while(1)
	{
		printk("ultrasonic range : %ld \n",Ranging(CM));
		sleep(1000);
	}	
	return 0;
}

static int __exit sonar_exit(void)
{
	printk("Exit sonar Module\n");

}

module_init(sonar_init);
module_exit(sonar_exit);
