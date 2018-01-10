// boy.c
// Acoustic Real-Time Sensor ARTS, aka LARA

#include <common.h>
#include <boy.h> 

#include <ant.h> 
#include <ctd.h>
#include <mpc.h>
#include <ngk.h>
#include <sys.h>
#include <wispr.h>

BuoyData boy = {
  true, "QUEH", "LARA", "LR01", 
  3.0, 60.0, 8.0, 
  1, 1, 100, 0, 0, 0, 50, 
  (time_t)0, (time_t)0,
};

/*
 * deploy or reboot, loop over phase 1-4
 * uses: .startPhase
 * sets: .dockDepth .phase  
 */
void boyMain(int starts) {
  // normal case is last
  boy.phase = boy.startPhase;
  if (starts>1) {
    // post-deploy restart
    reboot(&boy.phase);
  } else if (boy.phase!=deploy_pha) {
    // testing, skip ahead to startPhase
    flogf("\nboyMain(): testing, start phase %d", boy.phase);
  } else {
    // normal start
    deployPhase(&boy.dockDepth);
  }
    
  while (true) {
    switch (boy.phase) {
    case data_pha: // data collect by WISPR
      dataPhase();
      boy.phase=rise_pha;
      break;
    case rise_pha: // Ascend buoy, check for current and ice
      risePhase();
      boy.phase=call_pha;
      break;
    case call_pha: // Call home via Satellite
      callPhase();
      boy.phase=drop_pha;
      break;
    case drop_pha: // Descend buoy, science sampling
      dropPhase();
      boy.phase=data_pha;
      break;
    }
  } // while true
} // boyMain() 

/*
 * set up ctd, serial port
 */
void boyInit(Serial *port) {
} // boyInit

/*
 * figure out whats happening, continue as possible
 */
void sysReboot(int *phase) {
  // load info from saved previous phase
  // check STARTSVEE - was saved state really the last boot?
  // match hardware to saved state
  // ask antmod for our velocity
} // reboot()

/*
 * wispr recording and detecting, buoy is docked to ngk
 */
void dataPhase(void) {
  flogf("\n\t|phase ONE");
  boy.phaseStartT=time(0);

  // Initialize System Timers
  Check_Timers(power.interval);

  // Stay here until system_timer says it's time to send data, user input for
  // different phase, NIGK R
  while (!boy.DATA && boy.phase == 1) {
    // sleep needs a lot of optimizing to be worth the trouble
    // Sleep();
    Incoming_Data();
    if (System_Timer() == 2)
      boy.DATA = true;
  }
  if (WISPR_Status()) { // moved here from p3
    WISPRSafeShutdown();
  }

} // phase1()

/*
 * turn on ant, ascend. check angle, go up halfway, check angle, surface.
 * sideways is caused by ocean current pushing the buoy and antmod
 * uses: ctd.delay .sideMax
 * sets: boy.phaseStartT .phase ant.depth ctd.depth
 * sets: sys.alarm[] 
 */
void phase2(void) {
  flogf("\n\t| phase2()");
  time_t riseStartT=0;
  float depth, depthStart, sideways, halfway, velocity;
  //
  boy.phaseStartT=time(0);
  antInit();
  depthStart = depth = antDepth();
  // algor: current check. rise halfway, checking response
  if (boyOceanCurrentCheck()) {
    sys.alarm[dockedCurrent_alm] += 1;
    boy.phase = data_pha;
    return;
  }
  halfway = depth/2.0;
  while (depth>halfway) {
    // start rise (or retry if ngk timeout)
    if (!riseStartT) {
      riseStartT = time(0);
      timStart(ngk_tim, 16);       // response comes in 13s
      ngkAscend();
      // start tracking antDepth
    }
    // ngk: "going up" or "stopped"
    switch (ngkResponse()) {
    case 0: break;
    case 1: // unexpected stop
      flogf("\np0(): ngk stop during rise, before halfway");
      boy.phase = drop_pha;      // go down
      return;
    case 2: // rise ack
      timStop(ngk_tim);
      // start velocity measure
      riseStartT = time(0);
      depthStart = antDepth();
    }
    // ngk timeout (note, this ignores other *_tim)
    if (timCheck()==ngk_tim) {
      sys.alarm[ngkTimeout_alm] += 1;
      if (depthStart-antDepth() < 3) {
        // not rising. log, reset, retry
        flogf("\n\t|p2() timeout on ngk, retry rise"); 
        riseStartT = 0;
      } else {
        // odd, we are rising; log but ignore
        flogf("\n\t|p2() timeout on ngk, but rising"); 
      }
    } // if ngk_tim
    depth = antDepth();
  } // while (depth>halfway)
  // algor: halfway. figure velocity, stop
  ngk.lastRise = (depthStart-depth) / (time(0)-riseStartT);
  ngkStop();
  // algor: current check. rise to surface, checking response
  if (boyOceanCurrentCheck()) {
    sys.alarm[midwayCurrent_alm] += 1;
    boy.phase = drop_pha;
    return;
  }
  ngkAscend();
  // set ngk.firstRise
} // phase2 //

