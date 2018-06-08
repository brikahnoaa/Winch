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

BoyInfo boy;

///
// deploy or reboot, then loop over phases data/rise/irid/fall
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
    flogf("\n!boyMain(): cycle %d of %d\n", cycle, boy.cycleMax);
    if (boy.cycleMax && (cycle >= boy.cycleMax))
      sysStop("cycleLimit");
    sysFlush();                    // flush all log file buffers
    boy.phaseT = time(0);
    switch (boy.phase) {
    case data_pha: // data collect by WISPR
      phaseNext = dataPhase();
      cycle++;
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
  return fall_pha;
} // reboot()

///
// wispr recording and detecting, buoy is docked to ngk
// data is gathered for about 24hours (data_tmr)
// wsp powers down for % of each hour (wispr_tmr)
// organize data files, transfer data to antmod ??
// sleep needs a lot of optimizing to be worth the trouble
// uses: data_tmr duty_tmr
PhaseType dataPhase(void) {
  int detect;
  flogf("\n+dataPhase()@%s", utlDateTime());
  if (!boy.useWsp) {
    utlNap(10);
    return rise_pha;
  }
  wspStart(wsp2_pam);
  wspDetect(&detect);
  flogf("\ndataPhase detections: %d", detect);
  wspStorm(utlBuf);
  flogf("\nstorm: %s", utlBuf);
  wspStop();
  return rise_pha;
} // dataPhase

///
// ascend. check angle due to current, up midway, re-check angle, surface.
// sets: boy.alarm[]
PhaseType risePhase(void) {
  bool success;
  flogf("\n+risePhase()@%s", utlDateTime());
  antStart();
  ctdStart();
  ngkStart();
  // if current is too strong at bottom
  if (oceanCurrChk()) {
    sysAlarm(bottomCurr_alm);
    //?? return fall_pha;
  }
  success = rise(boy.currChkD, 0);
  if (success>0) {
    flogf("\n\t| rise fails at %3.1f m", antDepth());
    //??  return fall_pha;
  }
  // if current is too strong at midway
  if (oceanCurrChk()) {
    sysAlarm(midwayCurr_alm);
    //?? return fall_pha;
  }
  // surface, 1 meter below float level
  success = rise(antSurfD()+1, 0);
  if (success>0) {
    flogf(" | fails at %3.1f m", antDepth());
    //?? return fall_pha;
  }
  // success
  return irid_pha;
} // risePhase


