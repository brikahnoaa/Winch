// tmr.h interval timer
#define TMR_H

// timers 0-9 are for local use, ad hoc
enum {
  null_tmr=10,
  day_tmr, hour_tmr, minute_tmr, second_tmr, phase_tmr, data_tmr, 
  iri_tmr, ngk_tmr, utl_tmr, wsp_tmr, s16_tmr, s39_tmr, 
  sizeof_tmr
  };
typedef int TmrType;

bool tmrExp(TmrType tim);
bool tmrOn(TmrType tim);
bool tmrOff(TmrType tim);
time_t tmrQuery(TmrType tim);
void tmrStart(TmrType tim, time_t secs);
void tmrStop(TmrType tim);
void tmrStopAll(void);
TmrType tmrAnyExp(void);

