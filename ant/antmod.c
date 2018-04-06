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
struct { char *name, c; bool power; TUPort *port; } dev[4] = {
  { "BUOY", 'B', false, NULL },
  { "SBE", 'S', false, NULL },
  { "IRID", 'I', false, NULL },
  { "ANT", 'A', false, NULL }
  };
TUPort *buoy, *devPort; // dev port of connnected upstream device

//
///	main
void main() {
  short ch;
  int arg, arg2;

  // escape to pico
  prerun();
  DBG1(echoDn=true;echoUp=true;)
  // set up hw
  init();
  buf = (uchar *)malloc(BUFSIZE);
  // initial connection is SBE
  power('I', false);
  power('S', true);
  devPort = OpenSbePt();
  buoy = OpenBuoyPt();

  // exit via biosreset{topicodos}
  while (true) {
    // look for chars on both sides, process
    // note: using vars buoy,devport is faster than dev[id].port
    // get from dev upstream
    if (devPort && TURxQueuedCount(devPort)) {
      ch=TURxGetByte(devPort, true); // blocking, best to check queue first
      TUTxPutByte(buoy, ch, true); // block if queue is full
      DBG( if (echoDn) printchar(ch); )
    } // char from device

    // get from buoy
    if (TURxQueuedCount(buoy)) {
      // blocking, best to check queue first
      ch=TURxGetByte(buoy, true) & 0x00FF; 
      if (ch<8) {
        // get argument
        arg=(int) TURxGetByte(buoy, true) & 0x00FF; // blocking
        switch (ch) { // command
          case 0: // null
            flogf("\nERR: NULL\n");
            break;
          case 1: // ^A Antenna G|I
            antennaSwitch(arg);
            break;
          case 2: // ^B Binary Block 2bytes arg
            // get another byte
            DBG1( printchar('-'); printchar((char) arg);)
            arg2=(int) TURxGetByte(buoy, true) & 0x00FF;
            arg=(int)(arg<<8) + arg2;
            DBG1( printchar('-'); printchar((char) (arg2)); printchar('-');)
            transBlock((long) arg);
            break;
          case 3: // ^C Connect I|S
            power(arg, true);
            break;
          case 4: // ^D powerDown I|S
            power(arg, false);
            break;
          case 5: // ^E exit to pico
            BIOSResetToPicoDOS(); 
            break;
          default: // uhoh
            flogf("ERR: illegal command %d\n", ch);
            cdrain();
            break; // exit
        } // switch (command)
        DBG1(printf("cmd:%d arg:%d\n", ch, arg);)
      // if (ch<8)
      } else { 
        // regular char
        TUTxPutByte(devPort, ch, true);
        DBG( if (echoUp) printchar(ch); )
      }
    } // if buoy

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
          echoDn = !echoDn; 
          cprintf("\nechoDn: %s\n", echoDn ? "on" : "off"); 
          break;
        case 'u':
          echoUp = !echoUp; 
          cprintf("\nechoUp: %s\n", echoUp ? "on" : "off"); 
          break;
        default:
          help();
      } // switch (ch)
    } // if console 
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
  if (sbePort == NULL) {
    flogf("\n!!! Error opening SBE channel...");
  } else {
    dev[SBE].port = sbePort;
  }
  RTCDelayMicroSeconds(RS232_SETTLE);
  TURxFlush(sbePort);
  return sbePort;
} // OpenSbePt

TUPort* OpenBuoyPt(void) {
  TUPort *BuoyPt=NULL;
  short com4rxch, com4txch;
  DBG1(flogf("Opening the buoy COM4 port at %ld \n", BAUD);)
  PIOSet(COM4PWR); // PWR On COM4 device
  com4rxch = TPUChanFromPin(COM4RX);
  com4txch = TPUChanFromPin(COM4TX);
  PIORead(IRQ5);

  // Define COM4 tuport
  BuoyPt = TUOpen(com4rxch, com4txch, BAUD, 0);
  RTCDelayMicroSeconds(RS232_SETTLE);
  TUTxFlush(BuoyPt);
  TURxFlush(BuoyPt);
  if (BuoyPt == NULL) 
    flogf("\n!!! Error opening COM4 port...");
  else
    dev[BUOY].port=BuoyPt;
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
      " ^B Blockmode (2B=length) \n"
      " ^C powerup I|S \n"
      " ^D powerDown I|S \n"
      " ^E Exit to pico \n"
      " ^F unused \n"
      " ^G unused \n"
      "On console (com1):\n s=status x=exit *=this message\n"
      DBG("  if debug, d=echo Downstream, u=echo Upstream\n")
      };

  printf("\nProgram: %s: v%s, %s %s \n", __FILE__, VERSION, __DATE__,
         __TIME__);
  printf("Persistor CF%d SN:%ld   BIOS:%d.%02d   PicoDOS:%d.%02d\n", CFX,
         BIOSGVT.CFxSerNum, BIOSGVT.BIOSVersion, BIOSGVT.BIOSRelease,
         BIOSGVT.PICOVersion, BIOSGVT.PICORelease);
  printf(ProgramDescription, BAUD);
} // help()

//
// init() - initialize hardware, open com ports
//
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

  power('B', true);
  buoy=dev[BUOY].port;
} // init()

//
// status() 
//
void status() {
  cprintf("antenna:%c iridgps-A3LA:%s sbe39:%s \n",
    antSw, dev[IRID].power ? "on" : "off", dev[SBE].power ? "on" : "off");
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

// short count, exit, first thing
void prerun() {
  short i=2;
  SCIRxFlush();
  cprintf("Press any key to exit. 3 seconds, %d", i);
  while (i--) {
    RTCDelayMicroSeconds(1000000L);
    if (SCIRxGetByte(false) != RxD_NO_DATA) BIOSResetToPicoDOS();
    cprintf(" %d", i);
    coflush();
  }
  cprintf("\n");
}

// block transfer from buoy to devID
void transBlock(long b) {
  DBG(int len;)
  long count;
  // long TURxGetBlock(TUPort *tup, uchar *buffer, long bytes, short millisecs);
  // long TUTxPutBlock(TUPort *tup, uchar *buffer, long bytes, short millisecs);
  count = TURxGetBlock(buoy, buf, b, 50000);
  if (count != b) 
    cprintf("Error: getblock %ld != expected %ld \n", count, b);
  count = TUTxPutBlock(devPort, buf, b, 10000);
  if (count != b) 
    cprintf("Error: putblock %ld != expected %ld \n", count, b);
  DBG(if (echoUp||echoDn) cprintf(" [[%ld]] ", count);)
  DBG(
  len = (int) TURxQueuedCount(devPort); // accumulated
  if (len > 0)
      cprintf("%d bytes accumulated on devPort \n", len);
  )
}
