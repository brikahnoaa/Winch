/*
 * rtc.h: interface to the bfin RTC
 *
 */
#ifndef _RTC_H
#define _RTC_H

// default: date -d "Jan 01 00:00:00 GMT 2009" +%s
#define BFIN_DEFAULT_TIME 1230768000

extern time_t rtc_set_alarm(char *rtc_devname, time_t sec);
extern time_t rtc_get_time(char *rtc_devname);
extern time_t rtc_sync_sys(char *rtc_devname);
extern time_t rtc_set_time(char *rtc_devname, time_t sec);

extern time_t set_system_time(time_t sec, suseconds_t usec);
extern time_t get_system_time(time_t *sec, suseconds_t *usec);
extern double time_to_double(time_t sec, suseconds_t usec);

#endif
