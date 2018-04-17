// wsp.c sbe16
#include <utl.h>
#include <wsp.h>
#include <mpc.h>
#include <tmr.h>

#define EOL "\r"

WspInfo wsp;

// general note: wsp wants \r only for input, outputs \r\n
// Bug:! TUTxPrint translates \n into \r\n, which sorta kinda works if lucky
//
// \r input, echos input.  \r\n before next output.
// pause 0.33s between \rn and s> prompt.
// wakeup takes 1.045s, writes extra output "SBE 16plus\r\nS>"
// pause between ts\r\n and result = 4.32s
// sbe16 response is just over 3sec in sync, well over 4sec in command

///
// sets: wsp.port .wspPending
void wspInit(void) {
  DBG0("wspInit()")
  mpcPamDev(wsp1_pam);
  wsp.port = mpcPamPort();
  tmrStop(wsp_tmr);
  if (strlen(wsp.logFile))
    wsp.log = utlLogFile(wsp.logFile);
} // wspInit

///
void wspStop(void){
  mpcPamDev(null_pam);
  if (wsp.log) 
    close(wsp.log);
  wsp.log = 0;
} // wspStop

///
// data waiting
bool wspData() {
  DBG1("wD")
  return TURxQueuedCount(wsp.port);
} // wspData

///
// sets: wsp.depth .wspPending 
bool wspRead(void) {
  char *p0, *p1, *p2, *p3;
  if (!wspData()) return false;
  DBG1("wspRead()")
  utlRead(wsp.port, utlBuf);
  if (wsp.log) 
    write(wsp.log, utlBuf, strlen(utlBuf));
  // Temp, conductivity, depth, fluromtr, PAR, salinity, time
  // ' 20.6538,  0.01145,    0.217,   0.0622, 01 Aug 2016 12:16:50\r\n'
  // note: leading # in syncmode '# 20.6...'
  // note: picks up trailing S> prompt if not in syncmode
  p0 = utlBuf;
  p1 = strtok(p0, "\r\n#, ");
  if (!p1) return false;
  p2 = strtok(NULL, ", "); 
  if (!p2) return false;
  p3 = strtok(NULL, ", ");
  if (!p3) return false;
  wsp.depth = atof( p3 );
  DBG1("= %4.2", wsp.depth)
  tmrStop(wsp_tmr);
  wsp.time = time(0);
  return true;
} // wspRead

