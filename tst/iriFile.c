// iridFile.c
#include <main.h>

extern IriInfo iri;
extern IriData irid;
extern BoyInfo boy;
extern SysInfo sys;

void main(void){
  // Serial port;
  // char c;
  // char *buff;
  // int len, cnt;
  sysInit();
  mpcInit();
  antInit();
  iriInit();
  //
  antStart();
  iriStart();
  //
  // len = dbg.t2;
  // cnt = dbg.t1;
  // cprintf("\nlength dbg.t2=%d, count dbg.t1=%d ", len, cnt);
  // cprintf("\nbaud iri.rudBaud=%d", iri.rudBaud);
  // buff = malloc(len);
  // antSwitch(gps_ant);
  // iriStats();
  antSwitch(irid_ant);
  if (iriSig()) return;
  if (iriDial()) return;
  if (iriProjHello(all.buf)) return;
  iriSendFile("test\\test.log");
  iriSendFile("test\\test.log");
  iriHup();
  iriSig();
  iriStop();
  antStop();
}
