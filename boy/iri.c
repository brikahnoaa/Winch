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
  // we malloc GpsStats instead of static to make swapping them easier
  irid.stats1 = malloc(sizeof(GpsStats));
  irid.stats2 = malloc(sizeof(GpsStats));
} // iriInit

///
// turn on, clean, set params, talk to a3la
// assumes: antStart()
// sets: irid.projHdr .blkSz .buf .block .rudUsec
int iriStart(void) {
  static char *self="iriStart";
  int cs;
  DBG0("iriStart() %s", utlTime());
  // set projHdr to 13 char project header, 0 in byte 14
  // poke in cs high and low bytes
  sprintf(irid.projHdr, "???cs%4s%4s", iri.project, iri.platform);
  cs = iriCRC(irid.projHdr+5, 8);
  irid.projHdr[3] = (char) (cs >> 8) & 0xFF;
  irid.projHdr[4] = (char) (cs & 0xFF);
  // if blkSz has changed (or first use) then alloc irid.buf
  if (irid.blkSz != iri.blkSz) {
    irid.blkSz = iri.blkSz;
    if (irid.buf) free(irid.buf);
    irid.buf = (char *) malloc(irid.blkSz+IRID_BUF_BLK);
    irid.block = irid.buf+IRID_BUF_BLK;    // offset into buf
    DBG1("\n%s: alloc irid.buf=%d", self, irid.blkSz);
  }
  // set up timing for data //  10^6 * 10bits / baud
  if (iri.baud)
    irid.rudUsec = (int) ((pow(10, 6)*10) / iri.baud);
  else irid.rudUsec = 0;
  // log?
  if (iri.logging) 
    utlLogOpen(&irid.log, "iri");
  // power up a3la
  antDevice(cf2_dev);
  TUTxPutByte(irid.port, 3, false);
  TUTxPutByte(irid.port, 'I', false);
  antDevice(a3la_dev);
  if (!utlReadExpect(irid.port, all.buf, "COMMAND MODE", 12)) return 1;
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
// get gps date time twice, err if not consistent
// rets: stats->date,time 
int iriDateTime(GpsStats *stats) {
  static char *self="iriDateTime";
  static char *rets="1=iriSats 2=inconsistent 3=crazySmall";
  GpsStats stats1;
  time_t diff, secs1, secs;
  DBG();
  if (iriSats()) raise(1);
  iriDateTimeGet(&stats1);
  iriDateTimeGet(stats);
  utlDateTimeToSecs(&secs1, stats1.date, stats1.time);
  utlDateTimeToSecs(&secs, stats->date, stats->time);
  diff = secs1 - secs;
  if (diff<5L || diff>5L) raise(2);
  if (secs<pow(10, 9)) raise(3);
  return 0;
  //
  except: {flogf(" %s", rets); return dbg.x;}
} // iriDateTime

///
// get gps date time 
// rets: stats->date,time
int iriDateTimeGet(GpsStats *stats) {
  utlWrite(irid.port, "at+pd", EOL);
  if (!utlReadExpect(irid.port, all.buf, "OK", 12)) return 2;
  utlMatchAfter(all.str, all.buf, "Date=", "-0123456789");
  flogf(" Date=%s", all.str);
  strcpy(stats->date, all.str);
  utlWrite(irid.port, "at+pt", EOL);
  if (!utlReadExpect(irid.port, all.buf, "OK", 12)) return 3;
  utlMatchAfter(all.str, all.buf, "Time=", ".:0123456789");
  flogf(" Time=%s", all.str);
  strcpy(stats->time, all.str);
  if (iri.setTime) 
    iriSetTime(stats);
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
// sets: system time
int iriSetTime(GpsStats *stats) {
  static char *self="iriSetTime";
  static char *rets="1=nullInput +10=utlDateTimeToSec";
  time_t gpsSeconds, diff;
  if (!stats->date || !stats->time) raise(1);
  if (utlDateTimeToSecs(&gpsSeconds, stats->date, stats->time)) raise(2);
  diff = time(0) - gpsSeconds;
  if (diff < -2L || diff > 2L) {
    flogf("\n%s(%s %s)\t| off by %ld seconds", 
        self, stats->date, stats->time, diff);
    RTCSetTime(gpsSeconds, NULL);
  }
  return 0;
  // 
  except: {flogf(" %s", rets); return dbg.x;}
} // iriSetTime

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
int iriCRC(char *buf, int cnt) {
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
      flogf("\nCONNECTED");
      if (irid.rudUsec)
        flogf(" baud@%d +%dus/%dB", iri.baud, irid.rudUsec, iri.sendSz);
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
// rets: *buf<-landResp 1=retries 2=noCarrier +10=landResp +20=landCmds
// sets: irid.buf
int iriProjHello(char *buf) {
  static char *self="iriProjHello";
  static char *rets="1=retries 2=noCarrier +10=landResp +20=landCmds";
  int r, try, hdr=13;
  char *s=NULL;
  try = iri.hdrTry;
  while (!s) {
    if (try-- <= 0) raise(1);
    flogf(" projHdr");
    utlWriteBlock(irid.port, irid.projHdr, hdr);
    s = utlReadExpect(irid.port, all.str, "ACK", iri.hdrResp);
    if (strstr(all.str, "NO CARRIER")) raise(2);
  }
  flogf(" hello");
  sprintf(irid.block, "hello");
  iriSendBlock(5, 1, 1);
  if ((r = iriLandResp(buf))) raise(10+r);
  if (strstr(buf, "cmds")) {
    if ((r = iriLandCmds(buf))) raise(20+r);
    iriProcessCmds(buf);
  }
  return 0;
  //
  except: {flogf(" %s", rets); return dbg.x;}
} // iriProjHello

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
  int cs, i, send, size;
  long uDelay;
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
  // send hdr
  if (irid.log) 
    write(irid.log, utlNonPrintBlock(irid.buf, IRID_BUF_BLK), IRID_BUF_BLK);
  TUTxPutBlock(irid.port, irid.buf, (long) IRID_BUF_BLK, 9999);
  utlDelay(iri.hdrPause);
  // send data
  // pause every send# chars to slow down baud stream
  send = iri.sendSz;
  uDelay = (long) send * irid.rudUsec;
  for (i=0; i<bsiz; i+=send) {
    if (TURxQueuedCount(irid.port)) raise(1); // junk in the trunk?
    if (i+send>bsiz) send = bsiz-i; // last chunk
    TUTxPutBlock(irid.port, irid.block+i, (long) send, 9999);
    if (irid.log) 
      write(irid.log, irid.block+i, (long) send);
    // extra delay us per byte to emulate lower baud rate
    RTCDelayMicroSeconds(uDelay);
    utlX();
  }
  RTCDelayMicroSeconds(iri.blkPause);
  return 0;
  //
  except: {flogf(" %s", rets); return dbg.x;}
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
  // block & buf, size could change during run
  if (irid.blkSz != iri.blkSz) {
    DBG1("\n%s: setting blkSize to %d", self, iri.blkSz);
    irid.blkSz = iri.blkSz;
    if (irid.block) free(irid.block);
    if (irid.buf) free(irid.buf);
    irid.block = malloc(iri.blkSz);
    irid.buf = malloc(iri.blkSz + IRID_BUF_BLK);
  }
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
  utlDelay(iri.filePause);
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
  //
  except: {flogf(" %s", rets); return dbg.x;}
} // iriSendFile

///
// process cmds from Land. could be a.b=c;d.e=f
// rets: 0=success #=number of fails
int iriProcessCmds(char *buff) {
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
int iriLandResp(char *buff) {
  static char *self="LandResp";
  static char *rets="1=respTimeout";
  if (utlGetUntilWait(irid.port, buff, "\n", iri.landResp)) raise(1);
  flogf("\n%s(%s)", self, utlNonPrint(buff));
  return 0;
  //
  except: {flogf(" %s", rets); return dbg.x;}
} // iriLandResp

///
// just got landResp(cmds), read and format land cmds
// 1sec delay between "cmds" and cmd message
// 1ms delay at byte 5 (checksum)
int iriLandCmds(char *buff) {
  static char *self="iriLandCmds";
  static char *rets="1=hdrShort 2=!'@@@' 3=!'C11' 4=szBad";
  static int nonMsg=5, hdr=10;
  static short respms=4000;
  unsigned char *p, myBuf[12];
  int got, msgSz;
  DBG();
  got = (int) TURxGetBlock(irid.port, myBuf, (long)hdr, (short)respms);
  DBG3(" <[%d'%s'<[", got, utlNonPrintBlock(myBuf, got));
  if (got<hdr) raise(1);
  // 2 len bytes // block length includes hdr from size on
  p = myBuf;
  if (p[0]!='@') raise(2);
  while (p[0]=='@') p++; // skip @
  p += 2; // skip checksum - should we check it??
  msgSz = p[0] << 8; // compiler is fussy about syntax here
  msgSz += p[1] - nonMsg;
  if (!(p[2]=='C' && p[3]==1 && p[4]==1)) raise(3);
  if (msgSz>1024 || msgSz<1) raise(4);
  // msg into buff
  got = (int) TURxGetBlock(irid.port, buff, (long)msgSz, (short)respms);
  DBG3(" <[%d'%s'<[", got, utlNonPrintBlock(buff, got));
  // cmds
  flogf("\n%s(%s)", self, utlNonPrint(buff));
  return 0;
  //
  except: {flogf(" %s", rets); return dbg.x;}
} // iriLandCmds

///
// call is done
void iriHup(void) {
  int try=3;
  utlWrite(irid.port, "done", "");
  while (try--) {
    utlDelay(iri.hupMs);
    utlWrite(irid.port, "+++", "");
    utlDelay(iri.hupMs);
    if (utlReadExpect(irid.port, all.buf, "OK", 2)) break;
  }
  utlWrite(irid.port, "at+clcc", EOL);
  if (utlReadExpect(irid.port, all.buf, "OK", 5))
    flogf("\nclcc->%s", utlNonPrint(all.buf));
  utlWrite(irid.port, "at+chup", EOL);
  utlReadExpect(irid.port, all.buf, "OK", 5);
} // iriHup

///
// return: 0 success
int iriPrompt() {
  TURxFlush(irid.port);
  utlWrite(irid.port, "at", EOL);
  if (!utlReadExpect(irid.port, all.buf, "OK", 4)) return 1;
  else return 0;
} // iriPrompt

///
// includes logging in block writes
// uses: irid.port .log
int iriSend(char *buff, long len) {
  TUTxPutBlock(irid.port, buff, len, 9999);
  if (irid.log) write(irid.log, buff, len);
  return len;
}
