// files.c
#include <utl.h>
#include <mpc.h>
#include <sys.h>

#define MAX 12
int fd[MAX];

int first();
int last();
int count();

int first() {
  int i=0;
  for (i=0; i<MAX; i++)
    if (fd[i]) return i;
  return -1;
}
    
int last() {
  int i=0;
  for (i=MAX; i>=0; i--)
    if (fd[i]) return i;
  return -1;
}

int count() {
  int i=0;
  int j=0;
  for (i=0; i<MAX; i++)
    if (fd[i]) j++;
  return j;
}


void main(void){
  char s[]="t1t";
  char c;
  int f=0;
  sysInit();
  mpcInit();

  printf("q to exit, c=count, f=first close, l=last close, o=open\n");
  while (true) {
    if (cgetq()) {
      c=cgetc();
      printf("\n %c: ", c);
      if (c=='Q') break;
      if (c=='q') break;
      if (c=='c') {
        printf("%d open files\n", count());
        continue;
      }
      if (c=='f') {
        f=first();
        printf(" close %d ", f);
        utlLogClose(&fd[f]);
        continue;
      }
      if (c=='l') {
        f=last();
        printf(" close %d ", f);
        utlLogClose(&fd[f]);
        continue;
      }
      if (c=='o') {
        all.cycle++;
        f=last()+1;
        printf(" open %d ", f);
        utlLogOpen(&fd[f], s);
        continue;
      }
    }
  }
}
