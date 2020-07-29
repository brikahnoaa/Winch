// boy.c
#include <main.h>

#define MINUTE 60

BoyData boyd;
BoyInfo boy = {
  { "deploy_pha",
  "rise_pha", "irid_pha", "fall_pha",
  "data_pha", "reboot_pha", "error_pha"
  }
}; // remainder of struct is 0 filled


///
// deploy or reboot, then loop over phases data/rise/irid/fall
// sets: phase phasePrev
void boyMain() {
  PhaseType phase, phaseNext, phasePrev;
  time_t phaseStart;
  //
  phase = boy.startPh;
  if (dbg.test) {
    phase = rise_pha;
    flogf("\ndbg.test mode");
  }
  else if (all.starts>1) 
    reboot();      // reset to known state
  flogf("\nboyMain(): starting with phase %s", boy.phaName[phase]);
  //
  while (true) {
    utlX();
    // rise is first phase in a cycle
    time(&phaseStart);
    flogf("\ncycle %d @%s ", all.cycle, utlDateTime());
    switch (phase) {
    case deploy_pha:
      s16Start();
      phaseNext = deployPhase();
      s16Stop();
      break;
    case rise_pha: // Ascend buoy, check for current and ice
      // ?? catch errs in device starts
      all.cycle++;
      antStart();
      s16Start();
      s39Start();
      ngkStart();
      phaseNext = risePhase();
      ngkStop();
      break;
    case irid_pha: // Call home via Satellite
      phaseNext = iridPhase();
      break;
    case fall_pha: // Descend buoy, science sampling
      ngkStart();
      phaseNext = fallPhase();
      ngkStop();
      break;
    case data_pha: // data collect by WISPR
      antStop();
      s16Stop();
      s39Stop();
      phaseNext = dataPhase();
      break;
    case error_pha:
      phaseNext = errorPhase();
      break;
    } // switch (phase)
    //
    phasePrev = phase;
    phase = phaseNext;
    // check these every phase
    // reset command from land - HW reset, app mode reboots
    // reboot() on restart
    if (boy.reset) {
      sysStop("reset command from land");
      BIOSReset();
    }
    // stop command from land
    if (boy.stop) {
      reboot();
      sysStop("stop command from land");
      BIOSResetToPicoDOS();
    }
  } // while true
} // boyMain() 

///
// initialize. malloc. module inits. 
// ?? device tests?
void boyInit(void) {
  static char *self="boyInit";
  DBG();
  flogf("\n  System Starts %d", all.starts);
  boyd.buff = malloc(BUFSZ);
} // boyInit

///
// ascend. check angle due to current, up midway, re-check angle, surface.
// sets: boy.alarm[]
PhaseType risePhase(void) {
  static char *self="risePhase";
  int result;
  flogf("\n%s %s", self, utlDateTime());
  if (dbg.test && dbg.noRise) return irid_pha;
  // if current is too strong at bottom
  // R,01,00
  time(&boyd.riseBgnT);
  // algorithm for rise, default no brake
  // if (boy.useBrake)
    // result = riseUp(antSurfD());
  // else
    result = riseUp(0);
  if (result) 
    utlErr(boy_err, "rise phase failure");
  boyd.surfD = s39Depth(); // ??
  time(&boyd.riseEndT);
  return irid_pha;
} // risePhase

///
// ??
// on irid/gps (takes 30 sec).  // read gps date, loc. 
PhaseType iridPhase(void) {
  flogf("\niridPhase %s", utlDateTime());
  if (dbg.test && dbg.noIrid) return fall_pha;
  boyEngLog();
  if (iridCall()) { // 0==success
    // ?? check for fail
  }
  return fall_pha;
} // iridPhase