/*
 * phase Three
 * Testing iridium/gps connection. 
 * If failed, release ngk cable another meter or two.
 * repeat to minimum CTD depth.
 */
void phase3(void) {
  // global: static char uploadfile[] = "c:00000000.dat
  // global ulong PwrOff PwrOn
  short result = 0;
  int gpsFails = 0;
  short count = 0;
  static short IridCallsNoParams = 0;
  char filenum[9] = "00000000";
  flogf("\n\t|phase THREE");
  boy.phaseStartT=time(0);

  if (WISPR_Status()) {
    WISPRSafeShutdown();
  }
  wisprInit(false);

  // should do this at boot
  if (boy.RESTART) { 
    ParseStartupParams(true); 
  } 

  // ?? why?
  Time(&PwrOff); 
  PwrOff -= PwrOn;
  // VEEPROM: SystemParameters MPC;
  sprintf(&uploadfile[2], "%08ld.dat", MPC.FILENUM);
  cprintf("\n\t|File Number: %08ld", MPC.FILENUM);
  // writefile 1) MPC 2) Ngk Info 3) Ngk Status
  // v
  WriteFile(PwrOff);
  // Init New LogFile, set PwrOn which is start of dataxint cycle
  Time(&PwrOn);


  while (result <= 0) { 
    // -1=false gps, -2=false irid, 1=success 2=fake cmds 3=real cmds
    // DBG( Incoming_Data();)
    result = IRIDGPS(); 

    if (result >= 1 || gpsFails > 4) {
      // IRIDIUM Successful success/fake/real/5th, next phase
      boy.phase = 4;
    }
    if (result == 1 || result == 2) { 
      // Upload Success / Commands
      IridCallsNoParams++; // call sessions w/o Params
      flogf("\n\t|Successful IRID Call: %d", IridCallsNoParams);
      if (IridCallsNoParams > 3) {
        // Load default parameters in "default.cfg" file
        ParseStartupParams(true); 
      } // calls>3
    } // Upload Success / Commands
    else if (result == 3) { 
      // Real Commands
      ParseStartupParams(false);
      IridCallsNoParams = 0;
      flogf("\n\t|Successful IRID Call: %d", IridCallsNoParams);
    } // Real Commands
    else if (result == -2) {
      // GPS Success, IRID Fail
      flogf("\n\t|phase3(): Failed Iridium Transfer");
      IridCallsNoParams++;
      if (IridCallsNoParams > 3) {
        IridCallsNoParams = 0;
        ParseStartupParams(true);
      } // calls>3
      boy.phase = 4;
      break;
    } // GPS Success, IRID Fail
    else if (result == -1) {
      gpsFails++;
      // Bad GPS- GPS fails usually from bad reception.
      flogf("\n\t|phase3(): Failed GPS attempt: %d", gpsFails);
      if (gpsFails >= 5) {
        flogf("\n\t|Exiting phase3()");
        // ?? close ports?
        break;
      } // >=5
      // removed section, go up further
    } // Bad GPS- GPS fails usually from bad reception
  } // while result<=0

  // in recovery, stay on surface 
  if (NIGK.RECOVERY) boy.phase = 1; 
  // NIGK.RECOVERY may be cleared by Params load 
  
  boy.RESTART = false;
  MPC.FILENUM++;
  sprintf(filenum, "%08ld", MPC.FILENUM);
  VEEStoreStr(FILENUM_NAME, filenum);
  create_dtx_file(MPC.FILENUM);
  CTD_CreateFile(MPC.FILENUM); 
  boy.TDEPTH = NIGK.TDEPTH;

  if (WISP.DUTYCYCL > 50) {
    wisprInit(true);
    WISPRPower(true);
  }

  boy.DATA = false;

} // phase3 //
/*
 * phase4
 */
