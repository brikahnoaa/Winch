// utl.c - utility stuff
#include <main.h>

// allow up to .05 second between chars, normally chars take .001-.016
#define CHAR_DELAY 50

// all is a global structure for shared data: all.cycle
AllData all;
UtlInfo utl;

///
// malloc static buffers (heap is 384K, stack only 16K)
void utlInit(void) {
  DBG2("utlInit()");
  // utl.ret is semi-global, it is returned by some char *utlFuncs()
  all.buf = malloc(BUFSZ);
  all.str = malloc(BUFSZ);
  utl.buf = malloc(BUFSZ);
  utl.ret = malloc(BUFSZ);
  utl.str = malloc(BUFSZ);
  
  // sync this with enum ErrType
  utl.errName[ant_err] = "ant";
  utl.errName[boy_err] = "boy";
  utl.errName[cfg_err] = "cfg";
  utl.errName[s16_err] = "s16";
  utl.errName[iri_err] = "iri";
  utl.errName[ngk_err] = "ngk";
  utl.errName[wsp_err] = "wsp";
  utl.errName[log_err] = "log";
}

///
// sets: (*line)
int utlTrim(char *line) {
  char c;
  int len = strlen(line);
  DBG2("utlTrim()");
  // trim off crlf at end 
  c = line[len-1]; if (c=='\r' || c=='\n') line[--len] = 0;
  c = line[len-1]; if (c=='\r' || c=='\n') line[--len] = 0;
  return len;
}

///
// search str for sub, then return string that matches set
// out = matched string, or null string if no match
// sets: *out, rets: out
int utlMatchAfter(char *out, char *str, char *sub, char *set) {
  char *here;
  int len=0;
  DBG1("utlMatchAfter(%s)", sub);
  out[0] = 0;
  here = strstr(str, sub);
  if (here==NULL) return 0;
  // skip substring
  here += strlen(sub);
  len = strspn(here, set);
  if (len)
    strncpy(out, here, len);
  out[len] = 0;
  return len;
} // all.strMatchAfter

///
// readWait(1) until we get the expected string (or timeout)
// in: port, buf for content, expect to watch for, wait timeout
// uses: utl.buf
// rets: char* to expected str, or null
char *utlExpect(Serial port, char *buf, char *expect, int wait) {
  char *r=NULL;
  int sz=0;
  DBG1("utlExpect(%s, %d)", expect, wait);
  buf[0] = 0;
  tmrStart(utl_tmr, wait);
  // loop until expected or timeout
  while (true) {
    if (tmrExp(utl_tmr)) {
      DBG0("utlExpect(%s, %d) timeout", expect, wait);
      DBG1("->'%s'", buf);
      return NULL;
    }
    if (utlRead(port, utl.buf)) {
      sz += strlen(utl.buf);
      if (sz>=BUFSZ) return NULL;
      strcat(buf, utl.buf);
    }
    r = strstr(buf, expect);
    if (r) break;
    utlNap(1);
  }
  tmrStop(utl_tmr);
  return r;
} // utlExpect

///
// put block to serial; queue, don't block, it should all buffer
void utlWriteBlock(Serial port, char *out, int len) {
  int delay, sent;
  delay = 2 + (int)TUBlockDuration(port, (long)len);
  sent = (int)TUTxPutBlock(port, out, (long)len, (short)delay);
  DBG2("[>>]=%d", sent);
  DBG4("'%s'", utlNonPrintBlock(out, len));
  // DBG2(">>'%s'", utlNonPrintBlock(out, len));
  if (len!=sent)
    flogf("\nERR\t|utlWriteBlock(%s) sent %d of %d", out, sent, len);
} // utlWriteBlock

///
// put string to serial; queue, don't block, it should all buffer
// uses: utl.str
void utlWrite(Serial port, char *out, char *eol) {
  int len, delay, sent;
  strcpy(utl.str, out);
  if (eol!=NULL)
    strcat(utl.str, eol);
  len = strlen(utl.str);
  delay = 2 + (int)TUBlockDuration(port, (long)len);
  sent = (int)TUTxPutBlock(port, utl.str, (long)len, delay);
  DBG2(" >%d>", sent);
  DBG3(" >'%s'>", utlNonPrint(utl.str));
  if (len!=sent) 
    flogf("\nERR\t|utlWrite(%s) sent %d of %d", out, sent, len);
} // utlWrite

