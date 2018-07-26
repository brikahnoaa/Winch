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
  gps.projHdr[3] = (char) (cs >> 8) & 0x00FF;
  gps.projHdr[4] = (char) (cs & 0x00FF);
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
    gps.setTime = false;
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
  if (diff < -1 || diff > 1) {
    flogf("\ngpsSetTime()\t| off by %ld seconds", diff);
    RTCSetTime(gpsSeconds, NULL);
  }
  return true;
} // gpsSetTime
  

///
// sets: gps.sats
// rets: 0=success
int gpsSats(void){
  antSwitch(gps_ant);
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
  return 2;
} // gpsSats

///
// sets: gps.signal
// rets: 0=success
int iridSig(void) {
  flogf("iridSig()");
  antSwitch(irid_ant);
  tmrStart(gps_tmr, gps.timeout);
  while (!tmrExp(gps_tmr)) {
    utlWrite(gps.port, "at+csq", EOL);
    if (!utlExpect(gps.port, utlBuf, "OK", 12)) return 1;
    if (utlMatchAfter(utlStr, utlBuf, "CSQ:", "0123456789")) {
      gps.signal = atoi(utlStr);
      flogf(" csq=%d", gps.signal);
      if (gps.signal>gps.signalMin) return 0;
    } // if CSQ
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
  flogf("\niridDial() %s", utlTime());
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
      return 0;
    }
    flogf(" (%d)", i);
  }
  utlErr(gps_err, "call retry exceeded");
  return 4;
} // iridDial

///
// create a block of zero, send
// uses: utlBuf=zero utlRet=comm
int iridSendTest(int msgLen) {
  int hdr1=13, hdr2=10, hdrTry=3, hdrPause=10;
  int min=48;
  int cs, i, bufLen;
  char *s;
  char land[128];
  RTCTimer rt;
  if (msgLen<min)
    msgLen = min;
  DBG0("iridSendTest(%d)", msgLen)
  bufLen = msgLen+hdr2;
  msgLen += 5;
  memset(utlBuf, 0, bufLen);
  DBG2("%s", utlNonPrint(gps.projHdr))
  // make data
  // 3 bytes of leader which will be @@@; (three bytes of 0x40); 
  // 2 bytes of crc checksum;
  // 2 bytes of message length;
  // 1 byte of message type;  (‘T’ or ‘I’ =Text,‘B’= Binary, ‘Z’ = Zip 
  // 1 byte block number;
  // 1 byte number of blocks.
  sprintf(utlBuf, "@@@cs%c%cT%c%c", 
    (char) msgLen>>8, (char) msgLen & 0xFF, (char) 1, (char) 1);
  sprintf(utlBuf+20, "Detected %d %s", global.det, utlDateTime());
  // poke in cs high and low bytes
  cs = iridCRC(utlBuf+5, bufLen-5);
  utlBuf[3] = (char) (cs >> 8);
  utlBuf[4] = (char) (cs & 0xFF);
  DBG2("%s", utlNonPrint(utlBuf))
  while (hdrTry--) {
    flogf(" projHdr");
    utlWriteBlock(gps.port, gps.projHdr, hdr1);
    s = utlExpect(gps.port, land, "ACK", hdrPause);
    if (s) {
      DBG4("(%d)", s)
      break;
    }
  }
  if (hdrTry < 0) return 2;
  // send data
  flogf(" data");
  utlWriteBlock(gps.port, utlBuf, bufLen);
  // land ho!
  utlBuf[0] = 0;
  flogf("\nland->");
  tmrStart(rudics_tmr, gps.rudResp);
  RTCElapsedTimerSetup(&rt);
  while (!tmrExp(rudics_tmr)) {
    if (TURxQueuedCount(gps.port)) {
      i = utlRead(gps.port, utlBuf);
      flogf(" (%ld %d %s)", RTCElapsedTime(&rt)/1000, i, 
        utlNonPrintBlock(utlBuf, i));
    }
  }
  utlWrite(gps.port, "done", NULL);
  return 0;
} // iridSendTest

///
// call done
void iridHup(void) {
  int try=3;
  while (try--) {
    utlWriteBlock(gps.port, "+++", 3);
    utlDelay(500);
    utlWrite(gps.port, "at", EOL);
    if (utlExpect(gps.port, utlBuf, "OK", 5)) break;
  }
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

