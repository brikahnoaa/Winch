// cfg.h
#define CFG_H

#define SYS_CFG "SYS.CFG"

typedef struct CfgInfo {
  char file[32];
  int cnt;      // # of params in cfgParam
  int def;      // # set from cfgParam default
  int err;      // # err
  int sys;      // # set in sys.cfg
  int vee;      // # set in VEE
} CfgInfo;

static bool cfgCmp(char *a, char*b);
static void cfgDefault(void);
static void cfgSet(void *ptr, char type, char *val );
static void cfgVee(void);

bool cfgString(char *str);
int cfgRead(char *file);
void cfgDump(void);
void cfgInit(void);
