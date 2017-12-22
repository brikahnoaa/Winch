#include <test.h>
#include <time.h>

int main(){
  time_t t=time(0);
  printf( "%ld, %s\n", t, ctime(&t));
}
