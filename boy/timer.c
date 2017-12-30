// timer.c - interval timer
#include <timer.h>
#include <utils.h>

/*
 * interval timer, abstracted for general purpose
 * fast check for next timeout, so low overhead
 * 
 * null_tim
 * sink_tim - checking depth as we sink
 * settle_tim - let the mooring settle
 * winch_tim - expect response from winch
 * wispr_tim - detection duty cycle
 * rest_tim - no detection, sleep
 * raise_tim - go up for to phone home
 * drop_tim - enough talk, hang up and go down
 */

typedef struct Timer {
  time_t when;
  TimerType timT;
  struct Timer* next, prev;
} Timer;

// head is an empty node, never removed from list, only head.next is used
Timer emptyNode = {(time_t)0, null_tim, NULL, NULL};

// tim.head->next is first list item
static struct {
  time_t nextWhen;
  int nextTim;
  Timer* head;
} tim = {(time_t)0, (Timer*)NULL, &emptyNode};


/*
 * add an interval timer to list
 * check time against nextWhen
 */
void timAdd(TimerType timT, int secs) {
  debug0("timAdd()")
  // global tim
  Timer* new = malloc(sizeof(Timer));
  // set values, insert node after head
  new.when = time(0)+secs;
  new.timT = timT;
  new.next = tim.head->next;
  new.prev = tim.head;
  tim.head->next = new;
  if (new.when < tim.nextWhen) {
    tim.nextTim = new;
    tim.nextWhen = new.when;
  }
} // timAdd

/*
 * remove and free node
 * set: tim.next*
 */
static void timRemove(Timer* timer) {
  debug0("timRemove()")
  // global tim
  if (timer->next) timer->next->prev = timer->prev;
  timer->prev->next = timer->next;
  free(timer);
  // 
  if (tim.head->next==NULL) {
    // empty list
    tim.nextTim = NULL;
    tim.nextWhen = (time_t)0;
  }
  // if timer was nextTim, find new next
  if (tim.nextTim==timer) {
    // start with top of list
    tim.nextTim = tim.head->next;
    // not null
    if (tim.nextTim)
      tim.nextWhen = tim.head->next->when;
    // check against rest of list
    for (Timer* i=tim.head->next->next; i!=NULL; i=i->next) {
      if (i->when < tim.nextWhen) {
        tim.nextTim = i;
        tim.nextWhen = i->when;
      } // if
    } // for
  } // if
} // timRemove

/*
 * remove all timers of a type, e.g. detection timers
 */
void timCancel(TimerType timT) {
  // global tim
  for (Timer* i=tim.head->next; i!=NULL; i=i->next) 
    if (i->timT==timT) 
      timRemove(i);
}

TimerType timCheck(void) {
  debug0("timCheck()")
  // global tim
  if (!tim.nextTim || (tim.nextWhen>time(0)) return null_tim;
  // timers elapsed
  int r = tim.nextTim->timT;
  // timRemove sets tim.next*
  timRemove(tim.nextTim);
  return r;
} // timCheck

time_t timQuery(TimerType timT) {
  // global tim
  time_t r=0;
  if (!tim.nextTim) return r;
  // tim.head->next is first list item
  for (Timer* i=tim.head->next; i!=NULL; i=i->next) {
    if (i->timT==timT)
      if (!r || (r>i->when))
        r = i->when;
  }
  return r;
}