///
PhaseType fallPhase() {
  static char *self="fallPhase";
  int result;
  flogf("\n%s %s", self, utlDateTime());
  if (dbg.test && dbg.noRise) return data_pha;
  antLogOpen();
  s16LogOpen();
  time(&boyd.fallBgnT);
  result = fallDn(0);
  if (result) {
    flogf( "\nERR %s: error number %d", self, result);
    utlErr(boy_err, "fall phase failure");
  }
  time(&boyd.fallEndT);
  antLogClose();
  s16LogClose();
  return data_pha;
} // fallPhase

///
// wispr recording and detecting, buoy is docked to ngk
// data is gathered for about 24hours (data_tmr)
// wsp powers down for % of each hour (wispr_tmr)
// organize data files, transfer data to antmod ??
// sleep needs a lot of optimizing to be worth the trouble
// uses: data_tmr duty_tmr
PhaseType dataPhase(void) {
  static char *self="dataPhase";
  int err, detected;
  float free;
  time_t now, togo;
  flogf("\ndataPhase %s", utlDateTime());
  // masters told us to stay down to avoid storm
  if (boy.stayDown>0) {
    boyd.cycleEndT += boy.stayDown*24*60*60;
    flogf("\n%s: stay down +%d days", self, boy.stayDown);
    flogf("\n%s: extending this cycle to %ld", self, boyd.cycleEndT);
    boy.stayDown=0;
  } // staydown
  while (true) {
    time(&now); togo = boyd.cycleEndT - now;
    if (togo < boy.dataRunM*60) break;
    // run
    err = wspDetectM( &detected, &free, boy.dataRunM );
    flogf( "\n%s: wspDetectM( detected %d, free %f, minutes %d ) -> %d",
        detected, free, boy.dataRunM, err);
    time(&now); togo = boyd.cycleEndT - now;
    if (err) break;
    if (togo < boy.dataRestM*60) break;
    // rest 
    pwrNap(boy.dataRestM*60);
  } // while
  if (togo>0)
    pwrNap(togo);
  cycleSetup();
  return rise_pha;
} // dataPhase

///
// set length of cycle
int cycleSetup(void) {
  time_t now;
  time(&now);
  if (boy.cycleMint) {
    boyd.cycleBgnT = now;
    boyd.cycleEndT = now + boy.cycleMint*60;
  } else {
    utlStop( "cycleSetup: need cycleMint" );
  }
  return 0;
} // cycleSetup

///
// from ship deck to ocean floor
// wait until under 10m, watch until not falling, wait 30s, risePhase
PhaseType deployPhase(void) {
  float depth, lastD;
  enum {deploy_tmr, drop_tmr};
  flogf("\ndeploy: testing sbe16, sbe39");
  if (dbg.test && dbg.noDeploy) return rise_pha;
  s16Start();
  // test sbe16
  s16Sample();
  s16DataWait();
  if (!s16Read())
    utlErr(s16_err, "sbe16 failure");
  flogf(" sbe16@%3.1f", s16Depth());
  antStart();
  // test sbe39
  s39Sample();
  s39DataWait();
  if (!s39Read())
    utlErr(s39_err, "sbe39 failure");
  //
  flogf(" sbe39@%3.1f", s39Depth());
  flogf("\ndeployPhase()\t| ant@%3.1fm buoy@%3.1fm %s", 
      s39Depth(), s16Depth(), utlDateTime());
  flogf("\n\t| wait up to %d minutes to reach bottom", boy.depWaitM);
  tmrStart( deploy_tmr, MINUTE*boy.depWaitM );
  // wait until under 10m
  while (true) {
    s39Sample();
    s39DataWait();
    depth = s39Depth();
    flogf("\ndeployPhase@%4.2fm %s", depth, utlTime());
    if (depth>10) break;
    if (tmrExp(deploy_tmr)) 
      utlStop("deployP() shipside timeout");
    utlNap(30);
  }
  flogf("\n\t| %4.2fm>10m, watch for depth to settle down\n", depth);
  // at most 5min to descend, already waited 2min
  tmrStart(drop_tmr, MINUTE*5);
  while (true) {
    // must fall at least 1m in 15 sec
    utlNap(15);
    s39Sample();
    s39DataWait();
    lastD = depth;
    depth = s39Depth();
    flogf(" %3.1f", depth);
    if (depth-lastD<1.0) break;
    if (tmrExp(drop_tmr)) break;
  }
  flogf("\n\t| down, pause for %ds", boy.depSetlM);
  utlNap(boy.depSetlM);      // default 60sec
  // we are down
  boyd.dockD = depth;
  flogf("\n\t| boyd.dockD = %4.2f", boyd.dockD);
  flogf("\n\t| go to surface, call home");
  return rise_pha;
} // deployPhase

