// tmr.c - interval timer
#include <utl.h>
#include <tmr.h>

// initialized to all zeros, that works
static struct {
  bool on[sizeof_tmr];
  time_t exp[sizeof_tmr];
} tmr;
  
//
// sets: tmr.on .exp[]
//
void tmrStart(TmrType tim, int secs) {
  DBG2("tmrStart(%d,%d)", tim, secs)
  tmr.on[tim] = true;
  tmr.exp[tim] = time(0)+secs;
} // tmrStart

void tmrStop(TmrType tim) {
  DBG2("tmrStop()")
  tmr.on[tim] = false;
} // tmrStop

///
// stop all timers
void tmrStopAll(void) {
  TmrType t;
  for(t = null_tmr+1; t<sizeof_tmr; t++)
    tmr.on[t] = false;
} // tmrCheck

///
// is timer expired? turn it off
// exp==now is not expired, so now+1 runs at least 1 sec 
// sets: tmr.on[] .exp[]
bool tmrExp(TmrType tim) {
  DBG(utlX();)
  if (tmr.on[tim] && (tmr.exp[tim] < time(0))) {
    // on and expired
    tmr.on[tim] = false;
    DBG1("tmrExp(%d)", tim)
    return true;
  } else {
    // off or still running
    return false;
  }
} // tmrExp

///
// is timer still running?
// exp==now is not expired, so now+1 runs at least 1 sec 
bool tmrOn(TmrType tim) {
  return (tmr.on[tim] && (tmr.exp[tim] >= time(0)));
} // tmrOn

///
bool tmrOff(TmrType tim) {
  return (!tmr.on[tim]);
}

//
// how long until a timer expires (else 0=off, neg=expired)
//
int tmrQuery(TmrType tim) {
  if (tmr.on[tim]) 
    return (int)(tmr.exp[tim]-time(0))+1;
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
