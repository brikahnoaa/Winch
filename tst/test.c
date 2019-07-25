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

int yorn( char *ask );
void veePrint(void);

/// rets: 0=n 1=y
// secret: exit on q
int yorn(char *ask) {
  short c;
  if (ask) cprintf("%s ", ask);
  cprintf( "(y/n) " );
  cdrain();
  c = cgetc();
  cprintf( "%c\n", c );
  cdrain();
  switch (c) {
    case 'n': return(0);
    case 'N': return(0);
    case 'y': return(1);
    case 'Y': return(1);
    case 'q': exit(1);
    case 'Q': exit(1);
  }
  // try again
  return(yorn(NULL));
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
    cprintf("%s=%s\n", name, val);
    vv = VEEFetchNext(vv);
  } // (vv)
} // cfgVee

///
int main(void) {
  cprintf( "\n Set up system variables \n\n" );
  cprintf( "\n" );
  cprintf("These are the settings now:\n");
  veePrint();
  cprintf( "\n" );
  //
  if (yorn("Are we setting up for deployment?")) {
    if (!yorn("Is the winch acoustic modem turned on?"))
      if (!yorn("Please press in the button on the amodem to activate now. Done?"))
        return 1;
    cprintf("\n Clearing all variables \n");
    if (yorn("Confirm, prepare winch to deploy:")) {
      VEEClear();
      // VEEStoreStr("SYS.QPBCS", "32");
    } else {
      cprintf("\n No changes made \n");
    }
  } else {
    cprintf("\n Testing \n");
    if (yorn("Run hardware tests?")) {
    }
  }
  //
  cprintf("Done.\n");
  return 0;
} // main
