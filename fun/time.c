#include <test.h>
#include <time.h>
#include <math.h>

int main(){
  float b=sqrt(pow(12,2)-pow(9,2));
  time_t t=time(0);
  printf( "%ld, %s\n", t, ctime(&t));
}
