// cfg.c
#include <utl.h>
#include <cfg.h>

#include <ant.h>
#include <boy.h>
#include <ctd.h>
#include <gps.h>
#include <mpc.h>
#include <ngk.h>
#include <pwr.h>
#include <sys.h>
#include <wsp.h>

CfgInfo cfg;

extern AntInfo ant;
extern BoyInfo boy;
extern CtdInfo ctd;
extern GpsInfo gps;
extern MpcInfo mpc;
extern NgkInfo ngk;
extern PwrInfo pwr;
extern SysInfo sys;
extern UtlInfo utl;
extern WspInfo wsp;

typedef struct CfgParam {
  char *id;
  char *var;
  void *ptr;
  char type;                // b, c, f, i, l, s
  char *def;
} CfgParam;
// 
// static CfgParam cfgP[] = array of {id, var, ptr, type}
// scan for id or var name to set or update configurable data
//
// &ptr can be any extern var or struct component
// type := bcifls bool char* int float long short
// in order as found in *.h typedef struct
static CfgParam cfgP[] = {
  {"asC", "ant.sampClear",  &ant.sampClear,   'b',  "true"},
  {"asS", "ant.sampStore",  &ant.sampStore,   'b',  "false"},
  {"as3", "ant.sbe39",      &ant.sbe39,       'b',  "true"},
  {"alF", "ant.logFile",    &ant.logFile,     'c',  "s39"},
  {"asD", "ant.subD",       &ant.subD,        'f',  "3.49"},
  {"asD", "ant.surfD",      &ant.surfD,       'f',  "1.14"},
  {"ade", "ant.delay",      &ant.delay,       'i',  "3"},
  {"asI", "ant.sampInt",    &ant.sampInt,     'i',  "10"},
  {"arS", "ant.ringSize",   &ant.ringSize,    'i',  "5"},
  {"biA", "boy.iridAuton",  &boy.iridAuton,   'b',  "true"},
  {"bnD", "boy.noData",     &boy.noData,      'b',  "false"},
  {"bne", "boy.noDeploy",   &boy.noDeploy,    'b',  "false"},
  {"bnI", "boy.noIrid",     &boy.noIrid,      'b',  "false"},
  {"bnR", "boy.noRise",     &boy.noRise,      'b',  "false"},
  {"bte", "boy.testing",    &boy.testing,     'b',  "false"},
  {"blF", "boy.logFile",    &boy.logFile,     'c',  "boy"},
  {"bat", "boy.ant2tip",    &boy.ant2tip,     'f',  "2.35"},
  {"bba", "boy.boy2ant",    &boy.boy2ant,     'f',  "14.29"},
  {"bcD", "boy.currChkD",   &boy.currChkD,    'f',  "10.0"},
  {"bcX", "boy.currMax",    &boy.currMax,     'f',  "99.0"},
  {"bfT", "boy.fallVTest",  &boy.fallVTest,   'f',  "0.2"},
  {"brA", "boy.rateAccu",   &boy.rateAccu,    'f',  "1.5"},
  {"brT", "boy.riseVTest",  &boy.riseVTest,   'f',  "0.33"},
  {"bcy", "boy.cycle",      &boy.cycle,       'i',  "0"},
  {"bcM", "boy.cycleMax",   &boy.cycleMax,    'i',  "0"},
  {"bfN", "boy.fileNum",    &boy.fileNum,     'i',  "1"},
  {"biF", "boy.iridFreq",   &boy.iridFreq,    'i',  "1"},
  {"biH", "boy.iridHour",   &boy.iridHour,    'i',  "1"},
  {"biO", "boy.iridOp",     &boy.iridOp,      'i',  "5"},
  {"bnD", "boy.ngkDelay",   &boy.ngkDelay,    'i',  "8"},
  {"bfO", "boy.fallOp",     &boy.fallOp,      'i',  "30"},
  {"bfR", "boy.fallRetry",  &boy.fallRetry,   'i',  "3"},
  {"brO", "boy.riseOp",     &boy.riseOp,      'i',  "30"},
  {"brR", "boy.riseRetry",  &boy.riseRetry,   'i',  "3"},
  {"bsT", "boy.settleT",    &boy.settleT,     'i',  "120"},
  {"bsP", "boy.startPh",    &boy.startPh,     'i',  "0"},
  {"btC", "boy.testCnt",    &boy.testCnt,     'i',  "3"},
  {"btS", "boy.testSize",   &boy.testSize,    'i',  "64"},
  {"ccS", "ctd.clearSamp",  &ctd.clearSamp,   'b',  "true"},
  {"csS", "ctd.storeSamp",  &ctd.storeSamp,   'b',  "true"},
  {"clF", "ctd.logFile",    &ctd.logFile,     'c',  "s16"},
  {"cde", "ctd.delay",      &ctd.delay,       'i',  "6"},
  {"csI", "ctd.sampleInt",  &ctd.sampleInt,   'i',  "10"},
  {"gsT", "gps.setTime",    &gps.setTime,     'b',  "true"},
  {"gph", "gps.phoneNum",   &gps.phoneNum,    'c',  "0088160000519"},
  {"gpl", "gps.platform",   &gps.platform,    'c',  "LR01"},
  {"gpr", "gps.project",    &gps.project,     'c',  "QUEH"},
  {"gsS", "gps.sendSz",     &gps.sendSz,      'i',  "16"},
  {"ghP", "gps.hdrPause",   &gps.hdrPause,    'i',  "20"},
  {"ghT", "gps.hdrTry",     &gps.hdrTry,      'i',  "3"},
  {"gre", "gps.redial",     &gps.redial,      'i',  "5"},
  {"grB", "gps.rudBaud",    &gps.rudBaud,     'i',  "2400"},
  {"grR", "gps.rudResp",    &gps.rudResp,     'i',  "20"},
  {"gsM", "gps.signalMin",  &gps.signalMin,   'i',  "2"},
  {"gto", "gps.timeout",    &gps.timeout,     'i',  "60"},
  {"nde", "ngk.delay",      &ngk.delay,       'i',  "8"},
  {"nwI", "ngk.winchId",    &ngk.winchId,     'i',  "1"},
  {"pon", "pwr.on",         &pwr.on,          'b',  "false"},
  {"plF", "pwr.logFile",    &pwr.logFile,     'c',  "pwr"},
  {"pch", "pwr.charge",     &pwr.charge,      'f',  "12000.0"},
  {"pcM", "pwr.chargeMin",  &pwr.chargeMin,   'f',  "200.0"},
  {"pvM", "pwr.voltsMin",   &pwr.voltsMin,    'f',  "12.5"},
  {"spg", "sys.program",    &sys.program,     'c',  "LARA"},
  {"sve", "sys.version",    &sys.version,     'c',  "4.0"},
  {"uiC", "utl.ignoreCon",  &utl.ignoreCon,   'b',  "false"},
  {"won", "wsp.on",         &wsp.on,          'b',  "true"},
  {"wlo", "wsp.logging",    &wsp.logging,     'b',  "true"},
  {"wlF", "wsp.logFile",    &wsp.logFile,     'c',  "wsp"},
  {"wca", "wsp.card",       &wsp.card,        'i',  "1"},
  {"wcs", "wsp.cards",      &wsp.cards,       'i',  "2"},
  {"wcS", "wsp.cfSize",     &wsp.cfSize,      'i',  "512"},
  {"wdI", "wsp.detInt",     &wsp.detInt,      'i',  "10"},
  {"wdX", "wsp.detMax",     &wsp.detMax,      'i',  "10"},
  {"wdu", "wsp.duty",       &wsp.duty,        'i',  "50"},
  {"wfM", "wsp.freeMin",    &wsp.freeMin,     'i',  "5"},
  {"wga", "wsp.gain",       &wsp.gain,        'i',  "1"},
};


