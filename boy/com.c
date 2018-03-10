// com.c

//
// conventions:
// three letter object types: ant, boy, ctd, etc
// xxxInit xxxOpen xxxClose: Init is for one time actions, Open/Close repeat
// avoid globals in general. Prefer to pass &var when practical to void func().
// except for use of global object struct, which is noted on first line, e.g.
// // global boy .port .device
//


///
// the DBG? global vars are used indirectly by 
// #ifdef DEBUG0 
// #define DBG0 if (dbg0) flogf(args);
// DBG0( "is printed if both #define DEBUG0  and  c:> set DBG0=1   (!=999)" )
// sets: DBG0, DBG1, DBG2 (global vars)
extern bool dbg0 = dbg1 = dbg2 = false;
void comInit(void) {
#ifdef DEBUG0
  if (atoi(VEEFetchStr("DBG0", "999"))!=999) dbg0 = true;
#endif
#ifdef DEBUG1
  if (atoi(VEEFetchStr("DBG1", "999"))!=999) dbg1 = true;
#endif
#ifdef DEBUG2
  if (atoi(VEEFetchStr("DBG2", "999"))!=999) dbg2 = true;
#endif
}
