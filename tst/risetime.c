// risetime.c - test next 2pm
#include <utl.h>
#include <mpc.h>
#include <pwr.h>
#include <sys.h>
#include <tmr.h>

char *utlTime(void);
char *utlDate(void);
///
// HH:MM:SS now
// returns: global static char *utl.ret
char *utlTime(void) {
  struct tm *tim;
  time_t secs;
  time(&secs);
  tim = gmtime(&secs);
  sprintf(utl.ret, "%02d:%02d:%02d",
          tim->tm_hour, tim->tm_min, tim->tm_sec);
  return utl.ret;
} // utlTime

///
// Date String // MM-DD-YY 
// returns: global static char *utl.ret
char *utlDate(void) {
  struct tm *tim;
  time_t secs;
  
  time(&secs);
  tim = gmtime(&secs);
  sprintf(utl.ret, "%02d-%02d-%02d", tim->tm_mon+1,
          tim->tm_mday, tim->tm_year - 100);
  return utl.ret;
} // utlDate

void main(){
  struct tm tim;
  time_t secs;
  printf("done\n");
}
