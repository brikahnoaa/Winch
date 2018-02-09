// mpc.h

#define WTMODE nsStdSmallBusAdj // choose: nsMotoSpecAdj or nsStdSmallBusAdj
#define SYSCLK 16000 // Clock speed: 2000 works 160-32000 kHz Default: 16000

typedef enum { ant_dev, ctd_dev } DevType;

typedef struct MpcInfo {
  long diskFree;
  long diskSize;
  Serial port;
  DevType device;
} MpcInfo;

static void IRQ4_ISR(void);
static void IRQ5_ISR(void);
static void spur_ISR(void);

bool mpcDevSelect(DevType dev);
float mpcVoltage(float *volts);
void mpcInit(void);
void mpcSleep(void);
void mpcStop(void);
Serial mpcCom1Port(void);
DevType mpcCom1Dev(void);
