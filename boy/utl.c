// utl.c - utility stuff
#include <utl.h>
#include <mpc.h>
#include <pwr.h>
#include <sys.h>
#include <tmr.h>

// allow up to .05 second between chars, normally chars take .001-.016
#define CHAR_DELAY 50

// the globals below are used by all modules, malloc'd in utlInit()
// utl.ret is returned by some char *utlFuncs()
char *utlBuf, *utlStr;     

UtlInfo utl;
CommonInfo com;

///
// malloc static buffers (heap is 384K, stack only 16K)
void utlInit(void) {
  DBG2("utlInit()")
  utl.buf = malloc(BUFSZ);
  utl.str = malloc(BUFSZ);
  utl.ret = malloc(BUFSZ);
  utlBuf = malloc(BUFSZ);
  utlStr = malloc(BUFSZ);
  // sync with enum ErrType
  utl.errName[ant_err] = "ant";
  utl.errName[boy_err] = "boy";
  utl.errName[cfg_err] = "cfg";
  utl.errName[ctd_err] = "ctd";
  utl.errName[gps_err] = "gps";
  utl.errName[ngk_err] = "ngk";
  utl.errName[wsp_err] = "wsp";
  utl.errName[log_err] = "log";
}

void utlDelay(int x) { 
  RTCDelayMicroSeconds((long)x*1000); 
}

///
// sets: (*line)
int utlTrim(char *line) {
  char c;
  int len = strlen(line);
  DBG2("utlTrim()")
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
  DBG1("utlMatchAfter(%s)", sub)
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
} // utlStrMatchAfter

