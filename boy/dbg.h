// dbg.h  common to all
#define DBG_H

typedef struct DbgInfo {
  bool dbg0;
  bool dbg1;
  bool dbg2;
} DbgInfo;

extern DbgInfo dbg;

///
// the DBG* global vars are used in macros
// #ifdef DEBUG0
// #define DBG0 if (dbg0) flogf(args);
// DBG0( "is printed if both #define DEBUG0  and  c:> set DBG0=1   (!=999)" )
//
// DBG() blocks of code, as is
// DBG0() print func names
// DBG1() print more interesting things
// DBG2() print details

#define DEBUG
#define DEBUG0
#define DEBUG1
#define DEBUG2

#ifdef DEBUG
#define DBG(...) __VA_ARGS__
#else
#define DBG(...)
#endif

#ifdef DEBUG0
#define DBG0(...) if (dbg.dbg0) flogf("\n" __VA_ARGS__);
#else
#define DBG0(...)
#endif

#ifdef DEBUG1
#define DBG1(...) if (dbg.dbg1) flogf(" " __VA_ARGS__);
#else
#define DBG1(...)
#endif

#ifdef DEBUG2
#define DBG2(...) if (dbg.dbg2) flogf(" " __VA_ARGS__);
#else
#define DBG2(...)
#endif

void dbgInit(void);
