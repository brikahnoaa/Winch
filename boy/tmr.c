// tmr.c - interval timer
#include <utl.h>
#include <tmr.h>

// initialized to all zeros, that works
static struct {
  bool on[sizeof_tmr];
  time_t timer[sizeof_tmr];
} tmr;
  
//
// sets: tmr.on .timer[]
//
void tmrStart(TmrType tim, int secs) {
  DBG2("tmrStart()")
  tmr.on[tim] = true;
  tmr.timer[tim] = time(0)+secs;
} // tmrStart

void tmrStop(TmrType tim) {
  DBG2("tmrStop()")
  tmr.on[tim] = false;
} // tmrStop

//
// is timer expired? turn it off
// timer==now is not expired, so now+1 runs at least 1 sec 
//
bool tmrExp(TmrType tim) {
  if (tmr.on[tim] && (tmr.timer[tim] < time(0))) {
    // on and expired
    tmr.on[tim] = false;
    DBG2("tmrExp(%d)", tim)
    return true;
  } else {
    // off or still running
    return false;
  }
} // tmrExp

//
// how long until a timer expires (else 0=off, neg=expired)
//
int tmrQuery(TmrType tim) {
  if (tmr.on[tim]) 
    return (int)(tmr.timer[tim]-time(0))+1;
  else
    return 0;
} // tmrQuery

//
// check if any timer is expired
//
TmrType tmrAnyExp(void) {
  TmrType t;
  for(t = null_tmr+1; t<sizeof_tmr; t++)
    if (tmrExp(t))
      return t;
  return null_tmr;
} // tmrCheck
