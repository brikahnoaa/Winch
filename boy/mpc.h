// mpc.h

#define WTMODE nsStdSmallBusAdj // choose: nsMotoSpecAdj or nsStdSmallBusAdj
#define SYSCLK 16000 // Clock speed: 2000 works 160-32000 kHz Default: 16000

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

static void IRQ4_ISR(void);
static void IRQ5_ISR(void);
static void spur_ISR(void);

float mpcVoltage(float *volts);
void mpcInit(void);
void mpcSleep(void);
