// utils.h - utility and shared stuff

typedef TUPort *Serial;

// global extern
extern char scratch[];

void clockTime(char *out);
void clockTimeDate(char *out);
char *sprintfun (char *out, char *in);        // format unprintable string
int serRead(Serial port, char *in);
int serReadWait(Serial port, char *in, int wait);
int serWrite(Serial port, char *out);
void delayms(int x);
