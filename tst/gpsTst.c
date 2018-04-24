// antTst.c
#include <utl.h>
#include <ctd.h>
#include <mpc.h>
#include <sys.h>
#include <ant.h>
#include <tmr.h>

#define EOL "\r"
extern AntInfo ant;
extern CtdInfo ctd;

void main(void){
  char *here;
  int gpsT=90, iridT=120;
  sysInit();
  mpcInit();
  antInit();
  // ctdInit();
  // ctdStart();
  antStart();
  // a3laStart()
  TUTxPutByte(ant.port, 3, false);
  TUTxPutByte(ant.port, 'I', false);
  antDevice(a3la_dev);
  utlExpect(ant.port, utlBuf, "COMMAND", 12);
  flogf("'%s'", utlBuf);
  flogf("\nAT+PD\n");
  tmrStart(ant_tmr, gpsT);
  while (!tmrExp(ant_tmr)) {
    utlWrite(ant.port, "AT+PD", EOL);
    utlExpect(ant.port, utlBuf, "OK", 12);
    if (!strstr(utlBuf, "Invalid Position"))
      break;
    if (utlMatchAfter(utlStr, utlBuf, "Satellites Used=", "0123456789")) 
      flogf(" Sats=%s", utlStr);
    if (cgetq() && cgetc()=='Q') break;
    utlNap(3);
  } // while timer
  // replace crlf
  for (here=utlBuf; *here; here++) 
    if (*here=='\r' || *here=='\n')
      *here = '.';
  flogf("\n%s\n%d seconds\n", utlBuf, gpsT-tmrQuery(ant_tmr));
  // switch to irid
  antDevice(cf2_dev);
  antSwitch(irid_ant);
  antDevice(a3la_dev);
  flogf("\nCSQ\n");
  tmrStart(ant_tmr, iridT);
  while (!tmrExp(ant_tmr)) {
    utlWrite(ant.port, "AT+CSQ", EOL);
    utlExpect(ant.port, utlBuf, "OK", 12);
    // replace crlf
    for (here=utlBuf; *here; here++) 
      if (*here=='\r' || *here=='\n')
        *here = '.';
    flogf("%s\n", utlBuf);
    if (cgetq() && cgetc()=='Q') break;
  } // while timer
}
