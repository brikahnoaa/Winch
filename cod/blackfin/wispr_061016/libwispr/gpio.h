/*
 * GPIO user space helpers
 *
 * Licensed under the GPL-2 or later
 */

#ifndef _BF_GPIO_H
#define _BF_GPIO_H

/* 
 look at include/asm/gpio.h for gpio numbers corresponding to pf/pg numbers
*/
#define GPIO_PF0 0
#define GPIO_PF1 1
#define GPIO_PF2 2
#define GPIO_PF3 3
#define GPIO_PF4 4
#define GPIO_PF5 5
#define GPIO_PF6 6
#define GPIO_PF7 7
#define GPIO_PF8 8
#define GPIO_PF9 9
#define GPIO_PF10 10
#define GPIO_PF11 11
#define GPIO_PF12 12
#define GPIO_PF13 13
#define GPIO_PF14 14
#define GPIO_PF15 15

/* PG is only on the BF537 */
#define GPIO_PG0 16
#define GPIO_PG1 17
#define GPIO_PG2 18
#define GPIO_PG3 19
#define GPIO_PG4 20
#define GPIO_PG5 21
#define GPIO_PG6 22
#define GPIO_PG7 23
#define GPIO_PG8 24
#define GPIO_PG9 25
#define GPIO_PG10 26
#define GPIO_PG11 27
#define GPIO_PG12 28
#define GPIO_PG13 29
#define GPIO_PG14 30
#define GPIO_PG15 31

#define GPIO_DIR_IN     0
#define GPIO_DIR_OUT    1

int gpio_export(unsigned gpio);
int gpio_unexport(unsigned gpio);
int gpio_dir_out(unsigned gpio);
int gpio_dir_in(unsigned gpio);
int gpio_write(unsigned gpio, unsigned value);

#endif /* _BF_GPIO_H */