void phase4(void) {

  float depthChange = 0.0;
  float velocity, descentvelocity;
  ulong timeChange = 0, DescentStart, DescentStop;
  ulong timecheck = 0;
  static float prevDepth = 0;
  int interval = 1;

  flogf("\n%s|phase_Four():", Time(NULL));
  boy.phaseStartT=time(0);
  amodemInit(true);

  boy.SURFACED = false;

  PrintSystemStatus();
  // sanity check
  CTD_AverageDepth(9, &velocity);
  if (boy.BUOYMODE != 0) {
    Ngk_Stop();
    WaitForWinch(0);
    flogf("\nErr phase4(): buoy was in motion");
  }
  //
  // turn off antenna, which selects buoy ctd
  DevSelect(DEVX);
  // redundant?
  CTD_Select(DEVB);

  // Now descend.
  if (boy.BUOYMODE != 2) {
    boy.TOPDEPTH = boy.depth;
    DescentStart = Ngk_Descend();
    WaitForWinch(2);
    CTD_Sample();
  } else
    CTD_Sample();

  /****
  ADD IN CHECK HERE. if WaitForWinch returns false because of timeout and lack
  of AModem Response: then check CTD Average depth and make sure Velocity>0.25
  (descending.)
  If not, then call Winc_Descend Again. After 10, wait for an hour
 */

  prevDepth = boy.depth;

  while (boy.BUOYMODE == 2) {
    // reading a sample triggers a new one, in p4
    Incoming_Data();

    // Receive Stop command from Ngk...
    if (boy.BUOYMODE == 0)
      DescentStop = (time(NULL) - (ulong)(NIGK.DELAY));

    timecheck = time(NULL);
    // Check depth change every 60 seconds... This is an out of the while loop.
    // Hopefully it will only come here when AModem stops boy
    // doesn't hear the Ngk serial coming.
    if (timecheck - DescentStart > interval * 180) { // ?? known prob w ctd read
      flogf("\n\t|phase4() Check depth change");
      prevDepth -= boy.depth;
      if (prevDepth < 0)
        prevDepth = prevDepth * -1.0;
      if (prevDepth < 3) {
        flogf("\nERROR|Depth change less than 3 meters... Ngk hasn't "
              "responded STOP");
        DescentStop = Ngk_Stop();
        WaitForWinch(0);
      }
      prevDepth = boy.depth;
      interval++;
    }
  } // while mode==2

  if (boy.BUOYMODE == 0)
    boy.dockDepth = boy.depth;

  // Descent Velocity;
  descentvelocity = CTD_CalculateVelocity();
  flogf("\n\t|Calculated Descent Velocity: %.2fm/s", descentvelocity);

  // Total Vertical depth change. total time change, calculate estimated
  // velocity.
  depthChange = boy.dockDepth - boy.TOPDEPTH;
  boy.DESCENTTIME = (short)(DescentStop - DescentStart);
  flogf("\n\t|Rate of Descent: %fMeters/Minute",
        (depthChange / ((float)boy.DESCENTTIME / 60.0)));
  flogf("\n\t|User calculated Cable Payout: %fMeters",
        ((float)boy.DESCENTTIME / 60.0) * NIGK.FRATE);
  flogf("\n\t|Time for Descent: %d", boy.DESCENTTIME);
  PrintSystemStatus();
  powDelay(2);
  amodemInit(false);

  boy.phase = 1;
  boy.DATA = false;
} // phase_Four

