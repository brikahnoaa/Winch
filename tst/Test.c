// iriBlock.c
#include <main.h>

extern IriInfo iri;
extern IriData irid;
extern BoyInfo boy;
extern SysInfo sys;

void main(void){
  // Serial port;
  // char c;
  char *buff;
  int i, j, r;
  int blk, len, cnt;
  blk = 4; len = 1024; cnt = 1;
  sysInit();
  mpcInit();
  antInit();
  iriInit();
  //
  antStart();
  iriStart();
  //
  cprintf("\niriBlock sends dbg.t3 files with .t1 blocks of size .t2\n");
  if (dbg.t1) blk = dbg.t1;
  if (dbg.t2) len = dbg.t2;
  if (dbg.t3) cnt = dbg.t3;
  cprintf("\n block=%d dbg.t1=%d   length=%d dbg.t2=%d   count=%d dbg.t3=%d\n", 
      blk, dbg.t1, len, dbg.t2, cnt, dbg.t3);
  buff = irid.block;
  antSwitch(irid_ant);
  if (iriSig()) return;
  if (iriDial()) return;
  if (iriProjHello(buff)) return;
  //
  utlWrite(irid.port, "data", "");
  for (i=1; i<=blk; i++) {
    memset(buff, 0, len);
    sprintf(buff, "%d of %d =%d @%d [%d]", 
      i, blk, len, iri.baud, iri.blkSz);
    buff[len-1] = 'Z';
    r = iriSendBlock(len, i, blk);
    // utlDelay(500);
  }
  iriLandResp(all.buf);
  if (strstr(all.buf, "cmds"))
    r = iriLandCmds(all.buf);
  //
  utlWrite(irid.port, "data", "");
  for (j=1; j<=cnt; i++) {
    for (i=1; i<=blk; i++) {
      memset(buff, 0, len);
      sprintf(buff, "%d of %d =%d @%d [%d]", 
        i, blk, len, iri.baud, iri.blkSz);
      buff[len-1] = 'Z';
      r = iriSendBlock(len, i, blk);
      // utlDelay(500);
    }
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
