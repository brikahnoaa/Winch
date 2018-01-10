#include <common.h>
#include <ngk.h>

// pending delay firstRise lastRise firstFall lastFall
NgkInfo ngk = {
  false, 0.0, 0.0, 0.0, 0.0, 0.0,
};
// off ascentCalls ascentRcv descentCalls descentRcv 
// stopCalls stopRcv buoyRcv ngkCalls port
AmodemInfo amodem = {
  false, 0, 0, 0, 0, 0, 0, 0, 0, NULL,
};

void ngkConsole(void) {
  char in;
  cprintf("\n\t|NgkConsole():");
  in = cgetc();

  // Stop
  if (in == 'S')
    ngkStop();
  // Descend
  else if (in == 'F')
    ngkDescend();
  // Ascend
  else if (in == 'R')
    ngkAscend();
  // Buoy Status?
  else if (in == 'B')
    BuoyStatus();
  else
    cprintf("Bad Input Character.");

} // Console_Data

/*
 */
void AModemData(void) {
  // global scratch
  char *inString=scratch; // Array for input command/response
  short inchar;
  char prechar; // Two ints to grab different characters of the input
  int i, decimal = 0, multiplier = 4096; // location within input array
  bool symbol = false; // Symbol if '#' command is present or not
  float cableLength = 0.0;
  char *command;
  char hexCode[] = "0000";
  float depth;
  static int AscentStopTries = 0;

  for (i = 0; i < 64; i++) { // Scan through input from NIGKPort
    inchar = TURxGetByteWithTimeout(NIGKPort, 500);
    if (inchar == -1) {
      break;
    } else
      inString[i] = (char)inchar;
  }

  // if command from ngk. If stop, why? Update LARA System Statuses
  if (strchr(inString, '#') != NULL) {
    command = strtok(inString, "#");
    flogf("\n%s\t|Command: #%s", Time(NULL), command);
    prechar = command[0];
    symbol = true;
  }
  // Confirming a new ngk command//response from command
  else if (strchr(inString, '%') != NULL) {
    command = strtok(inString, "%");
    flogf("\n%s\t|Response: %%%s", Time(NULL), command);
    prechar = command[0];
    symbol = false;

  } else if (strstr(inString, "OK") != NULL) {
    cprintf("OK");
    TURxFlush(NIGKPort);
    TUTxFlush(NIGKPort);
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

      if (LARA.BUOYMODE == 1) { // ngk Stops buoy from Ascending... Lack of
                                // Tension? Are we close to TDepth? Try Calling?
        depth = LARA.DEPTH - ngk.TDEPTH;
        flogf("\n\t|AModemData() ngk Stopped Buoy");
        if (depth > 5.0 &&
            AscentStopTries <
                2) { // if buoy still too deep. try ascending again
          ngkAscend();
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
  return;

} // AModem_Data

/*
 * send up command, no brake, "#R,01,03"
 * sets: ngk.on .pending
 */
void ngkAscend(void) {
  DBG0("\n%s\t|ngkAscend():", Time(NULL))
  TUTxWaitCompletion(NIGKPort);
  TUTxPrintf(NIGKPort, "#R,01,03\n");
  ngk.on = true;          // motor on
  ngk.pending = true;     // response pending
} // Ascend

/*
 * down "#F,01,00"
 * sets: ngk.on .pending
 */
void ngkDescend(void) {
  DBG0("\n%s\t|ngkDescend():", Time(NULL))

  TUTxWaitCompletion(NIGKPort);
  TUTxPrintf(NIGKPort, "#F,01,00\n");
  ngk.on = true;          // motor on
  ngk.pending = true;     // response pending
} // Descend

/*
 * stop "#S,01,00"
 * sets: ngk.on .pending
 */
void ngkStop(void) {
  DBG0("\n%s\t|ngkStop():", Time(NULL))

  TUTxWaitCompletion(NIGKPort);
  TUTxPrintf(NIGKPort, "#S,01,00\n");
  ngk.on = false;          // motor off
  ngk.pending = true;     // response pending
}

/*
 * void Buoy(void)
 * A function call to the Buoy to get the status of equipment.
 * The return call through NIGKPort sends 2 bytes of data as the
 * response to the Buoy CommandWinch from the Deck Unit
 */
void BuoyStatus(void) {
  char BStatus[3] = "00"; // Base return call
  DBG0("%s|Buoy Status:", Time(NULL))

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

} // Buoy
/*
 * void amodemInit(bool)
 */
void amodemInit(bool on) {
  static bool once = true;
  short AModemRX, AModem_TX;
  flogf("\n\t|%s NIGK ngk TUPort", on ? "Open" : "Close");

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
      flogf("\n\t|Bad ngk TUPort\n");
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

} // amodemInit
/*
 * void GetWinchSettings()
 */
void GetWinchSettings(void) {
  char *p;

  p = VEEFetchData(NIGKDELAYNAME).str;
  ngk.DELAY = atoi(p ? p : NIGKDELAYDEFAULT);
  DBG1("ngk.DELAY=%u (%s)", ngk.DELAY, p ? "vee" : "def")

  p = VEEFetchData(NIGKANTENNALENGTHNAME).str;
  ngk.ANTLEN = atoi(p ? p : NIGKANTENNALENGTHDEFAULT);
  DBG1("ngk.ANTLEN=%u (%s)", ngk.ANTLEN, p ? "vee" : "def")

  p = VEEFetchData(NIGKTARGETDEPTHNAME).str;
  ngk.TDEPTH = atoi(p ? p : NIGKTARGETDEPTHDEFAULT);
  DBG1("ngk.TDEPTH=%u (%s)", ngk.TDEPTH, p ? "vee" : "def")

  p = VEEFetchData(NIGKRISERATENAME).str;
  ngk.RRATE = atoi(p ? p : NIGKRISERATEDEFAULT);
  DBG1("ngk.RRATE=%u (%s)", ngk.RRATE, p ? "vee" : "def")

  p = VEEFetchData(NIGKFALLRATENAME).str;
  ngk.FRATE = atoi(p ? p : NIGKFALLRATEDEFAULT);
  DBG1("ngk.FRATE=%u (%s)", ngk.FRATE, p ? "vee" : "def")

  p = VEEFetchData(NIGKPROFILESNAME).str;
  ngk.PROFILES = atoi(p ? p : NIGKPROFILESDEFAULT);
  DBG1("ngk.PROFILES=%u (%s)", ngk.PROFILES, p ? "vee" : "def")

  p = VEEFetchData(NIGKRECOVERYNAME).str;
  ngk.RECOVERY = atoi(p ? p : NIGKRECOVERYDEFAULT);
  DBG1("ngk.RECOVERY=%u (%s)", ngk.RECOVERY, p ? "vee" : "def")
}
/*
 * void ngkMonitor(int filehandle)
 */
void ngkMonitor(int filehandle) {
  // global WriteBuffer
  int byteswritten = 0;
  memset(WriteBuffer, 0, BUFSZ);
  sprintf(
      WriteBuffer, "NIGK "
                   "%s\nProfile:%d\nDelay:%d\nTDepth:%d\nRiseRate:%d\nFallRate:"
                   "%d\nASCENTCALLS:%d, RCV:%d\nDESCENTCALLS:%d, "
                   "RCV%d\nSTOPCALLS:%d, RCV%d\n\0",
      ngk.RECOVERY ? "RECOVERY" : "ON", ngk.PROFILES, ngk.DELAY, ngk.TDEPTH,
      ngk.RRATE, ngk.FRATE, amodem.ASCENTCALLS, amodem.ASCENTRCV,
      amodem.DESCENTCALLS, amodem.DESCENTRCV, amodem.STOPCALLS, amodem.STOPRCV);

  // Maybe include something about calculated velocities and cable length of
  // ngk after #of seconds.
  flogf("\n\t|NgkMonitor():\n%s", WriteBuffer);

  byteswritten = write(filehandle, WriteBuffer, strlen(WriteBuffer));
  DBG1("BytesWritten: %d", byteswritten)

  amodem.ASCENTCALLS = 0;
  amodem.ASCENTRCV = 0;
  amodem.DESCENTCALLS = 0;
  amodem.DESCENTRCV = 0;
  amodem.STOPCALLS = 0;
  amodem.STOPRCV = 0;
  amodem.BUOYRCV = 0;
  amodem.WINCHCALLS = 0;
}