/*
 * phase0 deploy buoy sequence
 * on ship, sinking, at bottom wait boy.settleTime, gather info, phase2
 * set: boy.dockDepth
 */
void phase0(void) {
  DBG0("phase0()")
  // global ctd.depth, boy.runStart
  short checkDepth=30; 
  time_t deployMax=boy.runStart + (2*60*60);

  flogf("\n%s\t|deploy(): wait until >10m", Time(NULL))
  while (boy.depth<10.0) {
    ctdSample();
    while (!incoming());         // wait for input
    DBG2(" P0 %.1f ", ctd.depth)
    powDelay(checkDepth);
    // too long? give up, shut down - assume accidental start
    if (time(0) > (deployMax))
      shutdown("\nERR P0() exceeded max deploy time");
  }
  // checkDepth=30 secs; if no change for settleTime=120 secs, then deployed
  float prevDepth=0.0, bigChange=1.0;
  short settle=120, noChange=0;
  while (noChange < settle)
    powDelay(checkDepth);
    ctdSample();
    if (CTD_Data()) nowD=boy.depth;
    else  flogf("\nERR in Phase0() - no CTD data");
    if (abs(prevDepth-boy.depth) > big) { // changed
      flogf("\n%s\t|Phase0(): depth change %4.1f", Time(NULL), (nowD-thenD));
      changeless=0;
    } else changeless++;
    if (time(0) > (deployMax))
      shutdown("\nERR P0() exceeded max deploy time");
    // no big change for checkDepth secs
    prevDepth = ctd.depth;
    noChange += checkDepth;
  }
  boy.dockDepth = ctd.depth;
  // do nothing for 3 minutes
  powDelay(3*60);
  flogf("\n%s\t|P0: deployed", Time(NULL));
  // rise
  boy.phase=2;
} // phase0()

/*
 * int Incoming_Data()
 * ?? very fragile, caution
 * called by phase1,2,3,4
 */
int Incoming_Data(void) {
  bool incoming = true;
  static int count = 0;
  int value = 0; // Need to update this if we ever need to return a legit value.

  DBG0("Incoming_Data\t")
  switch (boy.phase) {
  // Case 0: Only at startup when MPC.STARTUPS>0
  case 0:
    while (incoming) {
      AD_Check();
      if (tgetq(NIGKPort)) {
        AModem_Data();
      } else if (tgetq(devicePort)) { // ?? very messy handling of ctd
        // DBG1("CTD Incoming")
        CTD_Data();
        if ((!boy.SURFACED && boy.phase > 1) || boy.BUOYMODE > 0 ||
            boy.phase == 0) // if not surfaced (target depth not reached.) and
                             // ngk is moving (not stopped)
          CTD_Sample();
      } else if (cgetq()) {
        // DBG1("Console Incoming")
        Console(cgetc());
      } else
        incoming = false;
    }
    break;

  // CASE 1: MOORED AUH
  case 1:

    while (incoming) {
      DBG1("Incoming\t")
      AD_Check();
      // Data coming from WISPR Board
      if (tgetq(PAMPort)) {
        // DBG1("WISPR Incoming")
        WISPR_Data();
      } else if (tgetq(devicePort)) {
        CTD_Data();
      }
      // Console Wake up.
      else if (cgetq()) {
        // DBG1("Console Incoming")
        Console(cgetc());
      }
      // No more incoming data
      else
        incoming = false;
    }
    break;

  // CASE 2&4: WINCH IN ACTION. Incorporate AMODEM in here
  case 2:
  case 4:

    while (incoming) {
      DBG1("Incoming\t")
      // ?? does adcheck need to run between each incoming? how often?
      AD_Check();
      if (tgetq(PAMPort)) {
        DBG1("WISPR Incoming")
        WISPR_Data();
      } else if (tgetq(NIGKPort)) {
        // ??? not hearing on bench test
        DBG1("NIGK Incoming")
        AModem_Data();
      } else if (tgetq(devicePort)) {
        DBG1("CTD Incoming")
        CTD_Data();
        if ((!boy.SURFACED && (boy.phase == 2 || boy.phase == 4)) ||
            boy.BUOYMODE > 0) // if not surfaced (target depth not reached.)
                               // and ngk is moving (not stopped)
          CTD_Sample();
      } else if (cgetq()) {
        DBG1("Console Incoming")
        Console(cgetc());
      }
      // No more incoming data
      else
        incoming = false;
    }
    break;

  // CASE 3: GPSIRID
  case 3:
    while (incoming) {
      DBG1("Incoming\t")
      AD_Check();
      if (tgetq(PAMPort)) {
        // DBG1("WISPR Incoming")
        WISPR_Data();
      } else if (cgetq()) {
        // DBG1("Console Incoming")
        Console(cgetc());
      } else
        incoming = false;
    }
    break;

  default:
    // Lost ngk phase. Get boy System Status decided which phase it best
    // belongs.

    cprintf("default switch");
    break;
  }

  PutInSleepMode = true;

  return value;

} // Incoming_Data
/*
 * void Console
 * Platform Specific Console Communication
 */
