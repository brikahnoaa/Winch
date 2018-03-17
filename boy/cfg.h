// cfg.h
#define SYS_CFG "c:SYS.CFG"

typedef struct CfgInfo {
  char file[32];
  char wild[32];
} CfgInfo;
static void cfgSet( void *ptr, char type, char *val );

bool cfgString(char *str);
int cfgRead(char *file);
void cfgInit();