///
// rise to targetD // possible stopCmd at surface
// ?? compute riseRate
// uses: .riseRate .riseOrig .rateAccu .riseRetry
// sets: .riseRate
int rise(float targetD, int try) {
  bool twentyB=false, stopB=false, errB=false;
  float nowD, startD, lastD, velo;
  int i, est;        // estimated operation time
  MsgType msg;
  enum {targetT, ngkT, twentyT, fiveT};  // local timer names
  DBG0("rise(%3.1f)", targetD)
  ngkFlush();
  nowD = startD = antDepth();
  if (startD < targetD) return 1;
  if (try > boy.riseRetry) return 2;
  // .riseOrig=as tested, .riseRate=seen, .rateAccu=fudgeFactor
  est = (int) (((nowD-targetD) / boy.riseRate) * boy.rateAccu);
  tmrStart(targetT, est);
  tmrStart(ngkT, boy.ngkDelay*2);
  tmrStart(twentyT, 20);
  tmrStart(fiveT, 5);
  ngkSend(riseCmd_msg);
  flogf("\nrise()\t| riseCmd to winch at %s", utlTime());
  while (!stopB && !errB) {       // redundant, loop exits by break;
    utlX();
    // check: target, winch, 20s, 5s
    nowD = antDepth();
    // arrived?
    if (nowD<targetD) {
      flogf("\nrise()\t| reached targetD %3.1f at %s", nowD, utlTime());
      tmrStop(targetT);
      stopB = true;
      break;
    }
    // op timeout - longer than estimated time * 1.5 (rateAccu)
    if (tmrExp(targetT)) {
      flogf("\nrise()\t| ERR \t| rise timeout %ds @ %3.1f, stop", est, nowD);
      errB = true;
      break;
    }
    // winch
    if (ngkRecv(&msg)!=null_msg) {
      flogf(", %s from winch", ngkMsgName(msg));
      if (msg==stopCmd_msg)
        return -1;
      if (msg==riseRsp_msg)
        tmrStop(ngkT);
    }
    if (tmrExp(ngkT)) {
      flogf("\nrise()\t| no response from winch");
    }
    // 20 seconds
    if (tmrExp(twentyT)) {
      if (startD-nowD < 1.5) {
        // by now we should have moved up 5 meters
        flogf("\nrise()\t| ERR \t| depth %3.1f after 20 seconds", nowD);
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
      flogf("\nrise()\t| 5s: depth=%3.1f", nowD);
      if (antVelo(&velo)) 
        flogf(" velo=%4.2f", velo);
      if (twentyB) {
        if (lastD<=nowD) {
          flogf("\nrise()\t| ERR \t| not rising, %3.1f<=%3.1f", lastD, nowD);
          lastD = nowD;
        }
      }
    } 
  } // while !stop
  // stop - either normal or due to err
  for (i=0; i<boy.riseRetry; i++) {
    ngkSend(stopCmd_msg);
    msg = ngkRecvWait(&msg, boy.ngkDelay*2+2);
    if (msg==stopRsp_msg || msg==stopCmd_msg) break;
  }
  // ?? if (msg!=stopRsp_msg) damnation
  flogf("\nrise() \t| stopCmd-->%s", ngkMsgName(msg));
  // retry if error
  if (errB) {
    flogf(", retry %d", ++try);
    return rise(targetD, try);
  } else { 
    // normal stop
    return 0;
  }
} // rise

///
// ??
// turn off sbe, on irid/gps (takes 30 sec). 
// read gps date, loc. 
PhaseType iridPhase(void) {
  flogf("\n+iridPhase()@%s", utlDateTime());
  if (!boy.useGps) {
    utlNap(10);
    return fall_pha;
  }
  gpsStart();
  gpsStats();
  iridSig();
  gpsStop();
  return fall_pha;
} // iridPhase

///
PhaseType fallPhase() {
  fall(0);
  return data_pha;
} // fallPhase

///
// based on rise(), diffs commented out; wait for winch stop
// fall to dock // expect stopCmd 
// uses: .riseRate .riseOrig .rateAccu .riseRetry
// sets: .riseRate
int fall(int try) {
  bool fortyB=false, stopB=false, errB=false;
  float nowD, startD, lastD, velo;
  // int i;
  int est;        // estimated operation time
  MsgType msg;
  enum {targetT, ngkT, fortyT, fiveT};  // local timer names
  DBG0("rise()")
  // DBG0("rise(%3.1f)", targetD)
  ngkFlush();
  nowD = startD = antDepth();
  // if (startD < targetD) return 1;
  if (nowD > boy.dockD-2) return 1;
  if (try > boy.fallRetry) return 2;
  // crude, could have cable played out
  est = (int) ((boy.dockD/boy.fallRate)*boy.rateAccu);
  tmrStart(targetT, est);
  tmrStart(ngkT, boy.ngkDelay*2);
  tmrStart(fortyT, 40);
  tmrStart(fiveT, 5);
  ngkSend(fallCmd_msg);
  flogf("\nfall()\t| fallCmd to winch at %s", utlTime());
  while (!stopB && !errB) {       // redundant, loop exits by break;
    utlX();
    // check: target, winch, 20s, 3s
    nowD = antDepth();
    // arrived?
    // if (nowD<targetD) {
      // wait for winch to stop
      // tmrStop(targetT);
      // stopB = true;
      // break;
    // }
    // op timeout - longer than estimated time * 1.5 (rateAccu)
    if (tmrExp(targetT)) {
      flogf("\nfall()\t| ERR \t| fall timeout %ds @ %3.1f, stop", est, nowD);
      errB = true;
      break;
    }
    // winch
    if (ngkRecv(&msg)!=null_msg) {
      flogf(", %s from winch", ngkMsgName(msg));
      if (msg==stopCmd_msg)
        // return -1;
        return 0;
      if (msg==fallRsp_msg)
        tmrStop(ngkT);
    }
    if (tmrExp(ngkT)) {
      flogf("\nfall()\t| WARN no response from winch");
    }
    // 30 seconds
    if (tmrExp(fortyT)) {
      if (nowD-startD < 1.5) {
        // by now we should have moved down 5 meters or more
        flogf("\nfall()\t| ERR \t| depth %3.1f after 20 seconds", nowD);
        errB = true;
        break;
      } else {
        fortyB = true;
        lastD = nowD;
      }
    }
    // 5 seconds
    if (tmrExp(fiveT)) {
      tmrStart(fiveT, 5);
      if (nowD>boy.dockD-1) 
        flogf("\nfall()\t| reached boy.dockD %3.1f at %s", nowD, utlTime());
      flogf("\nfall()\t| 5s: depth=%3.1f", nowD);
      if (antVelo(&velo)) 
        flogf(" velo=%4.2f", velo);
      if (fortyB) {
        if (lastD>=nowD) {
          flogf("\nfall()\t| WARN not falling, %3.1f>=%3.1f", lastD, nowD);
          lastD = nowD;
        }
      }
    } 
  } // while !stop
  // fall just waits for dock
  // stop - either normal or due to err
  /*
  for (i=0; i<boy.fallRetry; i++) {
    ngkSend(stopCmd_msg);
    if (ngkRecvWait(&msg, boy.ngkDelay*2+2)==stopRsp_msg) break;
  }
  // ?? if (msg!=stopRsp_msg) damnation
  flogf(", stopCmd-->%s", ngkMsgName(msg));
  */
  // retry if error
  if (errB) {
    flogf("\nfall() \t| ERR retry %d", ++try);
    return fall(try);
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
  ngkStart();
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
  flogf("\n\t| %4.2fm>10, wait %dsec", depth, boy.settleT+15);
  lastD = depth;
  utlNap(boy.settleT);      // default 120sec
  utlNap(15);
  depth = antDepth();
  // at most 5 min to descend, already waited 2min
  tmrStart( deploy_tmr, 60*5 );
  // must fall at least 1m in 10 sec
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
  return rise_pha;
} // deployPhase

///
// ??
// cable is stuck. up/down tries??, down to dock. 
// go back to normal if resolved ??
PhaseType errorPhase(void) {
  flogf("\n+errorPhase()@%s", utlDateTime());
  return fall_pha;
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
  flogf("\noceanCurr() \t| aD=%3.1f cD=%3.1f boy2ant=%3.1f", aD, cD, c);
  if (a<0) {
    flogf("\noceanCurr() \t| ERR sbe16<sbe39");
    return -1.0;
  }
  if (c<a) {
    flogf("\noceanCurr() \t| ERR boy2ant<cD-aD");
    // ?? update boy2ant?
    return -1.0;
  }
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
  flogf(" current @ %.1f=%.1f ", antDepth(), sideways);
  if (sideways>boy.currMax) {
    flogf("too strong, cancel ascent");
    // ignore current when dbg ?? should be setting
    return false;
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