/// 
// ??
// figure out whats happening, continue as possible
// load info from saved previous phase
int reboot(void) {
  MsgType msg;
  flogf("\n === rebootPhase()\t| stop stop fall fall %s", utlDateTime());
  ngkSend(stopCmd_msg);
  ngkRecvWait(&msg, 30);
  ngkSend(stopCmd_msg);
  ngkRecvWait(&msg, 30);
  ngkSend(fallCmd_msg);
  ngkRecvWait(&msg, 30);
  ngkSend(fallCmd_msg);
  ngkRecvWait(&msg, 30);
  return 0;
} // reboot()

///
// cable is stuck. up/down tries??, down to dock. 
// go back to normal if resolved ??
PhaseType errorPhase(void) {
  flogf("errorPhase()");
  reboot();
  return deploy_pha;
} // errorPhase

///
// main action of iridPhase, allows better error handling
int iridCall(void) {
  static char *self="iridCall";
  int try=0, r=0;
  bool dialB=false, engB=false, s16B=false;
  tmrStart(phase_tmr, boy.iridOpM*60);
  iriStart();
  flogf("\n%s ===\n", utlTime());
  antSwitch(gps_ant);
  while (true) { // try datetime 3x
    // ?? need to turn off irid on errs 
    if (try++ > 2) raise(1);
    if (!iriDateTime(&boyd.gpsBgn)) break;
  }
  iriLatLng(&boyd.gpsBgn);
  antSwitch(irid_ant);
  while (!tmrExp(phase_tmr)) {
    flogf("\n%s Call Home ====\n", utlTime());
    if (dialB) {
      iriHup();
      dialB = false;
    }
    if ((r = iriSig())) {
      flogf("\nERR\t| iriSig()->%d", r);
      continue;
    } 
    if ((r = iriDial())) {
      flogf("\nERR\t| iriDial()->%d", r);
      continue;
    } else dialB = true;
    if ((r = iriProjHello(all.buf))) {
      flogf("\nERR\t| iriProjHello()->%d", r);
      continue;
    } 
    if (!engB) {
      utlLogPathName(all.str, "eng", all.cycle);
      if ((r = iriSendFile(all.str)))
        flogf("\nERR\t| iriSendFile(%s)->%d", all.str, r);
      engB=true;
    } // engB
    if (!s16B) {  
      utlLogPathName(all.str, "s16", all.cycle);
      if ((r = iriSendFile(all.str))) 
        flogf("\nERR\t| iriSendFile(%s)->%d", all.str, r);
      s16B=true;
    } // s16B
    if (engB && s16B) break;
  } // phase_tmr
  iriHup();
  // utlWrite(irid.port, "done", "");
  flogf("\n%s =====\n", utlTime());
  antSwitch(gps_ant);
  // ?? should be entered in eng log and sys log instead?
  iriDateTime(&boyd.gpsEnd);
  iriLatLng(&boyd.gpsEnd);
  // turn off a3la
  iriStop();
  return r;
} // iridCall

