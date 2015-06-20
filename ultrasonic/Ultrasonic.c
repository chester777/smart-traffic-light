#include "Ultrasonic.h"
#include <sys/time.h>
void Ultrasonic(int TP,int EP)
{
	pinMode(TP,OUTPUT);
	pinMode(EP,INPUT);
	Trig_pin=TP;
	Echo_pin=EP;
}

long Timing(void)
{
	digitalWrite(Trig_pin,LOW);
	delayMicroseconds(2);
	digitalWrite(Trig_pin,HIGH);
	delayMicroseconds(10);
	digitalWrite(Trig_pin,LOW);
	duration = pulseIn(Echo_pin,HIGH,10000L);
	return duration;
}

long Ranging(int sys)
{
	Timing();
	distance_cm = duration /29 / 2;
	distance_inc = duration / 74 / 2;

	if(sys)
		return distance_cm;
	else
		return distance_inc;
}
int pulseIn(int pin, int level, int timeout)
{
	struct timeval tn, t0, t1;

	long micros;

	gettimeofday(&t0, 0);

	micros = 0;

	while (digitalRead(pin) != level)
	{
		gettimeofday(&tn, 0);

		if (tn.tv_sec > t0.tv_sec) micros = 1000000L; else micros = 0;
		micros += (tn.tv_usec - t0.tv_usec);

		if (micros > timeout) return 0;
	}

	gettimeofday(&t1, 0);

	while (digitalRead(pin) == level)
	{
		gettimeofday(&tn, 0);

		if (tn.tv_sec > t0.tv_sec) micros = 1000000L; else micros = 0;
		micros = micros + (tn.tv_usec - t0.tv_usec);

		if (micros > timeout) return 0;
	}

	if (tn.tv_sec > t1.tv_sec) micros = 1000000L; else micros = 0;
	micros = micros + (tn.tv_usec - t1.tv_usec);

	return micros;
}
