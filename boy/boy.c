// boy.c

#include <utl.h>
#include <boy.h> 

#include <ant.h> 
#include <ctd.h>
#include <gps.h>
#include <mpc.h>
#include <ngk.h>
#include <pwr.h>
#include <sys.h>
#include <tmr.h>
#include <wsp.h>

#define MINUTE 60

extern GlobalInfo global;
BoyInfo boy;

///
// deploy or reboot, then loop over phases data/rise/irid/fall
// sets: boy.phase .phasePrev
void boyMain() {
  PhaseType phaseNext;
  int starts;
  starts = sysInit();
  mpcInit();
  pwrInit();
  antInit();
  boyInit();
  ctdInit();
  gpsInit();
  ngkInit();
  wspInit();
  flogf("\ system starts %d", starts);
  antStart();
  ctdStart();
  ngkStart();
  boy.phase = boy.startPh;
  if (boy.testing) 
    flogf("\nboy.testing");
  else if (starts>1) 
    boy.phase = reboot_pha;
  flogf("\nboyMain(): starting with phase %d", boy.phase);
    
  while (true) {
    utlX();
    // sysFlush();                    // flush all log file buffers
    boy.phaseT = time(0);
    flogf("\ncycle %d @%s ", boy.cycle, utlDateTime());
    switch (boy.phase) {
    case deploy_pha:
      phaseNext = deployPhase();
      break;
    case rise_pha: // Ascend buoy, check for current and ice
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
      boy.cycle++;
      // masters told us to stay down a few days
      if (boy.stayDown>0) {
        boy.stayDown--;
        utlNap(MINUTE*15);
        phaseNext=data_pha;
      }
      break;
    case reboot_pha:
      phaseNext = rebootPhase();
      break;
    case error_pha:
      phaseNext = errorPhase();
      break;
    } // switch
    boy.phasePrev = boy.phase;
    boy.phase = phaseNext;
    // check these every phase
    if (boy.cycleMax && (boy.cycle > boy.cycleMax)) 
      sysStop("cycleMax reached");
    // new day
    // sprintf(utlBuf, "copy sys.log log\\sys%03d.log", boy.cycle);
    // execstr(utlBuf);
  } // while true
} // boyMain() 

///
// open log
void boyInit(void) {
  DBG0("boyInit()")
  boy.log = utlLogFile(boy.logFile);
} // boyInit

/// 
// ??
// figure out whats happening, continue as possible
// load info from saved previous phase
// ask antmod for our velocity
// sets: boy.phase
PhaseType rebootPhase(void) {
  MsgType msg;
  flogf("rebootPhase()\t| stop stop fall continue");
  ngkSend(stopCmd_msg);
  ngkRecvWait(&msg, 30);
  ngkSend(stopCmd_msg);
  ngkRecvWait(&msg, 30);
  return deploy_pha;
} // reboot()

///
// ascend. check angle due to current, up midway, re-check angle, surface.
// sets: boy.alarm[]
PhaseType risePhase(void) {
  int result;
  flogf("risePhase()");
  if (boy.noRise) return irid_pha;
  // *Start() returns immed if already on (*.on = true)
  antStart();
  //ctdStart();
  // if current is too strong at bottom
  if (oceanCurrChk()) {
    sysAlarm(bottomCurr_alm);
    //?? return fall_pha;
  }
  // R,01,00
  result = rise(antSurfD()+1, 0);
  if (result) {
    flogf("\n\t| rise fails at %3.1f m", antDepth());
    //??  return fall_pha;
  }
  return irid_pha;
} // risePhase

