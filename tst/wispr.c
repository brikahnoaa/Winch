// wispr.c
#include <main.h>

void main(void){
  char c;
  Serial port;

  sysInit();
  mpcInit();
  wspInit();
  port = mpcPamPort();
  flogf("\n%s\n", utlDateTime());
  flogf("talk to the wispr com0, use with com1@115200, [tab]=exit\n");
  while (true) 
  {
    if (cgetq()) 
    {
      c=cgetc();
      if (c=='\t') 
        exit();
      utlWriteBlock(port, &c, 1);
    }
    if (utlRead(port, all.buf))
      cprintf("%s", all.buf);
  }
}
