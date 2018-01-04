// timer.c - interval timer
#include <common.h>
#include <timer.h>

// initialized to all zeros
static struct {
  TimerType next;
  time_t nextWhen;
  time_t timers[sizeof_tim];
} tim;

static void timNext(void);

/*
 * find next timer expiration
 * sets: tim.next*
 */
static void timNext(void) {
  tim.next = null_tim;
  tim.nextWhen = (time_t) 0;
  for (int i=null_tim; i<sizeof_tim; i++)  {
    // if timer is active
    if (tim.timers[i])  {
      // if .next is null, or this timer runs out sooner
      if ((tim.next==null_tim) ||
          (tim.nextWhen>tim.timers[i])) {
        // replace tim.next
        tim.next = i;
        tim.nextWhen = tim.timers[i];
        // test
        printf("tim.next = %d at %ld\n", tim.next, tim.nextWhen);
      }
    }
  }
} // timNext
  
/*
 * activate an interval timer, or reset timer if active
 * check time against nextWhen
 */
void timStart(TimerType timT, int secs) {
  debug0("timStart()")
  time_t when = time(0)+secs;
  tim.timers[timT] = when;
  if (tim.nextWhen>when) timNext();
} // timStart

/*
 * sets: tim.timers tim.next*
 */
void timStop(TimerType timT) {
  debug0("timStop()")
  tim.timers[timT] = 0;
  if (tim.next==timT) timNext();
} // timStop

/*
 * check if timer is expired
 * sets: tim.*
 */
TimerType timCheck(void) {
  debug0("timCheck()")
  TimerType r = null_tim;
  // .next is set and before now
  if ((tim.next!=null_tim) &&
      (tim.nextWhen<time(0))) {
    r = tim.next;
    timStop(tim.next);
  }
  return r;
} // timCheck

/*
 * how long until next timer expires
 */
int timQuery(TimerType timT) {
  return tim.timers[timT]-time(0);
} // timQuery

