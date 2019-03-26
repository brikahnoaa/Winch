// boy.c
#include <utl.h>
#include <ant.h> 
#include <ctd.h>
#include <gps.h>
#include <hps.h>
#include <mpc.h>
#include <ngk.h>
#include <pwr.h>
#include <sys.h>
#include <tmr.h>
#include <wsp.h>
#include <boy.h> 

#define MINUTE 60

BoyInfo boy;
BoyData boyd;

///
// deploy or reboot, then loop over phases data/rise/irid/fall
// sets: phase phasePrev
void boyMain() {
  PhaseType phase, phaseNext, phasePrev;
  time_t phaseStart;
  flogf("\n  System Starts %d", all.starts);
  ngkStart();
  phase = boy.startPh;
  if (tst.test) 
    flogf("\ntst.test mode");
  else if (all.starts>1) 
    reboot();      // reset to known state
  flogf("\nboyMain(): starting with phase %d", phase);
    
  while (true) {
    utlX();
    // sysFlush();                    // flush all log file buffers
    time(&phaseStart);
    flogf("\ncycle %d @%s ", all.cycle, utlDateTime());
    switch (phase) {
    case deploy_pha:
      phaseNext = deployPhase();
      break;
    case rise_pha: // Ascend buoy, check for current and ice
      // new day
      nextCycle();
      phaseNext = risePhase();
      break;
    case irid_pha: // Call home via Satellite
      phaseNext = iridPhase();
      break;
    case fall_pha: // Descend buoy, science sampling
      phaseNext = fallPhase();
      break;
    case data_pha: // data collect by WISPR
      phaseNext = dataPhase();
      break;
    case error_pha:
      phaseNext = errorPhase();
      break;
    } // switch
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
  DBG()
  // boyd.log = utlLogFile(boy.logFile);
} // boyInit

///
// ascend. check angle due to current, up midway, re-check angle, surface.
// sets: boy.alarm[]
PhaseType risePhase(void) {
  int result;
  flogf("risePhase()");
  if (tst.test && tst.noRise) return irid_pha;
  antStart();
  ctdStart();
  // if current is too strong at bottom
  if (boySafeChk(&boyd.oceanCurr, &boyd.iceTemp)) {
    sysAlarm(bottomCurr_alm);
    //?? return fall_pha;
  }
  // R,01,00
  time(&boyd.riseBgn);
  result = riseDo(antSurfD()+1, 0);
  if (result>1) {
    flogf("\n\t| rise fails at %3.1f m", antDepth());
  } else if (result==1) {
    flogf("\n\t| rise skipped, already at %3.1f m", antDepth());
  }
  boyd.surfD = antDepth();
  time(&boyd.riseEnd);
  return irid_pha;
} // risePhase

///
// ??
// on irid/gps (takes 30 sec).  // read gps date, loc. 
PhaseType iridPhase(void) {
  flogf("\niridPhase %s", utlDateTime());
  if (tst.test && tst.noIrid) return fall_pha;
  antStart();
  ctdStart();
  // log file mgmt
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
  flogf("\nfallPhase %s", utlDateTime());
  if (tst.test && tst.noRise) return data_pha;
  antStart();
  ctdStart();
  time(&boyd.fallBgn);
  fallDo(boy.currChkD);
  boySafeChk(&boyd.oceanCurr, &boyd.iceTemp);
  fallDo(0);
  time(&boyd.fallEnd);
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
  if (tst.test && tst.noData) return rise_pha;
  antStop();
  ctdStop();
  // ngkStop();
  success = wspDetectD(&boyd.detections, &boyd.spectr, 
      boy.iridHour, boy.iridFreq);
  switch (success) {
  case 1: flogf("\nDay watchdog"); break;
  case 11: flogf("\nhour.watchdog"); break;
  case 12: flogf("\nhour.startFail"); break;
  case 13: flogf("\nhour.minimum"); break;
  }
  // masters told us to stay down a few days
  if (boy.stayDown>0) {
    boy.stayDown--;
    return data_pha;
  } else 
    return rise_pha;
  // ngkStart();
} // dataPhase

///
// if targetD==0 then no brake, drift on surface
int riseDo(float targetD, int try) {
  static char *self="riseDo";
  static char *rets="1:<targetD 2:retry";
  bool twentyB=false, targetB=false, errB=false;
  float nowD, startD, lastD, velo;
  int pt;        // estimated phase time
  MsgType msg;
  enum {ngkTmr, twentyTmr, fiveTmr};  // local timer names
  DBG0("\n%s(targetD=%3.1f, try=%d)", self, targetD, try)
  if (try > boy.riseRetry) return 2;
  ctdStart();
  antStart();
  flogf(" sbe16@%3.1f sbe39@%3.1f", ctdDepth(), antDepth());
  nowD = startD = antDepth();
  if (startD < targetD) return 1;
  // .riseOrig=as tested, .riseRate=seen, .rateAccu=fudgeFactor
  // pt = sec/meter(3) * depth + fudge for possible current drift
  pt = 3*startD+boy.riseOpM*60;
  tmrStart(phase_tmr, pt);
  tmrStart(twentyTmr, 20);
  tmrStart(fiveTmr, 5);
  ngkFlush();
  if (targetD)
    ngkSend(riseCmd_msg);
  else
    ngkSend(surfCmd_msg);
  tmrStart(ngkTmr, boy.ngkDelay*2);
  flogf("\n\t| %s sent to winch at %s", 
    targetD?"riseCmd":"surfCmd", utlTime());
  while (!errB) {       // loop exits by break;
    utlX();
    if (ctdData())
      ctdRead();
    // check: target, winch, 20s, 5s
    if (antData())
      nowD = antDepth();
    // arrived?
    if (targetD && nowD<targetD && !targetB) {
      flogf("\n\t| reached targetD %3.1f at %s", nowD, utlTime());
      targetB = true;
      ngkSend(stopCmd_msg);
      tmrStart(ngkTmr, boy.ngkDelay*2);
    }
    // pt timeout - longer than estimated time + fudge
    if (tmrExp(phase_tmr)) {
      flogf("\n ERR \t| rise timeout %ds @ %3.1f, stop", pt, nowD);
      errB = true;
      break;
    }
    // winch
    if (ngkRecv(&msg)!=null_msg) {
      flogf("\n%s:\t| %s from winch", self, ngkMsgName(msg));
      // surface detect by winch
      if (msg==stopCmd_msg) break;
      // rise rsp
      if (msg==riseRsp_msg)
        tmrStop(ngkTmr);
      // target stop
      if (msg==stopRsp_msg) {
        if (targetB)
          break;
          // we are good to go
        flogf("\n ERR \t| unexpected stopRsp");
      }
      // ?? are we really stopped?
    }
    if (tmrExp(ngkTmr)) {
      flogf("\n ERR \t| no response from winch %s", utlTime());
      // ?? missed it? 20s timeout will tell
      // ngkSend(riseCmd_msg);
    }
    // 20 seconds
    if (tmrExp(twentyTmr)) {
      flogf("\n\t| 20sec %s startD-nowD %3.1f ", 
        utlTime(), startD-nowD);
      if (startD-nowD < 2) {
        // by now we should have moved up 4 meters in 13.5s
        flogf("ERR < 2m");
        // errB = true;
        // break;
      } else {
        twentyB = true;
        lastD = nowD;
      }
    }
    // 5 seconds
    if (tmrExp(fiveTmr)) {
      tmrStart(fiveTmr, 5);
      flogf("\n\t| %s depth=%3.1f", utlTime(), nowD);
      if (!antVelo(&velo)) 
        flogf(" velo=%4.2f", velo);
    } 
  } // while !stop
  // retry if error
  if (errB) {
    flogf("\n%s: ERR\t| retry %d", self, ++try);
    ngkSend(stopCmd_msg);
    utlNap(boy.ngkDelay*2);
    ngkFlush();
    return riseDo(targetD, try);
  } else { 
    // normal stop
    return 0;
  }
} // rise


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
    iridHup();
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
      continue;
    } 
    if (!helloB) {
      sprintf(all.str, "hello.txt");
      if ((r = iridSendFile(all.str))) {
        flogf("\nERR\t| iridSendFile(%s)->%d", all.str, r);
        continue;
      } else helloB=true;
    }
    utlNap(boy.filePause);
    if (!engB) {
      utlLogPathName(all.str, "eng", all.cycle);
      if ((r = iridSendFile(all.str))) {
        flogf("\nERR\t| iridSendFile(%s)->%d", all.str, r);
        continue;
      } else engB=true;
    }
    utlNap(boy.filePause);
    if (!s16B) {
      utlLogPathName(all.str, "s16", all.cycle-1);
      if ((r = iridSendFile(all.str))) {
        flogf("\nERR\t| iridSendFile(%s)->%d", all.str, r);
        continue;
      } else s16B=true;
    }
    iridHup();
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
// based on riseDo(), diffs commented out; wait for winch stop
// fall to dock // expect stopCmd 
// uses: .riseRate .riseOrig .rateAccu .riseRetry
// sets: .riseRate
int fallDo(float targetD) {
  static char *self="fallDo";
  MsgType recv=null_msg, send=null_msg, sent=null_msg, want=null_msg;
  bool twentyB=false, targetB=false;
  enum {ngkTmr, fiveTmr};  // local timer names
  float nowD, startD, velo;
  int err=0, ngkTries, phaseEst, ngkDelay;
  DBG()
  // 
  ctdStart();
  antStart();
  ctdDataWait(); 
  flogf("\n%s: sbe16@%3.1f sbe39@%3.1f", self, ctdDepth(), antDepth());
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
      if (++ngkTries<10) {
        ngkDelay += 10*ngkTries; // timeout increments by 10, then 20 ...
        send = sent;
        flogf("\n\t| WARN winch timeout, try %d", ngkTries);
      } else {
        err = 1;
        flogf("\n\t| ERR winch timeout retry limit");
        break;
      }
    } // msg timeout
    if (tmrExp(fiveTmr)) { // 5 seconds
      tmrStart(fiveTmr, 5);
      flogf("\n\t: %s depth=%3.1f", utlTime(), nowD);
      if (!antVelo(&velo)) 
        flogf(" velo=%4.2f", velo);
    }  // 5 seconds
    if (ctdData()) 
      ctdRead();
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
    utlErr(fall_err, "phase failure");
    flogf("\n%s: ERR \t| phase error %d", err);
    return err;
  } else { 
    // normal stop
    return 0;
  }
} // fall


