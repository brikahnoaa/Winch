// gps.c
#include <utl.h>
#include <gps.h>
#include <ant.h>
#include <tmr.h>

extern GlobalInfo global;

// gps and iridium routines have a lot of ways to fail, so return 0 or errcode
//
#define EOL "\r"
#define CALL_DELAY 20

GpsInfo gps;


///
// set up for gps, call this after gpsInit
// sets: gps.port .projHdr[]
void gpsInit(void) {
  int cs;
  DBG0("gpsInit()")
  gps.port = antPort();
  if (!gps.port)
    utlErr(gps_err, "no gps.port, was gpsInit called before antInit?");
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
  if (!utlExpect(gps.port, utlBuf, "COMMAND MODE", 12)) return 1;
  utlWrite(gps.port, "ate0", EOL);
  if (!utlExpect(gps.port, utlStr, "OK", 5)) return 2;
  return 0;
} // gpsStart

///
// turn off power to gpsmod
void gpsStop(void) {
  DBG0("gpsStop()")
  if (gps.log)
    close(gps.log);
  gps.log = 0;
  antDevice(a3la_dev);
  utlWrite(gps.port, "at*p", EOL);
  utlExpect(gps.port, utlBuf, "OK", 2);
  antDevice(cf2_dev);
  TUTxPutByte(gps.port, 4, false);      // ^D powerdown
  TUTxPutByte(gps.port, 'I', false);      // S | I
} // gpsStop

///
// get gps info; date, time, lon, lat
// sets: .date .time .lng .lat
// return: 0=success
int gpsStats(void){
  DBG0("gpsStats()")
  if (gpsSats()) return 1;
  // date
  utlWrite(gps.port, "at+pd", EOL);
  if (!utlExpect(gps.port, utlBuf, "OK", 12)) return 2;
  utlMatchAfter(utlStr, utlBuf, "Date=", "-0123456789");
  flogf(" Date=%s", utlStr);
  strcpy(gps.date, utlStr);
  // time
  utlWrite(gps.port, "at+pt", EOL);
  if (!utlExpect(gps.port, utlBuf, "OK", 12)) return 3;
  utlMatchAfter(utlStr, utlBuf, "Time=", ".:0123456789");
  flogf(" Time=%s", utlStr);
  strcpy(gps.time, utlStr);
  if (gps.setTime) {
    gpsSetTime();
  }
  flogf("\n");
  // lat lng
  utlWrite(gps.port, "at+pl", EOL);
  if (!utlExpect(gps.port, utlBuf, "OK", 12)) return 4;
  utlMatchAfter(utlStr, utlBuf, "Latitude=", ".:0123456789 NEWS");
  flogf(" Lat=%s", utlStr);
  strcpy(gps.lat, utlStr);
  utlMatchAfter(utlStr, utlBuf, "Longitude=", ".:0123456789 NEWS");
  flogf(" Lng=%s", utlStr);
  strcpy(gps.lng, utlStr);
  return 0;
} // gpsStats

