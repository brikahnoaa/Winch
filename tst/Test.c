// iriBlock.c
#include <test.h>

extern IriInfo iri;
extern IriData irid;
extern BoyInfo boy;
extern SysInfo sys;

void main(void){
  // Serial port;
  // char c;
  char *buff;
  int i, j, r;
  int blk, len, fil, del;
  blk = 4; len = 1024; fil = 1; del = 0;
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
  if (dbg.t3) fil = dbg.t3;
  if (dbg.t4) del = dbg.t4;
  cprintf("\n block.t1=%d   length.t2=%d   files.t3=%d   delay.t4=%d\n", 
      blk, len, fil, del);
  buff = irid.block;
  antSwitch(irid_ant);
  if (iriSig()) return;
  if (iriDial()) return;
  if (iriProjHello(buff)) return;
  //
  for (j=1; j<=fil; j++) {
    utlWrite(irid.port, "data", "");
    utlDelay(del);
    for (i=1; i<=blk; i++) {
      memset(buff, 0, len);
      sprintf(buff, "%d / %d =%d in #%d @%d [%d]", 
        i, blk, len, j, iri.baud, iri.blkSz);
      buff[len-1] = 'Z';
      r = iriSendBlock(len, i, blk);
      utlDelay(del);
      qq;
    }
    r = iriLandResp(all.buf);
    if (r) exit(r);
    if (strstr(all.buf, "cmds"))
      r = iriLandCmds(all.buf);
  }
  //
  iriHup();
  iriSig();
  flogf("\n%s\n", utlTime());
  iriStop();
  antStop();
  exit(0);
}