void Console(char in) {
  // are there side effects from any subroutines?
  // shutdown from here
  short c;

  DBG1("Incoming Char: %c", in)
  Delayms(2);
  switch (boy.phase) {
  case 1:
    switch (in) {
    case 'I':
    case 'i':
      WISPRPower(true);
      break;

    case 'E':
    case 'e':
      WISPRSafeShutdown();
      break;

    case 'D':
    case 'd':
      flogf("\n\t|REQUEST # DTX FROM WISPR?");
      c = cgetc() - 48;
      WISPRDet(c);
      break;

    case 'F':
    case 'f':
      WISPRDFP();
      break;

    case 'W':
    case 'w':
      flogf("\n\t|CHANGE TO WISPR #?");
      c = cgetc() - 48;
      ChangeWISPR(c);
      break;

    case 'P':
    case 'p':
      flogf("\n\t|CHANGE OF phase #?");
      c = cgetc() - 48;
      flogf(" phase: %d", c);
      boy.phase = c;
      break;

    case 'x':
      // boy.ON = false;
      // boy.DATA = boy.DATA ? false : true;
      shutdown();
      break;
    case '2':
      boy.DATA = boy.DATA ? false : true;
      break;
    }
    break;

  case 2:
  case 4:
    switch (in) {
    case 'W':
    case 'w':
      NgkConsole();
      break;
    case 'P':
    case 'p':
      flogf("\n\t|CHANGE OF phase #?");
      c = cgetc() - 48;
      boy.phase = c;
      break;
    case 'T':
    case 't':
      flogf("\n\t|Take CTD Sample");
      CTD_Sample();
      break;
    case 'x':
      // boy.ON = false;
      // boy.DATA = boy.DATA ? false : true;
      shutdown();
      break;
    case 'a':
    case 'A':
      PrintSystemStatus();
      break;
    case 'S':
    case 's':
      boy.SURFACED = true;
      break;
    }
    break;

  case 3:
    switch (in) {
    case 'x':
      // boy.ON = false; // exit from GPSIRID
      shutdown();
      break;
    case 'P':
    case 'p':
      flogf("\n\t|Change of phase #?");
      c = cgetc() - 48;
      boy.phase = c;
      break;
    }
    break;
  }

  // ?? PutInSleepMode = true;
  return;
}

/*
 * ExtFinishPulseRuptHandler		IRQ5 logging stop request interrupt
 */
IEV_C_FUNCT(ExtFinishPulseRuptHandler) {
#pragma unused(ievstack) // implied (IEVStack *ievstack:a0) parameter

  PinIO(IRQ5);

  PinRead(IRQ5);

} // ExtFinishPulseRuptHandler

/*
 * !! this routine is run for every PIT interrupt!
 ** plus other stuff in the loop that calls Sleep(), in phase1()
 * Usually sleeps until power interrupt PIT (20Hz), 
 * but can also break on serial ints irq4 (cons), irq5 (pam), or spurious
 * note, spurious could occur on other pins 
 */
