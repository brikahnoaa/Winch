// dbg.h  common to all included by utl.h
#ifndef H_DBG
#define H_DBG

typedef struct DbgInfo {
  bool dbg0;
  bool dbg1;
  bool dbg2;
  bool dbg3;
  bool dbg4;
  bool dbgx;
  bool noData;            // for test (false)
  bool noDeploy;          // for test (false)
  bool noIrid;            // for test (false)
  bool noRise;            // for test (false)
  bool test;              // are we in test mode (false)
  int t1;                 // test param (0)
  int t2;                 // test param (0)
  int t3;                 // test param (0)
  void (*funcPtr)(void);  // test program run by utlX 'd' 
} DbgInfo;

extern DbgInfo dbg;

///
// the DBG* global vars are used in macros
// #ifdef DEBUG0
// #define DBG0 if (dbg0) flogf(args);
// DBG0( "is printed if both #define DEBUG0  and  c:> set DBG0=1   (!=999)" )
//
// DBG() autoprint func names
// DBG0() print with func names
// DBG1() print more interesting things
// DBG2() print details
// DBG3() serial i/o
// DBG4() special, little use
// DBGX() block of code, as is

#define DEBUG0
#define DEBUG1
#define DEBUG2
#define DEBUG3
#define DEBUG4
#define DEBUGX

#ifdef DEBUG0
#define DBG() if (dbg.dbg0) flogf(" %s()", self)
#else
#define DBG(...)
#endif

#ifdef DEBUG0
#define DBGN(D_FMT, ...) if (dbg.dbg0) flogf(" %s" D_FMT, self, __VA_ARGS__)
#else
#define DBGN(...)
#endif

#ifdef DEBUG0
#define DBG0(...) if (dbg.dbg0) flogf(" " __VA_ARGS__)
#else
#define DBG0(...)
#endif

#ifdef DEBUG1
#define DBG1(...) if (dbg.dbg1) flogf("+" __VA_ARGS__)
#else
#define DBG1(...)
#endif

#ifdef DEBUG2
#define DBG2(...) if (dbg.dbg2) flogf("_" __VA_ARGS__)
#else
#define DBG2(...)
#endif

#ifdef DEBUG3
#define DBG3(...) if (dbg.dbg3) flogf(" " __VA_ARGS__)
#else
#define DBG3(...)
#endif

#ifdef DEBUG4
#define DBG4(...) if (dbg.dbg4) flogf(" " __VA_ARGS__)
#else
#define DBG4(...)
#endif

#ifdef DEBUGX
#define DBGX(...) if (dbg.dbgx) {__VA_ARGS__}
#else
#define DBGX(...)
#endif

void dbgInit(void);
void dbgx(bool on);
void dbg0(bool on);
void dbg1(bool on);
void dbg2(bool on);
void dbg3(bool on);
void dbgLevel(int i);

#endif
