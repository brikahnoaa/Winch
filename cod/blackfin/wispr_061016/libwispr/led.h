/*
 *  led.h:  LED controls
 *
*/

#ifndef _LED_H
#define _LED_H

#include "gpio.h"


//---------------------------------------------------------------
int led_open(unsigned gpio);
void led_set_on(unsigned gpio);
void led_set_off(unsigned gpio);
void led_close(unsigned gpio);

#endif

