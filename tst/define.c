#define DBG(...) \
#include <main.h>
#ifdef DEBUG \
__VA_ARGS__
#endif

int main(void){
  DBG(printf("this is %s good\n", "darn");)
#define DEBUG
  DBG(printf("this is %s good\n", "damn");)
}
