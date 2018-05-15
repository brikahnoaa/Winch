/*
 * wdt.c - blackfin watchdog timer
 * ------
 * chris jones, nov 2010
 */

#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <linux/ioctl.h>
#include <linux/watchdog.h>

#include "log.h"

extern int verbose_level;

// setup watchdog timer
static int wdt_fd = 0; // watchdog file descriptor
static int wdt_timeout = 90; // one minute
static char wdt_magic = 'V';

// close watchdog
int wdt_close()
{
   if (wdt_fd) {
      write(wdt_fd, &wdt_magic, 1);
      usleep(1000);
      write(wdt_fd, &wdt_magic, 1);
      //fprintf(wdt_fp, "V\n"); // disable nowayout
      close(wdt_fd);
   }
   return(1);
}

// open watchdog
int wdt_open(int timeout)
{
   if(timeout > 0) wdt_timeout = timeout;

   // open watchdog timer
   wdt_fd = open("/dev/watchdog", O_WRONLY);
   if(wdt_fd < 0) {
      log_printf("wdt_open: Error openning watchdog timer\n");
      perror ("open: ");
      return(-1);
   }
   // set wdt timeout
   if (ioctl (wdt_fd, WDIOC_SETTIMEOUT, &wdt_timeout) < 0) {
      perror ("ioctl: ");
      wdt_close(wdt_fd);
      return (-1);
   }

   //if(debug_level)
     log_printf("wdt_open: watchdog set with timeout=%d seconds\n", wdt_timeout);

   return(wdt_fd);

}

// keep watchdog alive
int wdt_keep_alive()
{
   if(wdt_fd) {
      ioctl(wdt_fd, WDIOC_KEEPALIVE, 0);
   }
   return(1);
}