///
// uses: .date .time
// sets: system time
bool gpsSetTime(void) {
  struct tm t;
  time_t gpsSeconds, diff;
  char *s;
  DBG0("gpsSetTime(%s %s)", gps.date, gps.time);
  if (!gps.date || !gps.time) {
    flogf("\ngpsSetTime()\t| called with null data");
    return false;
  }
  strcpy(utlStr, gps.date);
  if (!(s = strtok(utlStr, " -:."))) return false;
  t.tm_mon = atoi(s) - 1;
  if (!(s = strtok(NULL, " -:."))) return false;
  t.tm_mday = atoi(s);
  if (!(s = strtok(NULL, " -:."))) return false;
  t.tm_year = atoi(s) - 1900;
  strcpy(utlStr, gps.time);
  if (!(s = strtok(utlStr, " -:."))) return false;
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
    if (!utlExpect(gps.port, utlBuf, "OK", 12)) return 1;
    if (!strstr(utlBuf, "Invalid Position") && strstr(utlBuf, "Used=")) {
      tmrStop(gps_tmr);
      utlMatchAfter(utlStr, utlBuf, "Used=", "0123456789");
      flogf("\nGPS Sats=%s", utlStr);
      gps.sats = atoi(utlStr);
      return 0;
    }
  } // while timeout
  flogf("\ngpsSats\t| i got nothing");
  flogf("\n'%s'", utlBuf);
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
    if (!utlExpect(gps.port, utlBuf, "OK", 12)) return 1;
    if (utlMatchAfter(utlStr, utlBuf, "CSQ:", "0123456789")) {
      gps.signal = atoi(utlStr);
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
  DBG0("iridCRC()")
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
// uses: utlStr
// rets: 0=success
int iridDial(void) {
  char str[32];
  int i;
  DBG0("iridDial()")
  flogf(" %s", utlTime());
  // set up timing for data
  //  10^6 * 10bits / rudBaud
  gps.rudUsec = (int) ((pow(10, 6)*10) / gps.rudBaud);
  utlWrite(gps.port, "at+cpas", EOL);
  if (!utlExpect(gps.port, utlStr, "OK", 5)) return 2;
  utlWrite(gps.port, "at+clcc", EOL);
  if (!utlExpect(gps.port, utlStr, "OK", 5)) return 3;
  utlMatchAfter(str, utlStr, "+CLCC:", "0123456789");
  if (!strcmp(str, "006")==0) 
    utlWrite(gps.port, "at+chup", EOL);
  sprintf(str, "atd%s", gps.phoneNum);
  // dial
  for (i=0; i<gps.redial; i++) {
    utlNap(3);
    utlWrite(gps.port, str, EOL);
    utlReadWait(gps.port, utlStr, CALL_DELAY);
    DBG1("%s", utlStr);
    if (strstr(utlStr, "CONNECT 9600")) {
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
    s = utlExpect(gps.port, utlStr, "ACK", gps.hdrPause);
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
  flogf(" %dof%d", blockNum, blockMany);
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
// send file in chunks of gps.blockSize
int iridSendFile(char *fname) {
  // land ho!
  DBG0("iridSendFile(%s)", fname)
  // cmds0x0A
  TURxGetBlock(gps.port, utlBuf, 5, gps.rudResp*1000);
  utlBuf[4] = 0;
  if (strstr(utlBuf, "cmds")) 
    iridLandCmds(utlBuf);
  utlWrite(gps.port, "done", NULL);
  utlExpect(gps.port, utlBuf, "done", 5);
  utlWrite(gps.port, "done", NULL);
  return 0;
} // iridSendFile

///
// we just sent the last block, should get cmds or data directive
int iridLandResp(char *buff) {
  int r, len=5;
  tmrStart(gps_tmr, gps.rudResp);
  memset(buff, 0, 9);
  for (r=0; r<5; r++) {
    while (TURxQueuedCount(gps.port)==0)
      if (tmrExp(gps_tmr)) return 0;
    buff[r] = TURxGetByte(gps.port, true);
  }
  flogf("\niridLandResp(%s)", utlNonPrintBlock(buff,r));
  return r;
} // iridLandResp

///
// read and format land cmds
int iridLandCmds(char *buff) {
  int i, hdr=7;
  unsigned char c;
  short len, msgSz;
  DBG0("iridLandCmds()")
  tmrStart(gps_tmr, gps.rudResp);
  // skip @@@ @@ or @ - protocol is bad, first CS byte could = @
  for (i=0; i<3; i++) {
    // wait for byte
    while (TURxQueuedCount(gps.port)==0)
      if (tmrExp(gps_tmr)) return 0;
    c = TURxPeekByte(gps.port, 0);
    DBG4("%s", utlNonPrintBlock(&c,1))
    if (c=='@') 
      TURxGetByte(gps.port, false);
    else 
      break;
  }
  if (i==0) return 0;
  // @@@ 2 CS bytes, 2 len bytes, 'C', 1, 1
  for (i=0; i<hdr; i++) {
    // wait for byte
    while (TURxQueuedCount(gps.port)==0)
      if (tmrExp(gps_tmr)) return 0;
    buff[i]=TURxGetByte(gps.port, false);
  }
  DBG4("hdr(%s)", utlNonPrintBlock(buff, hdr))
  // 2 CS bytes
  DBG4("CS(%s)", utlNonPrintBlock(buff, 2))
  // 2 len bytes
  len = buff[2];
  len <<= 8;
  len += buff[3];
  // block len = msgSz + hdr - CS
  msgSz = len - (hdr - 2);
  // C 1 1
  if (!(buff[4]=='C' && buff[5]==1 && buff[6]==1)) {
    utlErr(rud_err, "land response header err");
    return 0;
  }
  DBG4("len(%d)", msgSz)
  // cmds
  for (i=0; i<msgSz; i++) {
    // wait for byte
    while (TURxQueuedCount(gps.port)==0)
      if (tmrExp(gps_tmr)) return 0;
    buff[i]=TURxGetByte(gps.port, false);
  }
  flogf("\nland(%d)->", i);
  flogf("''%s''", utlNonPrintBlock(buff, i));
  return i;
} // iridLandCmds

///
// call done
void iridHup(void) {
  int try=3;
  while (try--) {
    utlNap(1);
    utlWriteBlock(gps.port, "+++", 3);
    utlNap(1);
    if (utlExpect(gps.port, utlBuf, "OK", 2)) break;
  }
  utlWrite(gps.port, "at+clcc", EOL);
  utlWrite(gps.port, "at+chup", EOL);
  utlExpect(gps.port, utlBuf, "OK", 5);
} // iridHup

///
// return: 0 success
int iridPrompt() {
  TURxFlush(gps.port);
  utlWrite(gps.port, "at", EOL);
  if (!utlExpect(gps.port, utlBuf, "OK", 4)) return 1;
  else return 0;
} // iridPrompt

