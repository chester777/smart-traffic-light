#ifndef Ultrasonic_h
#define Ultrasonic_h

#include <wiringPi.h>

#define CM 1
#define INC 0


void Ultrasonic(int TP,int EP);
long Timing();
long Raging(int sys);
int pulseIn(int pin,int level, int timeout);


int Trig_pin;
int Echo_pin;
long duration,distance_cm,distance_inc;
#endif
