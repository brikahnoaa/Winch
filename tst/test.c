// iridFile.c
#include <main.h>

extern IriInfo iri;
extern IriData irid;
extern BoyInfo boy;
extern SysInfo sys;

void main(void){
  int r, try, i, hdr=13;
  char *s=NULL;
  ulong sec0, sec;
  ushort tick;
  int times[32];
  static char *self="main";
  // ulong RTCGetTime(ulong *seconds, ushort *ticks);
  // ulong RTCGetTime(ulong *seconds, ushort *ticks);
  //
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
    // ulong RTCGetTime(ulong *seconds, ushort *ticks);
  RTCGetTime(&sec0, null);
  r=0;
  while (true) {
    if ((all.str[r] = TURxGetByte(irid.port, true))) {
      RTCGetTime(&sec, &tick);
      times[r++] = (sec-sec0)*1000+(tick/40);
    }
    if (tmrExp(iri_tmr)) break;
  }
  catch:
  for (i=0; i<=r; i++)
    printf("\n time=%d char='%c' '%x'", 
        times[i], all.str[i], all.str[i]);
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
