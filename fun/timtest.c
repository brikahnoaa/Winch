#include <timer.c>

int main(){
  time_t now=time(NULL);
  time_t then=now+2;

  printf("now %ld\n", now);
  
  printf("start data ctd \n");
  timStart(data_tim, 3);
  timStart(ctd_tim, 2);
  printf("timer ctd_tim %d at timQuery %ld\n", ctd_tim, timQuery(ctd_tim));
  printf("timer data_tim %d at timQuery %ld\n", data_tim, timQuery(data_tim));
  printf("timer wispr_tim %d at timQuery %ld\n", wispr_tim, timQuery(wispr_tim));
  printf("stop ctd\n");
  timStop(ctd_tim);
  printf("timer ctd_tim %d at timQuery %ld\n", ctd_tim, timQuery(ctd_tim));
  printf("timer data_tim %d at timQuery %ld\n", data_tim, timQuery(data_tim));
  printf("timer wispr_tim %d at timQuery %ld\n", wispr_tim, timQuery(wispr_tim));
  printf("timCheck() loop...\n");
  TimerType i = null_tim;
  while (i==null_tim)
    i = timCheck();
  printf("timer %d at %ld\n", i, time(0));
  printf("timer data_tim %d at timQuery %ld\n", data_tim, timQuery(data_tim));
}
