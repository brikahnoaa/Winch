// WISPR HEADER FILE

// WISPR Control Structure

// extern bool Wispr_On;
extern TUPort *PAMPort;

short WISPR_Data();
void WISPRPower(bool);
bool WISPRExit(void);
void WISPRGain(short);
float GetWISPRFreeSpace();
void WISPRDFP(void);
void WISPRTFP();
void WISPRDet(int);
void WISPRSafeShutdown();
void ChangeWISPR(short);
void GetWISPRSettings();
void WISPRWriteFile(int);
void create_dtx_file();
bool WISPR_Status();
void GatherWISPRFreeSpace();
void UpdateWISPRFRS();
void WISPRGPS();
void OpenTUPort_WISPR(bool);
bool WISPRExpectedReturn(short, bool);

// pin defines moved to platform.h

// TUPort Baud Rates
#define BAUD 9600L
// This baud rate works with only the new wispr_053116 killerwhale file at the
// moment. 060116- AT
