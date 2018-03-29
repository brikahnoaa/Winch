// com.c
//
// conventions:
// three letter object types: ant, boy, ctd, etc
// xxxInit xxxOpen xxxClose: Init is for one time actions, Open/Close repeat
// avoid globals in general. Prefer to pass &var when practical to void func().
//
// pseudomods: like modules in C++
// Each xxx.[ch] file pair is a module, with modInit() and modStop()
// Each mod.h defines struct ModInfo mod, declared in mod.c and used locally,
//  and only accessed extern by rule-breaking cfg.c (settings)

#include <utl.h>

#include <ant.h>
#include <boy.h>
#include <ctd.h>
#include <mpc.h>
#include <ngk.h>
#include <pwr.h>
#include <sys.h>
#include <wsp.h>

///
// initHW and SW structures. call boyMain()
void main(void) {
  int starts;
  starts = sysInit();
  mpcInit();
  antInit();
  boyInit();
  ctdInit();
  ngkInit();
  pwrInit();
  boyMain(starts);
} // main
