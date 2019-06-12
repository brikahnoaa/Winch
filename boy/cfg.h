// cfg.h
#ifndef H_CFG
#define H_CFG

typedef struct CfgInfo {
  char file[64];
  int cnt;      // # of params in cfgParam
  int def;      // # set from cfgParam default
  int err;      // # err
  int sys;      // # set in sys.cfg
  int vee;      // # set in VEE
} CfgInfo;

typedef struct CfgParam {
  char type;                // b, c, f, i, l, s
  char *var;
  void *ptr;
  char *id;
  char *def;
} CfgParam;

static bool cfgCmp(char *a, char*b);
static void cfgDefault(void);
static void cfgSet(void *ptr, char type, char *val );
static void cfgVee(void);

int cfgRead(char *file);
int cfgString(char *str);
void cfgDump(void);
void cfgInit(void);

#endif
