// cfg.c
#include <main.h>
extern AntInfo ant;

void main(void){
  sysInit();
  printf("cfg.c");
  printf("\nant.initStr='%s'", ant.initStr);
  cfgString("ant.initStr=abc");
  printf("\nant.initStr='%s'", ant.initStr);
  cfgString("ant.initStr=abc def");
  printf("\nant.initStr='%s'", ant.initStr);
  cfgString("ant.initStr=ant.oant and how");
  printf("\nant.initStr='%s'", ant.initStr);
}