///
// ??
// on irid/gps (takes 30 sec).  // read gps date, loc. 
PhaseType iridPhase(void) {
  int r;
  flogf("iridPhase()");
  if (boy.noIrid) return fall_pha;
  antStart();
  if (boy.iridAuton) 
    antAuton(true);
  tmrStart(phase_tmr, boy.iridOp*MINUTE);
  gpsStart();
  flogf("\n%s ===\n", utlTime());
  antSwitch(gps_ant);
  gpsStats();
  antSwitch(irid_ant);
  while (!tmrExp(phase_tmr)) {
    flogf("\n%s ====\n", utlTime());
    // 0=success
    if ((r = iridSig())) {
      flogf("\nERR\t| iridSig(%d)", r);
      continue;
    }
    if (iridDial()) continue;
    // iridSendFile(boy.sendFile);
    iridHup();
  } // while
  flogf("\n%s =====\n", utlTime());
  antSwitch(gps_ant);
  gpsStats();
  antAuton(false);
  return fall_pha;
} // iridPhase

///
PhaseType fallPhase() {
  flogf("\nfallPhase()");
  if (boy.noRise) return data_pha;
  fall(boy.currChkD, 0);
  oceanCurrChk();
  fall(0, 0);
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
  int success, detect;
  flogf("dataPhase()");
  if (boy.noData) return rise_pha;
  //ctdStop();
  antStop();
  // ngkStop();
  success = wspDetectDay(&detect);
  switch (success) {
  case 1: flogf("\nDay watchdog"); break;
  case 11: flogf("\nhour.watchdog"); break;
  case 12: flogf("\nhour.startFail"); break;
  case 13: flogf("\nhour.minimum"); break;
  }
  global.det = detect;
  antStart();
  //ctdStart();
  return rise_pha;
  // ngkStart();
} // dataPhase

///
// rise
// if run, keep sending cable, run with current
// if free, no brake, drift free with current
int rise(float targetD, int try) {
  bool twentyB=false, targetB=false, errB=false;
  float nowD, startD, lastD, velo;
  int est;        // estimated operation time
  MsgType msg;
  enum {opT, ngkT, twentyT, fiveT};  // local timer names
  flogf("\nrise(%3.1f, %s, %d)", targetD, try);
  // utlNap(15);
  antSample();
  antDataWait();
  nowD = startD = antDepth();
  if (startD < targetD) return 1;
  if (try > boy.riseRetry) return 2;
  // .riseOrig=as tested, .riseRate=seen, .rateAccu=fudgeFactor
  // est = sec/meter(3) * depth + fudge for possible current drift
  est = 3*startD+boy.riseOp;
  tmrStart(opT, est);
  tmrStart(twentyT, 20);
  tmrStart(fiveT, 5);
  ngkFlush();
  if (targetD)
    ngkSend(riseCmd_msg);
  else
    ngkSend(surfCmd_msg);
  tmrStart(ngkT, boy.ngkDelay*2);
  flogf("\n\t| %s sent to winch at %s", 
    targetD?"riseCmd":"surfCmd", utlTime());
  while (!errB) {       // loop exits by break;
    utlX();
    // check: target, winch, 20s, 5s
    if (antData())
      nowD = antDepth();
    // arrived?
    if (targetD && nowD<targetD && !targetB) {
      flogf("\n\t| reached targetD %3.1f at %s", nowD, utlTime());
      targetB = true;
      ngkSend(stopCmd_msg);
      tmrStart(ngkT, boy.ngkDelay*2);
    }
    // op timeout - longer than estimated time + fudge
    if (tmrExp(opT)) {
      flogf("\n ERR \t| rise timeout %ds @ %3.1f, stop", est, nowD);
      errB = true;
      break;
    }
    // winch
    if (ngkRecv(&msg)!=null_msg) {
      flogf(", %s from winch", ngkMsgName(msg));
      // surface detect by winch
      if (msg==stopCmd_msg) break;
      // rise rsp
      if (msg==riseRsp_msg)
        tmrStop(ngkT);
      // target stop
      if (msg==stopRsp_msg) {
        if (!targetB)
          flogf("\n ERR \t| unexpected stopRsp");
          // ?? are we really stopped?
        break;
      } // stopRsp
    }
    if (tmrExp(ngkT)) {
      flogf("\n ERR \t| no response from winch %s", utlTime());
      // ?? missed it? 20s timeout will tell
      // ngkSend(riseCmd_msg);
    }
    // 20 seconds
    if (tmrExp(twentyT)) {
      flogf("\n\t| 20sec %s startD-nowD %3.1f ", 
        utlTime(), startD-nowD);
      if (startD-nowD < 2) {
        // by now we should have moved up 4 meters in 13.5s
        flogf("ERR < 2m");
        errB = true;
        break;
      } else {
        twentyB = true;
        lastD = nowD;
      }
    }
    // 5 seconds
    if (tmrExp(fiveT)) {
      tmrStart(fiveT, 5);
      flogf("\n\t| %s depth=%3.1f", utlTime(), nowD);
      if (!antVelo(&velo)) 
        flogf(" velo=%4.2f", velo);
    } 
  } // while !stop
  // retry if error
  if (errB) {
    flogf("\nrise() ERR\t| retry %d", ++try);
    ngkSend(stopCmd_msg);
    utlNap(boy.ngkDelay*2);
    ngkFlush();
    return rise(targetD, try);
  } else { 
    // normal stop
    return 0;
  }
} // rise


