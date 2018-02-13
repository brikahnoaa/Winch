// boy.c

#include <com.h>
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

//
// deploy or reboot, then loop over phases data/rise/call/drop
//
void boyMain(int starts) {
  // boy.phase set by sys.cfg
  if (starts>1) 
    boy.phase = reboot_pha;
  flogf("\nboyMain(): starting with phase %d", boy.phase);
    
  while (true) {
    sysFlush();                    // flush all log file buffers
    boy.phaseT=time(0);
    switch (boy.phase) {
    case data_pha: // data collect by WISPR
      boy.phase = dataPhase();
      break;
    case rise_pha: // Ascend buoy, check for current and ice
      boy.phase = risePhase();
      break;
    case call_pha: // Call home via Satellite
      boy.phase = callPhase();
      break;
    case drop_pha: // Descend buoy, science sampling
      boy.phase = dropPhase();
      break;
    case deploy_pha:
      boy.phase = deployPhase();
      break;
    case reboot_pha:
      boy.phase = rebootPhase();
      break;
    case error_pha:
      boy.phase = errorPhase();
      break;
    } // switch
  } // while true
} // boyMain() 

//
// 
//
void boyInit() {
} // boyInit

//
// figure out whats happening, continue as possible
// load info from saved previous phase
// ask antmod for our velocity
// sets: boy.phase
//
void rebootPhase() {
} // reboot()

//
// wispr recording and detecting, buoy is docked to ngk
// data is gathered for about 24hours (data_tmr)
// wsp powers down for % of each hour (wispr_tmr)
// organize data files, transfer data to antmod ??
// uses: data_tmr duty_tmr
//
void dataPhase(void) {
  flogf("\n\t|dataPhase()");
  // sleep needs a lot of optimizing to be worth the trouble
  // Sleep();
  wspStop();
  transferFiles();
} // dataPhase

//
// turn on ant, free space check, transfer files from buoy to antmod
// ascend. check angle due to current, up midway, re-check angle, surface.
// sets: boy.alarm[]
//
PhaseType risePhase(void) {
  flogf("\n\t|risePhase()");
  antMode(td_mod);
  // if current is too strong at bottom
  if (oceanCurrChk()) {
    sysAlarm(bottomCurr_alm);
    return drop_pha;
  }
  // MsgType riseOp(float targetD, int retry, int delay);
  if (!riseUp(boy.currChkD, 5, 1)) {
    flogf(" | fails at %03.1f m", antDepth());
    return drop_pha;
  }
  // if current is too strong at midway
  if (oceanCurrChk()) {
    sysAlarm(midwayCurr_alm);
    return drop_pha;
  }
  if (!riseSurf(5, 1)) {
    flogf(" | fails at %03.1f m", antDepth());
    return drop_pha;
  }
  // success
  return call_pha;
}

