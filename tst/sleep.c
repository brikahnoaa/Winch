// sleep
#include <utl.h>
#include <sys.h>
#include <mpc.h>
void main(void){
  cprintf("\n Low Power Sleep %s \n", utlDateTime());
  cdrain();
  utlSleep();
}
