// ant.c

#include <com.h>
#include <ant.h> 

#include <ant.h> 
#include <ctd.h>
#include <ump.h>
#include <sys.h>
#include <tmr.h>

AntInfo ant;

//
// deploy or reboot, then loop over phases data/rise/call/drop
// sets: ant.phase .phasePrev
//
void antMain(void) {
  flogf("\nantMain(): starting with mode %d", ant.mode);
    
  while (true) {
    sysFlush();                    // flush all log file buffers
    ant.phaseT = time(0);
    switch (ant.phase) {
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
    ant.phasePrev = ant.phase;
    ant.phase = phaseNext;
  } // while true
} // antMain() 

//
// ??
//
void antInit(void) {
} // antInit

//
// ??
// figure out whats happening, continue as possible
// load info from saved previous phase
// ask antmod for our velocity
// sets: ant.phase
//
PhaseType rebootPhase(void) {
  return drop_pha;
} // reboot()

//
// ??
// wispr recording and detecting, buoy is docked to ngk
// data is gathered for about 24hours (data_tmr)
// wsp powers down for % of each hour (wispr_tmr)
// organize data files, transfer data to antmod ??
// uses: data_tmr duty_tmr
//
PhaseType dataPhase(void) {
  flogf("\n\t|dataPhase()");
  // sleep needs a lot of optimizing to be worth the trouble
  // Sleep();
  wspStop();
  transferFiles();
  return rise_pha;
} // dataPhase

//
// turn on ant, free space check, transfer files from buoy to antmod
// ascend. check angle due to current, up midway, re-check angle, surface.
// sets: ant.alarm[]
//
PhaseType risePhase(void) {
  flogf("\n\t|risePhase() %s", timeStr());
  antMode(data_mod);
  // if current is too strong at bottom
  if (oceanCurrChk()) {
    sysAlarm(bottomCurr_alm);
    return drop_pha;
  }
  // MsgType riseOp(float targetD, int retry, int delay);
  if (!riseUp(ant.currChkD, 5, 1)) {
    flogf(" | fails at %3.1f m", antDepth());
    return drop_pha;
  }
  // if current is too strong at midway
  if (oceanCurrChk()) {
    sysAlarm(midwayCurr_alm);
    return drop_pha;
  }
  // surface
  if (!riseUp(0.0, 5, 1)) {
    flogf(" | fails at %3.1f m", antDepth());
    return drop_pha;
  }
  // success
  return call_pha;
}

//
// rise up to targetD, 0 means surfacing 
// when surfacing, expect stopCmd and don't set velocity 
// sets: ant.lastRise .firstRise, (*msg) 
// returns: bool
//
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
      if (antRising()) {
        // odd, we are rising but no response; log but ignore
        flogf("\n\t|risePhase() timeout on ngk, but rising so continue..."); 
        step = 2;
        continue;
      } else if (++err <= errMax) {
        // retry
        flogf(", retry"); 
        delayms(delay*1000);
        ngkSend( riseCmd_msg );
      } else { 
        flogf(", ERR abort"); 
        return false;
      }
    } // tmr
    // this shouldn't happen in step 1
    if (depth<=targetD) {          
      flogf("\nWARN\t| riseP() reached target depth without riseRsp_msg"
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
      if (antSteady()) {
        // odd, we are stopped but no response; log but ignore
        flogf("\n\t|riseP stopCmd timeout, but stopped so continue..."); 
        step = 4;
        continue; // while
      } else if (++err <= errMax) {
        // retry
        flogf(", retry"); 
        delayms(delay*1000);
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
      ant.lastRiseV = (startD-depth) / (time(0)-riseT);
      if (ant.firstRiseV==0)
        ant.firstRiseV = ant.lastRiseV;
    }
  return true;
} // riseUp

//
// ??
// turn off sbe, on irid/gps (takes 30 sec). 
// read gps date, loc. 
//
PhaseType callPhase(void) {
  return drop_pha;
} // callPhase

//
// antMod(stop), science(log), startT, dropCmd, science(stop)
// steps: 1 dropRsp, 2 docked
// failMode:=stage of failure 0,1; tryMax[], delay[] indexed by failMode
// if (err>errMax) failMode++ <= failMax
// 20 retries: 5@1sec, 5@10min, 10@1hour
// errMax[failMode], delay[failMode]
//
PhaseType dropPhase() {
  int err = 0, step = 1;
  int failMode = 0, failMax = 2;
  int errMax[3] = {5, 10, 20}, delay[3] = {1, 10*60, 60*60};
  float depth, startD;
  MsgType msg;
  time_t dropT = (time_t)0;
  flogf("\n\tdropP() %s", timeStr());
  antMode(stop_mod);
  mpcDevice(ctd_dev);
  /// 
  // step 1: dropCmd 
  // loop until dropRsp or dropping+timeout
  flogf(" dropCmd");
  ngkSend( dropCmd_msg );
  while (step==1) {
    depth = antDepth();
    switch (msg = ngkRecv()) {
    case null_msg: break;
    case dropRsp_msg:
      // start velocity measure
      if (err==0) // all OK, measure dropTime
        dropT = time(0);
      step = 2;
      continue; // while
    default: // unexpected msg
      flogf("\n\t|dropP() unexpected %s at %3.1f m", ngkMsgName(msg), depth);
    } // switch
    //
    if (tmrExp(winch_tmr)) {
      // minor err
      if (antDropping()) {
        // odd, we are dropping but no response; log but allow
        flogf(" timeout, but dropping ..."); 
        err++;    // indicate trouble
        step = 2;
        continue; // while(step 1)
      } 
      // err, delay and retry algorithm // failMode ++ as errs exceed errMax
      if (++err > errMax[failMode])  
        if (++failMode > failMax) 
          return error_pha;
      // retry dropCmd
      pwrNap(delay[failMode])
      flogf(" dropCmd"); 
      ngkSend( dropCmd_msg );
      } // if dropping
    } // if timeout
  } // while step1
  ///
  // step 2: dropping 
  // ctdSample loop until docked
  ctdSample();
  while (step==2) {
    // got science?
    if (ctdReady()) {
      lastD = depth;
      depth = ctdDepth();           // logs to file
      // ctd is slow, should drop .6m between samples
      if ((depth-lastD)<0.2)
        // NOT DROPPING stopCmd could be pending
        tmrStart(winch_tmr, 8);
      ctdSample();
    } // ctd
    //
    if (tmrExp(winch_tmr)) {
      // tmr set by NOT DROPPING above
      lastD = depth;
      depth = ctdDepth();           
      // ctd is slow, should drop .6m between samples
      if ((depth-lastD)<0.2)        // not dropping & timeout, panic
        return error_pha;
      else                          // dropping, tmr was a mistake
        ctdSample();
    } // timeout
    //
    switch (msg = ngkRecv()) {
    case null_msg: break;
    case stopCmd_msg:
      if (antDocked()) {            // normal and expected
        step = 3;
        continue; // while
      } else {
        // if stop but not docked, cable is stuck
        return error_pha;
      }
    case default:
      flogf("\n\t|dropP() unexpected %s at %3.1f m", ngkMsgName(msg), depth);
    } // switch
  } // while step2
  /// 
  // step 3: docked
  // velocity, finish science
  if (err==0) {
    // skip if we didn't stop clean
    ant.lastRiseV = (startD-depth) / (time(0)-dropT);
    if (ant.firstRiseV==0)
      ant.firstRiseV = ant.lastRiseV;
  }
  // turn off ant, clear ngk, clear ctd
  ngkStop();
  ctdStop();
  antStop();
  return data_pha;
} // dropPhase

