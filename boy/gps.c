// gps.c
#include <utl.h>
#include <gps.h>
#include <ant.h>
#include <tmr.h>

// gps and iridium routines have a lot of ways to fail, so return 0 or errcode
//
#define EOL "\r"

GpsInfo gps;


///
// set up for gps, call this after gpsInit
// sets: gps.mode .port
void gpsInit(void) {
  DBG0("gpsInit()")
  gps.port = antPort();
  if (!gps.port)
    utlErr(logic_err, "no gps.port, was gpsInit called before antInit?");
} // gpsInit

///
// turn on, clean, set params, talk to sbe39
// requires: antStart
void gpsStart(void) {
  DBG0("gpsStart() %s", utlDateTime())
  antDevice(cf2_dev);
  // power up a3la
  TUTxPutByte(gps.port, 3, false);
  TUTxPutByte(gps.port, 'I', false);
  antDevice(a3la_dev);
  utlExpect(gps.port, utlBuf, "COMMAND", 12);
  gpsSats();
} // gpsStart

///
// turn off power to gpsmod
void gpsStop() {
  if (gps.log)
    close(gps.log);
  gps.log = 0;
  antDevice(a3la_dev);
  utlWrite(gps.port, "at*p", EOL);
  antDevice(cf2_dev);
  TUTxPutByte(gps.port, 4, false);      // ^D powerdown
  TUTxPutByte(gps.port, 'I', false);      // S | I
} // gpsStop

///
// return: 0 success
int gpsPrompt() {
  TURxFlush(gps.port);
  utlWrite(gps.port, "at", EOL);
  if (utlExpect(gps.port, utlBuf, "OK", 4))
    return 0;
  else
    return 1;
} // gpsPrompt

///
// get gps info; date, time, long, lat
// sets: .date .time .long .lat
// return: how many satellites
int gpsStats(void){
  antSwitch(gps_ant);
  antDevice(cf2_dev);
  DBG0("gpsStats()")
  tmrStart(gps_tmr, gps.timeout);
  while (!tmrExp(gps_tmr)) {
    utlWrite(gps.port, "AT+PD", EOL);
    utlExpect(gps.port, utlBuf, "OK", 12);
    if (!strstr(utlBuf, "Invalid Position"))
      break;
  } // while timeout
  tmrStop(gps_tmr);
  if (utlMatchAfter(utlStr, utlBuf, "Satellites Used=", "0123456789")) 
    flogf(" Sats=%s", utlStr);
  gps.sats = atoi(utlStr);
  ////
  if (utlMatchAfter(utlStr, utlBuf, "Satellites Used=", "0123456789")) 
    flogf(" Sats=%s", utlStr);
  utlWrite(gps.port, "AT+PT", EOL);
  utlExpect(gps.port, utlBuf, "OK", 12);
  utlWrite(gps.port, "AT+PD", EOL);
  utlExpect(gps.port, utlBuf, "OK", 12);
  return 0;
} // gpsStats

/// ?? chatty
// gpsISig ??
int gpsISig(void) {
  // switch to irid
  antDevice(cf2_dev);
  antSwitch(irid_ant);
  antDevice(a3la_dev);
  flogf("\nCSQ\n");
  tmrStart(ant_tmr, iridT);
  while (!tmrExp(ant_tmr)) {
    utlWrite(gps.port, "AT+CSQ", EOL);
    utlExpect(gps.port, utlBuf, "OK", 12);
    // replace crlf
    for (here=utlBuf; *here; here++) 
      if (*here=='\r' || *here=='\n')
        *here = '.';
    flogf("%s\n", utlBuf);
    utlX();
  } // while timer
  return 0;
} // gpsISig

