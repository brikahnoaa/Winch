
#include <main.h>
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

#define waitHr 20

void main(void) {
  int i=0, mnt=5;
  tmrStart(day_tmr, waitHr*60*60);
  while (!tmrExp(day_tmr)) {
    printf( " %d, %ld", i, tmrQuery(day_tmr));
    utlNap( 60*mnt);
    i += mnt;
}
