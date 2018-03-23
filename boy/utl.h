// utl.h - utility and shared stuff

#define BUFSZ 1024
#define RS232_SETTLE 100
#define C_DRV ('C'-'A')


typedef TUPort * Serial;

// global extern
extern char scratch[];
// exception to module game
extern void sysStop(char *out);

char *utlDate(void);
char *utlTime(void);
char *utlTimeDate(void);
char *utlNonPrint (char *in);        // format unprintable string
int utlLogFile(char *fname);
int utlTrim(char *str);
int utlRead(Serial port, char *in);
int utlReadWait(Serial port, char *in, int wait);
void utlWrite(Serial port, char *out, char *eol);
void utlDelay(int x);
void utlPet(void);
void utlShutdown(char *out);
