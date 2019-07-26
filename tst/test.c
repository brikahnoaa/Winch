// s39.c
#include <main.h>

extern AntInfo ant;
extern S39Info s39;

void main(void){
  char c;
  sysInit();
  mpcInit();
  antInit();
  s39Init();
  antStart();
  s39Start();
  s39Sample();
  if (!s39DataWait())
    flogf("\ndata wait fail, no response from sbe39");
  if (!s39Read())
    flogf("\nread fails");
  flogf("\ns39Depth() -> %f", s39Depth());
  flogf("\ns39Temp() -> %f", s39Temp());
  s39DataWait();
  s39Read();
  flogf("\ns39Depth() -> %f", s39Depth());
  flogf("\ns39Temp() -> %f", s39Temp());
  // s39Auton(true);
  flogf("\n\nPress any to talk, Q to exit");
  flogf("\nconnected to s39");
  while (true) {
    if (cgetq()) {
      c=cgetc();
      if (c=='Q') break;
      TUTxPutByte(ant.port,c,false);
    }
    while (TURxQueuedCount(ant.port)) {
      c=TURxGetByte(ant.port,false);
      cputc(c);
    }
  }
  // s39Auton(false);
  // s39GetSamples();
  utlStop("\nnormal");
}
