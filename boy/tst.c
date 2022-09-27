// tst.c - generic test stub
//
// entering t during utlX bridges to tstLoop()
#include <utl.h>
#include <tst.h>

TstInfo tst;          // global

void tstFunc(void) {
  (*tst.funcPtr)();
} // tstFunc
