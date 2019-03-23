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
  // CC = toupper(cc);
  printf("\nctd functions\n");
  printf("q:quit ?:info "
      "d:data w:dataWait p:prompt r:read i:init s:sample o:start x:stop"
      "\n");
  while (c != 'q') {
    if (cgetq()) {
      c=cgetc();
      cputc(c);
      cputc(' ');
      switch (c) {
      case '?':
        printf("ctd.on=%d .log=%d .auton=%d .depth=%3.1f .temp=%3.1f\n",
            ctd.on, ctd.log, ctd.auton, ctd.depth, ctd.temp);
      case 'd':
        b=ctdData();
        printf("ctdData %s\n", b?"true":"false");
        break;
      case 'w':
        b=ctdDataWait();
        printf("ctdDataWait %s\n", b?"true":"false");
        break;
      case 'p':
        b=ctdPrompt();
        printf("ctdPrompt %s\n", b?"true":"false");
        break;
      case 'r':
        b=ctdRead();
        printf("ctdRead %s\n", b?"true":"false");
        break;
      case 'i':
        ctdInit();
        printf("ctdInit ok\n");
        break;
      case 's':
        ctdSample();
        printf("ctdSample ok\n");
        break;
      case 'o':
        ctdStart();
        printf("ctdStart ok\n");
        break;
      case 'x':
        ctdStop();
        printf("ctdStop ok\n");
        break;
      }
    }
  }
  }
}