void Sleep(void) {
  // these handlers just set the pin to I/O mode; wakeup, destroys data
  IEVInsertAsmFunct(IRQ4_ISR, level4InterruptAutovector); // Console Interrupt
  IEVInsertAsmFunct(IRQ4_ISR, spuriousInterrupt);
  IEVInsertAsmFunct(IRQ5_ISR, level5InterruptAutovector); // PAMPort Interrupt
  // IEVInsertAsmFunct(IRQ5_ISR, spuriousInterrupt);
  //
  SCITxWaitCompletion();    // clear serial buffers
  EIAForceOff(true);        // turn off rs232 transmitters
  CFEnable(false);          // disable CF card
  //
  PinBus(IRQ4RXD); // Console Interrupt
  while ((PinTestIsItBus(IRQ4RXD)) == 0)
    PinBus(IRQ4RXD);
  PinBus(IRQ5); // PAMPort Interrupt
  while ((PinTestIsItBus(IRQ5)) == 0)
    PinBus(IRQ5);
  //
  while (PinRead(IRQ5) && PinRead(IRQ4RXD) && !data)
    LPStopCSE(FastStop); // we will be here until interrupted
  //
  EIAForceOff(false); // turn on the RS232 driver
  CFEnable(true);     // turn on the CompactFlash card
  //
  PIORead(IRQ4RXD);
  while ((PinTestIsItBus(IRQ4RXD)) != 0)
    PIORead(IRQ4RXD);
  PIORead(IRQ5);
  while ((PinTestIsItBus(IRQ5)) != 0)
    PIORead(IRQ5);
  //
  PutInSleepMode = false;
  //
  // DBG2(".")
  Delayms(10);
} // Sleep


/*
 * SleepUntilWoken		Sleep until IRQ4 is interrupted
 * 
 * 1-st release 9/14/99
 * 2nd release 6/24/2002 by HM -Changed to use ADS8344/45
 */
void SleepUntilWoken(void) {

  ciflush(); // flush any junk
  flogf("\n%s|SleepUntilWoken()", Time(NULL));
  flogf("\nLow-power sleep mode until keyboard input is received...");

  // Install the interrupt handlers that will break us out by "break signal"
  // from RS232 COM input.
  IEVInsertAsmFunct(IRQ4_ISR, level4InterruptAutovector);
  IEVInsertAsmFunct(IRQ4_ISR, spuriousInterrupt);

  PITSet51msPeriod(PITOff); // disable timer (drops power)
  CTMRun(false);            // turn off CTM6 module
  SCITxWaitCompletion();    // let any pending UART data finish
  EIAForceOff(true);        // turn off the RS232 driver
  QSMStop();                // shut down the QSM
  CFEnable(false);          // turn off the CompactFlash card

  PinBus(IRQ4RXD); // make it an interrupt pin

  TickleSWSR();      // another reprieve
  TMGSetSpeed(1600); // Changed July 2015
  while (PinTestIsItBus(IRQ4RXD)) {
    //*HM050613 added to reduce current when Silicon System CF card is used
    //*(ushort *)0xffffe00c=0xF000; //force CF card into Card-1 active mode

    LPStopCSE(FullStop); // we will be here until interrupted
    TickleSWSR();        // by break
  }

  CSSetSysAccessSpeeds(nsFlashStd, nsRAMStd, nsCFStd, WTMODE);
  TMGSetSpeed(SYSCLK);

  // CONCLUDE
  PinIO(IRQ4RXD);

  EIAForceOff(false); // turn on the RS232 driver
  QSMRun();           // bring back the QSM
  CFEnable(true);     // turn on the CompactFlash card
  PIORead(IRQ4RXD);   // disable interrupt by IRQ4
  ciflush();          // discard any garbage characters
  flogf("\n%s|Aquisition ended!", Time(NULL));
  putflush(); // tell 'em we're back
              //			BIOSResetToPicoDOS();
              //}
} // SleepUntilWoken
/*
 * static void Irq3ISR(void)
 */
