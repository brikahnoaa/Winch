// timer.c - interval timer
#include <common.h>
#include <timer.h>

// initialized to all zeros, that works
static struct {
  bool on[sizeof_tim];
  time_t timer[sizeof_tim];
} tim;
  
/*
 * sets: tim.on .timer[]
 */
void timStart(TimerType tmr, int secs) {
  DBG0("timStart()")
  tim.on[tmr] = true;
  tim.timer[tmr] = time(0)+secs;
} // timStart

void timStop(TimerType tmr) {
  DBG0("timStop()")
  tim.on[tmr] = false;
} // timStop

/*
 * is timer expired? turn it off
 * timer==now is not expired, so now+1 runs at least 1 sec 
 */
bool timExp(TimerType tmr) {
  if (tim.on[tmr] && (tim.timer[tmr] < time(0))) {
    // on and expired
    tim.on[tmr] = false;
    return true;
  } else {
    // off or still running
    return false;
  }
} // timExp

/*
 * how long until a timer expires (else 0=off, neg=expired)
 */
int timQuery(TimerType tmr) {
  if (tim.on[tmr]) 
    return (int)(tim.timer[tmr]-time(0))+1;
  else
    return 0;
} // timQuery

/*
 * check if any timer is expired
 */
TimerType timAnyExp(void) {
  for(TimerType t = null_tim+1; t<sizeof_tim; t++)
    if (timExp(t))
      return t;
  return null_tim;
} // timCheck
