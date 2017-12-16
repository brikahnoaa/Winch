// interval.c - interval timer
#include <interval.h>
// #include <utils.h>
// test vv
#include <stdio.h>
#define debug0(x)
#include <time.h>
// test ^^

typedef struct Interval {
  time_t when;
  IntervalType intT;
  struct Interval* next, prev;
} Interval;

// head is an empty node, never removed from list
Interval head = {(time_t)0, Null_it, NULL, NULL};

static struct {
  time_t nextWhen;
  int nextIT;
  Interval* list;
} it = {(time_t)0, (Interval*)NULL, &head};

/* 
 * check watchdog, power, and array of intervals
 */

/*
 * interval timer, abstracted for general purpose
 * fast check for next timer, so low overhead
 * 
 * Returns: enum IntervalType * use bitshifted values i.e. flags
 * Null_it
 * Winch_it - expect response
 * Wispr_it - detection duty cycle
 * Data_it - surface and phone home
 * Call_it - give up call and descend
 */

/*
 * add an interval timer to list
 * check time against nextWhen
 */
void addIT(IntervalType intT, int secs) {
  debug0("addIT()")
  // global it
  Interval* new = malloc(sizeof(Interval));
  // set values, insert node
  new.when = time(NULL)+(time_t)secs;
  new.intT = intT;
  new.next = it.list->next;
  new.prev = it.list;
  it.list->next = new;
  if (new.when < it.nextWhen) {
    it.nextIT = new;
    it.nextWhen = new.when;
  }
} // addIT

/*
 * remove and free node, check it.next*
 */
static void removeIT(Interval* in) {
  debug0("removeIT()")
  // global it
  // removing tail?
  if (in->next) in->next->prev = in->prev;
  in->prev->next = in->next;
  free(in);
  // 
  if (it.list->next==NULL) {
    // empty list
    it.nextIT = NULL;
    it.nextWhen = (time_t)0;
  }
  // if it was nextIT, find another
  if (it.nextIT==in) {
    // start with top of list
    it.nextIT = it.list->next;
    // not null
    if (it.nextIT)
      it.nextWhen = it.list->next->when;
    // check against rest of list
    for (Interval* i=it.list->next->next; i!=NULL; i=i->next) {
      if (i->when < it.nextWhen) {
        it.nextIT = i;
        it.nextWhen = i->when;
      }
    } // for
  } // if
} // removeIT

int checkIT(void) {
  debug0("checkIT()")
  // global it
  int r;
  time_t now = time(NULL);
  // fast turnaround
  if ((it.nextIT==NULL) || (it.nextWhen>now)) return;
  // interval elapsed
  r = it.nextIT->intT;
  removeIT(it.nextIT);
  return r;
} // checkIT

/*
 * cancel all intervals of a type, e.g. detection interval
 * return: # removed
 */
int cancelIT(IntervalType iType) {
  // global it
  int r=0;
  // it.list->next is first list item
  for (Interval* i=it.list->next; i!=NULL; i=i->next) {
    if (i->intT==intType) {
      i = i->prev;
      removeIT(i->next);
      r++;
    }
  }
  return r;
}
