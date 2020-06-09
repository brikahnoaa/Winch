// iri.c
#include <main.h>

// gps and iridium routines have a lot of ways to fail; return 0=success
//
#define EOL "\r"
#define CALL_DELAY 30
// offset of checksum, subheader (included in checksum), msg block
#define IRID_BUF_CS 3
#define IRID_BUF_SUB 5
#define IRID_BUF_BLK 10

IriInfo iri;
IriData irid;


///
// sets: irid.port .projHdr[]
void iriInit(void) {
  static char *self="iriInit";
  DBG();
  irid.port = antPort();
  if (!irid.port)
    utlErr(iri_err, "no irid.port, was iriInit called before antInit?");
  iriBufMalloc();
} // iriInit

///
// called by iriInit, also by iriStart if buf size changes
// uses: iri.blkSz
// sets: irid.blkSz .buf .block
void iriBufMalloc(void) {
  static char *self="iriBufMalloc";
  DBG1("\n%s: setting blkSize to %d", self, iri.blkSz);
  irid.blkSz = iri.blkSz;
  if (irid.buf) free(irid.buf);
  irid.buf = (uchar *)malloc(iri.blkSz + IRID_BUF_BLK);
  irid.block = irid.buf+IRID_BUF_BLK;
} //iriBufMalloc

///
// turn on, clean, set params, talk to a3la
// assumes: antStart()
// sets: irid.projHdr .blkSz .buf .block .usec .timer
int iriStart(void) {
  static char *self="iriStart";
  long usec;
  int cs;
  DBG0("iriStart() %s", utlTime());
  // set projHdr to 13 char project header, 0 in byte 14
  // poke in checksum high and low bytes
  sprintf(irid.projHdr, "???cs%4s%4s", iri.project, iri.platform);
  cs = iriCRC(irid.projHdr+5, 8);
  irid.projHdr[3] = (char) (cs >> 8) & 0xFF;
  irid.projHdr[4] = (char) (cs & 0xFF);
  // block & buf, size could be changed during run (iri.blkSz)
  if (irid.blkSz != iri.blkSz) iriBufMalloc();
  if (iri.baud>0 && iri.baud<9600) {
    RTCElapsedTimerSetup(&irid.timer);
    // usec per byte, at real baud rate
    usec = TUBlockDuration(irid.port, 1000L);
    DBG4("%s: usec=%ld", self, usec);
    // usec per byte, at effective baud rate
    irid.usec = usec * 9600 / iri.baud;
  } else irid.usec = 0L;
  // log?
  if (iri.logging) 
    utlLogOpen(&irid.log, "iri");
  // power up a3la
  antDevice(cf2_dev);
  TUTxPutByte(irid.port, 3, false);
  TUTxPutByte(irid.port, 'I', false);
  antDevice(a3la_dev);
  if (!utlReadExpect(irid.port, all.buf, "COMMAND MODE", 12)) return 1;
  /// ??
  utlWrite(irid.port, "AT &C1 &D0 &K0 &R1 &S1 E1 Q0 S0=1 S7=45 S10=100 V1 X4", EOL);
  if (!utlReadExpect(irid.port, all.str, "OK", 5)) return 2;
  utlWrite(irid.port, "ate0", EOL);
  if (!utlReadExpect(irid.port, all.str, "OK", 5)) return 2;
  return 0;
} // iriStart

///
// turn off power to irimod
void iriStop(void) {
  static char *self="iriStop";
  DBG();
  if (irid.log)
    utlLogClose(&irid.log);
  antDevice(a3la_dev);
  utlWrite(irid.port, "at*p", EOL);
  utlReadExpect(irid.port, all.buf, "OK", 2);
  antDevice(cf2_dev);
  TUTxPutByte(irid.port, 4, false);      // ^D powerdown
  TUTxPutByte(irid.port, 'I', false);      // S | I
} // iriStop

