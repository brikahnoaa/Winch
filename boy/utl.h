// utl.h - utility and shared stuff

#define BUFSZ 1024

typedef TUPort * Serial;

// global extern
extern char scratch[];

char *clockTime(char *out);
char *clockTimeDate(char *out);
char *unsprintf (char *out, char *in);        // format unprintable string
int crlfTrim(char *str);
int serRead(Serial port, char *in);
int serReadWait(Serial port, char *in, int wait);
int serWrite(Serial port, char *out);
void delayms(int x);
