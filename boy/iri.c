// iri.c
#include <main.h>

// gps and iridium routines have a lot of ways to fail; return 0=success
//
#define EOL "\r"
#define CALL_DELAY 20
// OFF is where checksum starts, HDR is total block header size
#define IRID_OFF 5
#define IRID_HDR 10

IriInfo iri;
IriData irid;


///
// set up for iri, call this after iriInit
// sets: irid.port .projHdr[]
void iriInit(void) {
  int cs;
  static char *self="iriInit";
  DBG();
  irid.port = antPort();
  if (!irid.port)
    utlErr(iri_err, "no irid.port, was iriInit called before antInit?");
  // we malloc GpsStats instead of static to make swapping them easier
  irid.stats1 = malloc(sizeof(GpsStats));
  irid.stats2 = malloc(sizeof(GpsStats));
  // sets projHdr to 13 char project header, 0 in byte 14
  sprintf(irid.projHdr, "???cs%4s%4s", iri.project, iri.platform);
  // poke in cs high and low bytes
  cs = iriCRC(irid.projHdr+5, 8);
  irid.projHdr[3] = (char) (cs >> 8) & 0xFF;
  irid.projHdr[4] = (char) (cs & 0xFF);
} // iriInit

///
// turn on, clean, set params, talk to sbe39
// requires: antStart
int iriStart(void) {
  DBG0("iriStart() %s", utlTime());
  antDevice(cf2_dev);
  // power up a3la
  TUTxPutByte(irid.port, 3, false);
  TUTxPutByte(irid.port, 'I', false);
  antDevice(a3la_dev);
  if (!utlExpect(irid.port, all.buf, "COMMAND MODE", 12)) return 1;
  utlWrite(irid.port, "ate0", EOL);
  if (!utlExpect(irid.port, all.str, "OK", 5)) return 2;
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
  utlExpect(irid.port, all.buf, "OK", 2);
  antDevice(cf2_dev);
  TUTxPutByte(irid.port, 4, false);      // ^D powerdown
  TUTxPutByte(irid.port, 'I', false);      // S | I
} // iriStop

///
// get gps .date .time 
// sets: .date .time 
// return: 0=success
int iriDateTime(GpsStats *stats){
  static char *self="iriDateTime";
  DBG();
  if (iriSats()) return 1;
  // date
  utlWrite(irid.port, "at+pd", EOL);
  if (!utlExpect(irid.port, all.buf, "OK", 12)) return 2;
  utlMatchAfter(all.str, all.buf, "Date=", "-0123456789");
  flogf(" Date=%s", all.str);
  strcpy(stats->date, all.str);
  // time
  utlWrite(irid.port, "at+pt", EOL);
  if (!utlExpect(irid.port, all.buf, "OK", 12)) return 3;
  utlMatchAfter(all.str, all.buf, "Time=", ".:0123456789");
  flogf(" Time=%s", all.str);
  strcpy(stats->time, all.str);
  if (iri.setTime) 
    iriSetTime(stats);
  return 0;
} // iriDateTime

