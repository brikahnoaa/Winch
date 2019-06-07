#include <stdio.h>

void main(void) {
  char a[16];
  char c; short s; int i; long l; double d; void *p;
  printf("\nsizes: array %d, char %d, short %d, "
    "int %d, long %d, double %d, ptr %d \n",
    sizeof(a), sizeof(c), sizeof(s), 
    sizeof(i), sizeof(l), sizeof(d), sizeof(p));
  return;
}
