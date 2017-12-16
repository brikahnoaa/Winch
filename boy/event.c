// event.c - interval timer
#include <event.h>
#ifndef UnitTest
#include <utils.h>
#endif

/*
 * ev.timers[Event] tell us when a timer runs out
 * defaults to forever, a distant future very large time_t
 */

static struct {
  Event nextEv;
  time_t nextWhen;
  time_t forever;
  time_t timers[Sizeof_ev];
} ev;

/*
 * sets ev.*
 */
void eventInit() {
  debug0("eventInit()")
  // shift a bit up near the end of time (which may be signed)
  ev.forever = ((time_t)1 << (sizeof(time_t)*8)-2);
  ev.nextEv = Null_ev;
  ev.nextWhen = ev.forever;
  for (Event i=Null_ev; i<Sizeof_ev; i++) 
    ev.timers[i] = ev.forever;
} // eventInit

/*
 * interval timer, abstracted for general purpose
 * sets expired ev.timer
 * returns expired ev.timer
 */
Event evCheck(void) {
  if (ev.nextWhen>time(NULL)) 
    return Null_ev;
  else {
    // interval elapsed
    Event r = ev.nextEv;
    evClear(r); // changes ev.nextEv
    return r;
  }
} // evCheck

/*
 * activate an interval timer 
 */
void evDelay(Event event, int secs) {
  evSetAt(event, time(NULL)+secs);
} // evDelay

/*
 * sets ev.timer at a certain time
 * sets ev.next*
 */
void evSetAt(Event event, time_t when) {
  debug0("evSetAt()")
  ev.timers[event] = when;
  if (when < ev.nextWhen) {
    ev.nextEv = event;
    ev.nextWhen = when;
  }
} // evSetAt

/*
 * deactivate timer
 * sets ev.timer, ev.next*
 */
void evClear(Event event) {
  debug0("evClear()")
  ev.timers[event] = ev.forever;
  // was event the next in line?
  if (ev.nextEv==event) {
    ev.nextEv = Null_ev;
    ev.nextWhen = ev.forever;
    for (Event i=Null_ev+1; i<Sizeof_ev; i++) {
      if (ev.timers[i] < ev.nextWhen) {
        ev.nextEv = i;
        ev.nextWhen = ev.timers[i];
      }
    } // for
  } // if
} // evClear

/*
 * for testing
 */
void evDump() {
  printf("struct ev:\n  ev.nextEv %d, ev.nextWhen %ld, ev.forever %ld\n",
    ev.nextEv, ev.nextWhen, ev.forever);
  printf("  ev.timers[]  ");
  for (Event i=Null_ev; i<Sizeof_ev; i++) 
    printf("[%d] %ld  ", i, ev.timers[i]);
  printf("\n");
} // evDump
