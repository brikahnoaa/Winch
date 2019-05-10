// gps.c
#include <utl.h>
#include <gps.h>
#include <ant.h>
#include <tmr.h>
#include <cfg.h>

// gps and iridium routines have a lot of ways to fail; return 0=success
//
#define EOL "\r"
#define CALL_DELAY 20

GpsInfo gps;


///
// set up for gps, call this after gpsInit
// sets: gps.port .projHdr[]
void gpsInit(void) {
  int cs;
  static char *self="gpsInit";
  DBG()
  gps.port = antPort();
  if (!gps.port)
    utlErr(gps_err, "no gps.port, was gpsInit called before antInit?");
  gps.buf = malloc((size_t)1024 * 32);
  if (!gps.buf)
    utlErr(gps_err, "cannot malloc() - heap full?");
  gps.buf[0] = 0;
  // sets projHdr to 13 char project header, 0 in byte 14
  sprintf(gps.projHdr, "???cs%4s%4s", gps.project, gps.platform);
  // poke in cs high and low bytes
  cs = iridCRC(gps.projHdr+5, 8);
  gps.projHdr[3] = (char) (cs >> 8) & 0xFF;
  gps.projHdr[4] = (char) (cs & 0xFF);
} // gpsInit

///
// turn on, clean, set params, talk to sbe39
// requires: antStart
int gpsStart(void) {
  DBG0("gpsStart() %s", utlTime())
  antDevice(cf2_dev);
  // power up a3la
  TUTxPutByte(gps.port, 3, false);
  TUTxPutByte(gps.port, 'I', false);
  antDevice(a3la_dev);
  if (!utlExpect(gps.port, all.buf, "COMMAND MODE", 12)) return 1;
  utlWrite(gps.port, "ate0", EOL);
  if (!utlExpect(gps.port, all.str, "OK", 5)) return 2;
  return 0;
} // gpsStart

///
// turn off power to gpsmod
void gpsStop(void) {
  static char *self="gpsStop";
  DBG()
  if (gps.log)
    utlLogClose(&gps.log);
  antDevice(a3la_dev);
  utlWrite(gps.port, "at*p", EOL);
  utlExpect(gps.port, all.buf, "OK", 2);
  antDevice(cf2_dev);
  TUTxPutByte(gps.port, 4, false);      // ^D powerdown
  TUTxPutByte(gps.port, 'I', false);      // S | I
} // gpsStop

///
// get gps .date .time 
// sets: .date .time 
// return: 0=success
int gpsDateTime(GpsStats *stats){
  static char *self="gpsDateTime";
  DBG()
  if (gpsSats()) return 1;
  // date
  utlWrite(gps.port, "at+pd", EOL);
  if (!utlExpect(gps.port, all.buf, "OK", 12)) return 2;
  utlMatchAfter(all.str, all.buf, "Date=", "-0123456789");
  flogf(" Date=%s", all.str);
  strcpy(stats->date, all.str);
  // time
  utlWrite(gps.port, "at+pt", EOL);
  if (!utlExpect(gps.port, all.buf, "OK", 12)) return 3;
  utlMatchAfter(all.str, all.buf, "Time=", ".:0123456789");
  flogf(" Time=%s", all.str);
  strcpy(stats->time, all.str);
  if (gps.setTime) 
    gpsSetTime(stats);
  return 0;
} // gpsDateTime

// get gps .lat .lng
// sets: .lng .lat 
// return: 0=success
int gpsLatLng(GpsStats *stats){
  static char *self="gpsLatLng";
  DBG()
  if (gpsSats()) return 1;
  utlWrite(gps.port, "at+pl", EOL);
  if (!utlExpect(gps.port, all.buf, "OK", 12)) return 4;
  utlMatchAfter(all.str, all.buf, "Latitude=", ".:0123456789 NEWS");
  flogf(" Lat=%s", all.str);
  strcpy(stats->lat, all.str);
  utlMatchAfter(all.str, all.buf, "Longitude=", ".:0123456789 NEWS");
  flogf(" Lng=%s", all.str);
  strcpy(stats->lng, all.str);
  return 0;
} // gpsLatLng

