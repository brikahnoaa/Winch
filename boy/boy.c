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
    flushBuffers();
    boy.phaseT=time(0);
    switch (boy.phase) {
    case data_pha: // data collect by WISPR
      dataPhase();
      break;
    case rise_pha: // Ascend buoy, check for current and ice
      risePhase();
      break;
    case call_pha: // Call home via Satellite
      callPhase();
      break;
    case drop_pha: // Descend buoy, science sampling
      dropPhase();
      break;
    case deploy_pha:
      deployPhase();
      break;
    case reboot_pha:
      rebootPhase();
      break;
    case error_pha:
      errorPhase();
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
// sets: boy.phaseStartT 
// uses: data_tmr duty_tmr
//
void dataPhase(void) {
  flogf("\n\t|dataPhase()");
  // sleep needs a lot of optimizing to be worth the trouble
  // Sleep();
  wspStop();
} // dataPhase

//
// collect and organize data files.
// turn on ant, free space check, transfer files from buoy to antmod
// ascend. check angle due to current, up midway, re-check angle, surface.
// sets: boy.alarm[]
//
void risePhase(void) {
  float depth, startD, sideways, targetD, velocity;
  int retry = 0;
  time_t riseT;
  MsgType rsp;
  flogf("\n\t|risePhase()");
  //
  transferFiles();
  antInit();
  startD = depth = antDepth();
  // if current is too strong
  if (oceanCurrChk()) {
    sysAlarm(bottomCurr_alm);
    boy.phase = data_pha;
    return;
  }
  targetD = boy.currChkD;
  riseT = time(0);
  ngkSend( riseCmd_msg );
  while ((depth = antDepth()) > targetD) {
    // start rise (or retry if ngk timeout)
    // ngk: "going up" or "stopped"
    ngkRecv(&rsp);
    switch (rsp) {
    case null_msg: break;
    case riseRsp_msg:     // rise ack
      tmrStop(winch_tmr);
      // start velocity measure
      riseT = time(0);
      startD = antDepth();
      break;
    case stopCmd_msg:     // stopped by winch
    default: // unexpected msg
      flogf("\nERR\t|risePhase() ngk unexpected drop at %03.1f m", depth);
      boy.phase = drop_pha;      // go down, try again tomorrow
      return;
    } // switch
    if (ngkTimeout()) {
      if (startD-depth < 3) {
        // not rising from dock. log, reset, retry 5 times, abort
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
    } // switch
  } // while (depth>midway)
  // algor: midway. figure velocity, stop
  boy.lastRiseV = (startD-depth) / (time(0)-riseT);
  if (boy.firstRiseV==0)
    boy.firstRiseV = boy.lastRiseV;
  ngkSend( stopCmd_msg );
  // algor: current check. rise to surface, checking response
  if (oceanCurrChk()) {
    sysAlarm(midwayCurr_alm);
    boy.phase = drop_pha;
    return;
  }
  // 
  // go to surface. same loop but stop cmd expected
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
