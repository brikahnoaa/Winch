// boy.c

#include <utl.h>
#include <boy.h> 

#include <ant.h> 
#include <ctd.h>
#include <mpc.h>
#include <ngk.h>
#include <pwr.h>
#include <sys.h>
#include <tmr.h>
#include <wsp.h>

BoyInfo boy;

///
// deploy or reboot, then loop over phases data/rise/call/drop
// sets: boy.phase .phasePrev
void boyMain() {
  int starts;
  PhaseType phaseNext;
  // boy.phase set by sys.cfg
  starts = sysInit();
  mpcInit();
  antInit();
  boyInit();
  ctdInit();
  ngkInit();
  pwrInit();
  if (starts>1) 
    boy.phase = reboot_pha;
  flogf("\nboyMain(): starting with phase %d", boy.phase);
    
  while (true) {
    sysFlush();                    // flush all log file buffers
    boy.phaseT = time(0);
    switch (boy.phase) {
    case data_pha: // data collect by WISPR
      phaseNext = dataPhase();
      break;
    case rise_pha: // Ascend buoy, check for current and ice
      phaseNext = risePhase();
      break;
    case call_pha: // Call home via Satellite
      phaseNext = callPhase();
      break;
    case drop_pha: // Descend buoy, science sampling
      phaseNext = dropPhase();
      break;
    case deploy_pha:
      phaseNext = deployPhase();
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
  return drop_pha;
} // reboot()

///
// ??
// wispr recording and detecting, buoy is docked to ngk
// data is gathered for about 24hours (data_tmr)
// wsp powers down for % of each hour (wispr_tmr)
// organize data files, transfer data to antmod ??
// uses: data_tmr duty_tmr
PhaseType dataPhase(void) {
  flogf("\n\t|dataPhase()");
  // sleep needs a lot of optimizing to be worth the trouble
  // Sleep();
  wspStop();
  return rise_pha;
} // dataPhase

///
// turn on ant, free space check, transfer files from buoy to antmod
// ascend. check angle due to current, up midway, re-check angle, surface.
// sets: boy.alarm[]
PhaseType risePhase(void) {
  bool success;
  flogf("\n\t|risePhase() %s", utlDateTime());
  // if current is too strong at bottom
  if (oceanCurrChk()) {
    sysAlarm(bottomCurr_alm);
    return drop_pha;
  }
  antAuto(true);
  ctdAuton(true);
  success = riseUp(boy.currChkD, 5, 1);
  antAuto(false);
  ctdAuton(false);
  ctdLog();
  if (!success) {
    flogf("\n\t| riseUp fails at %3.1f m", antDepth());
    return drop_pha;
  }
  // if current is too strong at midway
  if (oceanCurrChk()) {
    sysAlarm(midwayCurr_alm);
    return drop_pha;
  }
  // surface
  antAuto(true);
  ctdAuton(true);
  success = riseUp(0.0, 5, 1);
  antAuto(false);
  ctdAuton(false);
  ctdLog();
  if (!success) {
    flogf(" | fails at %3.1f m", antDepth());
    return drop_pha;
  }
  // success
  return call_pha;
} // risePhase

///
// rise up to targetD, 0 means surfacing 
// when surfacing, expect stopCmd and don't set velocity 
// sets: boy.lastRise .firstRise, (*msg) 
// returns: bool
bool riseUp(float targetD, int errMax, int delay) {
  float depth, startD;
  MsgType msg;
  int err = 0, step = 1;
  time_t riseT;
  DBG0("riseUp(%dm)", targetD)
  startD = depth = antDepth();
  /// step 1: riseCmd
  // watch for riseRsp. on timeout, retry or continue if rising
  ngkSend( riseCmd_msg );
  while (step==1) {
    depth = antDepth();
    switch (msg = ngkRecv()) {
    case null_msg: break;
    case riseRsp_msg:
      // start velocity measure
      riseT = time(0);
      step = 2;
      continue; // while
    case stopCmd_msg:     // stopped by winch
      // ngkSend(stopRsp_msg) in ngkRecv
      flogf(", ERR winch stopCmd at %3.1fm in step 1", depth);
      return false;
    default: // unexpected msg
      flogf("\n\t|riseP unexpected %s at %3.1f m", ngkMsgName(msg), depth);
    } // switch
    if (tmrExp(winch_tmr)) {
      if (antMoving()>0.0) {
        // odd, we are rising but no response; log but ignore
        flogf("\n\t|risePhase() timeout on ngk, but rising so continue..."); 
        step = 2;
        continue;
      } else if (++err <= errMax) {
        // retry
        flogf(", retry"); 
        utlDelay(delay*1000);
        ngkSend( riseCmd_msg );
      } else { 
        flogf(", ERR abort"); 
        return false;
      }
    } // tmr
    // this shouldn't happen in step 1
    if (depth<=targetD) {          
      flogf("\nWARN\t| riseP() reached target depth without riseRsp_msg");
      step = 3;
      continue;
    }
  } // while step1
  /// step 2: depth
  // watch until target depth
  while (step==2) {
    depth = antDepth();
    if (depth<=targetD) 
      step = 3;
    // this shouldn't happen in step 2
    if ((msg = ngkRecv())!=null_msg)          
      flogf("\n\t|riseP unexpected %s at %3.1f m", ngkMsgName(msg), depth);
  } // while step2
  /// step 3: stopCmd 
  // watch for stopRsp. on timeout, retry or continue if stopped
  err = 0;
  ngkSend( stopCmd_msg );
  while (step==3) {
    depth = antDepth();
    switch (msg = ngkRecv()) {
    case null_msg: break;
    case stopRsp_msg:
      step = 4;
      continue; // while
    case stopCmd_msg:     // stopped by winch
      // ngkSend(stopRsp_msg) in ngkRecv
      flogf("\n\t | riseUp() stopped by winch at %3.1fm. Strange not fatal.", 
        depth);
      return false;
    default: // unexpected msg
      flogf("\n\t|riseP unexpected %s at %3.1f m", ngkMsgName(msg), depth);
    } // switch
    if (tmrExp(winch_tmr)) {
      if (antMoving()==0.0) {
        // odd, we are stopped but no response; log but ignore
        flogf("\n\t|riseP stopCmd timeout, but stopped so continue..."); 
        step = 4;
        continue; // while
      } else if (++err <= errMax) {
        // retry
        flogf(", retry"); 
        utlDelay(delay*1000);
        ngkSend( stopCmd_msg );
      } else { 
        flogf(", ERR abort"); 
        return false;
      }
    } // tmr
  } // while step1
  // velocity to target, not to surface
  if (targetD!=0)
    // skip if we didn't stop cleanly
    if (err==0) { 
      boy.riseVLast = (startD-depth) / (time(0)-riseT);
      if (boy.riseVFirst==0.0)
        boy.riseVFirst = boy.riseVLast;
    }
  return true;
} // riseUp

///
// ??
// turn off sbe, on irid/gps (takes 30 sec). 
// read gps date, loc. 
PhaseType callPhase(void) {
  return drop_pha;
} // callPhase

///
// antAuto, science(log), startT, dropCmd, science(stop)
// while (!docked) {
//  dropcmd, [rsp] or time. 
//  while(moving) {sleep 8} 
//  [stopcmd] or time, rsp. 
//  docked?}
// failMode:=stage of failure 0,1; tryMax[], delay[] indexed by failMode
// if (err>errMax) failMode++ <= failMax
// 20 retries: 5@1sec, 5@10min, 10@1hour
// errMax[failMode], delay[failMode]
PhaseType dropPhase() {
  int err = 0, step = 1;
  int failStage = 0, failMax = 2;
  int errMax[3] = {5, 10, 20}, delay[3] = {1, 10*60, 60*60};
  float depth, dropD;
  time_t dropT;
  MsgType msg;
  flogf("\n\tdropPhase() %s", utlDateTime());
  antAuto(true);
  ctdAuton(true);
  // step1. loop until dropRsp or dropping+timeout
  ngkSend( dropCmd_msg );
  while (true) {
    depth = antDepth();
    msg = ngkRecv();
    if (msg==dropRsp_msg) {
      // start velocity measure
      dropT = time(0);
      dropD = depth;
      break; // while
    } else if (msg!=null_msg) {
      flogf("\n\t|dropP() unexpected %s at %3.1f m", ngkMsgName(msg), depth);
    }
    //
    if (tmrExp(winch_tmr)) {
      if (antMoving()<0) {
        // odd, we are dropping but no response; log but allow
        flogf(" timeout, but dropping ..."); 
        break; // while step1
      } // if dropping
      // err, delay and retry algorithm // failStage ++ as errs exceed errMax
      if (++err > errMax[failStage])  
        if (++failStage > failMax) 
          return error_pha;
      // retry dropCmd, longer delay after more errs
      pwrNap(delay[failStage]);
      flogf(" dropCmd"); 
      ngkSend( dropCmd_msg );
    } // if timeout
  } // while step1
  // step 2: drop til you stop
  while (antMoving()<0)
    depth = antDepth();
  // step 3: stopCmd Rsp
  while (true) {
    msg = ngkRecv();
    if (msg==stopCmd_msg) {
      ngkSend(stopRsp_msg);
      break;
    } else if (msg!=null_msg) {
      flogf("\n\t|dropP() unexpected %s at %3.1f m", ngkMsgName(msg), depth);
    }
  }
  // step 4: docked?
  if (!boyDocked(depth)) {       // normal and expected
    // if stop but not docked, cable is stuck
    return error_pha;
  }
  // velocity, finish science
  if (err==0) {
    // skip if we didn't stop clean
    boy.dropVLast = (dropD-depth) / (time(0)-dropT);
    if (boy.dropVFirst==0)
      boy.dropVFirst = boy.dropVLast;
  }
  // turn off ant, clear ngk, clear ctd
  antAuto(false);
  ctdAuton(false);
  ctdLog();
  ngkStop();
  ctdStop();
  antStop();
  return data_pha;
} // dropPhase

///
// from ship deck to ocean floor
// wait until under 10m, watch until not dropping, wait 30s, riseP
PhaseType deployPhase(void) {
  mpcDevice(ant_dev);
  antInit();
  tmrStart( deploy_tmr, 60*60*2 );
  // wait until under 10m
  while (antDepth()<10.0) {
    pwrNap(30);
    if (tmrExp(deploy_tmr)) {
      flogf("\n%s\t|deployP() 2 hour timeout", utlDateTime());
      sysStop("deployP() 2 hour timeout");
    }
  }
  // watch until not moving
  antAuto(true);
  while (antMoving()!=0.0) 
    pwrNap(3);
  pwrNap(30);
  boy.dockD = antDepth();
  antAuto(false);
  return rise_pha;
} // deployPhase

///
// ??
// cable is stuck. short up, down to dock. 
// go back to normal if resolved ??
PhaseType errorPhase(void) {
  return drop_pha;
} // errorPhase

///
// wait currChkSettle, buoy ctd, ant td, compute
// uses: .boy2ant
float oceanCurr() {
  float aD, cD, a, b, c;
  // usually called while auto_ant
  antAuto(true);
  ctdAuton(true);
  mpcDevice(ctd_dev);
  cD=ctdDepth();
  mpcDevice(ant_dev);
  aD=antDepth();
  // pythagoras a^2 + b^2 = c^2
  a=cD-aD;
  c=boy.boy2ant;
  b=sqrt(pow(c,2)-pow(a,2));
  antAuto(false);
  ctdAuton(false);
  ctdLog();
  return b;
} // oceanCurr

///
// uses: boy.currMax
bool oceanCurrChk() {
  float sideways;
  flogf("\n\t| oceanCurrChk() ");
  sideways = oceanCurr();
  flogf(" @%.1f=%.1f ", antDepth(), sideways);
  if (sideways>boy.currMax) {
    flogf("too strong, cancel ascent");
    return true;
  }
  return false;
} // oceanCurrChk

///
// shutdown buoy, reflects boyInit
void boyStop(void) {} // ??

///
void boyFlush(void) {} // ??

///
bool boyDocked(float depth) {
  return (abs(depth-boy.dockD)<1.0);
}
