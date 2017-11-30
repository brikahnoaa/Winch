#include <common.h>
#include <winch.h>

// pending delay firstRise lastRise firstFall lastFall
WinchData winch = {
  false, 0.0, 0.0, 0.0, 0.0, 0.0,
};
// off ascentCalls ascentRcv descentCalls descentRcv 
// stopCalls stopRcv buoyRcv winchCalls port
AmodemData amodem = {
  false, 0, 0, 0, 0, 0, 0, 0, 0, NULL,
};

void winchConsole() {
  char in;
  cprintf("\n\t|WinchConsole():");
  in = cgetc();

  // Stop
  if (in == 'S')
    winchStop();
  // Descend
  else if (in == 'F')
    winchDescend();
  // Ascend
  else if (in == 'R')
    winchAscend();
  // Buoy Status?
  else if (in == 'B')
    BuoyStatus();
  else
    cprintf("Bad Input Character.");

} //____ Console_Data() _____//

/*
 */
void AModemData(void) {
  char *inString; // Array for input command/response
  short inchar;
  char prechar; // Two ints to grab different characters of the input
  int i, decimal = 0, multiplier = 4096; // location within input array
  bool symbol = false; // Symbol if '#' command is present or not
  float cableLength = 0.0;
  char *command;
  char hexCode[] = "0000";
  float depth;
  static int AscentStopTries = 0;

  inString = (char *)calloc(64, sizeof(char));

  for (i = 0; i < 64; i++) { // Scan through input from NIGKPort
    inchar = TURxGetByteWithTimeout(NIGKPort, 500);
    if (inchar == -1) {
      break;
    } else
      inString[i] = (char)inchar;
  }

  // if command from winch. If stop, why? Update LARA System Statuses
  if (strchr(inString, '#') != NULL) {
    command = strtok(inString, "#");
    flogf("\n%s\t|Command: #%s", Time(NULL), command);
    prechar = command[0];
    symbol = true;
  }
  // Confirming a new winch command//response from command
  else if (strchr(inString, '%') != NULL) {
    command = strtok(inString, "%");
    flogf("\n%s\t|Response: %%%s", Time(NULL), command);
    prechar = command[0];
    symbol = false;

  } else if (strstr(inString, "OK") != NULL) {
    cprintf("OK");
    TURxFlush(NIGKPort);
    TUTxFlush(NIGKPort);
    free(inString);
    return;
  } else
    flogf("\n%s\t|other: %s", Time(NULL), inString);

  if (prechar == 'R') {

    if (LARA.BUOYMODE == 0)
      CTDSample(1);
    LARA.BUOYMODE = 1;
    amodem.ASCENTRCV++;
    // if(!symbol)

  } else if (prechar == 'F') {

    if (LARA.BUOYMODE == 0)
      CTDSample(1);
    LARA.BUOYMODE = 2;
    amodem.DESCENTRCV++;

  } else if (prechar == 'S') {

    if (symbol) {
      amodem.STOPRCV++;
      flogf("\n%s|Stop Command Received", Time(NULL));

      if (LARA.BUOYMODE == 1) { // winch Stops buoy from Ascending... Lack of
                                // Tension? Are we close to TDepth? Try Calling?
        depth = LARA.DEPTH - winch.TDEPTH;
        flogf("\n\t|AModemData() winch Stopped Buoy");
        if (depth > 5.0 &&
            AscentStopTries <
                2) { // if buoy still too deep. try ascending again
          winchAscend();
          AscentStopTries++;
        } else if (depth < 2.0 ||
                   AscentStopTries >= 1) { // if buoy less than 5 meters to
                                           // target depth OR tried ascending at
                                           // least twice before.
          flogf("\n\t|Buoy close enough to target depth. Try Calling");
          LARA.PHASE++;
          LARA.SURFACED = true;
          AscentStopTries = 0;
        }
      }
    }
    LARA.BUOYMODE = 0;

  }

  else if (prechar == 'B')
    BuoyStatus();

  TURxFlush(NIGKPort);
  free(inString);
  return;

} //___ AModem_Data() ____//
/*
 */
ulong winchAscend(void) {
  DBG0("\n%s|WinchAscend():", Time(NULL))

  TUTxWaitCompletion(NIGKPort);
  TUTxPrintf(NIGKPort, "#R,01,03\n");
  Delayms(25);
  amodem.ASCENTCALLS++;
  return (time(NULL) + (ulong)winch.DELAY);
} //___ Ascend() ____//
/*
 */
ulong winchDescend(void) {
  DBG0("\n%s|WinchDescend():", Time(NULL))

  TUTxWaitCompletion(NIGKPort);
  TUTxPrintf(NIGKPort, "#F,01,00\n");
  Delayms(25);
  amodem.DESCENTCALLS++;
  return (time(NULL) + (ulong)winch.DELAY); // why return time?
} //___ Descend() ____//
/*
 */
ulong winchStop(void) {
  DBG0("\n%s|WinchStop():", Time(NULL))

  TUTxWaitCompletion(NIGKPort);
  TUTxPrintf(NIGKPort, "#S,01,00\n");
  Delayms(25);
  amodem.STOPCALLS++;
  return (time(NULL) + (ulong)winch.DELAY);
}
/*
 * void Buoy(void)
 * A function call to the Buoy to get the status of equipment.
 * The return call through NIGKPort sends 2 bytes of data as the
 * response to the Buoy CommandWinch from the Deck Unit
 */
