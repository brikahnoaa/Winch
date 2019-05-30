// dbg.c
#include <main.h>

DbgInfo dbg;        // global

///
// the DBG* global vars are used in macros
// #ifdef DEBUG0 
// #define DBG0 if (dbg0) flogf(args);
// DBG0( "is printed if both #define DEBUG0  and  c:> set DBG0=1   (!=999)" );
// sets: dbg.dbg0, .dbg1, .dbg2 .dbg3 (global vars)
void dbgInit(void) {
  if (atoi(VEEFetchStr("DBG0", "999"))!=999) dbg.dbg0 = true;
  else dbg.dbg0 = false;
  if (atoi(VEEFetchStr("DBG1", "999"))!=999) dbg.dbg1 = true;
  else dbg.dbg1 = false;
  if (atoi(VEEFetchStr("DBG2", "999"))!=999) dbg.dbg2 = true;
  else dbg.dbg2 = false;
  if (atoi(VEEFetchStr("DBG3", "999"))!=999) dbg.dbg3 = true;
  else dbg.dbg3 = false;
  if (atoi(VEEFetchStr("DBG4", "999"))!=999) dbg.dbg4 = true;
  else dbg.dbg4 = false;
  if (atoi(VEEFetchStr("DBGX", "999"))!=999) dbg.dbgx = true;
  else dbg.dbgx = false;
}

///
// turn dbg level on/off
void dbgx(bool on) {
  dbg.dbgx = on;
}

void dbg0(bool on) {
  dbg.dbg0 = on;
}

void dbg1(bool on) {
  dbg.dbg1 = on;
}

void dbg2(bool on) {
  dbg.dbg2 = on;
}

void dbg3(bool on) {
  dbg.dbg3 = on;
}

void dbgLevel(int i) {
  if (i>=0) dbg.dbg0 = true;
  if (i>=1) dbg.dbg1 = true;
  if (i>=2) dbg.dbg2 = true;
  if (i>=3) dbg.dbg3 = true;
}
