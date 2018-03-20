// cfg.c
#include <com.h>
#include <cfg.h>

#include <ant.h>
#include <boy.h>
#include <ctd.h>
#include <mpc.h>
#include <ngk.h>
#include <pwr.h>
#include <sys.h>
#include <wsp.h>

CfgInfo cfg;

extern AntInfo ant;
extern BoyInfo boy;
extern CtdInfo ctd;
extern MpcInfo mpc;
extern NgkInfo ngk;
extern PwrInfo pwr;
extern SysInfo sys;
extern WspInfo wsp;

typedef struct CfgParam {
  char *id;
  char *var;
  void *ptr;
  char type;                // b, c, f, i, l, s
} CfgParam;
// 
// static CfgParam cfgP[] = array of {id, var, ptr, type}
// scan for id or var name to set or update configurable data
//
// &ptr can be any extern var or struct component
// type := bcifls bool char* int float long short
// in order as found in *.h typedef struct
static CfgParam cfgP[] = {
  {"aln", "ant.gpsLong",    &ant.gpsLong,     'c'},
  {"alt", "ant.gpsLat",     &ant.gpsLat,      'c'},
  {"asD", "ant.surfD",      &ant.surfD,       'f'},
  {"blF", "boy.logFile",    &boy.logFile,     'c'},
  {"bat", "boy.ant2tip",    &boy.ant2tip,     'f'},
  {"bba", "boy.boy2ant",    &boy.boy2ant,     'f'},
  {"bcD", "boy.currChkD",   &boy.currChkD,    'f'},
  {"bcX", "boy.currMax",    &boy.currMax,     'f'},
  {"bcf", "boy.callFreq",   &boy.callFreq,    'i'},
  {"bch", "boy.callHour",   &boy.callHour,    'i'},
  {"bfn", "boy.fileNum",    &boy.fileNum,     'i'},
  {"bph", "boy.phase",      &boy.phase,       'i'},
  {"ccw", "cfg.wild",       &cfg.wild,        'c'},
  {"clF", "ctd.logFile",    &ctd.logFile,     'c'},
  {"cdy", "ctd.delay",      &ctd.delay,       'i'},
  {"ndy", "ngk.delay",      &ngk.delay,       'i'},
  {"pon", "pwr.on",         &pwr.on,          'b'},
  {"plF", "pwr.logFile",    &pwr.logFile,     'c'},
  {"pch", "pwr.charge",     &pwr.charge,      'f'},
  {"pcM", "pwr.chargeMin",  &pwr.chargeMin,   'f'},
  {"pvM", "pwr.voltsMin",   &pwr.voltsMin,    'f'},
  {"spt", "sys.platform",   &sys.platform,    'c'},
  {"spg", "sys.program",    &sys.program,     'c'},
  {"spj", "sys.project",    &sys.project,     'c'},
  {"svs", "sys.version",    &sys.version,     'c'},
  {"wlF", "wsp.logFile",    &wsp.logFile,     'c'},
  {"wdi", "wsp.detInt",     &wsp.detInt,      'i'},
  {"wdX", "wsp.detMax",     &wsp.detMax,      'i'},
  {"wdo", "wsp.detOff",     &wsp.detOff,      'i'},
  {"wdu", "wsp.duty",       &wsp.duty,        'i'},
  {"wgn", "wsp.gain",       &wsp.gain,        'i'},
  {"wnm", "wsp.count",      &wsp.count,       'i'},
};

static int cfgLen = sizeof(cfgP) / sizeof(CfgParam);

///
// read config from CONFIG_FILE
void cfgInit(void) {
  strcpy(cfg.file, VEEFetchStr( "SYS_CFG", SYS_CFG ));
  cfgRead(cfg.file);
  if (cfg.wild) {
    // wildcard match for config files
    // ??
  }
} // configFile

///
// input line is short or long name, =, value
// find setVar with id or name, call cfgSet()
// OK to have leading space and #comments
// uses: cfgP[] cfgLen
bool cfgString(char *str){
  char *ptr, *ref, *val;
  char s[128];
  int i;
  DBG0("cfgString()")
  strcpy(s, str);
  // erase after #
  ptr = strchr(s, '#');
  if (ptr!=NULL) 
    *ptr = 0;
  // skip leading space
  ref = s + strspn(s, " \t");
  // break at '='
  ptr = strchr(s, '=');
  if (ptr==NULL) return false;
  *ptr = 0;
  val = ptr+1;
  // find matching name
  for (i=0; i<cfgLen; i++) {
    if (strcmp(ref, cfgP[i].id)==0 || strcmp(ref, cfgP[i].var)==0) {
      cfgSet(cfgP[i].ptr, cfgP[i].type, val);
      DBG2("(%c) %s:=%s", cfgP[i].type, cfgP[i].var, val)
      return true;
    }
  } // for cfg
  return false;                 // name not found
}

//
// convert *val to type and poke into *ptr
//
static void cfgSet( void *ptr, char type, char *val ) {
  switch (type) {
  case 'b':     // bool is a char
    if (val[0]=='f'||val[0]=='F'||val[0]=='0')
      *(bool*)ptr = false;
    else 
      *(bool*)ptr = true;
    break;
  case 'c':
    strcpy((char *)ptr, val);
    break;
  case 'f':
    *(float*)ptr=atof(val);
    break;
  case 'i':
    *(int*)ptr=atoi(val);
    break;
  case 'l':
    *(long*)ptr=(long)atoi(val);
    break;
  case 's':
    *(short*)ptr=(short)atoi(val);
    break;
  default:
    flogf("\nERR\t| cfgSet() bad type");
  }
} // cfgSet

//
// read cfg strings from a file
// returns: number of cfg lines
//
int cfgRead(char *file) {
  char fname[32] = "C:";
  char *buf, *ptr;
  int r, fh;
  struct stat finfo;
  //
  flogf("\ncfgRead(%s)", file);
  strcat(fname, file);
  if (stat(fname, &finfo) < 0) {
    flogf("\t|ERR cannot open");
    return 0;
  }
  fh = open(fname, O_RDONLY);
  // cfg file is not large, read all of it into buf and null terminate
  buf = (char *)malloc(finfo.st_size+1);
  read(fh, buf, finfo.st_size);
  buf[finfo.st_size] = 0;             // note, [x] is last char of malloc(x+1)
  // parse cfg strings (dos or linux) and return count r
  r = 0;
  ptr = strtok(buf, "\r\n");
  while (ptr!=NULL) {
    if (cfgString(ptr))
      r++;
    ptr = strtok(NULL, "\r\n");
  }
  free(buf);
  DBG1("\ntest:: ant.surfD=%f, boy.phase=%d, wsp.detInt=%d, wsp.duty=%d", 
    ant.surfD, boy.phase, wsp.detInt, wsp.duty)
  return r;
}

