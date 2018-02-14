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

extern AntInfo ant;
extern BoyInfo boy;
extern CtdInfo ctd;
extern MpcInfo mpc;
extern NgkInfo ngk;
extern PwrInfo pwr;
extern SysInfo sys;
extern WspInfo wsp;

// 
// static CfgParam cfg[] = array of {id, var, ptr, type}
// scan it for name when updating a cfg
// { "bdh", "boy.depth", &boy.depth, 'f'},
//
// &ptr can be any extern var or struct component
// type := bcifls bool char* int float long short
// in order found in *.h typedef struct
static CfgParam cfg[] = {
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
  {"wgn", "wsp.gain",       &wsp.gain,        'i'},
  {"wnm", "wsp.count",      &wsp.count,       'i'},
};

static int cfgLen = sizeof(cfg) / sizeof(CfgParam);

//
// input line is short or long name, =, value
// find setVar with id or name, call cfgSet()
// uses: cfg cfgLen
//
bool cfgString(char *str){
  char *ref, *val;
  char s[80];
  int i;
  strcpy(s, str);
  ref=strtok(s, "=");
  if (ref==NULL) return false;
  val=strtok(NULL, "=");     // rest of string
  if (val==NULL) return false;
  // find matching name
  for (i=0; i<cfgLen; i++) {
    if (strcmp(ref, cfg[i].id)==0 || strcmp(ref, cfg[i].var)==0) {
      cfgSet(cfg[i].ptr, cfg[i].type, val);
      return true;
    }
  } // for cfg
  return false;                 // name not found
}

//
// convert *val to type and poke into *ptr
//
static void cfgSet( void *ptr, char type, char *val ) {
  flogf("\ncfgSet(%c, %s)", type, val);
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
    flogf("\nERR\t| bad type");
  }
} // cfgSet
