#include <unitTest.h>
#include <event.c>

int main(){
  Event e = WinchStop_ev;
  time_t now=time(NULL);
  time_t then=now+2;

  printf("now %ld\n", now);
  printf("then %ld\n", then);
  eventInit();
  evDump();
  evSet(e, 2);
  evDump();
  printf("evCheck() -> %d \n", evCheck());
  printf("evCheck() -> %d \n", evCheck());
  while (time(NULL) < (now+4)) {}
  evDump();
  printf("evCheck() -> %d \n", evCheck());
  printf("evCheck() -> %d \n", evCheck());
}

