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
  all.buf = malloc(BUFSZ);
  all.str = malloc(BUFSZ);
  utl.str = malloc(BUFSZ);
  // utl.buf used by utlRead
  utl.buf = malloc(BUFSZ);
  // utl.ret is returned by NonPrint and DateTime funcs
  utl.ret = malloc(BUFSZ);
  // sync this with enum ErrType
  utl.errName[ant_err] = "ant";
  utl.errName[boy_err] = "boy";
  utl.errName[cfg_err] = "cfg";
  utl.errName[s16_err] = "s16";
  utl.errName[s39_err] = "s39";
  utl.errName[iri_err] = "iri";
  utl.errName[ngk_err] = "ngk";
  utl.errName[wsp_err] = "wsp";
  utl.errName[log_err] = "log";
  utl.errName[dog_err] = "dog";
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
} // utlMatchAfter

///
// put block to serial
void utlWriteBlock(Serial port, char *out, int len) {
  int delay, sent;
  delay = 2 + (int)TUBlockDuration(port, (long)len);
  sent = (int)TUTxPutBlock(port, out, (long)len, (short)delay);
  DBG2(" >%d>", sent);
  DBG3(" >>%d'%s'>>", len, utlNonPrintBlock(out, len));
  if (len!=sent)
    flogf("\nERR\t|utlWriteBlock(%s) sent %d of %d", out, sent, len);
} // utlWriteBlock

///
// put string to serial; queue, don't block, it should all buffer
// uses: utl.str
void utlWrite(Serial port, char *out, char *eol) {
  int len;
  strcpy(utl.str, out);
  if (eol!=NULL) strcat(utl.str, eol);
  len = strlen(utl.str);
  utlWriteBlock(port, utl.str, len);
} // utlWrite

///
// read all the chars on the port, with a normal char delay; discard nulls=0
// char *in should be BUFSZ
// sets: *in = string
// rets: read, -1=overrun
int utlRead(Serial port, char *in) {
  short ch;
  int len;
  for (len=0; len<BUFSZ-1; len++) {
    ch = TURxGetByteWithTimeout(port, (short)CHAR_DELAY);
    if (ch<0) break; // timeout
    if (ch==0) len--; // discard null
    else in[len] = (char) ch;
  }
  in[len]=0;            // string
  DBG2(" <%d<", len);
  if (len) DBG3(" <<%d'%s'<<", len, utlNonPrintBlock(in, len));
  if (len>=BUFSZ-1) return -1;
  return len;
} // utlRead

///
// delay up to wait seconds for first char, null terminate
// assumes full string arrives promptly after a delay of several seconds
// sets: *in 
// rets: utlRead()
int utlReadWait(Serial port, char *in, int wait) {
  in[0] = 0;
  tmrStart(utl_tmr, wait);
  // wait for a char
  while (!TURxQueuedCount(port)) {
    utlDelay(CHAR_DELAY);
    if (tmrExp(utl_tmr)) return 0;
    utlX(); // twiddle thumbs
  }
  return( utlRead(port, in) );
}

///
// utlRead until we get the expected string (or timeout)
// note: reads past *expect if chars streaming, see utlLookFor utlLookForSecs
// args: port, buf for content, expect to watch for, wait timeout
// uses: utl.buf // sets: *in (string)
// rets: char* to expected str, or null
char *utlReadExpect(Serial port, char *in, char *expect, int wait) {
  char *r=NULL;
  int l, sz=0;
  DBG1("utlReadExpect(%s, %d)", expect, wait);
  in[0] = 0;
  tmrStart(utl_tmr, wait);
  // loop until expected or timeout
  while (!r) {
    l = utlRead(port, utl.buf);
    if (l) {
      sz += l;
      if (sz>=BUFSZ) break;
      strcat(in, utl.buf);
      r = strstr(in, expect);
      continue;
    } // read
    if (tmrExp(utl_tmr)) break;
    utlX();
  } // while tmr
  return r;
} // utlReadExpect

///
// read all the chars on the port, with a normal char delay; discard nulls=0
// like utlRead but stops reading on char match
// char *in should be BUFSZ, returns null terminated string
// sets: *in 
// rets: length, overrun=-1
int utlLookFor(Serial port, char *in, char *lookFor) {
  short ch;
  int len;
  for (len=0; len<BUFSZ; len++) {
    ch = TURxGetByteWithTimeout(port, (short)CHAR_DELAY);
    if (strchr(lookFor, ch)) break; // target char
    if (ch<0) break; // timeout
    if (ch==0) len--; // discard null
    else in[len] = (char) ch;
  }
  in[len]=0;            // string
  DBG2(" <%d<", len);
  if (len) DBG3(" <|%d'%s'<|", len, utlNonPrintBlock(in, len));
  if (len>=BUFSZ) return -1;
  return len;
} // utlLookFor

///
// wait then read chars until match in char *lookFor
// delay up to wait seconds for first char, null terminate
// like utlReadWait but stops reading on char match
// sets: *in 
// rets: 0=match 1=timeout 2=BUFSZ<
int utlLookForSecs(Serial port, char *in, char *lookFor, int secs) {
  static char *self="utlLookForSecs";
  static char *rets="0=match 1=timeout 2=BUFSZ<";
  char *p;
  int r=0, len=0;
  // note: in[len] == 0 all the way thru
  in[len] = 0;
  tmrStart(second_tmr, secs);
  while (true) { 
    if (len>=BUFSZ-1) { // overrun
      r=2;
      break;
    } // overrun
    if (tmrExp(second_tmr)) { // timeout
      r=1; 
      break; 
    } // timeout
    if (TURxQueuedCount(port)) { // get char
      in[len] = TURxGetByte(port, false);
      if (in[len] == 0) continue;  // skip null
      in[++len] = 0;
      p = strstr(in, lookFor);
      if (p) { // match
        *p=0;  // trim
        r=0;
        break;
      } // match
    } // get char
  }
  if (r>0) DBG1(" %s(%s)=%d ", self, lookFor, r);
  DBG2(" <%d<", len);
  DBG3(" <|%d'%s'<|", len, utlNonPrintBlock(in, len));
  return r;
} // utlLookForSecs