///
// from ship deck to ocean floor
// wait until under 10m, watch until not falling, wait 30s, risePhase
PhaseType deployPhase(void) {
  float depth, lastD;
  enum {deploy_tmr, drop_tmr};
  flogf("\ndeploy: testing sbe16, sbe39");
  if (tst.test && tst.noDeploy) return rise_pha;
  ctdStart();
  // test sbe16
  ctdSample();
  ctdDataWait();
  if (!ctdRead())
    utlErr(ctd_err, "sbe16 failure");
  flogf(" sbe16@%3.1f", ctdDepth());
  ctdStop();
  antStart();
  // test sbe39
  antSample();
  antDataWait();
  if (!antRead())
    utlErr(ant_err, "sbe39 failure");
  //
  flogf(" sbe39@%3.1f", antDepth());
  flogf("\ndeployPhase()\t| ant@%3.1fm buoy@%3.1fm %s", 
      antDepth(), ctdDepth(), utlDateTime());
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
  utlNap(boy.depSettle);      // default 120sec
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
// wait currChkSettle, buoy ctd, ant td, compute
// uses: .boy2ant
int oceanCurr(float *curr) {
  static char *self="oceanCurr";
  float aD, cD, a, b, c;
  if (!ctdPrompt()) utlErr(ctd_err, "oceanCurr ctdPrompt fail");
  ctdSample();
  ctdDataWait();
  if (!ctdRead()) {
    utlErr(ctd_err, "sbe16 data failure");
    return 1;
  }
  cD=ctdDepth();
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
  DBG()
  // delay 20sec before measure to stabilize
  utlNap(20);
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
  if (boyd.log) utlCloseFile(&boyd.log);
} // boyStop

///
void boyFlush(void) {} // ??

///
// do not use until 
bool boyDocked(float depth) {
  if (boyd.dockD==0.0) return false;
  else return (abs(depth-boyd.dockD)<1.0);
}

///
// uses: .cycle boyd.*Bgn .*End .oceanCurr .surfD
// sets: boyd.physical
// write some engineering data
int boyEngLog(void) {
  static char *self="boyEngLog";
  int log, r=0;
  GpsStats *gps;
  // HpsStats *hps;
  char *b;
  DBG()
  b=malloc(BUFSZ);
  b[0] = 0;
  sprintf(b+strlen(b), "== eng log cycle %d %s ==\n", 
      all.cycle, utlDateTime());
  sprintf(b+strlen(b), "program start %s, boot #%d\n",
      utlDateTimeFmt(all.startProg), all.starts);
  // sprintf(b+strlen(b), "=== physical stats ===\n");
  // hps=&boyd.physical;
  // hpsRead(hps);
  // sprintf(b+strlen(b), 
      // "current=%.2f, voltage=%.2f, pressure=%.2f, humidity=%.2f\n",
      // hps->curr, hps->volt, hps->pres, hps->humi);
  sprintf(b+strlen(b), "=== measures ===\n");
  sprintf(b+strlen(b), "temp=%.1f, oceanCurr=%.1f at dock=%.1fm\n", 
      boyd.iceTemp, boyd.oceanCurr, boyd.dockD);
  sprintf(b+strlen(b), "Spectrogram:\n%s\n", boyd.spectr);
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
  if (utlLogFile(&log, "eng")) 
    r=1;
  else {
    write(log, b, strlen(b));
    utlCloseFile(&log);
  }
  free(b);
  return r;
} // boyEngLog

///
// next cycle. manage log files, etc
int nextCycle(void) {
  static char *self="nextCycle";
  int r=0;
  DBG()
  all.cycle++;
  // close and restart syslog ??
  sprintf(all.buf, "copy sys.log log\\%03dsys.log", all.cycle);
  execstr(all.buf);
  // ?? close and reopen syslog
  return r;
} // nextCycle
