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
    utlErr(logic_err, "no gps.port, was gpsInit called before antInit?");
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
// return: 0 success
int iridPrompt() {
  TURxFlush(gps.port);
  utlWrite(gps.port, "at", EOL);
  if (!utlExpect(gps.port, utlBuf, "OK", 4)) return 1;
  else return 0;
} // iridPrompt

///
// get gps info; date, time, lon, lat
// sets: .date .time .lon .lat
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
  // lat lon
  utlWrite(gps.port, "at+pl", EOL);
  if (!utlExpect(gps.port, utlBuf, "OK", 12)) return 4;
  utlMatchAfter(utlStr, utlBuf, "Latitude=", ".:0123456789 NEWS");
  flogf(" Lat=%s", utlStr);
  strcpy(gps.lat, utlStr);
  utlMatchAfter(utlStr, utlBuf, "Longitude=", ".:0123456789 NEWS");
  flogf(" Lon=%s", utlStr);
  strcpy(gps.lon, utlStr);
  return 0;
} // gpsStats

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
  return 2;
} // gpsSats

/// 
// sets: gps.signal
int iridSig(void) {
  DBG0("iridSig()")
  // switch to irid
  antDevice(cf2_dev);
  antSwitch(irid_ant);
  antDevice(a3la_dev);
  tmrStart(gps_tmr, gps.timeout);
  while (!tmrExp(gps_tmr)) {
    utlWrite(gps.port, "at+csq", EOL);
    if (!utlExpect(gps.port, utlBuf, "OK", 12)) return 1;
    if (utlMatchAfter(utlStr, utlBuf, "CSQ:", "0123456789")) {
      gps.signal = atoi(utlStr);
      if (gps.signal>1) flogf(" csq=%s", utlStr);
      if (gps.signal>gps.signalMin) break;
    } // if CSQ
  } // while timer
  return 0;
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
  int hdr1=13, hdr2=10, hdrTry=8, hdrPause=20;
  int cs, bufLen, x;
  char *s;
  char land[128];
  DBG0("iridSendTest()")
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
  if (iridDial()) return 1;
  while (hdrTry--) {
    utlWriteBlock(gps.port, gps.projHdr, hdr1);
    s = utlExpect(gps.port, land, "ACK", hdrPause);
    if (s) {
      DBG4("(%d)", s)
      break;
    }
  }
  if (hdrTry < 0) return 2;
  // send data
  utlWriteBlock(gps.port, utlBuf, bufLen);
  // land ho!
  utlBuf[0] = 0;
  tmrStart(rudics_tmr, gps.rudResp);
  while (!tmrExp(rudics_tmr)) {
    // ACK is only for projheader, may be leftover, skip
    if (strstr(utlBuf, "ACK\n")) {
      strcpy(utlBuf, utlBuf+4);
      continue;
    }
    if (strstr(utlBuf, "done"))
      utlWriteBlock(gps.port, "done", 4);
    if (strstr(utlBuf, "cmds"))
      utlWriteBlock(gps.port, "done", 4);
    x = utlReadWait(gps.port, utlBuf, 1);
    if (x) {
      DBG4("%s", utlNonPrintBlock(utlBuf, x))
    }
  }
  tmrStop(rudics_tmr);
  flogf("\nland->%s", utlNonPrint(utlBuf));
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


/*
///
//
short Send_File(bool FileExist, long filelength) {
  char bitmap[64]; // 63-byte array corresponds to 64-bit map, val0 and val1 for
                   // resending the data block
  uchar NumOfBlks; // Number of data blocks to send <=63
  long filesize;
  ushort LastBlkLength;
  short i;
  long oldLength;
  ulong val0, val1; // 64-bit bit map for resending the data block
  short Reply = 0;
  // short Delay;
  DBG2(flogf(" .Send_File() ");)
  if (FileExist) {

    if (filelength > (long)IRID.MAXUPL + 500) {
      oldLength = filelength;
      filelength = (long)IRID.MAXUPL + 500;
      flogf("\n\t|Only sending first %ld of %ld bytes", filelength, oldLength);
    }

    NumOfBlks = (uchar)(filelength / (long)BlkLength);

    filesize = (long)BlkLength * NumOfBlks;
    if (filesize < filelength) // If block length*number of blocks is less than
                               // IridFile length
    {
      LastBlkLength =
          (ushort)(filelength -
                   filesize); // Make another block to be sent that has the end
      NumOfBlks += 1;         // Number of blocks = number of blocks + 1
    } else if (filesize == filelength)
      LastBlkLength = (ushort)BlkLength; // True IridFile size same

    // Limit to 64 blocks
    if (NumOfBlks > 63) {
      flogf("\n\t|Send_File() File too big. Only sending first 63 blocks.");
      putflush();
      NumOfBlks = 63;
      LastBlkLength = BlkLength;
    }

    flogf("\n%s|Send_File(%s):\n\t|%ld Bytes, %d BLKS", Time(NULL),
          IRIDFilename, filelength, NumOfBlks);
    cdrain(); // Log the IridFile length
    AD_Check();

    // Send the data IridFile in blocks
    for (i = 0; i < NumOfBlks; i++)
      bitmap[63 - i] = '1'; // Set bitmap all to '1s' to send all blocks
    // DBG1(flogf("\n\t|Check First Bitmap: %s", bitmap); )

    Send_Blocks(bitmap, NumOfBlks, BlkLength, LastBlkLength);
    // note- S_B delays per output size, flushes input
  } // FileExist
  // Delay = (short)LastBlkLength / 1000;
  // if (Delay == 0) Delay = 1;
  // delay for land to reply
  Delay_AD_Log(5);

  Reply = Check_If_Cmds_Done_Or_Resent(&val0, &val1);
  // Check if the send-data went OK
  // Reply = 0 resent request
  // Reply = 1 done. no more commands coming
  // Reply = 2 Fake Commands (To prompt multiple file upload
  // Reply = 3 Real Commands
  // Reply = -1 bad TX
  //      = -2 No carrier
  //      = -5 Inaccurate reply from land

  Num_Resent = 0; // Number of resent trials
  while (Reply <= 0 && Num_Resent < MAX_RESENT) {

    AD_Check();

    // For errors returned from Rudics Basestation
    if (Reply <= 0) {
      Num_Resent++;
      flogf("\n\t|TX INCOMPLETE. Reply=%d", Reply);
      putflush();
      CIOdrain();
      Delayms(10);
      if (Reply == 0) { // Request to resend bad blocks
        Convert_BitMap_To_CharBuf(val0, val1, &bitmap);
        DBG2(flogf("\n\t|Resend");
            Delayms(20);)
        Send_Blocks(bitmap, NumOfBlks, BlkLength, LastBlkLength);
      }
      //-1 Someting bad happened. TX Failed. Restart Modem
      else if (Reply == -1) {
        Num_Resent = MAX_RESENT;
        flogf("\n\t|Something else");
        putflush();
        CIOdrain();
      }
      //-2 No Carrier response from Modem.
      else if (Reply <= -2) {
        LostConnect = true;
        break; // NO CARRIER
      }
    }
  }

  // Resent effort failed more than XX times
  if (Num_Resent >= MAX_RESENT) {
    flogf("\n%s|Send_File() Resends exceeded Max: %d. ", Time(NULL),
          Num_Resent);
    putflush();
    CIOdrain();
    return -1;
  }

  else if (Reply == 0) {
    flogf("\n%s|Send_File() Land sent resend req.", Time(NULL));
    putflush();
    CIOdrain();
  }

  else if (Reply == -1) {
    flogf("\n%s|Send_File() Land TX garbled.", Time(NULL));
    putflush();
    CIOdrain();
  }

  else if (Reply <= -2) { // No Carrier.  Call again
    flogf("\n%s|Send_File() Lost carrier.", Time(NULL));
    LostConnect = true;
  }

  return Reply;
} // Send_File

///
//
int Send_Blocks(char *bitmap, uchar NumOfBlks, ushort BlockLength,
                ushort LastBlkLength) {

  uchar *buf, bmode[4];
  uchar BlkNum;
  long mlength;
  ushort blklen;
  uchar mlen[2];
  int crc_calc;
  long bytesread;
  const short dataheader = 10; 

  buf = (uchar *)calloc((int) (BlockLength+20), 1);

  DBG0(flogf(" .Send_Blocks() ");)
  IRIDFileHandle = open(IRIDFilename, O_RDONLY);
  if (IRIDFileHandle <= 0) {
    flogf("\nError: Send_Blocks: failed open(%s)", IRIDFilename);
    return -6;
  }
  DBG(else flogf("\n\t|Send_Blocks: open(%s)", IRIDFilename);)

  crc_calc = 0x0000;
  for (BlkNum = 1; BlkNum <= NumOfBlks;
       BlkNum++) { // For loop to send all blocks
    AD_Check();
    if (BlkNum == NumOfBlks)
      BlockLength = LastBlkLength; // If Last Blcok, get length of last block
    mlength = BlockLength + dataheader;    // PMEL IRID block size + 5
    blklen = BlockLength + 5;
    mlen[0] = (blklen & 0xFF00) >> 8; // Convert an integer to
    mlen[1] = (blklen & 0x00FF);      // 2-byte uchar.
    DBG1(flogf(", clear %d bytes", blklen + 5);)
    memset(buf, 0, (size_t) (blklen + 5)); // Flush the buffer

    bytesread = read(IRIDFileHandle, buf + dataheader, (size_t) BlockLength);
    DBG1(flogf("\n\t|Bytes Read: %ld", bytesread); )
    if (bitmap[64 - BlkNum] != '0') { // Send in reverse order
      DBG1(flogf("\n\t|SENDING BLK #%d %ld BYTES", BlkNum, mlength); )
      AD_Check();
      buf[5] = mlen[0]; // Block length
      buf[6] = mlen[1];
      buf[7] = 'T'; // Data type
      buf[8] = (uchar)BlkNum;
      buf[9] = (uchar)NumOfBlks;
      crc_calc = iridCRC(buf + 5, blklen); // PMEL site crc include first 5
      buf[0] = '@';
      buf[1] = '@';
      buf[2] = '@';
      buf[3] = (uchar)((crc_calc & 0xFF00) >> 8);
      buf[4] = (uchar)((crc_calc & 0x00FF));
      // TURxFlush(devicePort);
      // antMod blockmode for header // ^B, 2 byte length
      sprintf(bmode, "%c%c%c", (uchar)2, 
        (uchar)((mlength >> 8) & 0x00FF), (uchar)(mlength & 0x00FF));
      TUTxPutBlock(devicePort, bmode, (long) 3, 1000);
      TUTxPutBlock(devicePort, buf, mlength, 10000);
      DelayTX(mlength);
      AD_Check();
    } // if bitmap[]
    // pause that refreshes
    TURxFlush(devicePort);
    cdrain();
    Delayms(2000);
  }
  if (close(IRIDFileHandle) != 0)
    flogf("\nERROR  |Send_Blocks: File Close error: %d", errno);
  free(buf);
  return 0;
} // Send_Blocks


void Convert_BitMap_To_CharBuf(ulong val0, ulong val1, char *bin_str) {
  ulong remainder;
  int count, type_size;
  short NumBadBlks;

  NumBadBlks = 0;
  type_size = sizeof(ulong) * 8;

  for (count = 0; count < type_size; count++) {
    remainder = val0 % 2;
    if (remainder) {
      bin_str[count] = '1';
      NumBadBlks++;
    } else
      bin_str[count] = '0';
    val0 /= 2;
  }
  for (count = type_size; count < type_size * 2; count++) {
    remainder = val1 % 2;
    if (remainder) {
      bin_str[count] = '1';
      NumBadBlks++;
    } else
      bin_str[count] = '0';
    val1 /= 2;
  }

  // bin_str[count]='\0';
  // Debug
  flogf("\n%s|Convert_BitMap_To_CharBuf() NUM OF BAD BLKS=%d", Time(NULL),
        NumBadBlks);
  putflush();
  CIOdrain(); // Delayms(10);
  //	flogf("\n%s", bin_str); Delayms(20);
  // debug end

} // Convert_BitMap_To_CharBuf
*/
