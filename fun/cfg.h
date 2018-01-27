// cfg.h
typedef struct CfgParam {
  char *id;
  char *var;
  void *ptr;
  char type;                // b, c, f, i, l, s
} CfgParam;

bool cfgString(char *str);
static void cfgSet( void *ptr, char type, char *val );


