// main.c
#include <main.h>

// this file is not used by Winch/tst

void main(void) {
  all.starts = sysInit();
  mpcInit();
  pwrInit();
  antInit();
  boyInit();
  iriInit();
  ngkInit();
  s16Init();
  s39Init();
  wspInit();

  boyMain();
}

