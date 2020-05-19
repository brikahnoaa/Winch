// mpc.h
#ifndef H_MPC
#define H_MPC

// PINS (outputs)
#define MDM_PWR 21
#define ANT_PWR 22  // antenna module Power pin (1=ON, 0=OFF)
#define ANT_SEL 23  // set = antMod cf2, clear = a3la
#define PAM_34 24   // 24 cannot be on with 29
#define PAM_4 25    // 24&25 -> pam4 (sbe16)
#define HPSENS 26   // power for pressure sensor
#define PAM_TX 27   // com2,3: (pam_12)(pam_34)
#define PAM_12 29   // 24 cannot be on with 29
#define PAM_2 30    // 29&30 -> wsp2
#define ANT_TX 31   // com1: ant mod, uMPC and A3LA
#define MDM_TX 35   //
#define WISPR_PWR_ON 37
#define WISPR_PWR_OFF 42
#define CONS_TX 46
// input or bus
#define RESET 7
#define PAM_RX 28   // selected by 24, 29
#define ANT_RX 32   // selected by 23
#define MDM_RX 33   // acoustic modem
#define CONS_RX 45
#define MDM_TX_RTS 48
#define MDM_RX_CTS 50

typedef enum { null_pam=0, wsp1_pam, wsp2_pam, wsp3_pam, sbe16_pam } MpcPamType;

typedef struct MpcInfo {
  long diskFree;
  long diskSize;
  MpcPamType pamDev;             // wsp_1,_2,_3, sbe_16
  Serial pamPort;
} MpcInfo;

static void spur_ISR(void);
static void IRQ4_ISR(void);
static void IRQ5_ISR(void);

void mpcPamDev(MpcPamType pam);
void mpcPamPwr(MpcPamType pam, bool on);
void mpcPamPulse(int pin);
void mpcInit(void);
void mpcSleep(void);
void mpcStop(void);
Serial mpcPamPort(void);

#endif
