// timer.h interval timer

typedef enum {
  null_tim=0,
  ctd_tim,          // ctd.pending waiting for ctd response
  deploy_tim,       // steps during deploy phase0
  data_tim,         // rise to surface, send data
  rudics_tim,       // max time to spend on the phone
  winch_tim,        // win.pending amodem command response
  wispr_tim,        // wispr duty cycle
  sizeof_tim,
  } TimerType;

time_t timQuery(TimerType timT);
void timStart(TimerType timT, int secs);
void timStop(TimerType timT);
TimerType timCheck(void);

