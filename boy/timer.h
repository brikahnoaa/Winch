// timer.h interval timer

typedef enum {
  null_tim=0,
  sink_tim,
  settle_tim,
  winch_tim,
  wispr_tim,
  rest_tim,
  raise_tim,
  drop_tim,
  } TimerType;

void timAdd(TimerType timT, int secs);
TimerType timCheck(void);
void timCancel(TimerType timT);
time_t timQuery(TimerType timT);
static void timRemove(Timer* timer);

