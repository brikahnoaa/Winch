// it.c - interval timer
#include <it.h>
#include <utils.h>

/*
 * interval timer, abstracted for general purpose
 * fast check for next timeout, so low overhead
 * 
 * Null_it
 * Sink_it - checking depth as we sink
 * Settle_it - let the mooring settle
 * Winch_it - expect response from winch
 * Wispr_it - detection duty cycle
 * Rest_it - no detection, sleep
 * Raise_it - go up for to phone home
 * Drop_it - enough talk, hang up and go down
 */

typedef struct Interval {
  time_t when;
  IntervalType intT;
  struct Interval* next, prev;
} Interval;

// head is an empty node, never removed from list, only head.next is used
Interval emptyNode = {(time_t)0, Null_it, NULL, NULL};

// it.head->next is first list item
static struct {
  time_t nextWhen;
  int nextIT;
  Interval* head;
} it = {(time_t)0, (Interval*)NULL, &emptyNode};


/*
 * add an interval timer to list
 * check time against nextWhen
 */
void itAdd(IntervalType intT, int secs) {
  debug0("itAdd()")
  // global it
  Interval* new = malloc(sizeof(Interval));
  // set values, insert node after head
  new.when = time(0)+secs;
  new.intT = intT;
  new.next = it.head->next;
  new.prev = it.head;
  it.head->next = new;
  if (new.when < it.nextWhen) {
    it.nextIT = new;
    it.nextWhen = new.when;
  }
} // itAdd

/*
 * remove and free node
 * set: it.next*
 */
static void itRemove(Interval* in) {
  debug0("itRemove()")
  // global it
  if (in->next) in->next->prev = in->prev;
  in->prev->next = in->next;
  free(in);
  // 
  if (it.head->next==NULL) {
    // empty list
    it.nextIT = NULL;
    it.nextWhen = (time_t)0;
  }
  // if in was nextIT, find new next
  if (it.nextIT==in) {
    // start with top of list
    it.nextIT = it.head->next;
    // not null
    if (it.nextIT)
      it.nextWhen = it.head->next->when;
    // check against rest of list
    for (Interval* i=it.head->next->next; i!=NULL; i=i->next) {
      if (i->when < it.nextWhen) {
        it.nextIT = i;
        it.nextWhen = i->when;
      } // if
    } // for
  } // if
} // itRemove

/*
 * remove all intervals of a type, e.g. detection interval
 */
void itCancel(IntervalType intT) {
  // global it
  for (Interval* i=it.head->next; i!=NULL; i=i->next) 
    if (i->intT==intT) 
      itRemove(i);
}

IntervalType itCheck(void) {
  debug0("itCheck()")
  // global it
  if (!it.nextIT || (it.nextWhen>time(0)) return Null_it;
  // interval elapsed
  int r = it.nextIT->intT;
  // itRemove sets it.next*
  itRemove(it.nextIT);
  return r;
} // itCheckIT

time_t itQuery(IntervalType intT) {
  // global it
  time_t r=0;
  if (!it.nextIT) return r;
  // it.head->next is first list item
  for (Interval* i=it.head->next; i!=NULL; i=i->next) {
    if (i->intT==intT)
      if (!r || (r>i->when))
        r = i->when;
  }
  return r;
}

