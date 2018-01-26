// utils.c - utility stuff
#include <utils.h>

// allow up to .05 second between chars, normally chars take .001-.016
#define CHAR_DELAY 50

char scratch[BUFSZ];

/*
 * put string to serial; queue, don't block, it should all buffer
 */
int serWrite(Serial port, char *out) {
  int len = strlen(out);
  int sent;
  sent = (int) TUTxPutBlock(port, *out, (long)len, (short)CHAR_DELAY);
  if (len!=sent) {
    flogf("\nERR\t| serWrite(%s) sent %d of %d", out, sent, len);
  return sent;
}

/*
 * read all the chars on the port, with a normal delay
 */
int serRead(Serial port, char *in) {
  int len = 0;
  if (TURxQueuedCount(port)>0) {
    // len = (int) TURxGetBlock(port, in, (long)BUFSZ, (short)CHAR_DELAY);
    for (len=0; len<BUFSZ; len++) {
      in[len] = TURxGetByteWithTimeout(port, (short)CHAR_DELAY);
      if (in[len]<0) {
        // normal exit
        break;
      }
    }
  }
  in[len]=0;        // string
  return len;
}


/*
 *  delay up to wait seconds for first char, null terminate
 *  assumes full string arrives promptly after a delay of several seconds
 *  return: length
 */
int serReadWait(Serial port, char *in, int wait) {
  int len = 0;
  in[0] = TURxGetByteWithTimeout(port, (short) wait*1000);
  TickleSWSR(); // could have been a long wait
  if (in[0]<=0) {
    // first char
    in[0]=0;
  } else
    // rest of input
    delayms(CHAR_DELAY);
    len = serRead(port, in+1) + 1;
  return len;
}

/*
 * HH:MM:SS now
 * sets: (&out)
 */
void clockTime(char *out) {
  RTCtm *rtc_time;
  ulong secs;
  ushort ticks;

  RTCGetTime(&secs, &ticks);
  rtc_time = RTClocaltime(&secs);
  sprintf(out, "%02d:%02d:%02d",
          rtc_time->tm_hour, rtc_time->tm_min, rtc_time->tm_sec);
} // clockTime

/* 
 * format non-printable string; null terminate
 * modifies out[] and returns *out, can be used in DBG1()
 */
char *unsprint (char *out, *in) {
  char ch, *ptr = out;
  // walk thru input until 0
  while (ch = *in++) {
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


void delayms(int x) { RTCDelayMicroSeconds((long)x*1000); }
