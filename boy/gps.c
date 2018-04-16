// gps.c - iridium/gps
#include <utl.h>
#include <gps.h>
#include <ant.h>
#include <tmr.h>

#define EOL "\r"

GpsInfo gps;

///
// set up for gps, call this after gpsInit
// sets: gps.mode .port
void gpsInit(void) {
  DBG0("gpsInit()")
  gps.port = antPort();
  if (!gps.port)
    utlErr(logic_err, "no gps.port, was gpsInit called before gpsInit?");
} // gpsInit

///
// turn on, clean, set params, talk to sbe39
void gpsOn(bool on) {
  if (gps.on==on) return;
  DBG0("gpsStart() %s", utlDateTime())
  if (on) {
    antOn(true);
    antDevice(a3la_dev);
    antSwitch(gps_ant);
  } else {
    gpsStop();
  }
  gps.on = on;
} // gpsStart

///
// turn off power to gpsmod 
void gpsStop() {
  if (gps.log)
    close(gps.log);
  gps.log = 0;
  antDevice(ctd_dev);
} // gpsStop

///
bool gpsPrompt() {
  DBG1("gP")
  TURxFlush(gps.port);
  utlWrite(gps.port, "", EOL);
  utlReadWait(gps.port, utlBuf, gps.delay);
  if (strstr(utlBuf, "OK"))
    return true;
  else 
    return false;
} // gpsPrompt

///
// data waiting
int gpsData() {
  int r=TURxQueuedCount(gps.port);
  DBG1("gDa=%d", r)
  return r;
} // gpsData