///
// sets: *ret //??
// rets: err code
int iriDateTimeToSecs(time_t *ret, char *date, time_t *time) {
  struct tm t;
  char *s;
  static char *self="iriDateTimeToSecs";
  static char *rets="1..6=field missing (sep=[-:.])";
  strcpy(all.str, date);
  if (!(s = strtok(all.str, " -:."))) raise(1);
  t.tm_mon = atoi(s) - 1;
  if (!(s = strtok(NULL, " -:."))) raise(2);
  t.tm_mday = atoi(s);
  if (!(s = strtok(NULL, " -:."))) raise(3);
  t.tm_year = atoi(s) - 1900;
  strcpy(all.str, time);
  if (!(s = strtok(all.str, " -:."))) raise(4);
  t.tm_hour = atoi(s);
  if (!(s = strtok(NULL, " -:."))) raise(5);
  t.tm_min = atoi(s);
  if (!(s = strtok(NULL, " -:."))) raise(6);
  t.tm_sec = atoi(s);
  *ret = mktime(&t);
  return 0;
} // iriDateTimeToSec

///
// get gps date time twice, err if not consistent
// ?? why set irid.stats? used how?
// sets: irid.stats
// rets: *stats
int iriDateTime(GpsStats *stats) {
  static char *self="iriDateTime";
  static char *rets="1=iriSats 2=getFail 3=badTime >5=differ";
  time_t secs, secs2, diff;
  DBG();
  if (iriSats()) raise(1);
  if (iriDateTimeGet(stats)) raise(2);
  if (iriDateTimeGet(&irid.stats)) raise(2);
  iriDateTimeToSecs(&secs, stats->date, stats->time);
  iriDateTimeToSecs(&secs2, irid.stats.date, irid.stats.time);
  // now is about 1.5Msec, sanity check 1M<now<2M
  if (secs<pow(10, 9) || secs>pow(10, 9)*2) raise(3);
  // compare two readings
  diff = secs - secs2;
  if (abs((int)diff)>5) raise((int)diff);
  diff = time(0) - secs2;
  if (abs((int)diff)>1) {
    flogf("\n%s(%s %s)\t| system time off by %ld seconds",
        self, stats->date, stats->time, diff);
    RTCSetTime(secs2, NULL);
  }
  flogf("\n%s: %s  %s", self, stats->date, stats->time);
  return 0;
} // iriDateTime

///
// get gps date time 
// rets: stats->date,time
int iriDateTimeGet(GpsStats *stats) {
  utlWrite(irid.port, "at+pd", EOL);
  if (!utlReadExpect(irid.port, all.buf, "OK", 12)) return 2;
  utlMatchAfter(all.str, all.buf, "Date=", "-0123456789");
  strcpy(stats->date, all.str);
  utlWrite(irid.port, "at+pt", EOL);
  if (!utlReadExpect(irid.port, all.buf, "OK", 12)) return 3;
  utlMatchAfter(all.str, all.buf, "Time=", ".:0123456789");
  strcpy(stats->time, all.str);
  return 0;
} // iriDateTimeGet

// get gps .lat .lng
// sets: .lng .lat 
// return: 0=success
int iriLatLng(GpsStats *stats){
  static char *self="iriLatLng";
  DBG();
  if (iriSats()) return 1;
  utlWrite(irid.port, "at+pl", EOL);
  if (!utlReadExpect(irid.port, all.buf, "OK", 12)) return 4;
  utlMatchAfter(all.str, all.buf, "Latitude=", ".:0123456789 NEWS");
  flogf(" Lat=%s", all.str);
  strcpy(stats->lat, all.str);
  utlMatchAfter(all.str, all.buf, "Longitude=", ".:0123456789 NEWS");
  flogf(" Lng=%s", all.str);
  strcpy(stats->lng, all.str);
  return 0;
} // iriLatLng

///
// sets: irid.sats
// rets: 0=success
int iriSats(void){
  tmrStart(iri_tmr, iri.timeout);
  while (!tmrExp(iri_tmr)) {
    utlNap(2);
    utlWrite(irid.port, "at+pd", EOL);
    if (!utlReadExpect(irid.port, all.buf, "OK", 12)) return 1;
    if (!strstr(all.buf, "Invalid Position") && strstr(all.buf, "Used=")) {
      tmrStop(iri_tmr);
      utlMatchAfter(all.str, all.buf, "Used=", "0123456789");
      flogf("\nGPS Sats=%s", all.str);
      irid.sats = atoi(all.str);
      return 0;
    }
  } // while timeout
  flogf("\niriSats\t| i got nothing");
  flogf("\n'%s'", all.buf);
  return 2;
} // iriSats

