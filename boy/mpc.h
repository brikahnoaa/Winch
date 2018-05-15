// mpc.h
#define MPC_H

#define WTMODE nsStdSmallBusAdj // choose: nsMotoSpecAdj or nsStdSmallBusAdj
#define SYSCLK 16000 // Clock speed: 2000 works 160-32000 kHz Default: 16000
#define PAM_BAUD 9600

typedef enum { null_pam, wsp1_pam, wsp2_pam, wsp3_pam, sbe16_pam } MpcPamType;

typedef struct MpcInfo {
  long diskFree;
  long diskSize;
  MpcPamType pamDev;             // wsp_1,_2,_3, sbe_16
  Serial pamPort;
} MpcInfo;

static void mpcPamPulse(int pin);
static void spur_ISR(void);
static void IRQ4_ISR(void);
static void IRQ5_ISR(void);

void mpcPamDev(MpcPamType pam);
void mpcInit(void);
void mpcSleep(void);
void mpcStop(void);
Serial mpcPamPort(void);
