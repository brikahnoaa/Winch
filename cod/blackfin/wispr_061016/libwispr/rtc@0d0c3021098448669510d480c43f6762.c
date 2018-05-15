/*
 * rtc.c - interface to the bfin Real Time Clock
 *
 * ------
 * chris jones
 */

#include <stdio.h>
#include <getopt.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>

#include <sys/ioctl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <linux/rtc.h>
#include <linux/ioctl.h>

#include "rtc.h"
#include "log.h"

extern int verbose_level;

static char tmstr[32];
static struct tm sys;

/*
*  get rtc time
*/
time_t rtc_get_time(char *rtc_devname)
{
  int fd;
  time_t sec;
  struct rtc_time rtc;

  /* open rtc device */
  if ((fd = open (rtc_devname, O_RDONLY)) < 0) {
    perror (rtc_devname);
    return (-1);
  }

  // get rtc time
  if (ioctl (fd, RTC_RD_TIME, &rtc) < 0) {
    perror ("read rtc time");
    return (-1);
  }
  close (fd);

  sys.tm_sec = rtc.tm_sec;
  sys.tm_min = rtc.tm_min;
  sys.tm_hour = rtc.tm_hour;
  sys.tm_mday = rtc.tm_mday;
  sys.tm_mon = rtc.tm_mon;
  sys.tm_year = rtc.tm_year;
  sys.tm_wday = rtc.tm_wday;
  sys.tm_yday = rtc.tm_yday;

  sec = mktime(&sys);

  if (verbose_level > 0) {
     asctime_r(&sys,tmstr);
     log_printf( "rtc_get_time = %s", tmstr);
     //log_printf( "rtc_get_time = %d", rtc.tm_sec);
  }
  return (sec);
}

/*
 * set rtc time in seconds (unix time)
*/
time_t rtc_set_time (char *rtc_devname, time_t sec)
{
  int fd;
  time_t sys_t;
  //time_t rtc_t;
  struct rtc_time rtc;

  /* open rtc device */
  if ((fd = open(rtc_devname, O_RDONLY)) < 0) {
    perror(rtc_devname);
    return(-1);
  }

  /* record offset of mktime(), so we can reverse it */
  //memset(&sys, 0, sizeof(sys));
  //sys.tm_year = 70;
  //off_t = mktime(&sys);

  // get system time
  sys_t = sec;
  gmtime_r(&sys_t, &sys);

  // convert sys time to rtc time struct
  rtc.tm_sec =  sys.tm_sec;
  rtc.tm_min =  sys.tm_min;
  rtc.tm_hour = sys.tm_hour;
  rtc.tm_mday = sys.tm_mday;
  rtc.tm_mon =  sys.tm_mon;
  rtc.tm_year = sys.tm_year;
  rtc.tm_wday = sys.tm_wday;
  rtc.tm_yday = sys.tm_yday;

  // set rtc time
  if (ioctl (fd, RTC_SET_TIME, &rtc) < 0) {
    perror ("read rtc time");
    return (-1);
  }
  close (fd);

  if (verbose_level) {
    asctime_r(&sys,tmstr);
    log_printf( "rtc_set_time: %s", tmstr);
  }
  return (sec);
}

/* 
 * set rtc alarm
 */
time_t rtc_set_alarm (char *rtc_devname, time_t sec)
{
  int fd;
  time_t rtc_sec, wake_sec;
  struct rtc_wkalrm wake;
  struct tm tmp;

  //if (verbose_level) fprintf(stdout, " Wakeup in %hd seconds.\n", sec);

  /* record offset of mktime(), so we can reverse it */
  //memset(&sys, 0, sizeof(sys));
  //sys.tm_year = 70;
  //off_t = mktime(&sys);

  // read rtc time
  rtc_sec = rtc_get_time(rtc_devname);

  // add sleep duaration to rtc time in seconds
  wake_sec = rtc_sec + sec + 1;

  // convert wakeup time in seconds to rtc time struct
  gmtime_r(&wake_sec, &tmp);
  wake.time.tm_sec =   tmp.tm_sec;
  wake.time.tm_min =   tmp.tm_min;
  wake.time.tm_hour =  tmp.tm_hour;
  wake.time.tm_mday =  tmp.tm_mday;
  wake.time.tm_mon =   tmp.tm_mon;
  wake.time.tm_year =  tmp.tm_year;
  wake.time.tm_wday =  tmp.tm_wday;
  wake.time.tm_yday =  tmp.tm_yday;
  wake.time.tm_isdst = tmp.tm_isdst;

  /* open rtc device */
  if ((fd = open(rtc_devname, O_RDONLY)) < 0) {
    perror(rtc_devname);
    return(-1);
  }

  // set rtc alarm 
  wake.enabled = 1;
  if (ioctl (fd, RTC_WKALM_SET, &wake) < 0) {
    perror ("set rtc wakeup alarm");
    return (-1);
  }

  close (fd);

  // wait a little bit before going on
  //usleep (100000);

  if (verbose_level > 0) {
    ctime_r(&wake_sec, tmstr);
    log_printf( "rtc_set_alarm: wakeup at %s", tmstr);
  }

  return(wake_sec);
}

/*
*  set system time using the rtc clk
*/
time_t rtc_sync_sys(char *rtc_devname)
{
  time_t sec;
  //time_t rtc_t;
  //struct rtc_time rtc;
  struct timeval tv;
  struct timezone tz;
  char tmstr[32];

  // get rtc time
  sec = rtc_get_time(rtc_devname);

  tv.tv_sec = sec;
  tv.tv_usec = 0;
  tz.tz_minuteswest = 0;
  tz.tz_dsttime = 0;

  // set system time
  if (settimeofday (&tv, &tz) < 0) {
    perror ("settimeofday");
    return (-1);
  }

  // get system time
  sec = time(0);
  gmtime_r(&sec, &sys);

  if (verbose_level) { 
    asctime_r(&sys,tmstr);
    log_printf( "rtc_sync_sys: %s", tmstr);
  }
  return (sec);
}


//---------------------------------------------------------
// system time functions
time_t set_system_time(time_t sec, suseconds_t usec)
{
  struct timeval tv;
  tv.tv_sec = sec;
  tv.tv_usec = usec;
  settimeofday(&tv, NULL);
  return (tv.tv_sec);
}

time_t get_system_time(time_t *sec, suseconds_t *usec)
{
	struct timeval tp;
	gettimeofday(&tp, NULL);
	*usec = tp.tv_usec;
	*sec = tp.tv_sec;
	//fprintf(stdout,"get_system_time: %ld.%06ld\n",tp.tv_sec,tp.tv_usec);
	return(tp.tv_sec);
}

double time_to_double(time_t sec, suseconds_t usec)
{
	double sec_d;
	sec_d = (double)(sec) + (double)(usec)*0.000001;
	return(sec_d);
}