///
// sets: irid.signal
// rets: 0=success 2=lowSignal
int iriSig(void) {
  static char *self="iriSig";
  DBG();
  tmrStart(iri_tmr, iri.timeout);
  while (!tmrExp(iri_tmr)) {
    utlWrite(irid.port, "at+csq", EOL);
    if (!utlReadExpect(irid.port, all.buf, "OK", 12)) return 1;
    if (utlMatchAfter(all.str, all.buf, "CSQ:", "0123456789")) {
      irid.signal = atoi(all.str);
      flogf(" csq=%d", irid.signal);
      if (irid.signal>iri.signalMin) return 0;
    } // if CSQ
  // accept min signal if its all we got
  if (irid.signal>=iri.signalMin) return 0;
  } // while timer
  return 2;
} // iriSig

///
int iriCRC(uchar *buf, int cnt) {
  long accum=0x00000000;
  int i, j;
  static char *self="iriCRC";
  DBG();
  if (cnt <= 0) return 0;
  while (cnt--) {
    accum |= *buf++ & 0xFF;
    for (i = 0; i < 8; i++) {
      accum <<= 1;
      if (accum & 0x01000000)
        accum ^= 0x00102100;
    }
  }
  // compatibility with XMODEM CRC
  for (j = 0; j < 2; j++) {
    accum |= 0 & 0xFF;
    for (i = 0; i < 8; i++) {
      accum <<= 1;
      if (accum & 0x01000000)
        accum ^= 0x00102100;
    }
  }
  return (accum >> 8);
} // iriCRC

///
// call home
// uses: all.str
// rets: 0=success
int iriDial(void) {
  char str[32];
  int i;
  static char *self="iriDial";
  DBG();
  flogf(" %s", utlTime());
  utlWrite(irid.port, "at+cpas", EOL);
  if (!utlReadExpect(irid.port, all.str, "OK", 5)) return 2;
  utlWrite(irid.port, "at+clcc", EOL);
  if (!utlReadExpect(irid.port, all.str, "OK", 5)) return 3;
  utlMatchAfter(str, all.str, "+CLCC:", "0123456789");
  if (!strcmp(str, "006")==0) {
    utlWrite(irid.port, "at+chup", EOL);
    if (!utlReadExpect(irid.port, all.str, "OK", 5)) return 4;
  }
  utlRead(irid.port, all.str); // flush
  sprintf(str, "atd%s", iri.phoneNum);
  // dial
  for (i=0; i<iri.redial; i++) {
    utlWrite(irid.port, str, EOL);
    utlReadWait(irid.port, all.str, CALL_DELAY);
    DBG1("%s", all.str);
    if (strstr(all.str, "CONNECT 9600")) {
      flogf("\nCONNECTED@~%d %ldus", iri.baud, irid.usec);
      return 0;
    }
    flogf(" (%d)", i);
    utlNap(3);
  }
  utlErr(iri_err, "call retry exceeded");
  return 4;
} // iriDial

///
// send proj hdr followed by "Hello", catch landResponse
// rets: *resp<-landResp 1=retries 2=noCarrier +10=landResp +20=landCmds
// sets: irid.buf
int iriProjHello(uchar *resp) {
  static char *self="iriProjHello";
  static char *rets="1=retries 2=noCarrier +10=landResp +20=landCmds";
  int r, try, hdr=13;
  char *s=NULL;
  try = iri.hdrTry;
  while (!s) {
    if (try-- <= 0) raise(1);
    flogf(" projHdr");
    iriSendSlow(irid.projHdr, hdr);
    s = utlReadExpect(irid.port, all.str, "ACK", iri.hdrResp);
    if (strstr(all.str, "NO CARRIER")) raise(2);
  }
  flogf(" hello");
  sprintf(irid.block, "hello");
  iriSendBlock(5, 1, 1);
  if ((r = iriLandResp(resp))) raise(10+r);
  if (strstr(resp, "cmds")) {
    if ((r = iriLandCmds(resp))) raise(20+r);
    iriProcessCmds(resp);
  }
  return 0;
} // iriProjHello

