// com.c
#include<com.h>

//
// conventions:
// three letter object types: ant, boy, ctd, etc
// xxxInit xxxOpen xxxClose: Init is for one time actions, Open/Close repeat
// avoid globals in general. Prefer to pass &var when practical to void func().
// except for use of global object struct, which is noted on first line, e.g.
// // global boy .port .device
//

///
// the DBG* global vars are used in macros
// #ifdef DEBUG0 
// #define DBG0 if (dbg0) flogf(args);
// DBG0( "is printed if both #define DEBUG0  and  c:> set DBG0=1   (!=999)" )
// sets: com.dbg0, .dbg1, .dbg2 (global vars)
void comInit(void) {
  if (atoi(VEEFetchStr("DBG0", "999"))!=999) com.dbg0 = true;
  else com.dbg0 = false;
  if (atoi(VEEFetchStr("DBG1", "999"))!=999) com.dbg1 = true;
  else com.dbg1 = false;
  if (atoi(VEEFetchStr("DBG2", "999"))!=999) com.dbg2 = true;
  else com.dbg2 = false;
}
