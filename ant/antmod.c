// antmod.c

//  Program to communicate with T-D sensor and irid/gps switch
//  July 12, 2017 blk
//  April 2018 blk
// buoy is on com4. all serial is transfered between buoy and sbe
// EXCEPT for commands that begin with 001-007 ^A-^G

#include <antmod.h>

#define VERSION "3.1"
#define RS232_SETTLE 1000
// Definitions of uMPC TPU ports
#define SBEPWR 23 // SB#39plus TD power
#define SBERX 32  // Tied to IRQ2
#define SBETX 31

#define PAMPWR 24     // PAM PWR on/off
#define PAMZEROBIT 29 // PAM selecton
#define PAMONEBIT 30  // PAM selection
#define PAMRX 28
#define PAMTX 27

#define ADCPWR 19
#define ANTSW 1

#define A3LAPWR 21 // IRIDGPS PWR
#define A3LARX 33  // IRIDGPS tied to /IRQ3
#define A3LATX 35

#define COM4PWR 22 // COM4 Enable
#define COM4RX 26  // Tied to /IRQ5
#define COM4TX 25
//#define       SYSCLK   8000           // choose: 160 to 32000 (kHz)
#define SYSCLK 16000            // choose: 160 to 32000 (kHz)
#define WTMODE nsStdSmallBusAdj // choose: nsMotoSpecAdj or nsStdSmallBusAdj
#define BAUD 9600L
#define BUFSIZE 4096

#define BUOY 0
#define SBE 1
#define IRID 2
#define ANT 3

DBG( bool echoDn=false; bool echoUp=false;)
uchar *buf;
char *LogFile = {"sys.log"}; 
char antSw;
struct { char *name, c; bool power; } dev[4] = {
  { "BUOY", 'B', false},
  { "SBE", 'S', false},
  { "IRID", 'I', false},
  { "ANT", 'A', false}
  };
TUPort *buoy, *sbe; // dev port of connnected upstream device

//
///	main
void main() {
  short ch;
  int arg;
  // escape to pico
  prerun();
  // DBG(echoDn=true;echoUp=true;)
  // set up hw
  init();
  DBG(flogf("\ndebug");)
  DBG1(flogf("\ndebug1");)
  buf = (uchar *)malloc(BUFSIZE);
  // initial connection is SBE
  power('I', false);
  power('S', true);
  sbe = OpenSbePt();
  buoyStr("ok\r\n");

  // exit via biosreset{topicodos}
  while (true) {
    // look for chars on both sides, process
    // get from dev upstream
    if (TURxQueuedCount(sbe)) {
      ch=TURxGetByte(sbe, true); // blocking, best to check queue first
      buoyCout(ch);
    } // char from device

    // get from buoy
    if (buoyQ()) {
      ch=buoyCin();
      if (ch<=8) {
        switch (ch) { // command
          case 0: // null
            DBG1(flogf("\nERR: NULL\n");)
            break;
          case 1: // ^A Antenna G|I
            arg=(int) buoyCin();
            antennaSwitch(arg);
            break;
          case 2: // ^B Break to SBE
            antBreak();
            break;
          case 3: // ^C Connect I|S
            arg=(int) buoyCin();
            power(arg, true);
            break;
          case 4: // ^D powerDown I|S
            arg=(int) buoyCin();
            power(arg, false);
            break;
          case 5: // ^E exit to pico
            BIOSResetToPicoDOS(); 
            break;
          case 6: // ^F print status
            status();
            break;
          case 7: // ^G
          case 8: // ^H help
            help();
            break;
        } // switch (command)
      // if (ch<=8)
      } else { 
        // regular char
        TUTxPutByte(sbe, ch, true);
      }
    } // if buoy

#ifdef BUOY_COM4
    // console
    if (SCIRxQueuedCount()) {
      ch=SCIRxGetChar();
      // SCIR is auto masked ch & 0x00FF;
      switch (ch) {
        case 'x': 
          BIOSResetToPicoDOS(); break;
        case 's':
          status(); break;
        case 'd':
          DBG(echoDn = !echoDn; )
          DBG(cprintf("\nechoDn: %s\n", echoDn ? "on" : "off"); )
          break;
        case 'u':
          DBG(echoUp = !echoUp; )
          DBG(cprintf("\nechoUp: %s\n", echoUp ? "on" : "off"); )
          break;
        default:
          help();
      } // switch (ch)
    } // if console 
#endif
  } // while run
} // main()




//
// OpenSbePt(true)
//
TUPort* OpenSbePt(void) {
  TUPort *sbePort=NULL;
  short sb39rxch, sb39txch;
  DBG1(flogf("Opening the SBE port 9600\n");)
  if (!dev[SBE].power)
    flogf("\nOpenSbePt() warning, not powered on");
  sb39rxch = TPUChanFromPin(SBERX);
  sb39txch = TPUChanFromPin(SBETX);
  sbePort = TUOpen(sb39rxch, sb39txch, BAUD, 0);
  if (sbePort == NULL) 
    flogf("\n!!! Error opening SBE channel...");
  RTCDelayMicroSeconds(RS232_SETTLE);
  TURxFlush(sbePort);
  return sbePort;
} // OpenSbePt

TUPort* OpenBuoyPt(void) {
  TUPort *BuoyPt=NULL;
  short com4rxch, com4txch;
  DBG1(flogf("Opening the buoy COM4 port at %ld \n", BAUD);)
  com4rxch = TPUChanFromPin(COM4RX);
  com4txch = TPUChanFromPin(COM4TX);
  PIORead(IRQ5);
  // Define COM4 tuport
  BuoyPt = TUOpen(com4rxch, com4txch, BAUD, 0);
  if (BuoyPt == NULL) 
    flogf("\n!!! Error opening COM4 port...");
  RTCDelayMicroSeconds(RS232_SETTLE);
  TUTxFlush(BuoyPt);
  TURxFlush(BuoyPt);
  return BuoyPt;
} // OpenBuoyPt




