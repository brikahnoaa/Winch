/*
*  wispr.h: 
*/
#ifndef _WISPR_H
#define _WISPR_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/mount.h>
#include <sys/statfs.h>
#include <sys/sysinfo.h>
#include <linux/watchdog.h>

#include "com.h"
#include "gpio.h"
#include "rtc.h"
#include "wdt.h"
#include "log.h"
#include "led.h"
#include "dat_file.h"
#include "wav_file.h"
#include "wav_test.h"
#include "sport_adc.h"

// default: date -d "Jan 01 00:00:00 GMT 2009" +%s
#define BFIN_DEFAULT_TIME 1230768000

#endif /* _WISPR_H */

