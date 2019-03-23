// tst.c - generic test stub
//
// entering t during utlX bridges to tstLoop()
#include <utl.h>
#include <tst.h>

#include <ctd.h>

void tstLoop(char cc) {
  bool b;
  char c, CC;
  float f;
  int i;
  b=false; c=0; f=0.0; i=0;
  CC = toupper(cc);
  if (CC=='C') {
    printf("ctd functions");
    printf("q:quit ");
    printf("d:data w:dataWait p:prompt r:read i:init s:sample o:start x:stop");
    printf("\n");
    while (c != 'q') {
      if (cgetq()) {
        c=cgetc();
        switch (c) {
        case 'd':
          b=ctdData();
          printf(" %s\n", b?"true":"false");
          break;
        case 'w':
          b=ctdDataWait();
          printf(" %s\n", b?"true":"false");
          break;
        case 'p':
          b=ctdPrompt();
          printf(" %s\n", b?"true":"false");
          break;
        case 'r':
          b=ctdRead();
          printf(" %s\n", b?"true":"false");
          break;
        case 'i':
          ctdInit();
          printf(" ok\n");
          break;
        case 's':
          ctdSample();
          printf(" ok\n");
          break;
        case 'o':
          ctdStart();
          printf(" ok\n");
          break;
        case 'x':
          ctdStop();
          printf(" ok\n");
          break;
        }
      }
    }
  }
}
