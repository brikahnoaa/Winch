/*
 * wdt.h - blackfin watchdog timer
 *
 */

#ifndef _BFIN_WDT_H
#define _BFIN_WDT_H

extern int wdt_open(int timeout);
extern int wdt_keep_alive();
extern int wdt_close();

#endif

