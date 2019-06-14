// iridFile.c
#include <main.h>

extern IriInfo iri;
extern IriData irid;
extern BoyInfo boy;
extern SysInfo sys;

void main(void){
  int r, try, i, hdr=13;
  char *s=NULL;
  long times[32];
  static char *self="main";
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
  try = iri.hdrTry;
  while (!s) {
    if (try-- <= 0) throw(1);
    flogf(" projHdr");
    utlWriteBlock(irid.port, irid.projHdr, hdr);
    s = utlExpect(irid.port, all.str, "ACK", iri.hdrPause);
    if (strstr(all.str, "NO CARRIER")) throw(2);
  }
  flogf("\n hello\n");
  sprintf(irid.block, "hello");
  iriSendBlock(5, 1, 1);
  flogf("\npause 30 sec");
  tmrStart(iri_tmr, 30);
  memset(all.str, 0, 32);
  r=0;
  while (true) {
    while (TURxQueuedCount(irid.port)==0)
      if (tmrExp(iri_tmr)) throw(1);
    all.str[r++] = TURxGetByte(irid.port, true);
    times[r] = time(0);
  }
  catch:
  for (i=0; i<=r; i++)
    printf("\n time=%ld char='%c' '%x'", 
        times[r]-times[0], all.str[r], all.str[r]);
  exit(0);



  if (iriProjHello(all.buf)) return;
  flogf("\n short size = %ld\n", sizeof(short));
  /*
  for (i=1; i<=cnt; i++) {
    memset(buff, 0, len);
    sprintf(buff, "%d of %d =%d @%d [%d]", 
      i, cnt, len, iri.rudBaud, iri.sendSz);
    buff[len-1] = 'Z';
    r = iriSendBlock(buff, len, i, cnt);
    cprintf("(%d)\n", r);
    // utlDelay(500);
  }
   */
  iriSendFile("test\\test.log");
  iriSendFile("test\\test.log");
  iriHup();
  iriSig();
  iriStop();
  antStop();
}