static void IRQ3_ISR(void) {
  PinIO(IRQ3RXX);
  RTE();
} // Irq3ISR //
/*
 * static void Irq2RxISR(void) CTD/Seaglider/ IRIDIUM
 */
static void IRQ2_ISR(void) {
  PinIO(IRQ2);
  RTE();
} // Irq2RxISR //
/*
 * static void IRQ4_ISR(void) CONSOLE
 */
static void IRQ4_ISR(void) {
  PinIO(IRQ4RXD);
  RTE();
} // Irq2ISR
/*
 * static void IRQ5_ISR(void) WISPR
 */
static void IRQ5_ISR(void) {
  PinIO(IRQ5);
  RTE();
} // Irq5ISR
/*
 * WriteFile      The Data File For Lara
1) Initially upload MPC parameters
2) Ngk Info
3) Ngk Status
 */
ulong WriteFile(ulong TotalSeconds) {
  // global uploadfile, WriteBuffer
  long BlkLength = BUFSZ;
  int filehandle;
  struct stat info;
  char detfname[] = "c:00000000.dtx";
  char logfile[sizeof "00000000.log"];
  int byteswritten = 0;
  int bytesWritten = 0;
  long maxupload;
  ulong LoggingTime;

  DBG1("\t|WriteFile(%s)", uploadfile)
  filehandle = open(uploadfile, O_WRONLY | O_CREAT | O_TRUNC);

  if (filehandle <= 0) {
    flogf("\nERROR  |WriteFile(): open error: errno: %d", errno);
    return -1;
  }
  DBG(else flogf("\n\t|WriteFile: %s Opened", uploadfile);)

  //*** boy Write ***//
  // ?? should this have geo loc?
  sprintf(WriteBuffer, "boy Program Ver:%.1f\naa:bb.cccc North ddd:ee.ffff "
                       "West\nFileNumber: %ld\nStarts:%d of "
                       "%d\nWriteTime:%s\nSeconds:%lu\nDetection "
                       "Interval:%3d\nData Inteval: %d\n\0",
          PROG_VERSION, MPC.FILENUM, MPC.STARTUPS, MPC.STARTMAX, Time(NULL),
          TotalSeconds, MPC.DETINT, MPC.DATAXINT);

  flogf("\n%s", WriteBuffer);
  bytesWritten = write(filehandle, WriteBuffer, strlen(WriteBuffer));
  DBG2("BytesWritten: %d", bytesWritten)

  // Only comes here if not rebooted.
  if (TotalSeconds != 0) {
    //*** Ngk Info   ***//
    // blk 
    Ngk_Monitor(filehandle);
    Delayms(50);
    memset(WriteBuffer, 0, BUFSZ);

    //*** Ngk Status ***//
    sprintf(WriteBuffer, "%s\n\0", PrintSystemStatus());
    bytesWritten = write(filehandle, WriteBuffer, strlen(WriteBuffer));
    DBG2("BytesWritten: %d", bytesWritten)
  }
  // Else, coming from reboot. Name the PowerLogging File.
  else {
    ADSFileName(MPC.FILENUM);
    Delayms(50);
  }

  //*** Power Monitoring Upload ***//
  Power_Monitor(TotalSeconds, filehandle, &LoggingTime);
  Delayms(50);
  Setup_ADS(true, MPC.FILENUM + 1, BITSHIFT);
  DOS_Com("del", MPC.FILENUM, "PWR", NULL);
  Delayms(500);

  //*** WISPR File Upload ***//
  WISPRWriteFile(filehandle);
  Delayms(50);

//*** CTD File Upload ***
#ifdef CTDSENSOR
  if (ctd.UPLOAD || TotalSeconds == 0) {
    sprintf(&detfname[2], "%08ld.ctd", MPC.FILENUM);
    Delayms(50);
    DBG1("\t|WriteFile:%ld ctd file: %s", MPC.FILENUM, detfname)
    stat(detfname, &info);
    if (info.st_size > (long)(IRID.MAXUPL - 1000))
      maxupload = IRID.MAXUPL - 1000;
    else
      maxupload = 0;
    Append_Files(filehandle, detfname, false, maxupload);
  }
  // Despite being upload, move CTD file to archive
  DOS_Com("move", MPC.FILENUM, "CTD", "CTD");
  Delayms(50);
#endif
  //*** MPC.LOGFILE upload ***// Note: occurring only after reboot.
  if (TotalSeconds == 0) { //||MPC.UPLOAD==1)
    sprintf(logfile, "%08ld.log", MPC.FILENUM);
    DBG1("\t|WriteFile: %ld log file: %s", MPC.FILENUM, logfile)
    stat(logfile, &info);
    if (info.st_size > (long)(IRID.MAXUPL - 2000))
      maxupload = IRID.MAXUPL - 2000;
    else
      maxupload = 0;
    Append_Files(filehandle, logfile, false, maxupload);
    Delayms(50);
  }
  Delayms(50);
  DOS_Com("move", MPC.FILENUM, "LOG", "LOG");

  // Close File
  close(filehandle);
  Delayms(25);

  // Return number of seconds of time on
  if (TotalSeconds == 0)
    return LoggingTime;
  else
    return 0;

} // WriteFile

