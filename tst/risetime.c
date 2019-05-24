// risetime.c - test next 2pm
#include <main.h>

char *utlTim(void);
///
// HH:MM:SS now
// returns: global static char *utl.ret
char *utlTim(void) {
  struct tm *tim;
  time_t secs;
  time(&secs);
  tim = gmtime(&secs);
  printf("%02d:%02d:%02d",
          tim->tm_hour, tim->tm_min, tim->tm_sec);
  return "";
} // utlTime

void main(){
  struct tm *tim;
  time_t secs;
  printf("done\n");
}
