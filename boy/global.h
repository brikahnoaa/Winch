// global.h - utility and shared stuff

#define PLATFORM LARA
#define PROG_VERSION 4.0
#define BUFSZ 1024

// PINS
#define AMODEMPWR 21
#define ANTMODPWR 22  // antMod Power pin (1=ON, 0=OFF)
#define DEVICECOM 23  // set = antMod, clear = sbe16
#define WISPRTHREE 24
#define WISPRFOUR 25
#define DIFARPWR 26   // unused
#define WISPRONE 29
#define WISPRTWO 30
#define DEVICETX 31
#define DEVICERX 32
#define AMODEMRX 33
#define AMODEMTX 35
#define WISPR_PWR_ON 37
#define WISPR_PWR_OFF 42

typedef unsigned long Time;
typedef TUPort *Serial;

// global externals
extern char scratch[];

void delayms(int x);
void serWrite(Serial port, char *out);
int serReadWait(Serial port, char *in, int wait);
char *unprintf (char *out, *in);    // format unprintable string

