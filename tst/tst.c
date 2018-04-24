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
  // char c;
  // Serial port;
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
  utlReadWait(ant.port, utlBuf, 9);
  flogf("'%s'", utlBuf);
  utlReadWait(ant.port, utlBuf, 9);
  flogf("'%s'", utlBuf);
  flogf("\nAT+PT\n");
  tmrStart(ant_tmr, 45);
  while (!tmrExp(ant_tmr)) {
    utlWrite(ant.port, "AT+PT", EOL);
    utlReadWait(ant.port, utlBuf, 3);
    flogf("'%s'", utlBuf);
    if (!strstr(utlBuf, "Satellites")) {
      utlReadWait(ant.port, utlBuf, 3);
      flogf("'%s'", utlBuf);
    }
    if (!strstr(utlBuf, "Invalid Position"))
      break;
    if (cgetq() && cgetc()=='Q') break;
    utlNap(3);
  } // while timer
  antDevice(cf2_dev);
  antSwitch(irid_ant);
  antDevice(a3la_dev);
  flogf("\nCSQ\n");
  tmrStart(ant_tmr, 45);
  while (!tmrExp(ant_tmr)) {
    utlWrite(ant.port, "AT+CSQ", EOL);
    utlReadWait(ant.port, utlBuf, 3);
    flogf("'%s'", utlBuf);
    if (!strstr(utlBuf, "CSQ:")) {
      utlReadWait(ant.port, utlBuf, 9);
      flogf("'%s'", utlBuf);
    }
    if (cgetq() && cgetc()=='Q') break;
    utlNap(3);
  } // while timer
}