///
// based on rise(), diffs commented out; wait for winch stop
// fall to dock // expect stopCmd 
// uses: .riseRate .riseOrig .rateAccu .riseRetry
// sets: .riseRate
int fall(float targetD, int try) {
  bool twentyB=false, targetB=false, errB=false;
  float nowD, startD, velo;
  // int i;
  int op;        // estimated operation time
  MsgType msg;
  enum {opTmr, ngkTmr, fiveTmr};  // local timer names
  DBG0("fall()")
  utlNap(15);
  antSample();
  antDataWait();
  nowD = startD = antDepth();
  if (try > boy.fallRetry) return 2;
  // could be cable far out, maybe dockD+100m
  op = boy.fallOp * MINUTE;
  tmrStart(opTmr, op);
  ngkFlush();
  flogf("\n\tfall() fallCmd to winch at %s", utlTime());
  ngkSend(fallCmd_msg);
  tmrStart(ngkTmr, boy.ngkDelay*2);
  tmrStart(fiveTmr, 5);
  ctdPrompt();
  ctdSample();
  antSample();
  while (!errB) {       // loop exits by break;
    if (ctdData()) 
      ctdRead();
    // check: op, winch, 5s
    if (antData())
      nowD = antDepth();
    // op timeout // antmod may stay at surface as extra is reeled in
    if (tmrExp(opTmr)) {
      flogf("\nfall() ERR \t| fall timeout %ds @ %3.1f, stop", op, nowD);
      errB = true;
      break;
    }
    // winch
    if (ngkRecv(&msg)!=null_msg) {
      flogf(", %s from winch", ngkMsgName(msg));
      if (msg==fallRsp_msg)
        tmrStop(ngkTmr);
      // reached dock, probably
      if (msg==stopCmd_msg) break;
      // stop at target ?? were we expecting this?
      if (msg==stopRsp_msg) break;
    }
    if (tmrExp(ngkTmr)) {
      flogf("\n\tfall() WARN no response from winch %s", utlTime());
      // errB = true;
      // break;
    }
    // stop, return if reached target
    if (targetD && nowD>targetD && !targetB) {
      ngkSend(stopCmd_msg);
      tmrStart(ngkTmr, boy.ngkDelay*2);
      targetB = true;
    }
    // 5 seconds
    if (tmrExp(fiveTmr)) {
      tmrStart(fiveTmr, 5);
      flogf("\n\tfall() %s depth=%3.1f", utlTime(), nowD);
      if (!antVelo(&velo)) 
        flogf(" velo=%4.2f", velo);
    } 
  } // while !stop !err
  // retry if error
  if (errB) {
    flogf("\n\tfall() ERR retry %d", ++try);
    ngkSend(stopCmd_msg);
    utlNap(boy.ngkDelay*2);
    ngkFlush();
    return fall(targetD, try);
  } else { 
    // normal stop
    return 0;
  }
} // fall

