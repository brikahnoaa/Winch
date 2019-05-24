
#include <main.h>
int main(){
  time_t now=time(NULL);

  printf("now %ld\n", now);
  
  printf("start data ctd \n");
  tmrStart(data_tmr, 3);
  tmrStart(ctd_tmr, 2);
  printf("timer ctd_tmr %d at tmrQuery %d\n", ctd_tmr, tmrQuery(ctd_tmr));
  printf("timer data_tmr %d at tmrQuery %d\n", data_tmr, tmrQuery(data_tmr));
  printf("timer wispr_tmr %d at tmrQuery %d\n", wispr_tmr, tmrQuery(wispr_tmr));
  printf("stop ctd\n");
  tmrStop(ctd_tmr);
  printf("timer ctd_tmr %d at tmrQuery %d\n", ctd_tmr, tmrQuery(ctd_tmr));
  printf("timer data_tmr %d at tmrQuery %d\n", data_tmr, tmrQuery(data_tmr));
  printf("timer wispr_tmr %d at tmrQuery %d\n", wispr_tmr, tmrQuery(wispr_tmr));
  printf("tmrCheck() loop...\n");
  TmrType i = null_tmr;
  while (i==null_tmr)
    i = tmrAnyExp();
  printf("timer %d at %ld\n", i, time(0));
  printf("timer data_tmr %d at tmrQuery %d\n", data_tmr, tmrQuery(data_tmr));
}
