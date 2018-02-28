// utl.c - utility stuff
// note: utl.h included from com.h
#include <com.h>

// allow up to .05 second between chars, normally chars take .001-.016
#define CHAR_DELAY 50

char scratch[BUFSZ];

void delayms(int x) { 
  RTCDelayMicroSeconds((long)x*1000); 
}

//
// sets: (*str)
//
int crlfTrim(char *str) {
  char c;
  int len = strlen(str);
  // trim off crlf at end 
  c = str[len-1]; if (c=='\r' || c=='\n') str[--len] = 0;
  c = str[len-1]; if (c=='\r' || c=='\n') str[--len] = 0;
  return len;
}

//
// put string to serial; queue, don't block, it should all buffer
// returns: charsSent
//
int serWrite(Serial port, char *out) {
  int len, delay, sent;
  DBG0("serWrite()")
  len = strlen(out);
  delay = CHAR_DELAY + (int)TUBlockDuration(port, (long)len);
  sent = (int)TUTxPutBlock(port, out, (long)len, (short)delay);
  DBG1("sent %d of %d", sent, len)
  DBG2("'%s'", unsprintf(scratch, out))
  if (len!=sent) 
    flogf("\nERR\t|serWrite(%s) sent %d of %d", out, sent, len);
  return sent;
}

//
// read all the chars on the port, with a normal delay
// returns: length
//
int serRead(Serial port, char *in) {
  int len;
  if (TURxQueuedCount(port)<1) return 0;
  DBG0("serRead()")
  // len = (int) TURxGetBlock(port, in, (long)BUFSZ, (short)CHAR_DELAY);
  for (len=0; len<BUFSZ; len++) {
    in[len] = TURxGetByteWithTimeout(port, (short)CHAR_DELAY);
    if (in[len]<0) {
      // normal exit
      break;
    }
  }
  in[len]=0;        // string
  DBG1("->'%s'", in)
  return len;
}

//
//  delay up to wait seconds for first char, null terminate
//  assumes full string arrives promptly after a delay of several seconds
//  return: length
//
int serReadWait(Serial port, char *in, int wait) {
  int len;
  DBG0("serReadWait(%d)")
  in[0] = TURxGetByteWithTimeout(port, (short) wait*1000);
  pet(); // could have been a long wait
  if (in[0]<=0) {
    // timeout
    in[0]=0;
    len = 0;
  } else {
    // rest of input, note serRead exits if nothing queued
    delayms(CHAR_DELAY);
    len = serRead(port, in+1) + 1;
    DBG1("->'%s'", in)
  }
  return len;
}

// check out __DATE__, __TIME__
//
// HH:MM:SS now
// sets: (*out)
// returns: out
//
char * clockTime(char *out) {
  struct tm *tim;
  time_t secs;

  time(&secs);
  tim = localtime(&secs);
  sprintf(out, "%02d:%02d:%02d",
          tim->tm_hour, tim->tm_min, tim->tm_sec);
  return out;
} // clockTime

//
// Time & Date String
// MM/DD/YY HH:MM:SS now
// sets: (*out)
// returns: out
//
char * clockTimeDate(char *out) {
  struct tm *tim;
  time_t secs;
  
  time(&secs);
  tim = localtime(&secs);
  sprintf(out, "%02d/%02d/%02d %02d:%02d:%02d", tim->tm_mon,
          tim->tm_mday, tim->tm_year - 100, tim->tm_hour,
          tim->tm_min, tim->tm_sec);
  return out;
} // clockTimeDate


// 
// format non-printable string; null terminate
// modifies out[] and returns *out, can be used in DBG1()
//
char *unsprintf (char *out, char *in) {
  char ch, *ptr = out;
  // walk thru input until 0
  while ((ch = *in++)!=0) {
    if ((ch<32)||(ch>126)) {
      // non printing char
      sprintf(ptr, " x%02X ", ch);
      ptr += 5;     // five char hex ' x1A '
    } else {
      *ptr = ch;
      ptr++;
    }
  }
  *ptr = 0;
  return (out);
} // printsafe

void pet() { TickleSWSR(); }              // pet the watchdog

void shutdown(char *out) {
  sysStop(out);
}