///
// from ship deck to ocean floor
// wait until under 10m, watch until not falling, wait 30s, riseUp()
PhaseType deployPhase(void) {
  float depth, lastD;
  enum {deploy_tmr, drop_tmr};
  flogf("\ndeploy: testing sbe16, sbe39");
  if (boy.noDeploy) return rise_pha;
  ctdSample();
  ctdDataWait();
  if (!ctdRead())
    utlErr(ctd_err, "sbe16 failure");
  flogf(" sbe16@%3.1f", ctdDepth());
  antSample();
  antDataWait();
  if (!antRead())
    utlErr(ant_err, "sbe39 failure");
  flogf(" sbe39@%3.1f", antDepth());
  flogf("\ndeployPhase()\t| ant@%3.1fm buoy@%3.1fm %s", 
    antDepth(), ctdDepth(), utlDateTime());
  flogf("\n\t| wait up to %d minutes to reach bottom", boy.deployWt);
  tmrStart( deploy_tmr, MINUTE*boy.deployWt );
  // wait until under 10m
  while (true) {
    antSample();
    antDataWait();
    depth = antDepth();
    flogf("\ndeployPhase@%4.2fm %s", depth, utlTime());
    if (depth>10) break;
    if (tmrExp(deploy_tmr)) 
      sysStop("deployP() 2 hour timeout");
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
  flogf("\n\t| down, pause for %ds", boy.settleT);
  utlNap(boy.settleT);      // default 120sec
  // we are down
  boy.dockD = depth;
  flogf("\n\t| boy.dockD = %4.2f", boy.dockD);
  flogf("\n\t| go to surface, call home");
  return rise_pha;
} // deployPhase

///
// ??
// cable is stuck. up/down tries??, down to dock. 
// go back to normal if resolved ??
PhaseType errorPhase(void) {
  flogf("errorPhase()");
  return fall_pha;
} // errorPhase

///
// wait currChkSettle, buoy ctd, ant td, compute
// uses: .boy2ant
int oceanCurr(float *curr) {
  float aD, cD, a, b, c;
  ctdPrompt();
  ctdSample();
  if (!ctdDataWait()) {
    utlErr(ctd_err, "ctdDataWait fail in oceanCurr()");
    return 1;
  }
  ctdSample();
  ctdDataWait();
  if (!ctdRead())
    utlErr(ctd_err, "sbe16 failure");
  antSample();
  antDataWait();
  if (!antRead())
    utlErr(ant_err, "sbe39 failure");
  cD=ctdDepth();
  aD=antDepth();
  // pythagoras a^2 + b^2 = c^2
  // solve for b:=horizontal displacement, caused by current
  a=cD-aD;
  c=boy.boy2ant;
  flogf("\noceanCurr() \t| ant=%3.1f boy=%3.1f", aD, cD);
  if (a<0) {
    flogf("\noceanCurr() \t| ERR sbe16-sbe39<0");
    return 2;
  }
  if (c<a) {
    flogf("\noceanCurr() \t| ERR boy2ant<cD-aD");
    // ?? update boy2ant?
    // return 3;
  }
  b=sqrt(pow(c,2)-pow(a,2));
  flogf(" sideways=%4.2f", b);
  *curr = b;
  return 0;
} // oceanCurr

///
// rets: 0 safe, 1 too much, -1 err
// uses: boy.currMax
int oceanCurrChk() {
  float sideways;
  flogf("\noceanCurrChk()");
  // delay 20sec before measure to stabilize
  utlNap(20);
  if (oceanCurr(&sideways)) {
    utlErr(boy_err, " oceanCurr failed");
    return -1;
  }
  flogf("\n\t|current @ %.1f = %.1f", antDepth(), sideways);
  if (sideways>boy.currMax) {
    flogf(" too strong, cancel ascent");
    // ignore current when dbg ?? should be setting
    return 1;
  }
  return 0;
} // oceanCurrChk

///
// shutdown buoy, reflects boyInit
void boyStop(void) {} // ??

///
void boyFlush(void) {} // ??

///
// do not use until 
bool boyDocked(float depth) {
  if (boy.dockD==0.0) return false;
  else return (abs(depth-boy.dockD)<1.0);
}

///
// cycle number
int boyCycle(void) {
  return boy.cycle;
} // boyCycle
