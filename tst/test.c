// rudics.c
#include <main.h>

extern IriInfo iri;
extern IriData irid;
extern BoyInfo boy;

int header(void);
int block(int len);

void main(void){
  Serial port;
  char c, *help;
  // int i, r;
  long cnt, bytes;
  bool run=true, cmdmode=false;
  sysInit();
  mpcInit();
  antInit();
  iriInit();
  //
  antStart();
  iriStart();
  //
  cprintf("\nbaud iri.baud=%d", iri.baud);
  antSwitch(irid_ant);
  iriSig();
  iriDial();
  printf("\nhello");
  iriProjHello("heyhey");
  port = irid.port;
  help = "q=quit '=cmd g=gps i=iri s=sig c=call b=blk h=hdr ?=hlp";
  cprintf ("\n%s", help);
  while (run) {
    if ((cnt = TURxQueuedCount(port))) {
      bytes = TURxGetBlock(port, all.str, cnt, 999);
      utlNonPrintBlock( all.str, (int) bytes );
    }
    if (cgetq()) {
      c=cgetc();
      cputc(c);
      if (c=='\'') { // tick, toggle cmdmode
        cmdmode = true;
        continue;
      }
      if (!cmdmode) {
        if (c=='\r') cputc('\n');
        else TUTxPutByte(port,c,false);
        continue;
      }
      switch(c) {
        case 'q': run=false; break;
        case 'g': antSwitch(gps_ant); break;
        case 'i': antSwitch(irid_ant); break;
        case 's': iriSig(); break;
        case 'c': iriDial(); iriProjHello("wassup?"); break;
        case 'x': iriHup(); break;
        case 'b': block(128); break;
        case 'h': header(); break;
        case '?': cprintf ("\n%s", help); break;
      } // case
      cmdmode = false;
    } // cgetc
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

int block(int len) {
  int i, r, cnt;
  len = dbg.t1;
  if (dbg.t2) cnt=dbg.t2;
  else cnt=1;
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