// get gps .lat .lng
// sets: .lng .lat 
// return: 0=success
int iriLatLng(GpsStats *stats){
  static char *self="iriLatLng";
  DBG();
  if (iriSats()) return 1;
  utlWrite(irid.port, "at+pl", EOL);
  if (!utlExpect(irid.port, all.buf, "OK", 12)) return 4;
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
bool iriSetTime(GpsStats *stats) {
  struct tm t;
  time_t gpsSeconds, diff;
  char *s;
  DBG0("iriSetTime(%s %s)", stats->date, stats->time);
  if (!stats->date || !stats->time) {
    flogf("\niriSetTime()\t| called with null data");
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
    flogf("\niriSetTime()\t| off by %ld seconds", diff);
    RTCSetTime(gpsSeconds, NULL);
  }
  return true;
} // iriSetTime
  

///
// sets: irid.sats
// rets: 0=success
int iriSats(void){
  tmrStart(iri_tmr, iri.timeout);
  while (!tmrExp(iri_tmr)) {
    utlNap(2);
    utlWrite(irid.port, "at+pd", EOL);
    if (!utlExpect(irid.port, all.buf, "OK", 12)) return 1;
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
// rets: 0=success
int iriSig(void) {
  flogf("\niriSig()");
  tmrStart(iri_tmr, iri.timeout);
  while (!tmrExp(iri_tmr)) {
    utlWrite(irid.port, "at+csq", EOL);
    if (!utlExpect(irid.port, all.buf, "OK", 12)) return 1;
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
  // set up timing for data
  //  10^6 * 10bits / rudBaud
  irid.rudUsec = (int) ((pow(10, 6)*10) / iri.rudBaud);
  utlWrite(irid.port, "at+cpas", EOL);
  if (!utlExpect(irid.port, all.str, "OK", 5)) return 2;
  utlWrite(irid.port, "at+clcc", EOL);
  if (!utlExpect(irid.port, all.str, "OK", 5)) return 3;
  utlMatchAfter(str, all.str, "+CLCC:", "0123456789");
  if (!strcmp(str, "006")==0) {
    utlWrite(irid.port, "at+chup", EOL);
    if (!utlExpect(irid.port, all.str, "OK", 5)) return 4;
  }
  sprintf(str, "atd%s", iri.phoneNum);
  // dial
  for (i=0; i<iri.redial; i++) {
    // fails "NO CONNECT" without this pause
    utlNap(4);
    // flush
    utlRead(irid.port, all.str);
    utlWrite(irid.port, str, EOL);
    utlReadWait(irid.port, all.str, CALL_DELAY);
    DBG1("%s", all.str);
    if (strstr(all.str, "CONNECT 9600")) {
      flogf(" CONNECT 9600");
      flogf("rudBaud@%d +%dus ", iri.rudBaud, irid.rudUsec);
      return 0;
    }
    flogf(" (%d)", i);
  }
  utlErr(iri_err, "call retry exceeded");
  return 4;
} // iriDial

///
// send proj hdr followed by "Hello", catch landResponse
// rets: *buf<-landResp
int iriProjHello(char *buf) {
  static char *self="iriProjHello";
  int r, try, hdr=13;
  char *s=NULL;
  try = iri.hdrTry;
  while (!s) {
    if (try-- <= 0) throw(1);
    flogf(" projHdr");
    utlWriteBlock(irid.port, irid.projHdr, hdr);
    s = utlExpect(irid.port, all.str, "ACK", iri.hdrPause);
  }
  flogf(", Hello?");
  iriSendBlock("hello", 5, 1, 1);
  if ((r = iriLandResp(buf))) throw(10+r);
  return 0;

  catch: return all.x;
} // iriProjHello

///
// send block Num of Many
// rets: 0=success 1=iri.hdrTry 2=block fail
int iriSendBlock(char *msg, int msgSz, int blockNum, int blockMany) {
  static char *self="iriSendBlock";
  int cs, i, bufSz, blockSz, sendSz;
  long uDelay;
  char *buff;
  DBG0("iriSendBlock(%d,%d,%d)", msgSz, blockNum, blockMany);
  buff = irid.buf;
  blockSz = msgSz+IRID_OFF;
  bufSz = msgSz+IRID_HDR;
  DBG2("projHdr:%s", utlNonPrint(irid.projHdr));
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
  memcpy(buff+IRID_HDR, msg, msgSz);
  // poke in cs high and low bytes
  cs = iriCRC(buff+IRID_OFF, bufSz-IRID_OFF);
  buff[3] = (char) (cs>>8 & 0xFF);
  buff[4] = (char) (cs & 0xFF);
  flogf(" %d/%d", blockNum, blockMany);
  // send data
  // pause every sendSz# chars to slow down baud stream
  sendSz = iri.sendSz;
  uDelay = (long) sendSz * irid.rudUsec;
  DBG2(" {%d %d %ld}", sendSz, irid.rudUsec, uDelay);
  for (i=0; i<bufSz; i+=sendSz) {
    // TUTxPutByte(irid.port, buff[i], false);
    // RTCDelayMicroSeconds((long) irid.rudUsec);
    if (i+sendSz>bufSz) {
      sendSz = bufSz-i;
      uDelay = (long) sendSz * irid.rudUsec;
    }
    TUTxPutBlock(irid.port, buff+i, (long) sendSz, 9999);
    // extra delay us per byte to emulate lower baud rate
    RTCDelayMicroSeconds(uDelay);
    utlX();
  }
  return 0;
} // iriSendBlock

///
// land ho! already did iriDial and iriProjHdr
// send fname as separate files of max iri.fileMax
// rets: 1:!file +10:!resp r:LandCmds
// sets: irid.block all.buf
int iriSendFile(char *fname) {
  static char *self="iriSendFile";
  int r=0, fh, bytes, bcnt, bnum;
  bool moreB=true;
  struct stat fileinfo;
  long size;
  flogf("\n%s(%s)", self, fname);
  // 
  if ( stat(fname, &fileinfo) ) {
    flogf("\nERR\t| errno %d on %s", errno, fname);
    return 1;
  }
  size = (long)fileinfo.st_size;
  if (size > 1024L*iri.fileMaxKB) {
    size = 1024L*iri.fileMaxKB;
    flogf("\n%s: ERR file too large, trunc to %ld", self, size);
  }
  fh = open(fname, O_RDONLY);
  if (fh<0) {
    flogf("\nERR\t| %s cannot open %s", self, fname);
    return 1;
  }
  /// read and send 
  // block & buf, size could change during run
  if (irid.blockSz != iri.blockSz) {
    flogf("\n%s: setting blockSize to %d", self, iri.blockSz);
    irid.blockSz = iri.blockSz;
    if (irid.block) free(irid.block);
    if (irid.buf) free(irid.buf);
    irid.block = malloc(iri.blockSz);
    irid.buf = malloc(iri.blockSz + IRID_HDR);
  }
  // send blocks
  bnum=(int)(size/irid.blockSz);
  if (size%irid.blockSz) bnum += 1; // add one if partial
  DBG2("\n%s: size=%ld, bnum=%d", self, size, bnum);
  for (bcnt=1; bcnt<=bnum; bcnt++) {
    bytes = read(fh, irid.block, irid.blockSz);
    iriSendBlock(irid.block, bytes, bcnt, bnum);
  } 
  close(fh);
  if ((r = iriLandResp(all.str))) 
    return 10+r;
  if (strstr(all.str, "cmds"))
    r = iriLandCmds(all.buf);
  iriProcessCmds(all.buf);
  return r;
} // iriSendFile

///
// process cmds from Land. could be a.b=c;d.e=f
int iriProcessCmds(char *buff) {
  static char *self="iriProcessCmds";
  char *p0;
  int r=0;
  p0 = strtok(buff, ";");
  while (p0) {
    if (strstr(p0, "=")) {
      flogf("\n%s: '%s'", self, utlNonPrint(p0));
      if (cfgString(p0))
        r++;
    }
    p0 = strtok(NULL, ";");
  }
  return r;
} // iriProcessCmds

///
// we just sent the last block, should get cmds or done
// looks like a bug in how "cmds" is sent, sometimes 0x0d in front
// 5 char the first time, six after; read until \n 0x0a
// rets: 0=success 1=respTO
int iriLandResp(char *buff) {
  int r, len=6;
  tmrStart(iri_tmr, iri.rudResp);
  memset(buff, 0, len);
  for (r=0; r<len; r++) {
    while (TURxQueuedCount(irid.port)==0)
      if (tmrExp(iri_tmr)) return 1;
    buff[r] = TURxGetByte(irid.port, true);
    if (buff[r]==0x0A)
      break;
  }
  buff[r]=0;
  flogf("\nLandResp(%s)", utlNonPrint(buff));
  if (!strstr(buff, "cmds") && !strstr(buff, "done"))
    return 2;
  return 0;
} // iriLandResp

///
// read and format land cmds
// rets: *buff\0 0=success 1=@TO 2=0@ 3=hdrTO 4=hdr!C11 
int iriLandCmds(char *buff) {
  int i, hdr=7;
  unsigned char c;
  short msgSz;
  static char *self="iriLandCmds";
  DBG();
  tmrStart(iri_tmr, iri.rudResp);
  // skip @@@ @@ or @ - protocol is sloppy, first CS byte could = @
  for (i=0; i<3; i++) {
    // wait for byte
    while (TURxQueuedCount(irid.port)==0)
      if (tmrExp(iri_tmr)) return 1;
    c = TURxPeekByte(irid.port, 0);
    DBG3("%s", utlNonPrintBlock(&c,1));
    if (c=='@') 
      TURxGetByte(irid.port, false);
    else 
      break;
  }
  if (i==0) return 2;
  // @@@ 2 CS bytes, 2 len bytes, 'C', 1, 1
  for (i=0; i<hdr; i++) {
    // wait for byte
    while (TURxQueuedCount(irid.port)==0)
      if (tmrExp(iri_tmr)) return 3;
    buff[i]=TURxGetByte(irid.port, false);
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
  memset(buff, 0, msgSz+1);
  for (i=0; i<msgSz; i++) {
    // wait for byte
    while (TURxQueuedCount(irid.port)==0)
      if (tmrExp(iri_tmr)) return 5;
    buff[i]=TURxGetByte(irid.port, false);
  }
  flogf("\nCMDS(%d)->", msgSz);
  flogf("''%s''", utlNonPrintBlock(buff, msgSz));
  return 0;
} // iriLandCmds

///
// send "data" because blocks will follow
void iriData(void) {
  utlWrite(irid.port, "data", "");
}

///
// call done
void iriHup(void) {
  int try=3;
  utlWrite(irid.port, "done", "");
  while (try--) {
    utlDelay(iri.hupMs);
    utlWrite(irid.port, "+++", "");
    utlDelay(iri.hupMs);
    if (utlExpect(irid.port, all.buf, "OK", 2)) break;
  }
  utlWrite(irid.port, "at+clcc", EOL);
  if (utlExpect(irid.port, all.buf, "OK", 5))
    flogf("\nclcc->%s", utlNonPrint(all.buf));
  utlWrite(irid.port, "at+chup", EOL);
  utlExpect(irid.port, all.buf, "OK", 5);
} // iriHup

///
// return: 0 success
int iriPrompt() {
  TURxFlush(irid.port);
  utlWrite(irid.port, "at", EOL);
  if (!utlExpect(irid.port, all.buf, "OK", 4)) return 1;
  else return 0;
} // iriPrompt

