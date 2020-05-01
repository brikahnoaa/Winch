// iriCall.c
#include <test.h>
// multiple calls instead of files

extern IriInfo iri;
extern IriData irid;
extern BoyInfo boy;
extern SysInfo sys;

void main(void){
  // Serial port;
  // char c;
  char *buff;
  int i, j, k, r;
  int blk, len, fil, call;
  blk = 2; len = 512; fil = 2; call = 4;
  sysInit();
  mpcInit();
  antInit();
  iriInit();
  //
  antStart();
  iriStart();
  //
  cprintf("\niriCall makes dbg.t4 calls for .t3 files with .t1 blocks of .t2\n");
  if (dbg.t1) blk = dbg.t1;
  if (dbg.t2) len = dbg.t2;
  if (dbg.t3) fil = dbg.t3;
  if (dbg.t4) call = dbg.t4;
  cprintf("\n block.t1=%d   length.t2=%d   files.t3=%d   call.t4=%d\n", 
      blk, len, fil, call);
  buff = irid.block;
  antSwitch(irid_ant);
  for (k=1; k<=call; k++) {
    if (iriSig()) return;
    if (iriDial()) return;
    if (iriProjHello(buff)) return;
    //
    for (j=1; j<=fil; j++) {
      utlWrite(irid.port, "data", "");
      for (i=1; i<=blk; i++) {
        memset(buff, 0, len);
        sprintf(buff, "%d / %d =%d in #%d @%d [%d]", 
          i, blk, len, j, iri.baud, iri.blkSz);
        buff[len-1] = 'Z';
        r = iriSendBlock(len, i, blk);
        utlDelay(222);
        qq;
      } // i
      r = iriLandResp(all.buf);
      if (r) break;
      if (strstr(all.buf, "cmds"))
        r = iriLandCmds(all.buf);
    } // j
    //
    iriHup();
    iriSig();
    flogf("\n%s\n", utlTime());
  } // k
  // talk about it
  for (i=1; i<10; i++) {
    cprintf("\ntalk to me: \n");
    j=CIOgets(all.str, 100);
    if (j==0) break;
    if (all.str[0]=='Q') break;
    utlRead(irid.port, all.buf);
    if (*all.buf) 
      cprintf("<<-%s<<-", all.buf);
    cprintf("->>%s->>", all.str);
    utlWrite(irid.port, all.str, "\r");
    utlReadWait(irid.port, all.buf, 10);
    if (*all.buf) 
      cprintf("<<-%s<<-", all.buf);
  } // talk loop
  iriStop();
  antStop();
  exit(0);
}
