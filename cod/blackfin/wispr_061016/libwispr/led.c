/*
 *  led.c:  LED controls
 *
*/

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include "led.h"

int led_open(unsigned gpio)
{
   // open gpio lines for LED control
   gpio_export(gpio); 
   gpio_dir_out(gpio); 

   // turn OFF
   gpio_write(gpio, 0);

   return(1);
}

void led_set_on(unsigned gpio)
{
   gpio_write(gpio, 1);
};

void led_set_off(unsigned gpio)
{
   gpio_write(gpio, 0);
};

void led_close(unsigned gpio)
{
   // turn OFF
   gpio_write(gpio, 0);
   gpio_unexport(gpio);
};