//
// from ship deck to ocean floor
// wait until under 10m, watch until not dropping, wait 30s, riseP
//
PhaseType deployPhase(void) {
  mpcDevice(ant_dev);
  antMode(idle_mod);
  tmrStart( deploy_tmr, 60*60*2 );
  // wait until under 10m
  while (antDepth()<10.0) {
    pwrNap(30);
    if (tmrExp(deploy_tmr) {
      flogf("\n%s\t|deployP() 2 hour timeout", timeStr(scratch));
      sysStop("deployP() 2 hour timeout");
    }
  }
  // watch until not stationary
  antMode(data_mod);
  while (!antSteady()) 
    pwrNap(3);
  pwrNap(30);
  mpcDevice(ctd_dev);
  // use ant ctd for dockD, for later compare at end of drop_pha
  ant.dockD = ctdDepth();
  mpcDevice(ant_dev);
  return rise_pha;
}

//
// ??
// cable is stuck. short up, down to dock. 
// go back to normal if resolved ??
// 
PhaseType errorPhase(void) {
  return drop_pha;
}

//
// wait currChkSettle, buoy ctd, ant td, compute
// uses: .ant2ant
//
float oceanCurr() {
  float aD, cD, a, b, c;
  // usually called while antMod is data_mod
  antMode(idle_mod);
  mpcDevice(ctd_dev);
  cD=ctdDepth();
  mpcDevice(ant_dev);
  aD=antDepth();
  // pythagoras a^2 + b^2 = c^2
  a=cD-aD;
  c=ant.ant2ant;
  b=sqrt(pow(c,2)-pow(a,2));
  return b;
}

//
// uses: ant.currMax
//
bool oceanCurrChk() {
  float sideways;
  flogf("\n\t| oceanCurrChk() ");
  sideways = oceanCurr();
  flogf(" @%.1f=%.1f ", antDepth(), sideways);
  if (sideways>ant.currMax) {
    flogf("too strong, cancel ascent");
    return true;
  }
  return false;
} // oceanCurrChk

//
// shutdown buoy, reflects antInit
//
// ??
void antStop(void) {}

// ??
void antFlush(void) {}

// ??
void transferFiles(void) {}

bool antDocked(float depth) {
  return (abs(depth-ant.dockD)<0.2);
}
