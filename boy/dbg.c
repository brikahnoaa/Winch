// dbg.c
#include <main.h>

DbgInfo dbg;        // global

///
// the DBG* global vars are used in macros
// #ifdef DEBUG0 
// #define DBG0 if (dbg0) flogf(args);
// DBG0( "is printed if both #define DEBUG0  and  c:> set DBG0=1   (!=999)" );
void dbgInit(void) {
  if (atoi(VEEFetchStr("DBG0", "999"))!=999) { dbg0(true); }
  if (atoi(VEEFetchStr("DBG1", "999"))!=999) { dbg1(true); }
  if (atoi(VEEFetchStr("DBG2", "999"))!=999) { dbg2(true); }
  // echo serial lines in/out
  if (atoi(VEEFetchStr("DBG3", "999"))!=999) { dbg3(true); }
  // special test case
  if (atoi(VEEFetchStr("DBG4", "999"))!=999) { dbg4(true); }
  // execute block of code, not just print
  if (atoi(VEEFetchStr("DBGX", "999"))!=999) { dbgx(true); }
}

///
// turn dbg level on/off
// sets: dbg.dbg0 .dbg1 .dbg2 .dbg3 .dbg4 .dbgx
void dbgx(bool on) { flogf(" DBGX"); dbg.dbgx = on; }

void dbg0(bool on) { flogf(" DBG0"); dbg.dbg0 = on; }

void dbg1(bool on) { flogf(" DBG1"); dbg.dbg1 = on; }

void dbg2(bool on) { flogf(" DBG2"); dbg.dbg2 = on; }

void dbg3(bool on) { flogf(" DBG3"); dbg.dbg3 = on; }

void dbg4(bool on) { flogf(" DBG4"); dbg.dbg4 = on; }

void dbgLevel(int i) {
  if (i>=0) dbg0(true);
  if (i>=1) dbg1(true);
  if (i>=2) dbg2(true);
  if (i>=3) dbg3(true);
  if (i>=4) dbg4(true);
}
