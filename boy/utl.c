// utl.c - utility stuff
#include <com.h>
// note: utl.h included from com.h

// allow up to .05 second between chars, normally chars take .001-.016
#define CHAR_DELAY 50

static char str[BUFSZ];         // used by local routines, *str returned
char scratch[BUFSZ];            // used by other modules

void utlDelay(int x) { 
  RTCDelayMicroSeconds((long)x*1000); 
}

///
// sets: (*line)
int utlTrim(char *line) {
  char c;
  int len = strlen(line);
  // trim off crlf at end 
  c = line[len-1]; if (c=='\r' || c=='\n') line[--len] = 0;
  c = line[len-1]; if (c=='\r' || c=='\n') line[--len] = 0;
  return len;
}

///
// put string to serial; queue, don't block, it should all buffer
// uses: scratch
void utlWrite(Serial port, char *out, char *eol) {
  int len, delay, sent;
  DBG0("utlWrite()")
  strcpy(scratch, out);
  if (eol!=NULL)
    strcat(scratch, eol);
  len = strlen(scratch);
  delay = CHAR_DELAY + (int)TUBlockDuration(port, (long)len);
  sent = (int)TUTxPutBlock(port, scratch, (long)len, (short)delay);
  DBG1("sent %d of %d", sent, len)
  DBG2("'%s'", utlNonPrint(scratch))
  if (len!=sent) 
    flogf("\nERR\t|utlWrite(%s) sent %d of %d", out, sent, len);
}

///
// read all the chars on the port, with a normal delay
// returns: length
int utlRead(Serial port, char *in) {
  int len = 0;
  if (TURxQueuedCount(port)<1) return 0;
  DBG0("utlRead()")
  // len = (int) TURxGetBlock(port, in, (long)BUFSZ, (short)CHAR_DELAY);
  for (len=0; len<BUFSZ; len++) {
    in[len] = TURxGetByteWithTimeout(port, (short)CHAR_DELAY);
    if (in[len]<0) break;
  }
  in[len]=0;            // string
  DBG1("%d->'%s'", len, utlNonPrint(in))
  return len;
}

///
// delay up to wait seconds for first char, null terminate
// assumes full string arrives promptly after a delay of several seconds
// return: length
int utlReadWait(Serial port, char *in, int wait) {
  int len;
  DBG0("utlReadWait(%d)")
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
  DBG1("%d->'%s'", len, utlNonPrint(in))
  return len;
}

// ?? check out __DATE__, __TIME__

///
// HH:MM:SS now
// returns: global static char *str
char * utlTime() {
  struct tm *tim;
  time_t secs;

  time(&secs);
  tim = localtime(&secs);
  sprintf(str, "%02d:%02d:%02d",
          tim->tm_hour, tim->tm_min, tim->tm_sec);
  return str;
} // utlTime

///
// Time & Date String
// MM/DD/YY HH:MM:SS now
// returns: global static char *str
char * utlTimeDate() {
  struct tm *tim;
  time_t secs;
  
  time(&secs);
  tim = localtime(&secs);
  sprintf(str, "%02d/%02d/%02d %02d:%02d:%02d", tim->tm_mon,
          tim->tm_mday, tim->tm_year - 100, tim->tm_hour,
          tim->tm_min, tim->tm_sec);
  return str;
} // utlTimeDate


///
// format non-printable string; null terminate
// returns: global static char *str
char *utlNonPrint (char *in) {
  char ch, *ptr = str;
  if (strlen(in)>BUFSZ) 
    return "utlNonPrint(char *in) is too big";
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
  return (str);
} // printsafe

void utlPet() { TickleSWSR(); }              // pet the watchdog

void utlShutdown(char *out) {
  sysStop(out);
}
