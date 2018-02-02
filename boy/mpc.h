// mpc.h

#define WTMODE nsStdSmallBusAdj // choose: nsMotoSpecAdj or nsStdSmallBusAdj
#define SYSCLK 16000 // Clock speed: 2000 works 160-32000 kHz Default: 16000
/*
#define BUOY_ID '0'
#define WINCH_ID '1'
#define COM1BAUD 9600L
#define MDM_BAUD 4800L
// PINS
#define MDM_PWR 21
#define ANT_PWR 22  // antenna module Power pin (1=ON, 0=OFF)
#define COM1SELECT 23  // set = antMod, clear = sbe16
#define WISPR3 24
#define WISPR4 25
// #define DFRPWR 26   // unused
#define WISPR1 29
#define WISPR2 30
#define COM1_TX 31
#define COM1_RX 32
#define MDM_RX 33
#define MDM_TX 35
#define WISPR_PWR_ON 37
#define WISPR_PWR_OFF 42
*/

typedef enum { ant_ser, ctd_ser } SerialDevType;

typedef struct MpcData {
  float joules;
  float volts;
  float voltMin;
  long diskFree;
  long diskSize;
  Serial com1;
  SerialDevType device;
} MpcData;
extern MpcData mpc;

static void IRQ4_ISR(void);
static void IRQ5_ISR(void);
static void spur_ISR(void);

float mpcVoltage(float *volts);
void mpcInit(void);
void mpcSleep(void);
