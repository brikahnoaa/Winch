// boy.c
#include <main.h>

#define MINUTE 60

BoyInfo boy;
BoyData boyd;
WspData wspd;

///
// deploy or reboot, then loop over phases data/rise/irid/fall
// sets: phase phasePrev
void boyMain() {
  PhaseType phase, phaseNext, phasePrev;
  time_t phaseStart;
  //
  phase = boy.startPh;
  if (dbg.test) 
    flogf("\ndbg.test mode");
  else if (all.starts>1) 
    reboot();      // reset to known state
  flogf("\nboyMain(): starting with phase %d", phase);
  antStart();
  s16Start();
  //
  while (true) {
    utlX();
    // rise is first phase in a cycle
    time(&phaseStart);
    flogf("\ncycle %d @%s ", all.cycle, utlDateTime());
    switch (phase) {
    case deploy_pha:
      phaseNext = deployPhase();
      break;
    case rise_pha: // Ascend buoy, check for current and ice
      antLogOpen();
      s16LogOpen();
      phaseNext = risePhase();
      antLogClose();
      s16LogClose();
      break;
    case irid_pha: // Call home via Satellite
      phaseNext = iridPhase();
      all.cycle++;
      break;
    case fall_pha: // Descend buoy, science sampling
      antLogOpen();
      s16LogOpen();
      phaseNext = fallPhase();
      antLogClose();
      s16LogClose();
      break;
    case data_pha: // data collect by WISPR
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
// open log
void boyInit(void) {
  static char *self="boyInit";
  DBG();
  flogf("\n  System Starts %d", all.starts);
  ngkStart();
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
  if (boySafeChk(&boyd.oceanCurr, &boyd.iceTemp)) {
    sysAlarm(bottomCurr_alm);
    //?? return fall_pha;
  }
  // R,01,00
  time(&boyd.riseBgn);
  // algorithm for rise, default no brake
  if (boy.useBrake)
    result = riseDo(antSurfD());
  else
    result = riseDo(0);
  if (result) 
    utlErr(phase_err, "rise phase failure");
  boyd.surfD = antDepth(); // ??
  time(&boyd.riseEnd);
  return irid_pha;
} // risePhase

///
// ??
// on irid/gps (takes 30 sec).  // read gps date, loc. 
PhaseType iridPhase(void) {
  flogf("\niridPhase %s", utlDateTime());
  if (dbg.test && dbg.noIrid) return fall_pha;
  boyEngLog();
  if (boy.iridAuton) 
    antAuton(true);
  if (iridDo()) { // 0==success
    // ?? check for fail
  }
  if (boy.iridAuton) 
    antAuton(false);
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
  time(&boyd.fallBgn);
  result = fallDo(0);
  if (result) {
    flogf( "\nERR %s: error number %d", self, result);
    utlErr(phase_err, "fall phase failure");
  }
  time(&boyd.fallEnd);
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
  int success;
  flogf("\ndataPhase %s", utlDateTime());
  if (dbg.test && dbg.noData) return rise_pha;
  // save power
  antStop();
  // ngkStop();
  success = wspDetectD(&wspd, boy.callFreq, boy.callHour);
  switch (success) {
  case 1: flogf("\nDay watchdog"); break;
  case 11: flogf("\nhour.watchdog"); break;
  case 12: flogf("\nhour.startFail"); break;
  case 13: flogf("\nhour.minimum"); break;
  }
  // masters told us to stay down multiple cycles
  if (boy.stayDown>0) {
    flogf("\n\ndataPhase: stay down +%d cycles", boy.stayDown);
    boy.stayDown--;
    return data_pha;
  } else {
    antStart();
    // ngkStart();
    return rise_pha;
  }
  // ngkStart();
} // dataPhase

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
  antSample();
  antDataWait();
  if (!antRead())
    utlErr(ant_err, "sbe39 failure");
  //
  flogf(" sbe39@%3.1f", antDepth());
  flogf("\ndeployPhase()\t| ant@%3.1fm buoy@%3.1fm %s", 
      antDepth(), s16Depth(), utlDateTime());
  flogf("\n\t| wait up to %d minutes to reach bottom", boy.depWait);
  tmrStart( deploy_tmr, MINUTE*boy.depWait );
  // wait until under 10m
  while (true) {
    antSample();
    antDataWait();
    depth = antDepth();
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
    antSample();
    antDataWait();
    lastD = depth;
    depth = antDepth();
    flogf(" %3.1f", depth);
    if (depth-lastD<1.0) break;
    if (tmrExp(drop_tmr)) break;
  }
  flogf("\n\t| down, pause for %ds", boy.depSettle);
  utlNap(boy.depSettle);      // default 60sec
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
// ask antmod for our velocity
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
int iridDo(void) {
  int r=0;
  bool helloB=false, engB=false, s16B=false;
  tmrStart(phase_tmr, boy.iridOpM*60);
  gpsStart();
  flogf("\n%s ===\n", utlTime());
  antSwitch(gps_ant);
  gpsDateTime(&boyd.gpsBgn);
  gpsLatLng(&boyd.gpsBgn);
  antSwitch(irid_ant);
  while (!tmrExp(phase_tmr)) {
    // 0=success
    flogf("\n%s ====\n", utlTime());
    if ((r = iridSig())) {
      flogf("\nERR\t| iridSig()->%d", r);
      continue;
    } 
    if ((r = iridDial())) {
      flogf("\nERR\t| iridDial()->%d", r);
      continue;
    } 
    if ((r = iridProjHdr())) {
      flogf("\nERR\t| iridProjHdr()->%d", r);
      iridHup();
      continue;
    } 
    if (!helloB) {
      // sprintf(all.str, "hello.txt");
      // if ((r = iridSendFile(all.str))) {
      sprintf(all.str, "hello");
      if ((r = iridSendBlock(all.str, 5, 1, 1))) {
        flogf("\nERR\t| iridSendBlock(%s)->%d", all.str, r);
        iridHup();
        continue;
      } else {
        helloB=true;
        flogf("\n\tHelloHelloHello");
      }
    }
    utlNap(boy.filePause);
    if (!engB) {
      utlLogPathName(all.str, "eng", all.cycle);
      if ((r = iridSendFile(all.str))) {
        flogf("\nERR\t| iridSendFile(%s)->%d", all.str, r);
        iridHup();
        continue;
      } else engB=true;
    }
    utlNap(boy.filePause);
    if (!s16B) {  
      utlLogPathName(all.str, "s16", all.cycle);
      if ((r = iridSendFile(all.str))) {
        flogf("\nERR\t| iridSendFile(%s)->%d", all.str, r);
        iridHup();
        continue;
      } else s16B=true;
    }
    // done?
    if (helloB && engB && s16B) break;
  } // while
  flogf("\n%s =====\n", utlTime());
  antSwitch(gps_ant);
  gpsDateTime(&boyd.gpsEnd);
  gpsLatLng(&boyd.gpsEnd);
  // turn off a3la
  gpsStop();
  return r;
} // iridDo

///
// if targetD==0 then no brake, drift on surface
// uses: .dockD .fallOpM
int riseDo(float targetD) {
  static char *self="riseDo";
  MsgType recv=null_msg, send=null_msg, sent=null_msg, want=null_msg;
  bool targetB=false;
  enum {ngkTmr, fiveTmr};  // local timer names
  float nowD, startD, velo;
  int err=0, ngkTries, phaseEst, ngkDelay;
  DBG();
  // 
  s16DataWait(); 
  flogf("\n%s: sbe16@%3.1f sbe39@%3.1f", self, s16Depth(), antDepth());
  nowD = startD = antDepth();
  // winch
  ngkFlush();
  ngkTries = 0;
  ngkDelay = boy.ngkDelay*2;      // increments on every retry
  if (targetD)
    send = riseCmd_msg;           // rise then brake
  else
    send = surfCmd_msg;           // rise then drift, no brake
  want = riseRsp_msg;
  // phaseEst = sec/meter(3) * depth + fudge for possible current drift
  phaseEst = 3*boyd.dockD+boy.riseOpM*60;
  tmrStart(phase_tmr, phaseEst);
  tmrStart(fiveTmr, 5);
  while (!err) {       // loop exits by break;
    utlX();
    /// check target first
    if (targetD && !targetB && nowD<targetD) { // reached target
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
    if (tmrExp(fiveTmr)) { // 5 seconds
      tmrStart(fiveTmr, 5);
      flogf("\n\t: %s depth=%3.1f", utlTime(), nowD);
      if (!antVelo(&velo)) 
        flogf(" velo=%4.2f", velo);
    }  // 5 seconds
    if (s16Data()) 
      s16Read();
    if (antData()) 
      nowD = antDepth();
    if (tmrExp(phase_tmr)) {
      flogf("\n%s: ERR \t| phase timeout %ds @ %3.1f, ending phase", 
          self, phaseEst, nowD);
      err = 2;
      ngkSend(stopCmd_msg);
      break;
    }
  } // while !err
  // fail if error
  if (err) {
    flogf("\n%s: ERR \t| phase error %d at %3.1fm", self, err, antDepth());
    return err;
  } else { 
    // normal stop
    return 0;
  }
} // riseDo

///
// based on riseDo()
// uses: .dockD .fallOpM
int fallDo(float targetD) {
  static char *self="fallDo";
  MsgType recv=null_msg, send=null_msg, sent=null_msg, want=null_msg;
  bool targetB=false;
  enum {ngkTmr, fiveTmr};  // local timer names
  float nowD, startD, velo;
  int err=0, ngkTries, phaseEst, ngkDelay;
  DBG();
  // 
  s16DataWait(); 
  flogf("\n%s: sbe16@%3.1f sbe39@%3.1f", self, s16Depth(), antDepth());
  nowD = startD = antDepth();
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
  tmrStart(fiveTmr, 5);
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
    if (tmrExp(fiveTmr)) { // 5 seconds
      tmrStart(fiveTmr, 5);
      flogf("\n\t: %s depth=%3.1f", utlTime(), nowD);
      if (!antVelo(&velo)) 
        flogf(" velo=%4.2f", velo);
    }  // 5 seconds
    if (s16Data()) 
      s16Read();
    if (antData()) 
      nowD = antDepth();
    if (tmrExp(phase_tmr)) {
      flogf("\n%s: ERR \t| phase timeout %ds @ %3.1f, ending phase", 
          self, phaseEst, nowD);
      err = 2;
      break;
    }
  } // while !err
  // fail if error
  if (err) {
    flogf("\n%s: ERR \t| phase error %d at %3.1fm", self, err, antDepth());
    return err;
  } else { 
    // normal stop
    return 0;
  }
} // fallDo


///
// wait currChkSettle, buoy s16, ant td, compute
// uses: .boy2ant
int oceanCurr(float *curr) {
  static char *self="oceanCurr";
  float aD, cD, a, b, c;
  if (!s16Prompt()) utlErr(s16_err, "oceanCurr s16Prompt fail");
  s16Sample();
  s16DataWait();
  if (!s16Read()) {
    utlErr(s16_err, "sbe16 data failure");
    return 1;
  }
  cD=s16Depth();
  antSample();
  antDataWait();
  if (!antRead()) {
    utlErr(ant_err, "sbe39 data failure");
    return 1;
  }
  aD=antDepth();
  // pythagoras a^2 + b^2 = c^2
  // solve for b:=horizontal displacement, caused by current
  a=cD-aD;
  c=boy.boy2ant;
  flogf("\n%s\t: ant=%4.2f boy=%4.2f", self, aD, cD);
  if (a<0) {
    flogf("\n%s\t: ERR sbe16-sbe39<0", self);
    return 2;
  }
  if (c<a) {
    flogf("\n%s\t: boy2ant<cD-aD, updating boy.boy2ant", self);
    boy.boy2ant = a;
    c=boy.boy2ant;
    // ?? update boy2ant?
  }
  b=sqrt(pow(c,2)-pow(a,2));
  flogf(" sideways=%4.2f", b);
  *curr = b;
  return 0;
} // oceanCurr

///
// rets: 0 safe, +1 current!, +10 ice!, -1 err
// uses: boy.currMax
int boySafeChk(float *curr, float *temp) {
  static char *self="boySafeChk";
  float sideways;
  int r=0;
  DBG();
  if (dbg.test) return -10;
  // delay 10sec before measure to stabilize
  utlNap(10);
  if (oceanCurr(&sideways)) {
    utlErr(boy_err, ": oceanCurr failed");
    return -1;
  }
  flogf("\n%s: lateral @ %.1f = %.1f", self, antDepth(), sideways);
  *curr = sideways;
  if (sideways>boy.currMax) {
    flogf(" !! too strong, cancel ascent");
    // not today ??
    r += 1;
  }
  // ice check
  *temp=antTemp();
  return r;
} // boySafeChk

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
      antSurfD(), antDepth(), s16Depth());
  // sprintf(b+strlen(b), "=== physical stats ===\n");
  // hps=&boyd.physical;
  // hpsRead(hps);
  // sprintf(b+strlen(b), 
      // "current=%.2f, voltage=%.2f, pressure=%.2f, humidity=%.2f\n",
      // hps->curr, hps->volt, hps->pres, hps->humi);
  sprintf(b+strlen(b), "=== measures ===\n");
  sprintf(b+strlen(b), "temp=%.1f, oceanCurr=%.1f at dock=%.1fm\n", 
      boyd.iceTemp, boyd.oceanCurr, boyd.dockD);
  sprintf(b+strlen(b), "Wispr: disk %3.0f%% free on wispr#%d, %d detections\n",
      wspd.free, wspd.card, wspd.detects);
  sprintf(b+strlen(b), "Spectrogram:\n  %s\n", wspd.spectr);
  sprintf(b+strlen(b), "rise begin %s, ", utlDateTimeFmt(boyd.riseBgn));
  sprintf(b+strlen(b), "rise end %s\n", utlDateTimeFmt(boyd.riseEnd));
  sprintf(b+strlen(b), "fall begin %s, ", utlDateTimeFmt(boyd.fallBgn));
  sprintf(b+strlen(b), "fall end %s\n", utlDateTimeFmt(boyd.fallEnd));
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

