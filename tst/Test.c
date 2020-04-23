// wspCmd.c
#include <test.h>

// extern IriInfo iri;
// extern IriData irid;
// extern BoyInfo boy;
extern WspInfo wsp;

void main(void){
  // Serial port;
  // char c;
  char *buff;
  int i, j, r;
  int blk, len, cnt;
  blk = 4; len = 1024; cnt = 1;
  sysInit();
  mpcInit();
  wspInit();
  // get cmd first
  CIOgets(all.str, 80);
  //
  wspStart();
  //
}
