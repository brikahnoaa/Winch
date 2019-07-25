// rudTalk.c
#include <main.h>

extern IriInfo iri;
extern IriData irid;
extern BoyInfo boy;

int header(void);
int block(int len, int cnt);

void main(void){
  Serial port;
  char *help;
  // int i, r;
  int cnt, x, y;
  bool run=true;
  sysInit();
  mpcInit();
  antInit();
  iriInit();
  //
  antStart();
  iriStart();
  //
  port = irid.port;
  //
  utlWrite(irid.port, "at", "\r");
  //
  cprintf("\ndbg.t1=length=%d iri.baud=%d", dbg.t1, iri.baud);
  antSwitch(irid_ant);
  iriSig();
  help = "q=quit '=cmd g=gps i=iri s=sig c=call b=blk h=hdr x=hup ?=hlp";
  cprintf ("\n%s\n", help);
  while (run) {
    if ((cnt = utlGetBlock(port, all.buf, BUFSZ, 500)))
      cprintf("\n<%d'%s'<", cnt, utlNonPrintBlock(all.buf, cnt));
    cprintf("\n");
    cnt = getstr(all.str, 999);
    if (cnt==0) continue;
    if (all.str[0]=='\'') { // input, tick==cmdmode
      switch(all.str[1]) {
        case 'q': run=false; break;
        case 'g': antSwitch(gps_ant); break;
        case 'i': antSwitch(irid_ant); break;
        case 'p': iriPrompt(); break;
        case 's': iriSig(); break;
        case 'c': iriDial(); iriProjHello("wassup?"); break;
        case 'x': iriHup(); break;
        case 'b': x=dbg.t1; y=dbg.t2; block(x?x:128, y?y:1); break;
        case 'h': header(); break;
        case '?': cprintf ("\n%s\n", help); break;
      } // case
    } else {
      utlWrite(port, all.str, "\r");
    } // input
  } // run
  iriHup();
  iriSig();
  flogf("\n%s\n", utlTime());
  iriStop();
  antStop();
  return;
}

#define IRID_BUF_SUB 5
#define IRID_BUF_BLK 10

// hdr only, expect user to type stuff
int header(void) {
  static char *self="header";
  static char *rets="1=inFromLand";
  int size;
  int bsiz=1000, bnum=1, btot=1;
  utlWrite(irid.port, "data", "");
  // make hdr - beware null terminated sprintf, use memcpy
  size = bsiz+IRID_BUF_BLK-IRID_BUF_SUB;
  sprintf(all.str, "@@@CS%c%cT%c%c",
    (char) (size>>8 & 0xFF), (char) (size & 0xFF),
    (char) bnum, (char) btot);
  memcpy(irid.buf, all.str, IRID_BUF_BLK);
  TUTxPutBlock(irid.port, irid.buf, (long) IRID_BUF_BLK, 9999);
  return 0;
} // header

int block(int len, int cnt) {
  int i=1, r;
  utlWrite(irid.port, "data", "");
  for (i=1; i<=cnt; i++) {
    memset(irid.block, 0, len);
    sprintf(irid.block, "%d of %d =%d @%d", i, cnt, len, iri.baud);
    irid.block[len] = 'Z';
    r = iriSendBlock(len, i, cnt);
  }
  iriLandResp(all.buf);
  if (strstr(all.buf, "cmds")) {
    len = iriLandCmds(all.buf);
    utlNonPrintBlock(all.buf, len);
  }
  return 0;
}
