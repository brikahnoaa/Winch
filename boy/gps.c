// gps.c
#include <utl.h>
#include <gps.h>
#include <ant.h>
#include <tmr.h>

// gps and iridium routines have a lot of ways to fail, so return 0 or errcode
//
#define EOL "\r"

GpsInfo gps;

///
// tell antmod to turn on a3la
int gpsInit(void){
  gps.port = antPort();
  return 0;
} // gpsInit

int gpsStart(void){
  // gpsStart()
  antDevice(cf2_dev);
  TUTxPutByte(gps.port, 3, false);
  TUTxPutByte(gps.port, 'I', false);
  antDevice(a3la_dev);
  if (utlExpect(gps.port, utlBuf, "COMMAND", 12))
    return 0;
  else
    return 1;
} // gpsStart
 
int gpsStop(void){
  // gpsStart()
  antDevice(cf2_dev);
  TUTxPutByte(gps.port, 4, false);
  TUTxPutByte(gps.port, 'I', false);
  antDevice(a3la_dev);
  return 0;
} // gpsStop
 
#define GPS_T 90
///
// rets: 1=err *sats=#
int gpsSats(int *sats){
  DBG0("gpsSats()")
  tmrStart(ant_tmr, GPS_T);
  while (!tmrExp(ant_tmr)) {
    utlWrite(gps.port, "AT+PD", EOL);
    utlExpect(gps.port, utlBuf, "OK", 12);
    if (!strstr(utlBuf, "Invalid Position"))
      break;
    if (utlMatchAfter(utlStr, utlBuf, "Satellites Used=", "0123456789"))
      flogf(" Sats=%s", utlStr);
    utlNap(3);
    utlX();
  } // while timer
  if (utlStr) {
    *sats = atoi(utlStr);
    return 0;
  }
  return 1;
} // gpsSats

/// 
// gpsISig
int gpsISig(void) {
  char *here;
  int iridT=120;
  // replace crlf
  for (here=utlBuf; *here; here++) 
    if (*here=='\r' || *here=='\n')
      *here = '.';
  flogf("\n%s\n%d seconds\n", utlBuf, gpsT-tmrQuery(ant_tmr));
  // switch to irid
  antSwitch(irid_ant);
  flogf("\nCSQ\n");
  tmrStart(ant_tmr, iridT);
  while (!tmrExp(ant_tmr)) {
    utlWrite(gps.port, "AT+CSQ", EOL);
    utlExpect(gps.port, utlBuf, "OK", 12);
    // replace crlf
    for (here=utlBuf; *here; here++) 
      if (*here=='\r' || *here=='\n')
        *here = '.';
    flogf("%s\n", utlBuf);
    utlX();
  } // while timer
} // gpsISig

///
// A.8.77 +CLCC - Request Current Call Status
// 0  Active
// 1  Call Held
// 2  Dialing (MO Call)
// 4  Incoming (MT Call)
// 6  Idle
int iridCallStatus(void) {
}
