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
  char *var;
  void *ptr;
  char *id;
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
  {"ant.auton",     &ant.auton,       "aau", 'b',  "false"},
  {"ant.sampClear", &ant.sampClear,   "asC", 'b',  "true"},
  {"ant.sampStore", &ant.sampStore,   "asS", 'b',  "true"},
  {"ant.sbe39",     &ant.sbe39,       "as3", 'b',  "true"},
  {"ant.logFile",   &ant.logFile,     "alF", 'c',  "s39"},
  {"ant.subD",      &ant.subD,        "asD", 'f',  "3.49"},
  {"ant.surfD",     &ant.surfD,       "asD", 'f',  "1.14"},
  {"ant.delay",     &ant.delay,       "ade", 'i',  "3"},
  {"ant.sampInt",   &ant.sampInt,     "asI", 'i',  "10"},
  {"ant.ringSize",  &ant.ringSize,    "arS", 'i',  "5"},
  {"boy.reset",     &boy.reset,       "bre", 'b',  "false"},
  {"boy.stop",      &boy.stop,        "bst", 'b',  "false"},
  {"boy.iridAuton", &boy.iridAuton,   "biA", 'b',  "false"},
  {"boy.test",      &boy.test,        "btt", 'b',  "false"},
  {"boy.logFile",   &boy.logFile,     "blF", 'c',  "boy"},
  {"boy.ant2tip",   &boy.ant2tip,     "bat", 'f',  "2.35"},
  {"boy.boy2ant",   &boy.boy2ant,     "bba", 'f',  "14.29"},
  {"boy.currChkD",  &boy.currChkD,    "bcD", 'f',  "10.0"},
  {"boy.currMax",   &boy.currMax,     "bcX", 'f',  "99.0"},
  {"boy.fallVTest", &boy.fallVTest,   "bfT", 'f',  "0.2"},
  {"boy.rateAccu",  &boy.rateAccu,    "brA", 'f',  "1.5"},
  {"boy.riseVTest", &boy.riseVTest,   "brT", 'f',  "0.33"},
  {"boy.cycleMax",  &boy.cycleMax,    "bcM", 'i',  "0"},
  {"boy.depWait",   &boy.depWait,     "bdW", 'i',  "240"},
  {"boy.depSettle", &boy.depSettle,   "bsT", 'i',  "120"},
  {"boy.fallOp",    &boy.fallOp,      "bfO", 'i',  "30"},
  {"boy.fallRetry", &boy.fallRetry,   "bfR", 'i',  "3"},
  {"boy.filePause", &boy.filePause,   "bfP", 'i',  "1"},
  {"boy.iridFreq",  &boy.iridFreq,    "biF", 'i',  "1"},
  {"boy.iridHour",  &boy.iridHour,    "biH", 'i',  "1"},
  {"boy.iridOp",    &boy.iridOp,      "biO", 'i',  "5"},
  {"boy.ngkDelay",  &boy.ngkDelay,    "bnD", 'i',  "8"},
  {"boy.riseOp",    &boy.riseOp,      "brO", 'i',  "30"},
  {"boy.riseRetry", &boy.riseRetry,   "brR", 'i',  "3"},
  {"boy.startPh",   &boy.startPh,     "bsP", 'i',  "0"},
  {"boy.stayDown",  &boy.stayDown,    "bsD", 'i',  "0"},
  {"ctd.clearSamp", &ctd.clearSamp,   "ccS", 'b',  "false"},
  {"ctd.sampStore", &ctd.sampStore,   "csS", 'b',  "true"},
  {"ctd.logFile",   &ctd.logFile,     "clF", 'c',  "s16"},
  {"ctd.delay",     &ctd.delay,       "cde", 'i',  "6"},
  {"ctd.sampleInt", &ctd.sampleInt,   "csI", 'i',  "10"},
  {"gps.setTime",   &gps.setTime,     "gsT", 'b',  "true"},
  {"gps.phoneNum",  &gps.phoneNum,    "gph", 'c',  "0088160000519"},
  {"gps.platform",  &gps.platform,    "gpl", 'c',  "LR01"},
  {"gps.project",   &gps.project,     "gpr", 'c',  "QUEH"},
  {"gps.fileMax",   &gps.fileMax,     "gfX", 'i',  "2000"},
  {"gps.fileBlock", &gps.fileBlock,   "gfB", 'i',  "512"},
  {"gps.hdrPause",  &gps.hdrPause,    "ghP", 'i',  "20"},
  {"gps.hdrTry",    &gps.hdrTry,      "ghT", 'i',  "3"},
  {"gps.hupMs",     &gps.hupMs,       "ghM", 'i',  "2000"},
  {"gps.redial",    &gps.redial,      "gre", 'i',  "5"},
  {"gps.rudBaud",   &gps.rudBaud,     "grB", 'i',  "1800"},
  {"gps.rudResp",   &gps.rudResp,     "grR", 'i',  "20"},
  {"gps.sendSz",    &gps.sendSz,      "gsS", 'i',  "512"},
  {"gps.signalMin", &gps.signalMin,   "gsM", 'i',  "2"},
  {"gps.timeout",   &gps.timeout,     "gto", 'i',  "60"},
  {"ngk.delay",     &ngk.delay,       "nde", 'i',  "8"},
  {"ngk.winchId",   &ngk.winchId,     "nwI", 'i',  "1"},
  {"pwr.on",        &pwr.on,          "pon", 'b',  "false"},
  {"pwr.logFile",   &pwr.logFile,     "plF", 'c',  "pwr"},
  {"pwr.charge",    &pwr.charge,      "pch", 'f',  "12000.0"},
  {"pwr.chargeMin", &pwr.chargeMin,   "pcM", 'f',  "200.0"},
  {"pwr.voltsMin",  &pwr.voltsMin,    "pvM", 'f',  "12.5"},
  {"sys.program",   &sys.program,     "spg", 'c',  "LARA"},
  {"sys.version",   &sys.version,     "sve", 'c',  "4.0"},
  {"tst.noData",    &tst.noData,      "tnD", 'b',  "false"},
  {"tst.noDeploy",  &tst.noDeploy,    "tne", 'b',  "false"},
  {"tst.noIrid",    &tst.noIrid,      "tnI", 'b',  "false"},
  {"tst.noRise",    &tst.noRise,      "tnR", 'b',  "false"},
  {"tst.testCnt",   &tst.testCnt,     "ttC", 'i',  "3"},
  {"tst.testSize",  &tst.testSize,    "ttS", 'i',  "64"},
  {"utl.ignoreCon", &utl.ignoreCon,   "uiC", 'b',  "false"},
  {"wsp.on",        &wsp.on,          "won", 'b',  "false"},
  {"wsp.logging",   &wsp.logging,     "wlo", 'b',  "true"},
  {"wsp.storm",     &wsp.storm,       "wst", 'b',  "true"},
  {"wsp.logFile",   &wsp.logFile,     "wlF", 'c',  "wsp"},
  {"wsp.card",      &wsp.card,        "wca", 'i',  "2"},
  {"wsp.cards",     &wsp.cards,       "wcs", 'i',  "2"},
  {"wsp.cfSize",    &wsp.cfSize,      "wcS", 'i',  "512"},
  {"wsp.detInt",    &wsp.detInt,      "wdI", 'i',  "10"},
  {"wsp.detMax",    &wsp.detMax,      "wdX", 'i',  "10"},
  {"wsp.duty",      &wsp.duty,        "wdu", 'i',  "30"},
  {"wsp.freeMin",   &wsp.freeMin,     "wfM", 'i',  "5"},
  {"wsp.gain",      &wsp.gain,        "wga", 'i',  "1"},
  {"wsp.hour",      &wsp.hour,        "who", 'i',  "7"},
  {"wsp.minute",    &wsp.minute,      "wmi", 'i',  "60"},
  {"wsp.minimum",   &wsp.minimum,     "wmm", 'i',  "4"},
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

