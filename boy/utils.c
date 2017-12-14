// utils.c - utility stuff
#include <utils.h>

// allow up to .05 second between chars, normally chars take .001-.016
#define CHARDELAY 50

char scratch[BUFSZ];

void delayms(int x) {RTCDelayMicroSeconds((long) 1000 * (long) x); }

/*
 * put string to serial
 */
void serWrite(Serial port, char *out) {
  while (*out) { TUTxPutByte(port, *out++, true); }
}

/*
 *  delay up to wait seconds for first char, null terminate
 *  return length
 */
int serReadWait(Serial port, char *in, int wait) {
  int len;
  in[0] = TURxGetByteWithTimeout(port, (short) wait*1000);
  TickleSWSR(); // could have been a long wait
  if (in[0]<=0) {
    DBG1("\t|getStringWait() timeout")
    in[0]=0;
    return (-1);
  }
  for (len=1; len<BUFSZ; len++) {
    in[len] = TURxGetByteWithTimeout(port, CHARDELAY);
    if (in[len]<0) {
      // expect timeout to end
      break;
    }
  }
  in[len]=0;
  return len;
}

/* 
 * format non-printable string; null terminate
 * modifies out[] and returns *out, can be used in DBG1()
 */
char *printSafe (char *out, *in) {
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


