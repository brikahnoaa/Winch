// sleep
#include <main.h>
void main(void){
  cprintf("\n Low Power Sleep %s \n", utlDateTime());
  cdrain();
  utlSleep();
}
