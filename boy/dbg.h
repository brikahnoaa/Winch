// dbg.h  common to all
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

// PINS
#define MDM_PWR 21
#define ANT_PWR 22  // antenna module Power pin (1=ON, 0=OFF)
#define COM1SEL 23  // set = antMod cf2, clear = ctd sbe16 in buoy
#define WISPR3 24
#define WISPR4 25
#define HPSENS 26   // power for pressure sensor
#define WISPR1 29
#define WISPR2 30
#define COM1TX 31
#define COM1RX 32
#define MDM_RX 33
#define MDM_TX 35
#define WISPR_PWR_ON 37
#define WISPR_PWR_OFF 42
#define MDM_RX_TTL 48
#define MDM_TX_TTL 50

#define null NULL

void dbgInit(void);
