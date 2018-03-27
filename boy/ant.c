// ant.c - for working with antenna module
#include <com.h>
#include <ant.h>
#include <mpc.h>

#define EOL "\n"

AntInfo ant;

///
// turn on antenna module, wait until ant responds
void antInit(void) {
  DBG0("antInit()")
  ant.port = mpcCom1();
  PIOSet(ANT_PWR);
  // get cf2 startup message ??
  ant.on = true;
} // antInit

///
// get depth from antmod
// sets: ant.depth .temp
// returns: depth
float antDepth(void) {
  char in[128];
  utlWrite(ant.port, "td", EOL);
  utlReadWait(ant.port, in, 9);
  ant.depth2 = ant.depth;
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
// rets: - | + | 0
float antMoving(void) {
  char in[128];
  utlWrite(ant.port, "moving", EOL);
  utlReadWait(ant.port, in, 9);
  return atof(in);
} // antMoving

///
// turn off power to antmod 
void antStop() {
  PIOClear(ANT_PWR);
  ant.on = false;
} // antStop