///
// sets: system time
bool gpsSetTime(GpsStats *stats) {
  struct tm t;
  time_t gpsSeconds, diff;
  char *s;
  DBG0("gpsSetTime(%s %s)", stats->date, stats->time);
  if (!stats->date || !stats->time) {
    flogf("\ngpsSetTime()\t| called with null data");
    return false;
  }
  strcpy(all.str, stats->date);
  if (!(s = strtok(all.str, " -:."))) return false;
  t.tm_mon = atoi(s) - 1;
  if (!(s = strtok(NULL, " -:."))) return false;
  t.tm_mday = atoi(s);
  if (!(s = strtok(NULL, " -:."))) return false;
  t.tm_year = atoi(s) - 1900;
  strcpy(all.str, stats->time);
  if (!(s = strtok(all.str, " -:."))) return false;
  t.tm_hour = atoi(s);
  if (!(s = strtok(NULL, " -:."))) return false;
  t.tm_min = atoi(s);
  if (!(s = strtok(NULL, " -:."))) return false;
  t.tm_sec = atoi(s);
  gpsSeconds = mktime(&t);
  diff = time(0) - gpsSeconds;
  if (diff < -2L || diff > 2L) {
    flogf("\ngpsSetTime()\t| off by %ld seconds", diff);
    RTCSetTime(gpsSeconds, NULL);
  }
  return true;
} // gpsSetTime
  

///
// sets: gps.sats
// rets: 0=success
int gpsSats(void){
  tmrStart(gps_tmr, gps.timeout);
  while (!tmrExp(gps_tmr)) {
    utlNap(2);
    utlWrite(gps.port, "at+pd", EOL);
    if (!utlExpect(gps.port, all.buf, "OK", 12)) return 1;
    if (!strstr(all.buf, "Invalid Position") && strstr(all.buf, "Used=")) {
      tmrStop(gps_tmr);
      utlMatchAfter(all.str, all.buf, "Used=", "0123456789");
      flogf("\nGPS Sats=%s", all.str);
      gps.sats = atoi(all.str);
      return 0;
    }
  } // while timeout
  flogf("\ngpsSats\t| i got nothing");
  flogf("\n'%s'", all.buf);
  return 2;
} // gpsSats

///
// sets: gps.signal
// rets: 0=success
int iridSig(void) {
  flogf("\niridSig()");
  tmrStart(gps_tmr, gps.timeout);
  while (!tmrExp(gps_tmr)) {
    utlWrite(gps.port, "at+csq", EOL);
    if (!utlExpect(gps.port, all.buf, "OK", 12)) return 1;
    if (utlMatchAfter(all.str, all.buf, "CSQ:", "0123456789")) {
      gps.signal = atoi(all.str);
      flogf(" csq=%d", gps.signal);
      if (gps.signal>gps.signalMin) return 0;
    } // if CSQ
  // accept min signal if its all we got
  if (gps.signal>=gps.signalMin) return 0;
  } // while timer
  return 2;
} // iridSig

///
int iridCRC(char *buf, int cnt) {
  long accum=0x00000000;
  int i, j;
  static char *self="iridCRC";
  DBG()
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
} // iridCRC

///
// call home
// uses: all.str
// rets: 0=success
int iridDial(void) {
  char str[32];
  int i;
  static char *self="iridDial";
  DBG()
  flogf(" %s", utlTime());
  // set up timing for data
  //  10^6 * 10bits / rudBaud
  gps.rudUsec = (int) ((pow(10, 6)*10) / gps.rudBaud);
  utlWrite(gps.port, "at+cpas", EOL);
  if (!utlExpect(gps.port, all.str, "OK", 5)) return 2;
  utlWrite(gps.port, "at+clcc", EOL);
  if (!utlExpect(gps.port, all.str, "OK", 5)) return 3;
  utlMatchAfter(str, all.str, "+CLCC:", "0123456789");
  if (!strcmp(str, "006")==0) {
    utlWrite(gps.port, "at+chup", EOL);
    if (!utlExpect(gps.port, all.str, "OK", 5)) return 4;
  }
  sprintf(str, "atd%s", gps.phoneNum);
  // dial
  for (i=0; i<gps.redial; i++) {
    // fails "NO CONNECT" without this pause
    utlNap(4);
    // flush
    utlRead(gps.port, all.str);
    utlWrite(gps.port, str, EOL);
    utlReadWait(gps.port, all.str, CALL_DELAY);
    DBG1("%s", all.str);
    if (strstr(all.str, "CONNECT 9600")) {
      flogf(" CONNECT 9600");
      flogf("rudBaud@%d +%dus ", gps.rudBaud, gps.rudUsec);
      return 0;
    }
    flogf(" (%d)", i);
  }
  utlErr(gps_err, "call retry exceeded");
  return 4;
} // iridDial