///
// read all the chars on the port, with a normal char delay
// char *in should be BUFSZ
// returns: len
int utlRead(Serial port, char *in) {
  int len;
  in[0]=0;
  if (TURxQueuedCount(port)<1) return 0;
  for (len=0; len<BUFSZ; len++) {
    in[len] = TURxGetByteWithTimeout(port, (short)CHAR_DELAY);
    if (in[len]<0) break;
  }
  in[len]=0;            // string
  DBG2(" <%d<", len);
  DBG3(" <'%s'<", utlNonPrint(in));
  return len;
} // utlRead

///
// delay up to wait seconds for first char, null terminate
// assumes full string arrives promptly after a delay of several seconds
// return: length
int utlReadWait(Serial port, char *in, int wait) {
  int len;
  char c;
  in[0] = c = 0;
  tmrStart(second_tmr, wait);
  // wait for a char
  while (c<=0) {
    // delay is a short
    c = TURxGetByteWithTimeout(port, 128);
    utlPet(); // could be a long wait
    if (tmrExp(second_tmr)) return 0;
  }
  in[0] = c;
  // rest of input, note utlRead exits if nothing queued
  for (len=1; len<BUFSZ; len++) {
    in[len] = TURxGetByteWithTimeout(port, (short)CHAR_DELAY);
    if (in[len]<0) break;
  }
  in[len]=0;            // string
  DBG2(" <%d<", len);
  DBG3(" <'%s'<", utlNonPrint(in));
  return len;
}

///
// write time to log file
void utlLogTime(void) {
  flogf(" %s", utlTime());
}

///
// HH:MM:SS now
// returns: global char *utl.ret
char *utlTime(void) {
  struct tm *tim;
  time_t secs;
  time(&secs);
  tim = gmtime(&secs);
  sprintf(utl.ret, "%02d:%02d:%02d",
      tim->tm_hour, tim->tm_min, tim->tm_sec);
  return utl.ret;
} // utlTime

///
// Date String // YY-MM-DD 
// returns: global char *utl.ret
char *utlDate(void) {
  struct tm *tim;
  time_t secs;
  time(&secs);
  tim = gmtime(&secs);
  sprintf(utl.ret, "%02d-%02d-%02d", 
          tim->tm_year-100, tim->tm_mon+1, tim->tm_mday);
  return utl.ret;
} // utlDate

///
// YYYY-MM-DD HH:MM:SS 
// returns: global char *utl.ret
char *utlDateTimeFmt(time_t secs) {
  struct tm *tim;
  tim = gmtime(&secs);
  sprintf(utl.ret, "%04d-%02d-%02d %02d:%02d:%02d",  
          tim->tm_year + 1900, tim->tm_mon+1, tim->tm_mday, 
          tim->tm_hour, tim->tm_min, tim->tm_sec);
  return utl.ret;
} // utlDateTime

///
// YYYY-MM-DD HH:MM:SS 
// returns: global char *utl.ret
char *utlDateTime(void) {
  time_t secs;
  time(&secs);
  return utlDateTimeFmt(secs);
} // utlDateTime

///
// MMDDYYYYHHMMSS 
// returns: global char *utl.ret
char *utlDateTimeS16(void) {
  struct tm *tim;
  time_t secs;
  time(&secs);
  tim = gmtime(&secs);
  sprintf(utl.ret, "%02d%02d%04d%02d%02d%02d", 
          tim->tm_mon+1, tim->tm_mday, tim->tm_year + 1900, 
          tim->tm_hour, tim->tm_min, tim->tm_sec);
  return utl.ret;
} // utlDateTimeS16

///
// format non-printable string; null terminate
// returns: global char *utl.ret
char *utlNonPrint (char *in) {
  return (utlNonPrintBlock(in, strlen(in)));
} // utlNonPrint

/*
  unsigned char ch;
  char *out = utl.ret;
  int i, o;
  // walk thru input until 0 or BUFSZ
  i = o = 0;
  while (in[i] && o<BUFSZ-6) {
    ch = in[i++];
    if ((ch<32)||(ch>126)) {
      // non printing char
      sprintf(out+o, " x%02X ", ch);
      o += 5;     // five char hex ' x1A '
    } else 
      out[o++] = ch;
  }
  out[o] = 0;
  return (out);
} // utlNonPrint
*/

