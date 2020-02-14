// iridBlock.c
#include <main.h>

extern IriInfo iri;
extern IriData irid;
extern BoyInfo boy;
extern SysInfo sys;

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
  cnt = tst.t1;
  len = tst.t2;
  cprintf("\n count tst.t1=%d   length tst.t2=%d\n", cnt, len);
  buff = irid.block;
  antSwitch(irid_ant);
  if (iriSig()) return;
  if (iriDial()) return;
  if (iriProjHello(buff)) return;
  //
  utlWrite(irid.port, "data", "");
  for (i=1; i<=cnt; i++) {
    memset(buff, 0, len);
    sprintf(buff, "%d of %d =%d @%d [%d]", 
      i, cnt, len, iri.baud, iri.sendSz);
    buff[len-1] = 'Z';
    r = iriSendBlock(len, i, cnt);
    // utlDelay(500);
  }
  iriLandResp(all.buf);
  if (strstr(all.buf, "cmds"))
    r = iriLandCmds(all.buf);
  //
  utlWrite(irid.port, "data", "");
  for (i=1; i<=cnt; i++) {
    memset(buff, 0, len);
    sprintf(buff, "%d of %d =%d @%d [%d]", 
      i, cnt, len, iri.baud, iri.sendSz);
    buff[len-1] = 'Z';
    r = iriSendBlock(len, i, cnt);
    // utlDelay(500);
  }
  iriLandResp(all.buf);
  if (strstr(all.buf, "cmds"))
    r = iriLandCmds(all.buf);
  //
  iriHup();
  iriSig();
  flogf("\n%s\n", utlTime());
  // iriStop();
  // antStop();
}