//
// help() - help message to console
//
void help() {
  // Identify the progam and build
  char *ProgramDescription = {
      "\n"
      "Buoy, com4 at %d BAUD\n"
      " ^A Antenna G|I \n"
      " ^B 5sec Break to SBE \n"
      " ^C powerup I|S \n"
      " ^D powerDown I|S \n"
      " ^E Exit to pico \n"
      " ^F status \n"
      " ^G unused \n"
      " ^H help msg \n"
      DBG("  if debug, d=echo Downstream, u=echo Upstream\n")
      };

  printf("\nProgram: %s: v%s, %s %s \n", __FILE__, VERSION, __DATE__,
         __TIME__);
  printf("Persistor CF%d SN:%ld   BIOS:%d.%02d   PicoDOS:%d.%02d\n", CFX,
         BIOSGVT.CFxSerNum, BIOSGVT.BIOSVersion, BIOSGVT.BIOSRelease,
         BIOSGVT.PICOVersion, BIOSGVT.PICORelease);
  printf(ProgramDescription, BAUD);
  status();
} // help()

///
// init() - initialize hardware, open com ports
void init() {
  // I am here because the main eletronics powered this unit up and *.app
  // program started.
  // Make sure unnecessary devices are powered down
  PIOClear(A3LAPWR);
  PIOClear(SBEPWR);
  PIOClear(COM4PWR);
  PIOClear(PAMPWR);
  PIOClear(ADCPWR);
  CSSetSysAccessSpeeds(nsFlashStd, nsRAMStd, nsCFStd, WTMODE);
  TMGSetSpeed(SYSCLK);
  // Initialize activity logging
  Initflog(LogFile, true);
  // settle antenna
  antennaSwitch('I');
  RTCDelayMicroSeconds(500000L);
  antennaSwitch('G');
  // Initialize to open TPU UART
  TUInit(calloc, free);
  // buoy
  PIOSet(COM4PWR);
  buoy = OpenBuoyPt();
} // init()

void status() {
  char out[80];
  sprintf(out, "antenna:%c iridgps-A3LA:%s sbe39:%s \n",
    antSw, dev[IRID].power ? "on" : "off", dev[SBE].power ? "on" : "off");
  buoyStr(out);
}

//
// antennaSwitch(ch) - change antenna=G|I, else return current state
//
void antennaSwitch(char c) {
  // global short antSw
  DBG1(printf("antennaSwitch %c\n", c);)
  switch(c) {
    case 'G': PIOClear(ANTSW); break;
    case 'I': PIOSet(ANTSW); break;
    default: flogf("antennaSwitch(%c) ?\n", c); return;
  }
  antSw=c;
} // antennaSwitch()

//
// char2id('G') - returns id 0-2, no match -1
//
short char2id(short ch) {
  switch (ch) {
    case 'I': return IRID;
    case 'S': return SBE;
    case 'B': return BUOY;
    case 'A': return ANT;
    default: return -1;
  }
} // char2id()

//
// power(I|S, on) - power device on/off
// returns 1 if power unchanged, 0 changed, -1 failed
//
short power(short c, bool on) {
  short id;
  id=char2id(c);
  if (id == -1) {
    flogf( "ERR power(%c) '%d'\n", c, (short)c);
    return -1;
  }
  if (dev[id].power == on) return 1;
  DBG1(printf("dev:%c on:1/off:0:%d\n", c, on);)
  switch (c) {
    case 'I': 
      if (on) PIOSet(A3LAPWR);
      else PIOClear(A3LAPWR);
      break;
    case 'S':
      if (on) PIOSet(SBEPWR);
      else PIOClear(SBEPWR);
      break;
  }
  dev[id].power=on;
  return 0;
} // power()

// print ascii or hex for non-printables
void printchar(char c) {
  // < or >
  if ((c>=32)&&(c<=126)) // printable
    cprintf("%c", c);
  else cprintf(" x%02X ", c);
  if (c==10) printf("\n");
  cdrain();
}

void antBreak(void) {
  TUTxBreak(sbe, 5000);
}

// short count, exit, first thing
void prerun() {
  short i=2;
  ciflush();
  cprintf("Press [SPACE] key to exit. 3 seconds, %d", i);
  while (i--) {
    RTCDelayMicroSeconds(1000000L);
    if (SCIRxGetByte(false) == ' ') {
      VEEStoreStr("ERR", "User power down");
      BIOSResetToPicoDOS();
    }
    cprintf(" %d", i);
    coflush();
  }
  cprintf("\n");
}

/// 
// check buoy port for input
int buoyQ(void) {
#ifdef BUOY_COM4
  return TURxQueuedCount(buoy);
#else
  return (cgetq());
#endif
}

///
// get char from buoy, blocking
char buoyCin(void) {
#ifdef BUOY_COM4
  return TURxGetByte(buoy, true);
#else
  return(cgetc());
#endif
}

/// 
// push ch to buoy
void buoyCout(char ch) {
#ifdef BUOY_COM4
  TUTxPutByte(buoy, ch, true); // block if queue is full
#else
  cputc(ch);
#endif
}

///
// push string to buoy
void buoyStr(char *out) {
#ifdef BUOY_COM4
  TUTxPrintf(out);
#else
  cprintf(out);
#endif
}

