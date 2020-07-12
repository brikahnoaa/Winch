// sbe16.c
#include <main.h>

extern S16Info s16;

void main(void){
  char c;
  sysInit();
  mpcInit();
  s16Init();
  s16Start();
  s16Sample();
  s16DataWait();
  // s16Read();
  flogf("s16Depth %2.1f", s16Depth());
  if (!s16Prompt())
    flogf("s16Prompt fail\n");
  flogf("\nPress Q to exit, type to sbe16\n");
  while (true) {
    if (cgetq()) {
      c=cgetc();
      if (c=='Q') break;
      TUTxPutByte(s16.port,c,false);
    }
    if (TURxQueuedCount(s16.port)) {
      c=TURxGetByte(s16.port,false);
      cputc(c);
    }
  }
  s16Stop();
}