///
//
int iridHdr(char *buf) {
  bool Ack = false;
  short Num_ACK = 0;
  short AckMax = 20;
  short Status = 0;
  int crc, crc1, crc2;
  // need an extra char for null terminated
  unsigned char buf[16], proj[12], bmode[4];

  // Flush IO Buffers
  TUTxFlush(devicePort); TURxFlush(devicePort);

  // note - sprintf is zero terminated, strncpy is not
  sprintf(proj, "%4s%4s", gps.project, gps.platform);
  DBG0(flogf("\n%s|SendProjHdr(%s)", Time(NULL), proj);)
  crc = Calc_Crc(proj, 8);
  crc1 = crc;
  crc2 = crc;
  sprintf(buf, "???%c%c%c%c%c%c%c%c%c%c%c%c", 
    (char)((crc1 >> 8) & 0x00FF), (char)(crc2 & 0x00FF), 
    proj[0], proj[1], proj[2], proj[3], 
    proj[4], proj[5], proj[6], proj[7]);

  // antMod blockmode for header // ^B, 2 byte length
  sprintf(bmode, "%c%c%c", (char)2, (char)0, (char)13 );

  while (Ack == false && Num_ACK < AckMax) { // Repeat
    AD_Check();
    TUTxPutBlock(devicePort, bmode, 3, 10000);
    TUTxPutBlock(devicePort, buf, 13, 10000); // 13 bytes + crlf
    DelayTX(16);
    // give rudics a chance to reply
    Delayms(500);
    TickleSWSR(); // another reprieve

    Status = GetIRIDInput("ACK", 3, NULL, NULL);
    if (Status == 1) {
      flogf("\n\t|ACK Received");
      LostConnect = false;
      Ack = true;
    } else {
      if (Status == -1) { // NO CARRIER
        flogf("\n\t|ACK Failed");
        LostConnect = true;
      }
    }
    Num_ACK++;
  }

  if (Num_ACK >= AckMax && Status != 1) {
    flogf("\n\t|ACK inquirey reached max = %d", AckMax);
    cdrain();
    if (LostConnect) {
      flogf("\n\t|LostConnection");
      cdrain();
      StatusCheck();
      Delayms(20);
    } else {
      TUTxFlush(devicePort);
      TURxFlush(devicePort);

      // Exit in-call data mode to check phone status.
      TUTxPrintf(devicePort, "+++");  // ??
      TUTxWaitCompletion(devicePort);
      Delayms(25);
      if (GetIRIDInput("OK", 2, NULL, NULL) == 1) {
        StatusCheck();
        HangUp();
        // StatusCheck(); //deal with phone status
        // if(CallStatus()==4){//enter back into the call?
      } else StatusCheck(); // What happens in this scenario?
    } // LostConnect

    AD_Check();
    flogf("\n%s|SendProjHdr() PLATFORM ID TX FAILED.", Time(NULL));
    putflush();
    CIOdrain();
    TX_Success = -1;
    Delayms(20);
  } // Num_ACK >= AckMax
  Delayms(10);
  return Ack;

} // SendProjHdr

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
      crc_calc = Calc_Crc(buf + 5, blklen); // PMEL site crc include first 5
      buf[0] = '@';
      buf[1] = '@';
      buf[2] = '@';
      buf[3] = (uchar)((crc_calc & 0xFF00) >> 8);
      buf[4] = (uchar)((crc_calc & 0x00FF));

      // TUTxFlush(devicePort);
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

#ifdef DEBUG1
    blklen=tgetq(devicePort);
    if (blklen) {
      cprintf("\n%d+", blklen);
      // blklen=0;
      // while ((scratch[blklen++]=TURxGetByteWithTimeout(devicePort, 1)) >= 0) {}
      // printsafe( (long) blklen, scratch);
        
      memset( scratch, 0, BUFSZ );
      printsafe( (long) TURxGetBlock(devicePort, scratch, 
                          (long) BUFSZ, (short) 1),
        scratch);
    }
#else
    TURxFlush(devicePort);
    cdrain();
#endif
    Delayms(2000);
  }

  if (close(IRIDFileHandle) != 0)
    flogf("\nERROR  |Send_Blocks: File Close error: %d", errno);
  free(buf);
  return 0;

} // Send_Blocks


///
//
int Calc_Crc(uchar *buf, int cnt) {
  long accum;
  int i, j;
  accum = 0x00000000;

  if (cnt <= 0)
    return 0;
  while (cnt--) {
    accum |= *buf++ & 0xFF;

    for (i = 0; i < 8; i++) {
      accum <<= 1;

      if (accum & 0x01000000)
        accum ^= 0x00102100;
    }
  }

  /* The next 2 lines forces compatibility with XMODEM CRC */
  for (j = 0; j < 2; j++) {
    accum |= 0 & 0xFF;

    for (i = 0; i < 8; i++) {
      accum <<= 1;

      if (accum & 0x01000000)
        accum ^= 0x00102100;
    }
  }

  return (accum >> 8);

} // Calc_Crc

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
