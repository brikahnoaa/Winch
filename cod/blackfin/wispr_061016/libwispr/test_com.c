/*
 * test_com.c
 * ------
 */

#include <stdio.h>
#include <stdio.h>
#include <getopt.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>
#include <sys/syscall.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <linux/rtc.h>
#include <sys/wait.h>
#include <sys/mount.h>
#include <assert.h>
#include <pthread.h>
#include <fract.h>
#include <filter.h>
#include <stats.h>
#include <math.h>
#include <cycle_count.h>
#include <time.h>
#include <sys/mman.h>
#include <linux/watchdog.h>
#include <sys/statfs.h>

#include "com.h"
#include "gpio.h"

char *progname;
int verbose_level = 0;

int main(int argc, char **argv)
{
   char *progname;

   progname = argv[0];
   
   // COM port params
   char tty_device[32] = "/dev/ttyBF1";
   com_t com;
   char com_msg[COM_MSGSIZE];

   // open com port
   //com.gps = &gps;
   if(com_open (tty_device, B9600) < 0) {
      //perror ("com_open");
      fprintf(stdout, "%s: error opening tty device\n", progname);
	  return(0);
   }

   // initial state
   int state = COM_RUN;
   int prev_state = COM_RUN;
   int mode = 0;
   com.state = state;
   com.mode = mode;

   // loop over buffers
   while(state != COM_EXIT) {

      // check for com actions
      int nrd = com_read_msg (com_msg, COM_MSGSIZE);

      // parse msg, if available
	  // blink led 1 to show com activity
      if (nrd > 0) {
         
         state = com_parse_msg (com_msg, nrd, &com);
         mode = com.mode;

         if((state == COM_PAUSE) & (prev_state == COM_RUN)) {
            fprintf(stdout,"%s: paused\n", progname);
         }

		 if((state == COM_RUN) & (prev_state != COM_RUN)) {
            fprintf(stdout,"%s: run\n", progname);
         }

		 if(verbose_level) {
            fprintf(stdout,"state=0x%x, mode=0x%x", state, mode);
         }
         prev_state = state;

	  }


      // if paused, continue to read adc, but don't do anything with the data
      if(state == COM_PAUSE) {
         sleep(1);
         continue;
      }
      if(state == COM_EXIT) break;

      fprintf(stdout,"%s: running\n", progname);
	  sleep(1);

   }

   // clean up the rest
   com_close();


   return 0;
}