///
// send proj hdr
int iridProjHdr(void) {
  int try, hdr=13;
  char *s=NULL;
  try = gps.hdrTry;
  while (!s) {
    if (try-- <= 0) return 1;
    flogf(" proj");
    utlWriteBlock(gps.port, gps.projHdr, hdr);
    s = utlExpect(gps.port, all.str, "ACK", gps.hdrPause);
  }
  flogf("\n");
  return 0;
} // iridProjHdr

///
// send block Num of Many
// rets: 0=success 1=gps.hdrTry 2=block fail
int iridSendBlock(char *msg, int msgSz, int blockNum, int blockMany) {
  int hdr=10;
  int cs, i, bufSz, blockSz, sendSz;
  long uDelay;
  char *buff;
  DBG0("iridSendBlock(%d,%d,%d)", msgSz, blockNum, blockMany)
  bufSz = msgSz+hdr;
  blockSz = msgSz+5;
  buff = malloc(bufSz);
  DBG2("projHdr:%s", utlNonPrint(gps.projHdr))
  // make data
  // 3 bytes of leader which will be @@@; (three bytes of 0x40); 
  // 2 bytes of crc checksum;
  // 2 bytes of message length;
  // 1 byte of message type;  (‘T’ or ‘I’ =Text,‘B’= Binary, ‘Z’ = Zip 
  // 1 byte block number;
  // 1 byte number of blocks.
  sprintf(buff, "@@@CS%c%cT%c%c", 
    (char) (blockSz>>8 & 0xFF), (char) (blockSz & 0xFF), 
    (char) blockNum, (char) blockMany);
  memcpy(buff+hdr, msg, msgSz);
  // poke in cs high and low bytes
  cs = iridCRC(buff+5, bufSz-5);
  buff[3] = (char) (cs>>8 & 0xFF);
  buff[4] = (char) (cs & 0xFF);
  flogf(" %d/%d", blockNum, blockMany);
  // send data
  sendSz = gps.sendSz;
  uDelay = (long) sendSz * gps.rudUsec;
  DBG4(" {%d %d %ld}", sendSz, gps.rudUsec, uDelay)
  for (i=0; i<bufSz; i+=sendSz) {
    // TUTxPutByte(gps.port, buff[i], false);
    // RTCDelayMicroSeconds((long) gps.rudUsec);
    if (i+sendSz>bufSz) {
      sendSz = bufSz-i;
      uDelay = (long) sendSz * gps.rudUsec;
    }
    TUTxPutBlock(gps.port, buff+i, (long) sendSz, 9999);
    // extra delay us per byte to emulate lower baud rate
    RTCDelayMicroSeconds(uDelay);
    utlX();
  }
  return 0;
} // iridSendBlock

///
// land ho! already did iridDial and iridProjHdr
// send fname as separate files of max gps.fileMax
// sets: gps.buf=malloc
int iridSendFile(char *fname) {
  static char *self="iridSendFile";
  static char *rets="1:!file +10:!resp r:LandCmds";
  int r=0, l, fh, len, block;
  struct stat fileinfo;
  flogf("\n%s(%s)", self, fname);
  fh = open(fname, O_RDONLY);
  if (fh<0) {
    flogf("\nERR\t| %s cannot open %s", self, fname);
    return 1;
  }
  stat(fname, &fileinfo);
  len = fileinfo.st_size;
  // read whole file into memory
  if (len>=gps.fileMax)
    block = read(fh, gps.buf, gps.fileMax);
  else
    block = read(fh, gps.buf, len);
  close(fh);
  // ?? send multiple blocks
  iridSendBlock(gps.buf, block, 1, 1);
  flogf(" [[%d/%d]]", block, len);
  if ((r = iridLandResp(all.str))) 
    return 10+r;
  if (strstr(all.str, "cmds"))
    r = iridLandCmds(gps.buf, &l);
  gps.buf[l] = 0;
  iridProcessCmds(gps.buf);
  utlWrite(gps.port, "data", "");
  return r;
} // iridSendFile

