// dosfile.c
#include <main.h>

void main(void){
  char cmd[128];
  sprintf(cmd, "copy sys.log test\\sys%04d.log", 2);
  sysInit();
  execstr(cmd);
}