///
// read config from CONFIG_FILE
void cfgInit(void) {
  cfg.cnt = sizeof(cfgP) / sizeof(CfgParam);
  strcpy(cfg.file, VEEFetchStr( "SYS_CFG", SYS_CFG ));
  cfgDefault();
  cfgRead(cfg.file);
  // if (cfg.wild) {
    // wildcard match for config files
    // ??
  // }
  cfgVee();
} // configFile

///
// set default parameters
// uses: cfgParam
// sets: *.*
// rets: # of successful sets
void cfgDefault(void) {
  CfgParam *param;
  int i;
  DBG0("cfgDefault()")
  param=cfgP;
  i=cfg.cnt;
  while (i--) {
    DBG1("\n%s=%s", param->var, param->def)
    // default value
    if (param->def[0])
      cfgSet(param->ptr, param->type, param->def);
    param++;
  }
} // cfgDefault

///
// input line is short or long name, =, value
// find setVar with id or name, call cfgSet()
// OK to have leading space and #comments
// uses: cfgP[] cfg.cnt
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
  for (i=0; i<cfg.cnt; i++) {
    id = cfgP[i].id;
    var = cfgP[i].var;
    ptr = cfgP[i].ptr;
    type = cfgP[i].type;
    // ignore case compare with cfgCmp
    if (cfgCmp(ref, id) || cfgCmp(ref, var)) {
      cfgSet(ptr, type, val);
      DBG1("\n(%c) %s=%s", type, var, val)
      return true;
    }
  } // for cfg
  utlErr(cfg_err, "cfgString() no match on name/id");
  flogf( " %s=%s ??", ref, val);
  return false;                 // name not found
} // cfgString

///
// compare strings for equivalence, ignore case
bool cfgCmp(char *a, char*b) {
  if (strlen(a)!=strlen(b))
    return false;
  while (*a && *b) 
    // convert char to lower, postincr, compare
    if (tolower(*a++)!=tolower(*b++))
      return false;
  return true;
}

///
// convert *val to type and poke into *ptr
static void cfgSet( void *ptr, char type, char *val ) {
  switch (type) {
  case 'b':     // bool is 0,1 or t,f or T,F or true,false
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

///
// read cfg strings from a file
// returns: number of cfg lines
int cfgRead(char *file) {
  char *buf, *ptr;
  int r, fh;
  struct stat finfo;
  //
  DBG0("cfgRead(%s)", file)
  if (stat(file, &finfo) < 0) {
    DBG1("| file not found")
    return 0;
  }
  fh = open(file, O_RDONLY);
  // cfg file is not large, read all of it into buf and null terminate
  buf = (char *)malloc(finfo.st_size+2);
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
    val = VEEFetchStr(name, "");
    // got dot? not qpbc? has value?
    if (strchr(name, '.')
        && !strstr(name, "SYS.QPBC")
        && val[0]!=0 ) {
      strcpy(cfgstr, name);
      strcat(cfgstr, "=");
      strcat(cfgstr, val);
      flogf("\nVEE: %s", cfgstr);
      cfgString(cfgstr);
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
  for (i=0; i<cfg.cnt; i++) {
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
  } // for i<cfg.cnt
  flogf("\ncfgDump() %d items", cfg.cnt);
  flogf("\n---\n");
  flogf("%s", buff);
} // cfgDump