/*
 * PrintSystemStatus()
 */
char *PrintSystemStatus(void) {
  // global stringout
  sprintf(stringout, "boy: "
                        "%d%d%d%d\ndockDepth:%5.2f\nCURRENTDEPTH:%5."
                        "2f\nTOPDEPTH:%5.2f\nTARGETDPETH:%d\nAVG.VEL:%5."
                        "2f\nCTDSAMPLES:%d\n\0",
          boy.DATA ? 1 : 0, boy.SURFACED ? 1 : 0, boy.phase, boy.BUOYMODE,
          boy.dockDepth, boy.depth, boy.TOPDEPTH, boy.TDEPTH, boy.AVGVEL,
          boy.CTDSAMPLES);
  flogf("\n%s", stringout);
  Delayms(100);

  return stringout;
}

/*
 * uses: ngk.boy2ant
 * sets: ant.on
 */
float boyOceanCurrent() {
  float aD, cD, a, b, c;
  // usually called while antMod is on
  if (!ant.on) antInit();
  aD=antDepth();
  mpcDevSwitch(ctd_dev);
  cD=ctdDepth();
  mpcDevSwitch(ant_dev);
  // a^2 + b^2 = c^2
  a=cD-aD;
  c=ngk.boy2ant;
  b=sqrt(pow(c,2)-pow(a,2));
  return b;
}

/*
 * uses: ant.depth boy.sidewaysMax
 */
bool boyOceanCurrentCheck() {
  flogf("\n\t| ocean current ");
  sideways = boyOceanCurrent();
  flogf(" @%.1f=%.1f ", ant.depth, sideways);
  if (sideways>boy.sidewaysMax) {
    flogf("too strong, cancel ascent");
    return true;
  }
  return false;
}

/*
 * shutdown buoy, sleep, reset
 */
void boyShutdown(void) {
  WISPRSafeShutdown();
  PIOClear(ANTENNAPWR); 
  PIOClear(AMODEMPWR); 
  SleepUntilWoken();
  BIOSReset();
}

/*
 *WISPR BOARD
 * * TPU 6    27 PAM1 WISPR TX
 * * TPU 7    28 PAM1 WISPR RX
 * * TPU 8    29 1= turns on MAX3222
 * * TPU 9    30 0= enables MAX3222
 *
 *Interrupts:
 * 
 * IRQ2 Wakes up from the sleep mode while waiting for ASC string from COM2
 * IRQ3
 * IRQ5 Interrupt to stop the program
 * 
 * ***************************************************************************
 * Before deploying, set CF2 time and SM2 time, format SD cards, 
 * replace SM2 and CF2 clock batteries
 * erase activity.log, set status=0, set startups=0,
 * verify boottime is correct, verify pwrondepth and pwroffdepth are correct,
 * make sure you have the correct mA multiplier set in the mAh calculation in
 * status 5 for the SM2 settings that you are using (compression, kHz, etc.)
 * as power consumption varies between those settings
 * 
 */
