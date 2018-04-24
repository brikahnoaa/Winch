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
  int starts, cycle=0;
  PhaseType phaseNext;
  // boy.phase set by sys.cfg
  starts = sysInit();
  mpcInit();
  antInit();
  boyInit();
  ctdInit();
  ngkInit();
  wspInit();
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
      if (boy.cycles && (cycle++ > boy.cycles))
        sysStop("cycleLimit");
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
// sleep needs a lot of optimizing to be worth the trouble
// uses: data_tmr duty_tmr
PhaseType dataPhase(void) {
  flogf("\n+dataPhase()@%s", utlDateTime());
  wspStart();
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
  ctdStart();
  // if current is too strong at bottom
  if (oceanCurrChk()) {
    sysAlarm(bottomCurr_alm);
    //?? return drop_pha;
  }
  success = riseUp(boy.currChkD, 5, 1);
  if (!success) {
    flogf("\n\t| riseUp fails at %3.1f m", antDepth());
    //??  return drop_pha;
  }
  // if current is too strong at midway
  if (oceanCurrChk()) {
    sysAlarm(midwayCurr_alm);
    //?? return drop_pha;
  }
  // surface
  success = riseUp(0.0, 5, 1);
  if (!success) {
    flogf(" | fails at %3.1f m", antDepth());
    //?? return drop_pha;
  }
  // success
  return call_pha;
} // risePhase