///
// wrapper for TURxGetBlock
// rets: got bytes
int utlGetBlock(Serial port, char *buff, int max, int respms) {
  int got;
  got = (int) TURxGetBlock(port, buff, (long)max, (short)respms);
  DBG3(" <[%d'%s'<[", got, utlNonPrintBlock(buff, got));
  return got;
}


///
// write time to log file
void utlLogTime(void) {
  flogf(" %s", utlTime());
}

///
// HH:MM:SS now
// rets: global char *utl.ret
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
// rets: global char *utl.ret
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
// rets: global char *utl.ret
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
// rets: global char *utl.ret
char *utlDateTime(void) {
  time_t secs;
  time(&secs);
  return utlDateTimeFmt(secs);
} // utlDateTime

///
// MMDDYYYYHHMMSS 
// rets: global char *utl.ret
char *utlDateTimeSBE(void) {
  struct tm *tim;
  time_t secs;
  time(&secs);
  tim = gmtime(&secs);
  sprintf(utl.ret, "%02d%02d%04d%02d%02d%02d", 
          tim->tm_mon+1, tim->tm_mday, tim->tm_year + 1900, 
          tim->tm_hour, tim->tm_min, tim->tm_sec);
  return utl.ret;
} // utlDateTimeSBE

///
// format non-printable string; null terminate
// rets: global char *utl.ret
char *utlNonPrint (char *in) {
  return (utlNonPrintBlock(in, strlen(in)));
} // utlNonPrint

///
// format non-printable string; null terminate
// rets: global char *utl.ret
char *utlNonPrintBlock (char *in, int len) {
  unsigned char ch;
  char *out = utl.ret;
  int i, o;
  // copy len bytes - note, o grows faster than i
  i = o = 0;
  while (i<len) {
    ch = in[i++];
    if (ch==0x0A) {
      out[o++] = '\\';
    } else if (ch==0x0D) {
      out[o++] = '/';
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
// sets: *log
int utlLogOpen(int *log, char *base) {
  int r=0, fd, flags;
  char path[64];
  static char *self="utlLogOpen";
  static char *rets="1=!open 2=!write";
  DBG();
  if (*log) {close(*log); *log=0;}
  utlLogPathName(path, base, all.cycle);
  flags = O_APPEND | O_CREAT | O_WRONLY;
  fd = open(path, flags);
  if (fd<=0) {
    sprintf(utl.str, "open ERR %d (errno %d), path %s", fd, errno, path);
    utlErr(log_err, utl.str);
    raise(1);
  } 
  DBG1("\n%s(%s):%d", self, base, fd);
  if (fd>5)
    flogf("\n %s(): high fd=%d for %s", fd, path);
  sprintf(utl.str, "\n---  %s ---\n", utlDateTime());
  r = write(fd, utl.str, strlen(utl.str)); 
  if (r<1) {
    sprintf(utl.str, "write ERR %d (errno %d) path %s", r, errno, path);
    utlErr(log_err, utl.str);
    close(fd);
    raise(2);
  }
  *log=fd;
  return 0;
} // utlLogOpen

/// 
// close file
// sets: *fd
int utlLogClose(int *fd) {
  static char *self="utlLogClose";
  static char *rets="1=!close";
  int f;
  DBG();
  if (*fd<1) return 0;   // no fd
  f=*fd;
  *fd=0;
  DBG2("\n%s():%d ", self, f);
  if (close(f)<0) {
    sprintf(utl.str, "close ERR (errno %d) fd=%d", errno, f);
    utlErr(log_err, utl.str);
    raise(1);
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
  utl.errCnt[err]++;
  flogf("\n------------------");
  flogf("\n-ERR(%s)\t|%d| %s %s", 
    utl.errName[err], utl.errCnt[err], utlTime(), str);
  flogf("\n------------------");
}

///
// nap called often
void utlNap(int sec) {
  DBG2("utlNap(%d)", sec);
  while(0<sec--) {
    utlX();
    utlDelay(1000);
  }
} // utlNap

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
  utlPet(utl.bone);  // use default utl.pet
  // are we responding to console?
  if (utl.console) {
    if (cgetq()) {
      c = tolower( cgetc() );
      switch (c) {
      case 'q':
      case 'x':
        utlStop("user quit");
        break;
      // toggle dbg on/off
      case '0': dbg.dbg0 = !dbg.dbg0; break;
      case '1': dbg.dbg1 = !dbg.dbg1; break;
      case '2': dbg.dbg2 = !dbg.dbg2; break;
      case '3': dbg.dbg3 = !dbg.dbg3; break;
      case '4': dbg.dbg4 = !dbg.dbg4; break;
      case 'd': (*dbg.funcPtr)(); break;
      case 't': utlTestLoop(); break;
      }
      ciflush();
    } // getq
  } // console
} // utlX

///
// set watchdog length to pet seconds, or default if 0
// this could be a macro
void utlPet(long pet) { all.watch = pet?pet:utl.bone; }

