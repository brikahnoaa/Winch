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
  antMode(idle_ant);
  PIOSet(ANT_PWR);
  // get cf2 startup "ok"
  tmrStart(init_tmr, 9);
  while (true) {
    utlRead(ant.port, utlBuf);
    if (strstr(utlBuf, "ok")) {
      tmrStop(init_tmr);
      continue; // while
    }
    if (tmrExp(init_tmr)) 
      utlStop("FATAL\t| antInit() startup fail");
  } // while
} // antInit

///
// get depth from antmod
// sets: ant.depth .temp .samples[]
// returns: depth
float antDepth(void) {
  int i;
  char in[128];
  if (ant.mode==auto_ant) {
    for (i=0; i<ant.sampleCnt; i++) 
      ant.samples[i+1] = ant.samples[i];
    ant.samples[0] = ant.depth;
  } else
    // get a sample
    utlWrite(ant.port, "td", EOL);
  // ant.depth2 = ant.depth;
  i = utlReadWait(ant.port, in, 9);
  if (i<5) {
    flogf( "\nERR\t| antDepth read fail" );
    // ?? is this fatal
    return 0.0;
  }
  // ?? range check
  ant.temp = atof(strtok(in, ", "));
  ant.depth = atof(strtok(NULL, ", "));
  return ant.depth;
} // antDepth

///
// get temp from antmod
// sets: ant.depth .temp
// returns: temp
float antTemp(void) {
  antDepth();
  return ant.temp;
} // antDepth

///
// turn autonomous on/off. idle_ant clears samples
AntModeType antMode(AntModeType mode) {
  int i;
  char *out;
  if (ant.mode==mode) return mode;
  DBG0("antMode(%d)", mode)
  switch (mode) {
  case auto_ant:
    out = "\n initlogging \n initlogging \n"
          "sampleInterval=0 \n txRealTime=y \n startnow \n";
    utlWriteLines(ant.port, out, EOL);
    // clear samples
    for (i=0; i<=ant.sampleCnt; i++) 
      ant.samples[i] = 0;
    break;
  case idle_ant:
    utlWrite(ant.port, "stop", EOL);
    break;
  } // switch
  ant.mode = mode;
  return ant.mode;
}

///
// if auto_ant, checks recent depth against previous
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
  antMode(idle_ant);
} // antStop