///
// simpler form of riseUp()
bool riseToSurf(void) {
  float depth, startD;
  MsgType msg;
  int err = 0, step = 1;
  int errMax = 5, delay = 5;
  time_t riseT;
  DBG0("riseToSurf()")
  startD = depth = antDepth();
  /// step 1: riseCmd
  // watch for riseRsp. on timeout, retry or continue if rising
  ngkSend( riseCmd_msg );
  while (step==1) {
    utlX();
    depth = antDepth();
    switch (msg = ngkRecv()) {
    case null_msg: break;
    case riseRsp_msg:
      // start velocity measure
      riseT = time(0);
      step = 2;
      continue; // while
    case stopCmd_msg:     // stopped by winch
      ngkSend(stopRsp_msg);
      if (depth<=antSurfMaxD())
        return true;
      else
        flogf(", stopCmd at %4.2fm riseToSurf() step 1", depth);
    default: // unexpected msg
      flogf("\n\t|riseUp() unexpected %s at %4.2f m", ngkMsgName(msg), depth);
    } // switch
    if (tmrExp(winch_tmr)) {
      if (startD - depth > 2) {       // ?? should be antMoving()
        // odd, we are rising but no riseRsp - log but ignore
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
  } // while step1
  // step 2: going to surf, watch for stopCmd
  while (step==2) {
    utlX();
    depth = antDepth();
    // ?? if (antMoving()>=0.0) {
    //  utlErr(ngk_err, "riseToSurf() step 2 not rising, retry");
    //  return riseToSurf();
    // }
    msg = ngkRecv();
    switch (msg) {
    case null_msg: break;
    case stopCmd_msg:     // stopped by winch
      ngkSend(stopRsp_msg);
      step = 3;
      continue;  // while step2
    default:
      flogf("\n\t|riseUp() unexpected %s at %3.1f m", ngkMsgName(msg), depth);
    } // switch
  } // while step2
  if (depth>antSurfMaxD())
    flogf("\n\t|riseToSurf() too deep at %4.2f", depth);
  return true;
} // riseToSurf

///
// rise up to targetD, 0 means surfacing 
// when surfacing, expect stopCmd and don't set velocity 
// sets: boy.lastRise .firstRise, (*msg) 
// returns: bool
bool riseUp(float targetD, int errMax, int delay) {
  float depth, startD, stopD;
  MsgType msg;
  int err = 0, step = 1;
  time_t riseT;
  // if surfacing use riseToSurf()
  if (targetD==0.0) return riseToSurf();
  DBG0("riseUp(%dm)", targetD)
  startD = depth = antDepth();
  /// step 1: riseCmd
  // watch for riseRsp. on timeout, retry or continue if rising
  ngkSend( riseCmd_msg );
  while (step==1) {
    utlX();
    depth = antDepth();
    switch (msg = ngkRecv()) {
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
    if (tmrExp(winch_tmr)) {
      if (startD - depth > 2) {       // ?? should be antMoving()
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
  while (step==2) {
    utlX();
    depth = antDepth();
    if (depth<=targetD) {
      ngkSend( stopCmd_msg );
      stopD = depth;
      step = 3;
    }
    // shouldn't get winch msg in step 2 // ?? utlErr
    if ((msg = ngkRecv())!=null_msg)          
      flogf("\n\t|riseUp() unexpected %s at %3.1f m", ngkMsgName(msg), depth);
  } // while step2
  /// step 3: stopCmd 
  // watch for stopRsp. on timeout, retry or continue if stopped
  err = 0;
  while (step==3) {
    utlX();
    depth = antDepth();
    switch (msg = ngkRecv()) {
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
    if (tmrExp(winch_tmr)) {
      stopD = antDepth();      // ?? should be antMoving
      utlNap(5);
      depth = antDepth();
      if (abs(stopD-depth)<0.5) {   // ?? bad hack
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
      } // if moving
    } // if tmr
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
  float depth, startD, dropD;
  time_t dropT;
  PhaseType r = data_pha;
  MsgType msg;
  flogf("\n+dropPhase() %s", utlTime());
  antFlush();
  startD = depth = antDepth();
  // step1. loop until dropRsp or dropping+timeout
  step = 1;
  DBG0("dP:1")
  ctdAuton(true);
  ngkSend( dropCmd_msg );
  while (step==1) {
    utlX();
    depth = antDepth();
    msg = ngkRecv();
    switch (msg) {
    case null_msg: break;
    case dropRsp_msg:
      // start velocity measure
      dropT = time(0);
      dropD = depth;
      step = 2;
      break; 
    default:
      flogf("\n\t|dropP() unexpected %s at %3.1f m", ngkMsgName(msg), depth);
    } // switch
    // err, delay and retry algorithm 
    if (tmrExp(winch_tmr)) {
      flogf(" timeout"); 
      // timeout
      if (depth - startD > 2) {  // no antMoving close to surface
        // odd, we are dropping but no response; log but allow
        flogf(" but dropping"); 
        step = 2;
        break; // while step1
      } // if depth
      if (++err > errMax[failStage] && ++failStage > failMax) {
        // note: ++err, ++failStage as errs exceed errMax
        flogf("\n\t|dropPhase() too many retries, panic");
        // r = error_pha;
        step = 6;
        break; // while step1
      }
      // here: timeout & !dropping & errs<max
      // retry dropCmd, use longer delay after more errs
      utlNap(delay[failStage]);
      flogf(" retry dropCmd"); 
      ngkSend( dropCmd_msg );
    } // if timeout
  } // while step1
  // step 2: drop til you stop
  if (step==2)
    step = 3;
  /*
  DBG0("dP:2")
  antFlush();
  utlNap(20);
  while (step==2) {
    utlX();
    depth = antDepth();
    if (antMoving()<=0.0)
      step = 3;
  }
   */ 
  DBG0("dP:3")
  // step 3: stopCmd Rsp
  tmrStart(winch_tmr, 300);        // ?? 
  while (step==3) {
    utlX();
    depth = antDepth();
    msg = ngkRecv();
    switch (msg) {
    case null_msg: break;
    case stopCmd_msg:
      ngkSend(stopRsp_msg);
      step = 4;
      break;
    default:
      flogf("\n\t|dropP() unexpected %s at %3.1f m", ngkMsgName(msg), depth);
    } // switch
    if (tmrExp(winch_tmr)) {
      flogf(" 5min timeout");
      step = 4;
    }
  } // while msg
  DBG0("dP:4")
  // step 4: docked?
  if (step==4) {
    depth = antDepth();
    if (!boyDocked(depth)) {
      // if stop but not docked, cable is stuck
      flogf(" not docked at %4.2f", depth);
      utlErr(ngk_err, "dropPhase step 4, we should be docked");
      sysAlarm(cableStuck_alm);
      err++;
    }
    // velocity, finish science
    if (err==0) {
      // skip if we didn't drop clean
      boy.dropVLast = (dropD-depth) / (time(0)-dropT);
      if (boy.dropVFirst==0)
        boy.dropVFirst = boy.dropVLast;
    } // if err
  } // if step4
  // get samples, depends on ant. ctd.logging
  // antGetSamples();
  // ctdGetSamples();
  // turn off ant, clear ngk, clear ctd
  ctdAuton(false);
  ctdStop();
  antStop();
  ngkStop();
  return r;
} // dropPhase

///
// from ship deck to ocean floor
// wait until under 10m, watch until not dropping, wait 30s, riseUp()
PhaseType deployPhase(void) {
  float depth, lastD;
  antStart();
  antAutoSample(true);
  tmrStart( deploy_tmr, 60*60*2 );
  depth = antDepth();
  flogf("\n+deployPhase()@%s %4.2fm", utlDateTime(), depth);
  // wait until under 10m
  while ((depth = antDepth())<10.0) {
    flogf("\ndeployPhase() at %4.2fm", depth);
    if (tmrExp(deploy_tmr)) 
      sysStop("deployP() 2 hour timeout");
    utlNap(30);
  }
  tmrStop(deploy_tmr);
  flogf("\n\t| %4.2fm>10 so wait until not moving\n", depth);
  lastD = depth;
  utlNap(120);
  // at most 5 min to descend, already waited 2min
  tmrStart( deploy_tmr, 60*5 );
  depth = antDepth();
  // must drop at least 1m in 10 sec
  while (depth-lastD>1.0) {
    utlNap(15);
    if (tmrExp(deploy_tmr)) 
      break;
    lastD = depth;
    depth = antDepth();
  }
  tmrStop(deploy_tmr);
  // we are docked
  depth = antDepth();
  boy.dockD = depth;
  flogf("\n\t| boy.dockD = %4.2f", boy.dockD);
  flogf("\n\t| go to surface, call home");
  utlNap(10);
  // rise to surface, 5 tries, short delay
  if (riseUp(0.0, 5, 1)) {
    flogf("\n\t| deployed @ %s", utlTime());
    return call_pha;
  } else {      // drop and sulk
    flogf("\nErr\t| deployed but riseUp fail @ %s", utlTime());
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
    DBGX(return false;)
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
