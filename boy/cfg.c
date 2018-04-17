// cfg.c
#include <utl.h>
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
  {"alg", "ant.logging",    &ant.logging,     'b'},
  {"aln", "ant.gpsLong",    &ant.gpsLong,     'c'},
  {"alt", "ant.gpsLat",     &ant.gpsLat,      'c'},
  {"alF", "ant.logFile",    &ant.logFile,     'c'},
  {"asD", "ant.surfD",      &ant.surfD,       'f'},
  {"asR", "ant.samRes",     &ant.samRes,      'f'},
  {"ady", "ant.delay",      &ant.delay,       'i'},
  {"afr", "ant.fresh",      &ant.fresh,       'i'},
  {"asC", "ant.samLen",     &ant.samLen,      'i'},
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
  {"clg", "ctd.logging",    &ctd.logging,     'b'},
  {"clF", "ctd.logFile",    &ctd.logFile,     'c'},
  {"cdy", "ctd.delay",      &ctd.delay,       'i'},
  {"cfr", "ctd.fresh",      &ctd.fresh,       'i'},
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
  {"wdX", "wsp.detMax",     &wsp.detMax,      'i'},
  {"wdu", "wsp.duty",       &wsp.duty,        'i'},
  {"wgn", "wsp.gain",       &wsp.gain,        'i'},
  {"wnm", "wsp.num",        &wsp.num,         'i'},
};


///
// read config from CONFIG_FILE
void cfgInit(void) {
  cfg.len = sizeof(cfgP) / sizeof(CfgParam);
  strcpy(cfg.file, VEEFetchStr( "SYS_CFG", SYS_CFG ));
  cfgRead(cfg.file);
  if (cfg.wild) {
    // wildcard match for config files
    // ??
  }
  cfgVee();
} // configFile

///
// input line is short or long name, =, value
// find setVar with id or name, call cfgSet()
// OK to have leading space and #comments
// uses: cfgP[] cfg.len
bool cfgString(char *str){
  char *p, *ptr, *ref, *val, *var, *id, type;
  char s[128];
  int i;
  strcpy(s, str);
  // erase after #
  p = strchr(s, '#');
  if (p!=NULL) 
    *p = 0;
  // skip leading space
  ref = s + strspn(s, " \t");
  // break at '='
  p = strchr(s, '=');
  if (p==NULL) return false;
  *p = 0;
  val = p+1;
  // find matching name
  for (i=0; i<cfg.len; i++) {
    id = cfgP[i].id;
    var = cfgP[i].var;
    ptr = cfgP[i].ptr;
    type = cfgP[i].type;
    // ignore case compare with cfgCmp
    if (cfgCmp(ref, id) || cfgCmp(ref, var)) {
      cfgSet(ptr, type, val);
      DBG2("\n(%c) %s=%s", type, var, val)
      return true;
    }
  } // for cfg
  return false;                 // name not found
} // cfgString

///
// compare strings for equivalence, ignore case
bool cfgCmp(char *a, char*b) {
  while (*a!=0) 
    // convert char to lower, postincr, compare
    if (tolower(*a++)!=tolower(*b++))
      return false;
  return true;
}

///
// convert *val to type and poke into *ptr
static void cfgSet( void *ptr, char type, char *val ) {
  switch (type) {
  case 'b':     // bool is a char
    if (val[0]=='f' || val[0]=='F' || val[0]=='0')
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
  char *buf, *ptr;
  int r, fh;
  struct stat finfo;
  //
  flogf("\ncfgRead(%s)", file);
  if (stat(file, &finfo) < 0) {
    flogf("\t|ERR cannot stat()");
    return 0;
  }
  fh = open(file, O_RDONLY);
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
  return r;
}

///
// read all vee vars, look for *.*=*, try those as settings
void cfgVee(void) {
  VEEVar *vv;
  char *name, *val, cfgstr[128];
  DBG0("cfgVee()")
  vv = VEEFetchNext(NULL);
  while (vv) {
    name = VEEGetName(vv);
    // got dot?
    if (strchr(name, '.')) {
      val = VEEFetchStr(name, "");
      if (val[0]==0) continue;        // break
      strcpy(cfgstr, name);
      strcat(cfgstr, "=");
      strcat(cfgstr, val);
      cfgString(cfgstr);
      DBG1("%s", cfgstr);
    }
    vv = VEEFetchNext(vv);
  }
} // cfgVee

///
// write value of all config settings
// ?? add option to dump into file
void cfgDump() {
  int i;
  char val[128], buff[4096];
  buff[0] = 0;
  // buffer output to make file writing easier
  for (i=0; i<cfg.len; i++) {
    switch (cfgP[i].type) {
    case 'b':
      sprintf(val, "=%d", *(bool *)cfgP[i].ptr);
      break;
    case 'c':
      sprintf(val, "=%s", (char *)cfgP[i].ptr);
      break;
    case 'f':
      sprintf(val, "=%f", *(float *)cfgP[i].ptr);
      break;
    case 'i':
      sprintf(val, "=%d", *(int *)cfgP[i].ptr);
      break;
    case 'l':
      sprintf(val, "=%ld", *(long *)cfgP[i].ptr);
      break;
    case 's':
      sprintf(val, "=%d", *(short *)cfgP[i].ptr);
      break;
    }
    // write varname=val onto buff
    strcat(buff, cfgP[i].var);
    strcat(buff, val);
    strcat(buff, "\r\n");
  } // for i<cfg.len
  flogf("\ncfgDump() %d items", cfg.len);
  flogf("\n---\n");
  flogf("%s", buff);
} // cfgDump