///
// process cmds from Land. could be a.b=c;d.e=f
int iridProcessCmds(char *buff) {
  char *p0;
  int r=0;
  p0 = strtok(buff, ";");
  while (p0) {
    if (strstr(p0, "=")) {
      flogf("\nsetting '%s'", utlNonPrint(p0));
      if (cfgString(p0))
        r++;
    }
    p0 = strtok(NULL, ";");
  }
  return r;
} // iridProcessCmds

///
// we just sent the last block, should get cmds or done
// looks like a bug in how "cmds" is sent, sometimes 0x0d in front
// 5 char the first time, six after; read until \n 0x0a
// rets: 0=success 1=respTO
int iridLandResp(char *buff) {
  int r, len=6;
  tmrStart(gps_tmr, gps.rudResp);
  memset(buff, 0, len);
  for (r=0; r<len; r++) {
    while (TURxQueuedCount(gps.port)==0)
      if (tmrExp(gps_tmr)) return 1;
    buff[r] = TURxGetByte(gps.port, true);
    if (buff[r]==0x0A)
      break;
  }
  buff[r]=0;
  flogf("\nLandResp(%s)", utlNonPrint(buff));
  if (!strstr(buff, "cmds") && !strstr(buff, "done"))
    return 2;
  return 0;
} // iridLandResp

///
// read and format land cmds
// rets: *buff\0 0=success 1=@TO 2=0@ 3=hdrTO 4=hdr!C11 
int iridLandCmds(char *buff, int *len) {
  int i, hdr=7;
  unsigned char c;
  short msgSz;
  static char *self="iridLandCmds";
  DBG()
  tmrStart(gps_tmr, gps.rudResp);
  *len = 0;
  // skip @@@ @@ or @ - protocol is sloppy, first CS byte could = @
  for (i=0; i<3; i++) {
    // wait for byte
    while (TURxQueuedCount(gps.port)==0)
      if (tmrExp(gps_tmr)) return 1;
    c = TURxPeekByte(gps.port, 0);
    DBG4("%s", utlNonPrintBlock(&c,1))
    if (c=='@') 
      TURxGetByte(gps.port, false);
    else 
      break;
  }
  if (i==0) return 2;
  // @@@ 2 CS bytes, 2 len bytes, 'C', 1, 1
  for (i=0; i<hdr; i++) {
    // wait for byte
    while (TURxQueuedCount(gps.port)==0)
      if (tmrExp(gps_tmr)) return 3;
    buff[i]=TURxGetByte(gps.port, false);
  }
  // 2 CS bytes
  // 2 len bytes
  msgSz = buff[2];
  msgSz <<= 8;
  msgSz += buff[3];
  // block length = msgSz + hdr - CS
  msgSz = msgSz - (hdr - 2);
  // C 1 1
  if (!(buff[4]=='C' && buff[5]==1 && buff[6]==1)) 
    return 4;
  // cmds
  *len = msgSz;
  memset(buff, 0, msgSz+1);
  for (i=0; i<msgSz; i++) {
    // wait for byte
    while (TURxQueuedCount(gps.port)==0)
      if (tmrExp(gps_tmr)) return 5;
    buff[i]=TURxGetByte(gps.port, false);
  }
  flogf("\nCMDS(%d)->", msgSz);
  flogf("''%s''", utlNonPrintBlock(buff, msgSz));
  return 0;
} // iridLandCmds

///
// call done
void iridHup(void) {
  int try=3;
  while (try--) {
    utlDelay(gps.hupMs);
    utlWriteBlock(gps.port, "+++", 3);
    utlDelay(gps.hupMs);
    if (utlExpect(gps.port, all.buf, "OK", 2)) break;
  }
  utlWrite(gps.port, "at+clcc", EOL);
  if (utlExpect(gps.port, all.buf, "OK", 5))
    flogf("\nclcc->%s", utlNonPrint(all.buf));
  utlWrite(gps.port, "at+chup", EOL);
  utlExpect(gps.port, all.buf, "OK", 5);
} // iridHup

///
// return: 0 success
int iridPrompt() {
  TURxFlush(gps.port);
  utlWrite(gps.port, "at", EOL);
  if (!utlExpect(gps.port, all.buf, "OK", 4)) return 1;
  else return 0;
} // iridPrompt

