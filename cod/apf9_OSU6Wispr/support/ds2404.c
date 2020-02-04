/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * $Id: ds2404.c,v 1.1 2005/02/22 21:58:37 swift Exp $
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * RCS Log:
 *
 * $Log: ds2404.c,v $
 * Revision 1.1  2005/02/22 21:58:37  swift
 * Stub to fake RTC functionality.
 *
 * Revision 1.1  2005/02/20 22:20:22  swift
 * Initial revision
 *
*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#include <time.h>

time_t ialarm(void);
time_t itimer(void);
time_t ialarm(void) {return 0;}
time_t itimer(void) {return 0;}