//
// rise up to targetD, 0 means surfacing 
// when surfacing, expect stopCmd and don't set velocity 
// sets: boy.lastRise .firstRise, (*msg) 
// returns: bool
//
bool riseUp(float targetD, int retry, int delay) {
  float depth, startD, dangerZone;
  int step = 1;
  time_t riseT;
  DBG0("riseUp(%d)", targetD)
  startD = depth = antDepth();
  dangerZone = 5; // ?? sbe16toTip + (velo*stopTime) + 1.0
  // step 1: riseCmd
  ngkSend( riseCmd_msg );
  while (step==1) {
    depth = antDepth();
    switch (msg = ngkRecv()) {
    case null_msg: break;
    case riseRsp_msg:
      // start velocity measure
      riseT = time(0);
      step = 2;
      break;
    case stopCmd_msg:     // stopped by winch
      // ngkSend(stopRsp_msg) in ngkRecv
      flogf(", ERR stopped by winch at %03.1f", depth);
      return false;
    case timeout_msg:
      if (startD-depth > 1.0) {
        // odd, we are rising but no response; log but ignore
        flogf("\n\t|risePhase() timeout on ngk, but rising so continue..."); 
        step = 2;
        break;
      }
      // not rising from dock. log, delay, retry, abort
      if (--retry<0) { // too many tries
        flogf(", ERR abort"); 
        return false;
      } else { 
        flogf(", retry"); 
        msdelay(delay*1000);
        // retry
        ngkSend( riseCmd_msg );
      }
      break;
    default: // unexpected msg
      flogf("\n\t|riseP unexpected %s at %03.1f m", ngkMsgName(msg), depth);
    } // switch
    if (depth<=targetD)           // this shouldn't happen in step 1
      step = 3;
  } // while step1
  // step 2: depth
  while (step==2) {
    depth = antDepth();
    if (depth<=targetD) 
      step = 3;
    if (msg = ngkRecv())          // this shouldn't happen in step 2
      flogf("\n\t|riseP unexpected %s at %03.1f m", ngkMsgName(msg), depth);
  } // while step2
  // step 3: stopCmd 
  ngkSend( stopCmd_msg );
  while (step==3) {
    depth = antDepth();
    if (depth<dangerZone)
      ngkSend( dropCmd_msg );
    switch (msg = ngkRecv()) {
    case null_msg: break;
    case stopRsp_msg:
      return true;
    case dropRsp_msg:
      return false;
    case stopCmd_msg:     // stopped by winch
      // ngkSend(stopRsp_msg) in ngkRecv
      // ?? surf check?
      flogf(", ERR stopped by winch at %03.1f", depth);
      return false;
    case timeout_msg:
      if (startD-depth > 1.0) {
        // odd, we are stopped but no response; log but ignore
        flogf("\n\t|risePhase() timeout on ngk, but rising so continue..."); 
        step = 2;
        break;
      }
      // not rising from dock. log, delay, retry, abort
      if (--retry<0) { // too many tries
        flogf(", ERR abort"); 
        return false;
      } else { 
        flogf(", retry"); 
        msdelay(delay*1000);
        // retry
        ngkSend( riseCmd_msg );
      }
      break;
    default: // unexpected msg
      flogf("\n\t|riseP unexpected %s at %03.1f m", ngkMsgName(msg), depth);
    } // switch
    // this shouldn't happen
    if ((depth = antDepth())<=targetD) 
      step = 3;
  } // while step1
  return true;
} // riseUp()

  boy.lastRiseV = (startD-depth) / (time(0)-riseT);
  if (boy.firstRiseV==0)
    boy.firstRiseV = boy.lastRiseV;
  }
  // 
  // go to surface. same loop but stop cmd expected, ice check ??
  // 
  while (!antSurf()) {
    switch (ngkRecv(&r)) {
    case null_msg: break;
    case riseRsp_msg: // rise ack
      tmrStop(ngk_tmr);
      // start velocity measure
      riseT = time(0);
      startD = antDepth();
      break;
    case dropRsp_msg: // unexpected
      flogf("\nERR\t|risePhase() ngk unexpected drop at %03.1f m", antDepth());
      boy.phase = drop_pha;      // go down, try again tomorrow
      return;
    case stopRsp_msg: // slack auto-stop
      continue;
    case timeRsp_msg: // timeout
      sysAlarm(ngkTimeout_alm);
      amodem.timeout[riseCmd_msg] += 1;
      if (startD-antDepth() < 3) {
        // not rising from dock. log, reset, retry 5 times or abort
        if (retry++ < 5) { // retry
          flogf("\n\t|risePhase() timeout on ngk, retry rise cmd %d", retry);
          riseT = time(0);
          ngkSend( riseCmd_msg );
        } else { // abort
          flogf("\nERR\t|risePhase() timeout on ngk, %d times, abort", retry);
          boy.phase = drop_pha;
          return;
        }
      } else { // depth
        // odd, we are rising; log but ignore
        flogf("\n\t|risePhase() timeout on ngk, but rising so continue...");
      } // depth
      break;
    } // switch
  } // while depth>surfD
  //
  // ant mod surfaced, floats may be below still
  // antSurfOp() expects stop within some secs
  // start warming gps, we don't need depth until dropP
  // were files transfered at end of dataPhase ??
} // risePhase

//
// turn off sbe, on irid/gps (takes 30 sec). 
// read gps date, loc. 
//
void callPhase(void) {
} // callPhase

//
// antMod(stop), science(log), startT, dropCmd, science(stop)
// err if dockD differs
//
void dropPhase(void) {
} // dropPhase


//
// wait currChkSettle, buoy ctd, ant td, compute
// uses: .boy2ant
//
float oceanCurr() {
  float aD, cD, a, b, c;
  // usually called while antMod is on
  antMode(idle_mod);
  mpcDevSwitch(ctd_dev);
  cD=ctdDepth();
  mpcDevSwitch(ant_dev);
  aD=antDepth();
  // pythagoras a^2 + b^2 = c^2
  a=cD-aD;
  c=boy.boy2ant;
  b=sqrt(pow(c,2)-pow(a,2));
  return b;
}

//
// uses: boy.sidewaysMax
//
bool oceanCurrChk() {
  flogf("\n\t| ocean current ");
  sideways = oceanCurr();
  flogf(" @%.1f=%.1f ", antDepth(), sideways);
  if (sideways>boy.sidewaysMax) {
    flogf("too strong, cancel ascent");
    return true;
  }
  return false;
}

//
// shutdown buoy, reflects boyInit
//
void boyStop(void) {}

void flushBuffers(void) {}

void deployPhase(void) {}
