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

typedef struct CfgParam {
  char *id;
  char *var;
  void *ptr;
  char type;                // b, c, f, i, l, s
} CfgParam;
// 
// static CfgParam cfg[] = array of {id, var, ptr, type}
// scan for id or var name to set or update configurable data
//
// &ptr can be any extern var or struct component
// type := bcifls bool char* int float long short
// in order as found in *.h typedef struct
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
  {"scw", "sys.cfgWild",    &sys.cfgWild,     'c'},
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
// OK to have leading space and #comments
// uses: cfg[] cfgLen
//
bool cfgString(char *str){
  char *ptr, *ref, *val;
  char s[128];
  int i;
  DBG0("cfgString()")
  strcpy(s, str);
  // erase after #, skip leading space, break at '='
  ptr = strchr(s, '#');
  if (ptr==NULL) return false;
  *ptr = 0;
  ref = s + strspn(s, " \t");
  ptr = strchr(s, '=');
  if (ptr==NULL) return false;
  *ptr = 0;
  val = ptr+1;
  // find matching name
  for (i=0; i<cfgLen; i++) {
    if (strcmp(ref, cfg[i].id)==0 || strcmp(ref, cfg[i].var)==0) {
      DBG1("%s:=%s", cfg[i].var, val)
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
    DBG2("\n%s", ptr)
    if (cfgString(ptr))
      r++;
    ptr = strtok(NULL, "\r\n");
  }
  free(buf);
  return r;
}

