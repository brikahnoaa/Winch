// antmod.c

//  Program to communicate with T-D sensor and irid/gps switch
//  July 12, 2017 blk
//  April 2018 blk
// buoy is on com4. all serial is transfered between buoy and sbe
// EXCEPT for commands that begin with 001-007 ^A-^G

#include <antmod.h>

#define VERSION "3.1"
#define RS232_SETTLE 100000
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
#define BUOY_BAUD 9600L
// #define BUOY_BAUD 19200L
#define IRID_BAUD 9600L
#define BUFSIZE 4096

#define BUOY 0
#define SBE 1
#define IRID 2

DBG( bool echoDn=false; bool echoUp=false;)
uchar *buf;
char *LogFile = {"sys.log"}; 
char antSw;
struct { char *name, c; bool power; TUPort *port; } dev[3] = {
  { "BUOY", 'B', false, NULL },
  { "SBE", 'S', false, NULL },
  { "IRID", 'I', false, NULL } };
// short devID; // ID of upstream device, 1-2
TUPort *buoy=NULL, *devPort=NULL; // dev port of connnected upstream device


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
  power('S', true);

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
TUPort* OpenSbePt(bool on) {
  TUPort *sbePort=NULL;
  long baud = 9600L;
  short sb39rxch, sb39txch;
  if (on) {
    DBG1(flogf("Opening the SBE port 9600\n");)
    PIOSet(SBEPWR); // turn on SBE serial term power
    sb39rxch = TPUChanFromPin(SBERX);
    sb39txch = TPUChanFromPin(SBETX);

    // Define SBE TD tuporst
    RTCDelayMicroSeconds(100000L);
    sbePort = TUOpen(sb39rxch, sb39txch, baud, 0);
    if (sbePort == NULL) {
      flogf("\n!!! Error opening SBE channel...");
    } else {
      dev[SBE].port = sbePort;
    }
    return sbePort;
  } else { // if (!on) {
    TUClose(dev[SBE].port);
    dev[SBE].port=NULL;
    PIOClear(SBEPWR); // SBE TD
    return NULL;
  }
} //OpenSbePt(bool on) 


// OpenIridPt(bool on)
// If on=true, open the com.
// If on=false, close the com.
// IRQ3
/*
TUPort* OpenIridPt(bool on) {
  TUPort *IridPt=NULL;
  long baud = IRID_BAUD;
  short iridrxch, iridtxch;
  if (on) {
    DBG1(flogf("Opening the high speed IRID/GPS port\n");)
    PIOSet(A3LAPWR);                   // PWR ON
    iridrxch = TPUChanFromPin(A3LARX); //
    iridtxch = TPUChanFromPin(A3LATX);
    // Important!! This is connected to TXin of the internal RS232 IC
    PIORead(48); 
    PIORead(33);
    PIORead(IRQ3RXX);               // Make IRQ3 read. Not bus.
    RTCDelayMicroSeconds(1000000L); // wait for IRID/GPS unit to warm up

    // Define dev[SBE].port 
    IridPt = TUOpen(iridrxch, iridtxch, baud, 0);
    RTCDelayMicroSeconds(100000L);
    TUTxFlush(IridPt);
    TURxFlush(IridPt);
    RTCDelayMicroSeconds(1000000L);
    if (IridPt == NULL) {
      flogf("\n!!! Error opening IRIDGPS channel...");
    } else {
      dev[IRID].port=IridPt;
    }
    return IridPt;
  } else { // if (!on) {
    TUClose(dev[IRID].port);
    dev[IRID].port=NULL;
    PIOClear(A3LAPWR); // Shut down IRID PWR
    flogf("Close IRIDGPS term\n");
    return NULL;
  }
} //OpenIridPt(bool on) 
*/

// OpenBuoyPt(bool on) for uMPC. On MPC it is for AMODEM com
// If on=true, open the com.
// If on=false, close the com.
TUPort* OpenBuoyPt(bool on) {
  TUPort *BuoyPt=NULL;
  long baud = BUOY_BAUD;
  short com4rxch, com4txch;
  if (on) {
    DBG1(flogf("Opening the buoy COM4 port at %ld \n", baud);)
    PIOSet(COM4PWR); // PWR On COM4 device
    com4rxch = TPUChanFromPin(COM4RX);
    com4txch = TPUChanFromPin(COM4TX);
    PIORead(IRQ5);

    // Define COM4 tuport
    BuoyPt = TUOpen(com4rxch, com4txch, baud, 0);
    RTCDelayMicroSeconds(RS232_SETTLE);
    TUTxFlush(BuoyPt);
    TURxFlush(BuoyPt);
    if (BuoyPt == NULL) 
      flogf("\n!!! Error opening COM4 port...");
    else
      dev[BUOY].port=BuoyPt;
    return BuoyPt;
  } else { // if (!on) {
    TUClose(dev[BUOY].port);
    dev[BUOY].port=NULL;
    PIOClear(COM4PWR); // Shut down COM4 device
    DBG1(flogf("Close COM4 port\n");)
    return NULL;
  }
} //OpenBuoyPt(bool on) 




//
// help() - help message to console
//
void help() {
  // Identify the progam and build
  char *ProgramDescription = {
      "\n"
      "Buoy, com4 at 9600 BAUD\n"
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
  printf(ProgramDescription, BUOY_BAUD);
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
    default: return -1;
  }
} // char2id()

//
// power(I|S, on) - power device on/off
// returns 1 if power unchanged, 0 changed, -1 failed
//
short power(short c, bool onoff) {
  short id;
  TUPort *r;
  id=char2id(c);
  if (id == -1) {
    flogf( "ERR power(%c) '%d'\n", c, (short)c);
    return -1;
  }
  DBG1(printf("dev:%c onoff:%d\n", c, onoff);)
  if (dev[id].power == onoff) return 1;
  switch (c) {
    // case 'I': r=OpenIridPt(onoff); break;
    case 'S': r=OpenSbePt(onoff); break;
    case 'B': r=OpenBuoyPt(onoff); break;
  }
  if (onoff && (r == NULL)) { // fail, on returns tup*
    BIOSResetToPicoDOS();
    // return -1;
  }
  dev[id].power=onoff;
  dev[id].port=r;  // currently also done in Open*Pt
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