///
// if targetD==0 then no brake, drift on surface
// uses: .dockD .fallOpM .ngkDelay .riseVpred
int riseUp(float targetD) {
  static char *self="riseUp";
  static char *rets="1=errLimit 2=phaseTimeEst";
  enum PhaseState {begin, starting, winching, stopping, end} state;
  float lastD, nowD, startD;
  int errCnt=0, errLimit=9, ngkTries=0, ngkLimit=5, shortTimer=10;
  int phaseTimeEst;
  MsgType recv, sent;
  DBG();
  recv=sent=null_msg; 
  nowD=lastD=startD=s39Depth();
  // phaseTimeEst = sec/meter * depth * fudge ?? for possible current drift
  phaseTimeEst = (1.0/boy.riseVpred)*boyd.dockD * 2.0;
  flogf("\n%s: rise phase estimate %d seconds", self, phaseEst);
  tmrStart(phase_tmr, phaseEst);
  //
  state=begin;
  flogf("\n%s: begin at sbe16@%3.1f sbe39@%3.1f", self, s16Depth(), nowD);
  if (targetD) 
    send=riseCmd_msg;
  else
    send=surfCmd_msg;
  ngkFlush();
  ngkSend(send);
  tmrStart(ngk_tmr, boy.ngkDelay);
  tmrStart(second_tmr, shortTimer);
  state=starting;
  //
  while (state!=end)
  { // loop from begin state to end state, unless errs are bad
    utlX();
    if (errCnt>errLimit) raisex(1);
    if (tmrExp(phase_tmr)) raisex(2);
    /// collect then review
    nowD = s39Depth();
    ngkRecv(&recv);
    if (s16Data()) 
    { // science! get, print data
      ptr = s16Read();
      flogf("\n{%s}", ptr);
    } // science
    /// whats up
    if (recv!=null_msg) 
      tmrStop(ngk_tmr);
    if (recv==riseRsp_msg) { 
      switch (state) {
      case starting:
        

    if (targetD && nowD<targetD) 
    { // reached target
      switch (state) {
      case starting:
      case winching:
        // stop
        send = stopCmd_msg;
        ngkSend(send);
        state = stopping;
        break;
      case stopping:
        // we know, waiting for response
        break;
      case default:
        // err
        // ??
      } // switch
    } // target



      send = stopCmd_msg;
      want = stopRsp_msg;
      ngkTries = 0;
      ngkDelay = boy.ngkDelay*2;
      targetB = true;
    } // reached target
    /// winch
    if (send!=null_msg) 
    { // send msg
      flogf("\n\t| %s send to winch at %s", ngkMsgName(send), utlTime());
      ngkSend(send);
      if (send==riseCmd_msg || send==surfCmd_msg) 
        want = riseRsp_msg;
      if (send==stopCmd_msg)
        want = stopRsp_msg;
      sent = send;
      send = null_msg;
      tmrStart(ngkTmr, ngkDelay);
    } // send msg
    if (ngkRecv(&recv)!=null_msg) 
    { // msg read
      tmrStop(ngkTmr);
      flogf("\n\t| %s from winch", ngkMsgName(recv));
      // reached dock, or jammed // ?? check depth for err?
      if (recv==stopCmd_msg) break;
      if (want!=null_msg) 
      { // want
        if (recv==want) 
        { // satisfied
          if (recv==stopRsp_msg) break; // all done here
          want = null_msg;
        } else { // retry
          flogf(", but we want %s", ngkMsgName(want));
          send = sent;
        } // satisfied
      } // want
    } // msg read
    if (tmrExp(ngkTmr)) 
    { // msg timeout
      ngkDelay += 20*(++ngkTries); // timeout increments by 20, then 40 ...
      send = sent;
      flogf("\n\t| WARN winch timeout, try %d", ngkTries);
    } // msg timeout
    if (tmrExp(tenTmr)) 
    { // 5 seconds - are we moving ??
      tmrStart(tenTmr, 10);
      flogf("\n\t: %s depth=%3.1f", utlTime(), nowD);
    }  // 5 seconds
    if (s16Data()) 
    { // science data 
      flogf("\ns16: %s", all.str);
      s16Read();
    } 
    if (tmrExp(phase_tmr)) 
    { // phase timeout
      flogf("\n%s: ERR \t| phase timeout %ds @ %3.1f, ending phase", 
          self, phaseEst, nowD);
      err = 2;
      ngkSend(stopCmd_msg);
      break;
    } // phase timeout
  } // while !err
  ///
  // fail if error
  if (err) {
    flogf("\n%s: ERR \t| phase error %d at %3.1fm", self, err, s39Depth());
    return err;
  } 
  // normal stop
  return 0;
} // riseUp

///
// ?? only does cycleMint
//

///
// calculate rise time (doc/algor) ??
// sets: *riseT 
void riseTime(time_t *riseT) {
  int hour;
  time_t r, now;
  struct tm *tmPtr, tmLocal;
  static char *self="riseTime";
  DBG();
  // get time, break it down
  time(&now);
  tmPtr = gmtime(&now);
  memcpy(&tmLocal, tmPtr, sizeof(struct tm));
  // figure target hour
  hour=boy.cycleHour;
  // check the hour, is it past? add one interval
  tmLocal.tm_hour = hour;
  tmLocal.tm_min = 0;
  tmLocal.tm_sec = 0;
  r = mktime(&tmLocal);
  if (r<now) {
    // next day/interval
    DBG1("r<now");
    // be careful with long math and big ints, do not (time_t) (24*60*60)
      r = r + (time_t) 24*60*60;
  }
  flogf("\nriseTime(): rise at %s", utlDateTimeFmt(r));
  *riseT = r;
  return;
} // riseTime

///
// based on riseUp()
// uses: .dockD .fallOpM
int fallDn(float targetD) {
  static char *self="fallDn";
  MsgType recv=null_msg, send=null_msg, sent=null_msg, want=null_msg;
  bool targetB=false;
  enum {ngkTmr, tenTmr};  // local timer names
  float nowD, startD;
  // float velo; ??
  int err=0, ngkTries, phaseEst, ngkDelay;
  DBG();
  // 
  s16DataWait(); 
  flogf("\n%s: sbe16@%3.1f sbe39@%3.1f", self, s16Depth(), s39Depth());
  nowD = startD = s39Depth();
  // winch
  ngkFlush();
  ngkTries = 0;
  ngkDelay = boy.ngkDelay*2;      // increments on every retry
  send = fallCmd_msg;
  want = fallRsp_msg;
  // could be cable far out, maybe dockD+100m
  // phaseEst = sec/meter(5) * depth + fudge for possible current drift
  phaseEst = 5*boyd.dockD+boy.fallOpM*60;
  tmrStart(phase_tmr, phaseEst);
  tmrStart(tenTmr, 10);
  while (!err) {       // loop exits by break;
    utlX();
    /// check target first
    if (targetD && !targetB && nowD>targetD) { // reached target
      send = stopCmd_msg;
      want = stopRsp_msg;
      ngkTries = 0;
      ngkDelay = boy.ngkDelay*2;
      targetB = true;
    } // reached target
    /// winch
    if (send!=null_msg) { // send msg
      flogf("\n\t| %s to winch at %s", ngkMsgName(send), utlTime());
      ngkSend(send);
      sent = send;
      send = null_msg;
      tmrStart(ngkTmr, ngkDelay);
    } // send msg
    if (ngkRecv(&recv)!=null_msg) { // msg read
      tmrStop(ngkTmr);
      flogf("\n\t| %s from winch", ngkMsgName(recv));
      // reached dock, or jammed // ?? check depth for err?
      if (recv==stopCmd_msg) break;
      if (want!=null_msg) { // want
        if (recv==want) { // satisfied
          if (recv==stopRsp_msg) break; // all done here
          want = null_msg;
        } else { // retry
          flogf(", but we want %s", ngkMsgName(want));
          send = sent;
        }
      } // want
    } // msg read
    if (tmrExp(ngkTmr)) { // msg timeout
      ngkDelay += 20*(++ngkTries); // timeout increments by 20, then 40 ...
      send = sent;
      flogf("\n\t| WARN winch timeout, try %d", ngkTries);
    } // msg timeout
    if (tmrExp(tenTmr)) { // 10 seconds
      tmrStart(tenTmr, 10);
      flogf("\n\t: %s depth=%3.1f", utlTime(), nowD);
    }  // 5 seconds
    if (s16Data()) 
      s16Read();
    if (s39Data()) 
      nowD = s39Depth();
    if (tmrExp(phase_tmr)) {
      flogf("\n%s: ERR \t| phase timeout %ds @ %3.1f, ending phase", 
          self, phaseEst, nowD);
      err = 2;
      break;
    }
  } // while !err
  // fail if error
  if (err) {
    flogf("\n%s: ERR \t| phase error %d at %3.1fm", self, err, s39Depth());
    return err;
  } else { 
    // normal stop
    return 0;
  }
} // fallDn


///
// drop winch, close log
void boyStop(void) {
  if (boyd.log) utlLogClose(&boyd.log);
} // boyStop

///
void boyFlush(void) {} // ??

///
// do not use until 
bool docked(float depth) {
  if (boyd.dockD==0.0) return false;
  else return (abs(depth-boyd.dockD)<1.0);
}

///
// uses: all.cycle boyd.*Bgn .*End .oceanCurr .surfD
// sets: boyd.physical
// write some engineering data
int boyEngLog(void) {
  static char *self="boyEngLog";
  int log, r=0;
  GpsStats *gps;
  // HpsStats *hps;
  char *b;
  DBG();
  b=malloc(BUFSZ);
  b[0] = 0;
  sprintf(b+strlen(b), "== eng log cycle %d %s ==\n", 
      all.cycle, utlDateTime());
  sprintf(b+strlen(b), "program start %s, boot #%d\n",
      utlDateTimeFmt(all.startProg), all.starts);
  sprintf(b+strlen(b), "antSurfD:%3.1f, s39:%3.1f, s16:%3.1f\n",
      antSurfD(), s39Depth(), s16Depth());
  // sprintf(b+strlen(b), "=== physical stats ===\n");
  // hps=&boyd.physical;
  // hpsRead(hps);
  // sprintf(b+strlen(b), 
      // "current=%.2f, voltage=%.2f, pressure=%.2f, humidity=%.2f\n",
      // hps->curr, hps->volt, hps->pres, hps->humi);
  sprintf(b+strlen(b), "=== measures ===\n");
  // sprintf(b+strlen(b), "temp=%.1f, oceanCurr=%.1f at dock=%.1fm\n", 
      // boyd.iceTemp, boyd.oceanCurr, boyd.dockD);
  // sprintf(b+strlen(b), "Wispr: disk %3.0f%% free on wispr#%d, %d detections\n",
      // wspd.free, wspd.card, wspd.detects);
  // sprintf(b+strlen(b), "Spectrogram:\n  %s\n", wspd.spectr);
  sprintf(b+strlen(b), "rise begin %s, ", utlDateTimeFmt(boyd.riseBgnT));
  sprintf(b+strlen(b), "rise end %s\n", utlDateTimeFmt(boyd.riseEndT));
  sprintf(b+strlen(b), "fall begin %s, ", utlDateTimeFmt(boyd.fallBgnT));
  sprintf(b+strlen(b), "fall end %s\n", utlDateTimeFmt(boyd.fallEndT));
  sprintf(b+strlen(b), "=== during last irid transmission ===\n");
  gps=&boyd.gpsBgn;
  sprintf(b+strlen(b), "gps start: lat=%s, long=%s, time=%s\n", 
      gps->lat, gps->lng, gps->time);
  gps=&boyd.gpsEnd;
  sprintf(b+strlen(b), "gps drift: lat=%s, long=%s, time=%s\n", 
      gps->lat, gps->lng, gps->time);
  sprintf(b+strlen(b), "ending surface depth=%.1f\n", boyd.surfD);
  // land cmds
  //
  flogf("\n%s", b);
  if (utlLogOpen(&log, "eng")) 
    r=1;
  else {
    write(log, b, strlen(b));
    utlLogClose(&log);
  }
  free(b);
  return r;
} // boyEngLog