///
// send chars at slower rate
// wait for elapsed time to meet baud, set timer, send char
// uses: irid.usec .port .timer
// sets: .timer
int iriSendSlow(uchar *c, int len) {
  static char *self="iriSendSlow";
  unsigned long elapsed;
  int i;
  DBG1("%s(%d):%ld", self, len, irid.usec);
  DBG3(">>>%d>>>", len);
  for (i=0; i<len; i++) {
    if (irid.usec) {
      RTCElapsedTimerSetup(&irid.timer);
      TUTxPutByte(irid.port, c[i], true);
      utlX(); // spare time
      elapsed = RTCElapsedTime(&irid.timer);
      if (elapsed < irid.usec) 
        RTCDelayMicroSeconds(irid.usec-elapsed);
    } else {
      TUTxPutByte(irid.port, c[i], true);
    }
  } // for len
  return 0;
} // iriSendSlow

///
// 3 bytes of leader which will be @@@; (three bytes of 0x40); 
// 2 bytes of crc checksum;
// 2 bytes of message length;
// 1 byte of message type;  (‘T’ or ‘I’ =Text,‘B’= Binary, ‘Z’ = Zip 
// 1 byte block number;
// 1 byte number of blocks.
// irid.block already contains msg
// uses: irid.buf .block iri.
int iriSendBlock(int bsiz, int bnum, int btot) {
  static char *self="iriSendBlock";
  static char *rets="1=inFromLand";
  int cs, size;
  DBG0("%s(%d,%d,%d)", self, bsiz, bnum, btot);
  // make hdr - beware null terminated sprintf, use memcpy
  size = bsiz+IRID_BUF_BLK-IRID_BUF_SUB;
  sprintf(all.str, "@@@CS%c%cT%c%c", 
    (char) (size>>8 & 0xFF), (char) (size & 0xFF), 
    (char) bnum, (char) btot);
  memcpy(irid.buf, all.str, IRID_BUF_BLK);
  // poke in cs high and low bytes
  cs = iriCRC(irid.buf+IRID_BUF_SUB, size);
  irid.buf[IRID_BUF_CS] = (char) (cs>>8 & 0xFF);
  irid.buf[IRID_BUF_CS+1] = (char) (cs & 0xFF);
  flogf(" %d/%d", bnum, btot);
  // send 
  if (TURxQueuedCount(irid.port)) raise(1); // junk in the trunk?
  iriSendSlow(irid.buf, bsiz+IRID_BUF_BLK); 
  if (irid.log) write(irid.log, irid.block, (long) bsiz);
  return 0;
} // iriSendBlock

///
// land ho! already did iriDial and iriProjHello
// sets: irid.block all.buf .str
int iriSendFile(char *fname) {
  static char *self="iriSendFile";
  static char *rets="1=statFail 2=openFail +10=landResp +20=landCmds";
  int r=0, fh, bytes, bcnt, bnum;
  bool moreB=true;
  struct stat fileinfo;
  long size;
  flogf("\n%s(%s)", self, fname);
  // size
  if ( stat(fname, &fileinfo) ) raise(1);
  size = (long)fileinfo.st_size;
  flogf(" {%ldB}", size);
  if (size > 1024L*iri.fileMaxKB) {
    size = 1024L*iri.fileMaxKB;
    flogf("\n%s: ERR file too large, trunc to %ld", self, size);
  }
  fh = open(fname, O_RDONLY);
  if (fh<0) raise(2);
  /// read and send 
  utlWrite(irid.port, "data", "");
  // send blocks
  bnum=(int)(size/irid.blkSz);
  DBG1("%dB/%dB", size, irid.blkSz);
  if (size%irid.blkSz) bnum += 1; // add one if partial
  for (bcnt=1; bcnt<=bnum; bcnt++) {
    bytes = read(fh, irid.block, irid.blkSz);
    iriSendBlock(bytes, bcnt, bnum);
  } 
  if (bytes) DBG1(" %dB", bytes); // size of last block
  close(fh);
  if ((r = iriLandResp(all.str))) raise(10+r);
  if (strstr(all.str, "cmds")) {
    if ((r = iriLandCmds(all.buf))) raise(20+r);
    iriProcessCmds(all.buf);
  }
  return 0;
} // iriSendFile

