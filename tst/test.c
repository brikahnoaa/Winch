// irid.c
#include <main.h>

extern BoyInfo boy;
extern SysInfo sys;
extern IriInfo iri;
extern IriData irid;

void main(void){
  // Serial port;
  // char c;
  char *buff;
  int len, cnt;
  int i, r;
  sysInit();
  mpcInit();
  antInit();
  iriInit();
  //
  antStart();
  iriStart();
  //
  len = dbg.t2;
  cnt = dbg.t1;
  cprintf("\nlength dbg.t2=%d, count dbg.t1=%d ", len, cnt);
  cprintf("\nbaud iri.rudBaud=%d", iri.rudBaud);
  buff = irid.buf->block;
  iriDateTime(irid.stats1);
  antSwitch(irid_ant);
  if (iriSig()) return;
  if (iriDial()) return;
  if (iriProjHello(all.buf)) return;
  for (i=1; i<=cnt; i++) {
    memset(buff, 0, len);
    sprintf(buff, "%d of %d =%d @%d [%d]", 
      i, cnt, len, iri.rudBaud, iri.sendSz);
    buff[len-1] = 'Z';
    r = iriSendBlock(len, i, cnt);
    cprintf("(%d)\n", r);
    // utlDelay(500);
  }
  iriLandResp(all.buf);
  if (strstr(all.buf, "cmds"))
    r = iriLandCmds(all.buf);
  iriHup();
  iriSig();
  flogf("\n%s\n", utlTime());
  iriStop();
  antStop();
}
