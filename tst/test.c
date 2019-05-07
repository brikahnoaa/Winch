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
  for (i=MAX-1; i>=0; i--)
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

  for (f=0; f<MAX; f++) fd[f]=0;
  sysInit();
  // mpcInit();

  flogf("\n q to exit, c=count, f=first close, l=last close, o=open, a=all\n");
  while (true) {
    if (cgetq()) {
      c=cgetc();
      flogf("\n %c: ", c);
      if (c=='Q') break;
      if (c=='q') break;
      if (c=='c') {
        flogf("\n%d open files\n", count());
        continue;
      }
      if (c=='f') {
        f=first();
        flogf("\n close %d ", f);
        utlLogClose(&fd[f]);
        continue;
      }
      if (c=='l') {
        f=last();
        flogf("\n close %d ", f);
        utlLogClose(&fd[f]);
        continue;
      }
      if (c=='o') {
        all.cycle++;
        f=last()+1;
        if (f>MAX) {
          flogf("\n too many files #%d", f);
          continue;
        }
        utlLogOpen(&fd[f], s);
        flogf("\n open #%d fd=%d ", f, fd[f]);
        continue;
      }
      if (c=='a') {
        for (f=0; f<MAX; f++)
          if (fd[f]) 
            flogf("\n %d:fd=%d", f, fd[f]);
        continue;
      }
    }
  }
}
