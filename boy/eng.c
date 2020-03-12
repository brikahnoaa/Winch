// eng.c - engineering data
#include <main.h>

EngInfo eng;

int engInit(void) {
  return 0;
} // engInit

///
// open or reopen log file
int engLogOpen(void) {
  int r=0;
  if (!eng.log)
    r = utlLogOpen(&eng.log, "eng");
  return r;
} // engLogOpen

///
///
// open or reopen log file
int engLogClose(void) {
  int r=0;
  if (!eng.log)
    r = utlLogClose(&eng.log);
  return r;
} // engLogClose


