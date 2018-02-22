#include <stdio.h>
#include <time.h>

char *clockTimeDate(char *out);

char * clockTimeDate(char *out) {
  struct tm *t;
  time_t secs;

  secs = time(0);
  t = localtime(&secs);
  sprintf(out, "%02d/%02d/%d %02d:%02d:%02d", t->tm_mon,
          t->tm_mday, t->tm_year-100, t->tm_hour,
          t->tm_min, t->tm_sec);
  return out;
} // clockTimeDate

void main(void) {
  char scratch[128];
  printf( "%s\n", clockTimeDate(scratch));
}
