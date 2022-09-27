// pins.c
#include <utl.h>
void main(void){
  uchar unusedpins[] = {15, 16, 17, 18, 19, 36, 0};
  uchar outputpins[] = {22, 23, 24, 25, 26, 27, 28, 29, 30, 0};
  PIOMirrorList(unusedpins);
  PIOMirrorList(outputpins);
}