///
// readWait(1) until we get the expected string (or timeout)
// in: port, buf for content, expect to watch for, wait timeout
// uses: utl.buf
// rets: char* to expected str, or null
char *utlExpect(Serial port, char *buf, char *expect, int wait) {
  char *r;
  DBG1("utlExpect(%s, %d)", expect, wait)
  buf[0] = 0;
  tmrStart(utl_tmr, wait);
  // loop until expected or timeout
  while (true) {
    if (tmrExp(utl_tmr)) {
      DBG0("utlExpect(%s, %d) timeout", expect, wait)
      DBG1("->'%s'", buf);
      return NULL;
    }
    if (utlRead(port, utl.buf))
      strcat(buf, utl.buf);
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
  delay = 2 * (int)TUBlockDuration(port, (long)len);
  sent = (int)TUTxPutBlock(port, out, (long)len, (short)delay);
  DBG3("[>>]=%d", sent)
  // DBG3(">>'%s'", utlNonPrintBlock(out, len))
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
  delay = CHAR_DELAY + (int)TUBlockDuration(port, (long)len);
  sent = (int)TUTxPutBlock(port, utl.str, (long)len, (short)delay);
  DBG2(">>=%d", sent)
  DBG3(">>'%s'", utlNonPrint(utl.str))
  if (len!=sent) 
    flogf("\nERR\t|utlWrite(%s) sent %d of %d", out, sent, len);
} // utlWrite

///
// read all the chars on the port, with a normal char delay
// char *in should be BUFSZ
// returns: len
int utlRead(Serial port, char *in) {
  int len = 0;
  if (TURxQueuedCount(port)<1) return 0;
  // len = (int) TURxGetBlock(port, in, (long)BUFSZ, (short)CHAR_DELAY);
  for (len=0; len<BUFSZ; len++) {
    in[len] = TURxGetByteWithTimeout(port, (short)CHAR_DELAY);
    if (in[len]<0) break;
  }
  in[len]=0;            // string
  DBG2("<<=%d", len)
  DBG3("<<'%s'", utlNonPrint(in))
  return len;
} // utlRead

///
// delay up to wait seconds for first char, null terminate
// assumes full string arrives promptly after a delay of several seconds
// return: length
int utlReadWait(Serial port, char *in, int wait) {
  int len;
  in[0] = TURxGetByteWithTimeout(port, (short) wait*1000);
  utlPet(); // could have been a long wait
  if (in[0]<=0) {
    // timeout
    in[0]=0;
    return 0;
  } 
  // rest of input, note utlRead exits if nothing queued
  for (len=1; len<BUFSZ; len++) {
    in[len] = TURxGetByteWithTimeout(port, (short)CHAR_DELAY);
    if (in[len]<0) break;
  }
  in[len]=0;            // string
  DBG2("<<(%d)=%d", wait, len)
  DBG3("<<'%s'", utlNonPrint(in))
  return len;
}

// ?? check out __DATE__, __TIME__

///
// write time to log file
void utlLogTime(void) {
  flogf(" %s", utlTime());
}

///
// HH:MM:SS now
// returns: global static char *utl.ret
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
// Date String // MM-DD-YY 
// returns: global static char *utl.ret
char *utlDate(void) {
  struct tm *tim;
  time_t secs;
  
  time(&secs);
  tim = gmtime(&secs);
  sprintf(utl.ret, "%02d-%02d-%02d", tim->tm_mon+1,
          tim->tm_mday, tim->tm_year - 100);
  return utl.ret;
} // utlDate

///
// MM-DD-YY HH:MM:SS 
// returns: global static char *utl.ret
char *utlDateTime(void) {
  struct tm *tim;
  time_t secs;
  time(&secs);
  tim = gmtime(&secs);
  sprintf(utl.ret, "%02d-%02d-%02d %02d:%02d:%02d", tim->tm_mon+1,
          tim->tm_mday, tim->tm_year - 100, tim->tm_hour,
          tim->tm_min, tim->tm_sec);
  return utl.ret;
} // utlDateTime

///
// MMDDYYYYHHMMSS 
// returns: global static char *utl.ret
char *utlDateTimeBrief(void) {
  struct tm *tim;
  time_t secs;
  time(&secs);
  tim = gmtime(&secs);
  sprintf(utl.ret, "%02d%02d%04d%02d%02d%02d", tim->tm_mon+1,
          tim->tm_mday, tim->tm_year + 1900, tim->tm_hour,
          tim->tm_min, tim->tm_sec);
  return utl.ret;
} // utlDateTimeBrief

///
// format non-printable string; null terminate
// returns: global static char *utl.ret
char *utlNonPrint (char *in) {
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

///
// format non-printable string; null terminate
// returns: global static char *utl.ret
char *utlNonPrintBlock (char *in, int len) {
  unsigned char ch;
  char *out = utl.ret;
  int i, o;
  // copy len bytes
  i = o = 0;
  while (len--) {
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
} // utlNonPrintBlock

void utlPet() { TickleSWSR(); }              // pet the watchdog

///
// takes a base name and makes a full path, opens file, writes dateTime
// ?? moves existing file to backup dir
// rets: fileID or 0=err
int utlLogFile(char *fname) {
  int log;
  char path[64], day[4];
  struct tm *tim;
  time_t secs;
  DBG0("utlLogFile(%s)", fname)
  time(&secs);
  tim = gmtime(&secs);
  sprintf(day, "%03d", tim->tm_yday);
  path[0]=0;
  strcat(path, "log\\");
  strcat(path, day);
  strcat(path, fname);
  strcat(path, ".log");
  DBG1("(%s)", path)
  log = open(path, O_APPEND | O_CREAT | O_RDWR);
  if (log<=0) {
    sprintf(utl.str, "utlLogFile(%s): open ERR %d for %s", fname, log, path);
    utlErr(log_err, utl.str);
    return 0;
  } else {
    flogf("\nlog file\t| %s", path);
    sprintf(utl.str, "\n---  %s ---\n", utlDateTime());
    write(log, utl.str, strlen(utl.str));
    return log;
  }
} // utlLogFile

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
  DBG2("utlNap(%d)", sec)
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
// do misc activity, frequently
void utlX(void) {
  char c;
  // ?? pwrChk();
  // ?? utlPet();
  // console?
  if (cgetq()) {
    if (!utl.ignoreCon) {
      c = cgetc();
      switch (c) {
      case 'q':
      case 'Q':
      case 'x':
      case 'X':
        utlStop("user quit");
        break;
      }
    } else 
      ciflush();
  }
} // utlX
