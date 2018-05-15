/*
 *  log.h: 
 *
*/

#ifndef _LOG_H
#define _LOG_H

extern int log_open(char *path, char *name);
extern int log_open_append(char *path, char *name);
extern void log_close();
extern int log_printf(const char *format, ...);

#endif