///
// format non-printable string; null terminate
// returns: global char *utl.ret
char *utlNonPrintBlock (char *in, int len) {
  unsigned char ch;
  char *out = utl.ret;
  int i, o;
  // copy len bytes - note, o grows faster than i
  i = o = 0;
  while (i<len) {
    ch = in[i++];
    if (ch==0x0A) {
      sprintf(out+o, "\\n ");
      o += 3;
    } else if (ch==0x0D) {
      sprintf(out+o, " \\r");
      o += 3;
    } else if ((ch<32)||(ch>126)) {
      // non printing char
      sprintf(out+o, " x%02X ", ch);
      o += 5;     // five char hex ' x1A '
    } else {
      out[o++] = ch;
    }
  }
  out[o] = 0;
  return (out);
} // utlNonPrintBlock

void utlPet() { TickleSWSR(); }              // pet the watchdog

///
// rets: path\name
void utlLogPathName(char *path, char *base, int day) {
  char day000[4];
  sprintf(day000, "%03d", day);
  path[0]=0;
  strcat(path, "log\\");
  strcat(path, day000);
  strcat(path, base);
  strcat(path, ".log");
  DBG1("(%s)", path);
  return;
} // utlLogPathName

///
// takes a base name and makes a full path, opens file, writes dateTime
// rets: fileID or 0=err
int utlLogOpen(int *log, char *base) {
  int r=0, fd, flags;
  char path[64];
  static char *self="utlLogOpen";
  DBG();
  if (*log) {close(*log); *log=0;}
  utlLogPathName(path, base, all.cycle);
  flags = O_APPEND | O_CREAT | O_WRONLY;
  fd = open(path, flags);
  if (fd<=0) {
    sprintf(utl.str, "open ERR %d (errno %d), path %s", fd, errno, path);
    utlErr(log_err, utl.str);
    return 1;
  } else {
    DBG1("\n%s path\t| %s", self, path);
    sprintf(utl.str, "\n---  %s ---\n", utlDateTime());
    DBG2("\n%s(%s):%d", self, path, fd);
    r = write(fd, utl.str, strlen(utl.str)); 
    if (r<1) {
      sprintf(utl.str, "write ERR %d (errno %d), path %s", r, errno, path);
      utlErr(log_err, utl.str);
      close(fd);
      return 2;
    }
  }
  *log=fd;
  return 0;
} // utlLogOpen

/// 
// close file
int utlLogClose(int *fd) {
  static char *self="utlLogClose";
  int f;
  DBG();
  if (*fd<1) return 0;   // no fd
  f=*fd;
  *fd=0;
  DBG2("\n%s():%d ", self, f);
  if (close(f)<0) {
    flogf("\n%s(): ERR closing file (fd=%d)", self, f);
    return 1;
  }
  return 0;
} // utlLogClose

/// file handling error - FATAL
// log error and shutdown
void utlCloseErr(char *str) {
  DBG0("utlCloseErr(%s)", str);
  sprintf(utl.str, "utlCloseErr(%s): close error", str);
  utlErr(log_err, utl.str);
  // ?? reboot
} // utlCloseErr

///
// ?? tbd sophist err handling, allow limit by type
void utlErr( ErrType err, char *str) {
  flogf("\n-ERR(%s)\t|%d| %s %s", 
    utl.errName[err], utl.errCnt[err], utlTime(), str);
  utl.errCnt[err]++;
}

///
// nap called often
void utlNap(int sec) {
  DBG2("utlNap(%d)", sec);
  while(sec-- > 0) {
    utlX();
    pwrNap(1);
  }
} // utlNap

///
// stop called often
void utlSleep(void) {
  mpcSleep();
} // utlSleep

///
// stop called often
void utlStop(char *out) {
  sysStop(out);
  BIOSResetToPicoDOS();
} // utlStop

///
// loop to run all tests
void utlTestLoop(void) {
  s16Test();
} // utlTestLoop

///
// do misc activity, frequently
void utlX(void) {
  char c;
  // ?? pwrChk();
  // ?? utlPet();
  // console?
  if (cgetq()) {
    if (!utl.ignoreCon) {
      c = tolower( cgetc() );
      switch (c) {
      case 'q':
      case 'x':
        utlStop("user quit");
        break;
      // turn dbg on/off
      case '0': dbg.dbg0 = !dbg.dbg0; break;
      case '1': dbg.dbg1 = !dbg.dbg1; break;
      case '2': dbg.dbg2 = !dbg.dbg2; break;
      case '3': dbg.dbg3 = !dbg.dbg3; break;
      case '4': dbg.dbg4 = !dbg.dbg4; break;
      case 'd': // adhoc func for debug
        (*dbg.funcPtr)();
        break;
      case 't': // start time test
        utlTestLoop();
        break;
      }
    } else 
      ciflush();
  }
} // utlX
