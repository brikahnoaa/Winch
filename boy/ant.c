// ant.c - for working with antenna module
#include <utl.h>
#include <ant.h>
#include <mpc.h>
#include <tmr.h>

#define EOL "\n"

AntInfo ant;

///
// turn on antenna module, wait until ant responds
// sets: ant.mode .port
void antInit(void) {
  DBG0("antInit()")
  ant.port = mpcCom1();
  ant.pending = false;
  ant.auton = false;
  TURxFlush(ant.port);
  TUTxFlush(ant.port);
  PIOSet(ANT_PWR);
  // get cf2 startup "ok"
  if ( utlReadWait(ant.port, utlBuf, 9)==0 )
    utlStop("FATAL\t| antInit() startup fail");
  // sbe39
  if (!(antPrompt() || antPrompt()))   // fails twice
    utlStop("ERR\t| antInit(): no prompt from ant");
  utlWrite(ant.port, utlDateTimeBrief(), EOL);
  utlReadWait(ant.port, utlBuf, 1);
  utlWrite(ant.port, "DelayBeforeSampling=0", EOL);
  utlReadWait(ant.port, utlBuf, 1);
  // ?? what else should be set?
} // antInit

///
// data waiting
bool antData() {
  return TURxQueuedCount(ant.port);
} // antData

///
// request sample
void antSample(void) {
  if (ant.auton || ant.pending) return;
  // wakeup
  antPrompt();
  utlWrite(ant.port, "TS", EOL);
  len = utlReadWait(ant.port, utlBuf, 1);
  if (len<3)
    flogf("\nERR antSample, TS command fail");
  ant.pending = true;
  // pending response, checked by antRead()
  tmrStart( ant_tmr, ant.delay );
} // antSample

// get depth from antmod
// if (!data !pending) if (auto) return else sample
//  while (!data) check timeout
//  read
//
// sets: ant.depth .temp .samples[]
// returns: depth
void antRead(void) {
  int i;
  char *p;
  if (!pending && !antData()) 
    // no data here or expected from sbe39
    if (ant.auton) 
      // most common case, use the last ant.depth or ant.temp 
      return;
    else 
      // poke sbe, set pending 
      antSample();
  // pending, so wait for data
  while (!antData()) {
    // error if timeout
    if (tmrExp(ctd_tmr)) {
      flogf("\nERR\t| antRead(): tmr expired");
      return; // ?? reset lara
    }
  } // while !data
  // data ready
  utlRead(ant.port, utlBuf);
  // ?? sanity check
  if (ant.auton) {
    // shift samples in array
    for (i=0; i<ant.sampleCnt; i++) 
      ant.samples[i+1] = ant.samples[i];
    ant.samples[0] = ant.depth;
  } // if auton
  // ?? range check
  ant.temp = atof(strtok(utlBuf, "#, "));
  ant.depth = atof(strtok(NULL, ", "));
}

float antDepth(void) {
  antSample();
  antRead();
  return ant.depth;
} // antDepth

float antTemp(void) {
  antSample();
  antRead();
  return ant.temp;
} // antTemp

///
// turn autonomous on/off. idle_ant clears samples
void antAuto(bool auton) {
  int i;
  char *out;
  if (ant.auton==auton) return;
  DBG0("antAuto(%d)", mode)
  if (auton) {
    out = "\n initlogging \n initlogging \n"
          "sampleInterval=0 \n txRealTime=y \n startnow \n";
    utlWriteLines(ant.port, out, EOL);
    // clear samples
    for (i=0; i<=ant.sampleCnt; i++) 
      ant.samples[i] = 0;
    // ant.pending tells antRead() to wait for first sample
    ant.pending = true;
    // swallow header
    utlReadWait(ant.port, utlBuf, 5);
    if (!strstr(utlBuf, "Start logging"))
      flogf("\nERR\t| antAuto - didn't get 'Start logging' header");
  } else {
    utlWrite(ant.port, "stop", EOL);
    // swallow tail
    utlNap(2);
    TURxFlush(ant.port);
  } // if auton
  ant.auton = auton;
}

///
// if auton_ant, checks recent depth against previous
// returns delta change in depth (0 if not enough samples)
// uses: ant.sampleCnt .sampleRes .depth
// rets: - | + | 0.0
float antMoving(void) {
  float d;
  // got samples?
  d = ant.samples[ant.sampleCnt];
  if (d==0.0) 
    return 0.0;
  // got delta?
  d = ant.depth-d;
  if (abs(d)<ant.sampleRes) 
    return 0.0;
  // got depth?
  if (ant.depth>(ant.surfD-ant.sampleRes))
    return 0.0;
  // delta
  return d;
} // antMoving

///
// turn off power to antmod 
void antStop() {
  PIOClear(ANT_PWR);
} // antStop

