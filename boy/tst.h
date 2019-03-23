// tst.h - generic test stub
#ifndef H_TST
#define H_TST

typedef struct TstInfo {
  bool fastData;          //
  bool noData;            // for test (false)
  bool noDeploy;          // for test (false)
  bool noIrid;            // for test (false)
  bool noRise;            // for test (false)
  bool test;              // are we in test mode (false)
  int fastRise;           // Rise after fD minutes, ignore riseH (120)
  int t1;                 // test param (0)
  int t2;                 // test param (0)
  int t3;                 // test param (0)
  void (*funcPtr)(void);  // test program run by utlX 't'
} TstInfo;

extern TstInfo tst;

#endif
