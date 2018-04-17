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
    utlX();
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
  flogf("\n+rebootPhase()@%s", utlDateTime());
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
  flogf("\n+dataPhase()@%s", utlDateTime());
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
  flogf("\n+risePhase()@%s", utlDateTime());
  antStart();
  // if current is too strong at bottom
  if (oceanCurrChk()) {
    sysAlarm(bottomCurr_alm);
    return drop_pha;
  }
  success = riseUp(boy.currChkD, 5, 1);
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
  success = riseUp(0.0, 5, 1);
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
    utlX();
    depth = antDepth();
    if (ngkData()) {
      msg = ngkRecv();
      switch (msg) {
      case null_msg: break;
      case riseRsp_msg:
        // start velocity measure
        riseT = time(0);
        step = 2;
        continue; // while
      case stopCmd_msg:     // stopped by winch
        ngkSend(stopRsp_msg);
        flogf(", ERR winch stopCmd at %3.1fm in step 1", depth);
        return false;
      default: // unexpected msg
        flogf("\n\t|riseUp() unexpected %s at %3.1f m", ngkMsgName(msg), depth);
      } // switch
    } // nD
    if (tmrExp(winch_tmr)) {
      if (antMoving()<0.0) {
        // odd, we are rising but no response; log but ignore
        flogf("\n\t|riseUp() timeout on ngk, but rising so continue..."); 
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
      flogf("\nWARN\t| riseUp() reached target depth without riseRsp_msg");
      step = 3;
      continue;
    }
  } // while step1
  /// step 2: depth
  // watch until target depth
  if (targetD==0.0)
    step = 3;
  while (step==2) {
    utlX();
    depth = antDepth();
    if (depth<=targetD) 
      step = 3;
    // this shouldn't happen in step 2
    if (ngkData())
      msg = ngkRecv();
    if (msg!=null_msg)          
      flogf("\n\t|riseUp() unexpected %s at %3.1f m", ngkMsgName(msg), depth);
  } // while step2
  /// step 3: stopCmd 
  // watch for stopRsp. on timeout, retry or continue if stopped
  err = 0;
  ngkSend( stopCmd_msg );
  while (step==3) {
    utlX();
    depth = antDepth();
    if (ngkData()) {
      msg = ngkRecv();
      switch (msg) {
      case null_msg: break;
      case stopRsp_msg:
        step = 4;
        continue; // while
      case stopCmd_msg:     // stopped by winch
        ngkSend(stopRsp_msg);
        flogf("\n\t | riseUp() stopped by winch at %3.1fm. Strange not fatal.", 
          depth);
        return false;
      default: // unexpected msg
        flogf("\n\t|riseUp() unexpected %s at %3.1f m", ngkMsgName(msg), depth);
      } // switch
    } // nD
    if (tmrExp(winch_tmr)) {
      if (antMoving()==0.0) {
        // odd, we are stopped but no response; log but ignore
        flogf("\n\t|riseUp() stopCmd timeout, but stopped so continue..."); 
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
  } // while step3
  // velocity to target, not to surface
  if (targetD!=0)
    // skip if we didn't stop cleanly
    if (err==0) { 
      boy.riseVLast = (startD-depth) / (time(0)-riseT);
      if (boy.riseVFirst==0.0)
        boy.riseVFirst = boy.riseVLast;
    }
  flogf("\n\t| riseUp() to depth %d", depth);
  return true;
} // riseUp

///
// ??
// turn off sbe, on irid/gps (takes 30 sec). 
// read gps date, loc. 
PhaseType callPhase(void) {
  flogf("\n+callPhase()@%s", utlDateTime());
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
  flogf("\n+dropPhase()@%s", utlDateTime());
  antFlush();
  // step1. loop until dropRsp or dropping+timeout
  ngkSend( dropCmd_msg );
  while (true) {
    utlX();
    depth = antDepth();
    if (ngkData()) {
      msg = ngkRecv();
      if (msg==dropRsp_msg) {
        // start velocity measure
        dropT = time(0);
        dropD = depth;
        break; // while
      } else if (msg!=null_msg) {
        flogf("\n\t|dropP() unexpected %s at %3.1f m", ngkMsgName(msg), depth);
      }
    } // nD
    if (tmrExp(winch_tmr)) {
      if (antMoving()>0.0) {
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
  while (antMoving()>0) {
    utlX();
    depth = antDepth();
  }
  // step 3: stopCmd Rsp
  while (true) {
    utlX();
    if (ngkData()) {
      msg = ngkRecv();
      if (msg==stopCmd_msg) {
        ngkSend(stopRsp_msg);
        break;
      } else if (msg!=null_msg) {
        flogf("\n\t|dropP() unexpected %s at %3.1f m", ngkMsgName(msg), depth);
      }
    } // nD
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
  // get samples, depends on ant. ctd.logging
  antGetSamples();
  ctdGetSamples();
  // turn off ant, clear ngk, clear ctd
  ngkStop();
  ctdStop();
  antStop();
  return data_pha;
} // dropPhase

///
// from ship deck to ocean floor
// wait until under 10m, watch until not dropping, wait 30s, riseUp()
PhaseType deployPhase(void) {
  antStart();
  antAutoSample(true);
  tmrStart( deploy_tmr, 60*60*2 );
  flogf("\n+deployPhase()@%s at %4.2f", utlDateTime(), antDepth());
  // wait until under 10m
  while (antDepth()<10.0) {
    utlX();
    flogf("\ndeployPhase() at %4.2f", antDepth());
    if (tmrExp(deploy_tmr)) 
      sysStop("deployP() 2 hour timeout");
    pwrNap(30);
  }
  // watch until not moving
  flogf(">10 so wait until not moving\n");
  while (antMoving()!=0.0) {
    utlX();
    antDepth();
    if (tmrExp(deploy_tmr)) 
      sysStop("deployP() 2 hour timeout");
    pwrNap(3);
  }
  tmrStop(deploy_tmr);
  boy.dockD = antDepth();
  antFlush();             // flush antMoving samples
  flogf("\n\t| boy.dockD = %4.2f", antDepth());
  flogf("\n\t| go to surface, call home");
  pwrNap(10);
  // rise to surface, 5 tries, short delay
  if (riseUp(0.0, 5, 1)) {
    flogf("\n\t| deployed @ %s", utlDateTime());
    return call_pha;
  } else {      // drop and sulk
    flogf("\nErr\t| deployed but riseUp fail @ %s", utlDateTime());
    return drop_pha;
  }
} // deployPhase

///
// ??
// cable is stuck. up/down tries??, down to dock. 
// go back to normal if resolved ??
PhaseType errorPhase(void) {
  flogf("\n+errorPhase()@%s", utlDateTime());
  return drop_pha;
} // errorPhase

///
// wait currChkSettle, buoy ctd, ant td, compute
// uses: .boy2ant
float oceanCurr() {
  float aD, cD, a, b, c;
  cD=ctdDepth();
  aD=antDepth();
  // pythagoras a^2 + b^2 = c^2
  // b:=horizontal displacement, caused by current
  a=cD-aD;
  c=boy.boy2ant;
  DBG1("aD=%4.2f cD=%4.2f boy2ant=%4.2f", aD, cD, c)
  if (a<0 || c<a) return -1.0;
  b=sqrt(pow(c,2)-pow(a,2));
  DBG1("sideways=%4.2f", b)
  return b;
} // oceanCurr

///
// uses: boy.currMax
bool oceanCurrChk() {
  float sideways;
  flogf("\n\t| oceanCurrChk() ");
  sideways = oceanCurr();
  if (sideways<0) {
    utlErr(logic_err, "oceanCurr invalid value");
    return false;
  }
  flogf(" @%.1f=%.1f ", antDepth(), sideways);
  if (sideways>boy.currMax) {
    flogf("too strong, cancel ascent");
    // ignore current when dbg ?? should be setting
    DBG(return false;)
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
// do not use until 
bool boyDocked(float depth) {
  if (boy.dockD==0.0) return false;
  else return (abs(depth-boy.dockD)<1.0);
}
