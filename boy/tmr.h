// tmr.h interval timer
#define TMR_H

// timers 0-9 are for local use, ad hoc
// typedef enum {
enum {
  null_tmr=10,
  ant_tmr, ctd_tmr, deploy_tmr, data_tmr, day_tmr,
  gps_tmr, hour_tmr, init_tmr, minute_tmr, rudics_tmr,
  utl_tmr, ngk_tmr, motion_tmr, wsp_tmr, sizeof_tmr
  };
//  } TmrType;
typedef int TmrType;

bool tmrExp(TmrType tim);
bool tmrOn(TmrType tim);
bool tmrOff(TmrType tim);
int tmrQuery(TmrType tim);
void tmrStart(TmrType tim, int secs);
void tmrStop(TmrType tim);
void tmrStopAll(void);
TmrType tmrAnyExp(void);

