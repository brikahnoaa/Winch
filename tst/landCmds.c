// landResp.c
#include <main.h>

extern IriInfo iri;
extern IriData irid;
extern BoyInfo boy;
extern SysInfo sys;

void main(void){
  int r, try, i, hdr=13;
  char *s=NULL;
  unsigned char c;
  uchar resp[32];
  ulong sec0, sec;
  ushort tick;
  float times[32];
  static char *self="main";
  sysInit();
  mpcInit();
  antInit();
  iriInit();
  //
  antStart();
  iriStart();
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
  flogf("\npause 10 sec");
  tmrStart(iri_tmr, 10);
  memset(resp, 0, 32);
  RTCGetTime(&sec0, null);
  r=0;
  while (!tmrExp(iri_tmr)) {
    c = TURxGetByteWithTimeout(irid.port, 200);
    if (c>=0) {
      resp[r] = c;
      RTCGetTime(&sec, &tick);
      times[r++] = (float)(sec-sec0)+(float)tick/40000.0;
    } else {
      printf(" %02X", c);
    }
  }
  catch:
  printf("\n<'%s'>", utlNonPrintBlock(resp, r));
  for (i=0; i<=r; i++)
    printf("\n time=%5.3f [%d] x%02X '%s'",
        times[i], i, resp[i], utlNonPrintBlock(resp+i,1));
  iriHup();
  iriSig();
  iriStop();
  antStop();
  exit(0);
}