///
// process cmds from Land. could be a.b=c;d.e=f
// rets: 0=success #=number of fails
int iriProcessCmds(uchar *buff) {
  static char *self="iriProcessCmds";
  char *p0;
  int r=0;
  p0 = strtok(buff, ";");
  while (p0) {
    if (strstr(p0, "=")) {
      flogf("\n%s: '%s'", self, utlNonPrint(p0));
      if (cfgString(p0)) r++;
    }
    p0 = strtok(NULL, ";");
  }
  return r;
} // iriProcessCmds

///
// we just sent the last block, should get 'cmds\n' or 'done\n'
// not safe to use utlRead, inconsistent timing
// rets: 0=success 1=respTO
int iriLandResp(uchar *buff) {
  static char *self="LandResp";
  static char *rets="1=respTimeout";
  int r;
  r = utlGetUntilWait(irid.port, buff, "\n", iri.landResp);
  flogf("\n%s(%s)", self, utlNonPrint(buff));
  if (r) raise(1);
  return 0;
} // iriLandResp

///
// just got landResp(cmds), read and format land cmds
// read header, then content of cmds
// 1sec delay between "cmds" and cmd message
// 1ms delay at byte 5 (checksum)
// rets: *buff (string)
int iriLandCmds(uchar *buff) {
  static char *self="iriLandCmds";
  static char *rets="1=timeout 2=short 3=!'@@@' 4=!'C11' 5=szBad";
  static int nonMsg=5, hdr=10, respms=3000;
  unsigned char *p, myBuf[12];
  int got, msgSz;
  DBG();
  got = utlGetBlock(irid.port, myBuf, hdr, respms);
  if (got==0) raise(1);
  if (got<hdr) raisex(2);
  // 2 len bytes // block length includes hdr from size on
  p = myBuf;
  if (p[0]!='@') raisex(3);
  while (p[0]=='@') p++; // skip @ - 1,2,3 is stupid protocol
  p += 2; // skip checksum - should we check it??
  msgSz = p[0] << 8; // compiler is fussy about syntax here
  msgSz += p[1] - nonMsg;
  if (!(p[2]=='C' && p[3]==1 && p[4]==1)) raisex(4);
  if (msgSz>1024 || msgSz<1) raisex(5);
  // msg into buff
  got = utlGetBlock(irid.port, buff, msgSz, respms);
  buff[got]=0;
  flogf("\n%s(%s)", self, utlNonPrint(buff));
  return 0;
  except: {
    memcpy(buff, myBuf, got);
    utlRead(irid.port, buff+got); // get rest of it
    flogf("\n%s: unexpected '%s'", self, utlNonPrint(buff));
    return(dbg.except);
  }
} // iriLandCmds

///
// call is done
void iriHup(void) {
  static char *self="iriHup";
  Serial port=irid.port;
  int try=3;
  utlWrite(port, "done", "");
  utlReadExpect(port, all.buf, "done", 5);
  // flush, but with dbg3
  utlRead(port, all.buf);
  if (iriPrompt()==0) return;
  while (try--) {
    utlDelay(iri.hupMs);
    utlWrite(port, "+++", "");
    utlDelay(iri.hupMs);
    if (utlReadExpect(port, all.buf, "OK", 2)) break;
  }
  utlWrite(port, "at+clcc", EOL);
  if (utlReadExpect(port, all.buf, "OK", 5))
    flogf("\nclcc->%s", utlNonPrint(all.buf));
  utlWrite(port, "at+chup", EOL);
  utlReadExpect(port, all.buf, "OK", 5);
} // iriHup

///
// return: 0 success
int iriPrompt() {
  utlWrite(irid.port, "", EOL);
  TURxFlush(irid.port);
  utlWrite(irid.port, "at", EOL);
  if (utlReadExpect(irid.port, all.buf, "OK", 4)) return 0;
  else if (strstr(all.buf, "CARRIER")) return 0;
  else return 1;
} // iriPrompt

