// ant.c - for working with antenna module
#include <common.h>
#include <ant.h>

/*
 * AntMode(G|I|S)
 * Switch antenna module between SBE39 TD, Iridium and GPS.
 * ^A Antenna G|I * ^B Binary byte * ^C Connect I|S * ^D powerDown I|S
 */
int AntMode(char r) {
  static char ant='-', dev='-';
  char a, d;
  DBG2(flogf("\n\t|AntMode(%c)", r);)
  DevSelect(DEVA);
  // select ant SBE16, switch ant device
  switch (r) {
    case 'G': { a='G'; d='I'; break; }
    case 'I': { a='I'; d='I'; break; }
    case 'S': { a=ant; d='S'; break; }
    default: { // bad case
      flogf("\nError DevSelect(%c): bad choice", r);
      return -1;
    }
  }
  if (d!=dev) {
    // turn on, connect
    TUTxPutByte(devicePort, 3, true);  // ^C connect device
    TUTxPutByte(devicePort, d, true);
    dev=d;
    Delayms(3000); // wait 3 sec for device start
  }
  if (a!=ant) {
    TUTxPutByte(devicePort, 1, true);  // ^A antenna
    TUTxPutByte(devicePort, a, true);  // G I
    ant=a;
    Delayms(1000); // wait 1 sec to settle antenna switch noise
  }
  return 0;
} //AntMode

