// sbe39.c
#include <main.h>

extern S39Info s39;

void main(void){
  char c;
  sysInit();
  mpcInit();
  antInit();
  s39Init();
  s39Start();
  s39Sample();
  s39DataWait();
  // s39Read();
  flogf("s39Depth %2.1f", s39Depth());
  if (!s39Prompt())
    flogf("s39Prompt fail\n");
  flogf("\nPress Q to exit, type to sbe39\n");
  while (true) {
    if (cgetq()) {
      c=cgetc();
      if (c=='Q') break;
      TUTxPutByte(s39.port,c,false);
    }
    if (TURxQueuedCount(s39.port)) {
      c=TURxGetByte(s39.port,false);
      cputc(c);
    }
  }
  s39Stop();
}
