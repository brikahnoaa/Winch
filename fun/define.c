#include <common.h>
#define BUOYID "00"
#define WINCHID "01"
// 00 or 03? 00 slacks at surface, 03 brakes underwater
#define RISE_CMD "#R," WINCHID ",03"
#define SURF_CMD "#R," WINCHID ",00"
#define DROP_CMD "#F," WINCHID ",00"
#define STOP_CMD "#S," WINCHID ",00"

#define mac1(x) printf(x);
int main(void){
  char *str="#R,01,00\r\n";
  if (memcmp(str+2, "," WINCHID ",", 4)==0)
    printf( "compare worked in %s\n", str);
}