void BuoyStatus(void) {
  char BStatus[3] = "00"; // Base return call
  DBG0("\n%s|Buoy Status:", Time(NULL))

  if (LARA.BUOYMODE != 0) // If CTDPort is Active and the Buoy is in motion
    BStatus[1] = '1';
  else if (LARA.BUOYMODE == 0) // If the Buoy is inactive and the CTDPort is off
    BStatus[0] = '1';

  TUTxWaitCompletion(NIGKPort);
  TUTxPrintf(
      NIGKPort, "%%B,01,%c%c\n", BStatus[0],
      BStatus[1]); // Send rest of status via acoustic remote to deck unit
  Delayms(5000);        // Make sure all the Buoy Calls have been received....
  TURxFlush(NIGKPort); // Before clearing the NIGKPort Rx
  amodem.BUOYRCV++;

} //___ Buoy() ____//
/*
 * void OpenTUPortNIGK(bool)
 */
void OpenTUPortNIGK(bool on) {
  static bool once = true;
  short AModemRX, AModem_TX;
  flogf("\n\t|%s NIGK winch TUPort", on ? "Open" : "Close");

  if (on) {
    AModemRX = TPUChanFromPin(AMODEMRX);
    AModemTX = TPUChanFromPin(AMODEMTX);

    PIOClear(AMODEMPWR);
    Delayms(250);
    PIORead(48);
    PIOSet(AMODEMPWR); // Powers up the DC-DC for the Acoustic Modem Port
    NIGKPort = TUOpen(AModemRX, AModem_TX, AMODEMBAUD, 0);
    Delayms(150);
    if (NIGKPort == 0)
      flogf("\n\t|Bad winch TUPort\n");
    else {
      TUTxFlush(NIGKPort);
      TURxFlush(NIGKPort);
      Delayms(5000); // Wait 5 seconds for NIGKPort to power up
      TUTxPrintf(NIGKPort, "\n");
      Delayms(250);
      TUTxFlush(NIGKPort);
      TURxFlush(NIGKPort);
    }
  } else {
    Delayms(500);
    PIOClear(AMODEMPWR);
    TUClose(NIGKPort);
  }
  return;

} //___ OpenTUPort_NIGK() ____//
/*
 * void GetWinchSettings()
 */
void GetWinchSettings() {
  char *p;

  p = VEEFetchData(NIGKDELAYNAME).str;
  winch.DELAY = atoi(p ? p : NIGKDELAYDEFAULT);
  DBG1("winch.DELAY=%u (%s)\n", winch.DELAY, p ? "vee" : "def")

  p = VEEFetchData(NIGKANTENNALENGTHNAME).str;
  winch.ANTLEN = atoi(p ? p : NIGKANTENNALENGTHDEFAULT);
  DBG1("winch.ANTLEN=%u (%s)\n", winch.ANTLEN, p ? "vee" : "def")

  p = VEEFetchData(NIGKTARGETDEPTHNAME).str;
  winch.TDEPTH = atoi(p ? p : NIGKTARGETDEPTHDEFAULT);
  DBG1("winch.TDEPTH=%u (%s)\n", winch.TDEPTH, p ? "vee" : "def")

  p = VEEFetchData(NIGKRISERATENAME).str;
  winch.RRATE = atoi(p ? p : NIGKRISERATEDEFAULT);
  DBG1("winch.RRATE=%u (%s)\n", winch.RRATE, p ? "vee" : "def")

  p = VEEFetchData(NIGKFALLRATENAME).str;
  winch.FRATE = atoi(p ? p : NIGKFALLRATEDEFAULT);
  DBG1("winch.FRATE=%u (%s)\n", winch.FRATE, p ? "vee" : "def")

  p = VEEFetchData(NIGKPROFILESNAME).str;
  winch.PROFILES = atoi(p ? p : NIGKPROFILESDEFAULT);
  DBG1("winch.PROFILES=%u (%s)\n", winch.PROFILES, p ? "vee" : "def")

  p = VEEFetchData(NIGKRECOVERYNAME).str;
  winch.RECOVERY = atoi(p ? p : NIGKRECOVERYDEFAULT);
  DBG1("winch.RECOVERY=%u (%s)\n", winch.RECOVERY, p ? "vee" : "def")
}
/*
 * void winchMonitor(int filehandle)
 */
void winchMonitor(int filehandle) {
  // global WriteBuffer
  int byteswritten = 0;
  memset(WriteBuffer, 0, BUFSZ);
  sprintf(
      WriteBuffer, "NIGK "
                   "%s\nProfile:%d\nDelay:%d\nTDepth:%d\nRiseRate:%d\nFallRate:"
                   "%d\nASCENTCALLS:%d, RCV:%d\nDESCENTCALLS:%d, "
                   "RCV%d\nSTOPCALLS:%d, RCV%d\n\0",
      winch.RECOVERY ? "RECOVERY" : "ON", winch.PROFILES, winch.DELAY, winch.TDEPTH,
      winch.RRATE, winch.FRATE, amodem.ASCENTCALLS, amodem.ASCENTRCV,
      amodem.DESCENTCALLS, amodem.DESCENTRCV, amodem.STOPCALLS, amodem.STOPRCV);

  // Maybe include something about calculated velocities and cable length of
  // winch after #of seconds.
  flogf("\n\t|WinchMonitor():\n%s", WriteBuffer);

  byteswritten = write(filehandle, WriteBuffer, strlen(WriteBuffer));
  DBG1("\nBytesWritten: %d", byteswritten)
  // free(writebuffer);

  amodem.ASCENTCALLS = 0;
  amodem.ASCENTRCV = 0;
  amodem.DESCENTCALLS = 0;
  amodem.DESCENTRCV = 0;
  amodem.STOPCALLS = 0;
  amodem.STOPRCV = 0;
  amodem.BUOYRCV = 0;
  amodem.WINCHCALLS = 0;
}
