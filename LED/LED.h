#define DEV_NAME "LED"

#define HIGH	1
#define LOW	0

#define POWER_ON(n)	gpio_set_value(n,HIGH)
#define POWER_OFF(n)	gpio_set_value(n,LOW)

#define BUTTON 26
#define	RED	17
#define YELLOW	27
#define GREEN	2

