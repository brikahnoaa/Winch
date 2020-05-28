// wspCmd.c
#include <test.h>


void main(void){
  int r;
  sysInit();
  mpcInit();
  pwrInit();
  wspInit();
  while (!(r=CIOgets(all.str, 100))) { // wait for input
    if (all.str[0]=='q')
      exit(0);
  //
  cprintf("\n wspr command: '%s'\n", all.str);
  wspStart();
  wspOpen(); // ready to accept command
  utlWrite(wsp.port, all.str, EOL);
  r=utlReadWait(wsp.port, all.buf, 44);
  cprintf("\n%s\n", utlNonPrintBlock(all.buf, r));
}
