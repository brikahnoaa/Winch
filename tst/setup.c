// setup.c

#include <main.h>

/*
  When the program starts on deck, make the screen user friendly.
  Can Lauren run the program on the deck by herself?
  For example, include inquiries like these:
  a. Have you activated the winch amodem? y/n
  (it should not proceed unless the operator responds ‘y’)
  b. Do you want to run the CTD pump?
  Y means it is a real deployment.  N means this is a simulation.
 */
/*
 * talk to user about setup. preview of setup. clear and set env vars.
 *
 * .1 current vars  .2 winch amodem (push on)?  
 * .3 test mode?  .4 ctd pump?
 */

int yorn( void );
void veePrint(void);

/// rets: 0=n 1=y
// secret: exit on q
int yorn( void ) {
  short c;
  printf( "(y/n) " );
  c = cgetc();
  switch (c) {
    case 'n': return(0);
    case 'N': return(0);
    case 'y': return(1);
    case 'Y': return(1);
    case 'q': exit(1);
    case 'Q': exit(1);
  }
  // try again
  printf( "\n" );
  return(yorn());
}

///
// print all vee vars, ignore qpbc
void veePrint(void) {
  static char *self="veePrint";
  VEEVar *vv;
  char *name, *val;
  DBG();
  vv = VEEFetchNext(NULL);
  while (vv) {
    name = VEEGetName(vv);
    val = VEEFetchStr(name, "");
    if (strstr(name, "SYS.QPBC")) continue;
    printf("%s=%s\n", name, val);
    vv = VEEFetchNext(vv);
  } // (vv)
} // cfgVee

///
void main(void) {
  int pump, test, ask;
  pump=test=ask=0;
  printf( "\n Set up system variables \n\n" );
  printf( "\n%d\n", yorn());
  veePrint();
} // main
