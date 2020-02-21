// cfg.c
#include <main.h>

CfgInfo cfg;

extern AntInfo ant;
extern BoyInfo boy;
extern S16Info s16;
// extern EngInfo eng;
extern IriInfo iri;
extern MpcInfo mpc;
extern NgkInfo ngk;
extern PwrInfo pwr;
extern SysInfo sys;
extern UtlInfo utl;
extern WspInfo wsp;

//typedef struct CfgParam {
//  char type;                // b, c, f, i, l, s
//  char *var;
//  void *ptr;
//  char *id;
//  char *def;
//} CfgParam;
// 
// static CfgParam cfgP[] = array of {id, var, ptr, type}
// scan for id or var name to set or update configurable data
//
// &ptr can be any extern var or struct component
// type := bcifls bool char* int float long short
// in order as found in *.h typedef struct
static CfgParam cfgP[] = {

  {'f',  "ant.subD",      &ant.subD,        "asD",  "3.49"},
  {'f',  "ant.surfD",     &ant.surfD,       "asD",  "1.14"},

  {'b',  "boy.reset",     &boy.reset,       "bre",  "false"},
  {'b',  "boy.stop",      &boy.stop,        "bst",  "false"},
  {'b',  "boy.useBrake",  &boy.useBrake,    "buB",  "false"},
  {'f',  "boy.ant2tip",   &boy.ant2tip,     "bat",  "2.35"},
  {'f',  "boy.boy2ant",   &boy.boy2ant,     "bba",  "14.29"},
  {'f',  "boy.currChkD",  &boy.currChkD,    "bcD",  "10.0"},
  {'f',  "boy.currMax",   &boy.currMax,     "bcX",  "99.0"},
  {'f',  "boy.iceDanger", &boy.iceDanger,   "biD",  "-1.2"},
  {'f',  "boy.predFallV", &boy.predFallV,   "bpF",  "0.2"},
  {'f',  "boy.predRiseV", &boy.predRiseV,   "bpR",  "0.33"},
  {'i',  "boy.callFreq",  &boy.callFreq,    "bcF",  "1"},
  {'i',  "boy.callHour",  &boy.callHour,    "bcH",  "1"},
  {'i',  "boy.depSettle", &boy.depSettle,   "bsT",  "60"},
  {'i',  "boy.depWait",   &boy.depWait,     "bdW",  "240"},
  {'i',  "boy.fallOpM",   &boy.fallOpM,     "bfO",  "30"},
  {'i',  "boy.iridOpM",   &boy.iridOpM,     "biO",  "5"},
  {'i',  "boy.ngkDelay",  &boy.ngkDelay,    "bnD",  "8"},
  {'i',  "boy.riseOpM",   &boy.riseOpM,     "brO",  "20"},
  {'i',  "boy.startPh",   &boy.startPh,     "bsP",  "0"},
  {'i',  "boy.stayDown",  &boy.stayDown,    "bsD",  "0"},

  {'c',  "cfg.file",      &cfg.file,        "cfe",  "lara.cfg"},

  {'b',  "dbg.noData",    &dbg.noData,      "dnD",  "false"},
  {'b',  "dbg.noDeploy",  &dbg.noDeploy,    "dne",  "false"},
  {'b',  "dbg.noIrid",    &dbg.noIrid,      "dnI",  "false"},
  {'b',  "dbg.noRise",    &dbg.noRise,      "dnR",  "false"},
  {'b',  "dbg.test",      &dbg.test,        "dtt",  "false"},
  {'i',  "dbg.t1",        &dbg.t1,          "dt1",  "0"},
  {'i',  "dbg.t2",        &dbg.t2,          "dt2",  "0"},
  {'i',  "dbg.t3",        &dbg.t3,          "dt3",  "0"},

  {'b',  "iri.setTime",   &iri.setTime,     "isT",  "true"},
  {'b',  "iri.logging",   &iri.logging,     "ilo",  "false"},
  {'c',  "iri.phoneNum",  &iri.phoneNum,    "iph",  "0088160000519"},
  {'c',  "iri.platform",  &iri.platform,    "ipl",  "LR01"},
  {'c',  "iri.project",   &iri.project,     "ipr",  "QUEH"},
  {'i',  "iri.blkSz",     &iri.blkSz,       "ibS",  "256"},
  {'i',  "iri.fileMaxKB", &iri.fileMaxKB,   "ifX",  "64"},
  {'i',  "iri.hdrResp",   &iri.hdrResp,     "ihR",  "30"},
  {'i',  "iri.hdrTry",    &iri.hdrTry,      "ihT",  "3"},
  {'i',  "iri.hupMs",     &iri.hupMs,       "ihM",  "2000"},
  {'i',  "iri.landResp",  &iri.landResp,    "ilR",  "20"},
  {'i',  "iri.redial",    &iri.redial,      "ire",  "25"},
  {'i',  "iri.baud",      &iri.baud,        "iba",  "1800"},
  {'i',  "iri.signalMin", &iri.signalMin,   "isM",  "2"},
  {'i',  "iri.timeout",   &iri.timeout,     "ito",  "60"},

  {'i',  "ngk.delay",     &ngk.delay,       "nde",  "8"},
  {'i',  "ngk.winchId",   &ngk.winchId,     "nwI",  "1"},

  {'b',  "pwr.on",        &pwr.on,          "pon",  "false"},
  {'f',  "pwr.charge",    &pwr.charge,      "pch",  "12000.0"},
  {'f',  "pwr.chargeMin", &pwr.chargeMin,   "pcM",  "200.0"},
  {'f',  "pwr.voltsMin",  &pwr.voltsMin,    "pvM",  "12.5"},

  // {'b',  "s16.auton",     &s16.auton,       "6an",  "false"},
  // {'i',  "s16.autoInter", &s16.autoInter,   "6aI",  "10"},
  {'c',  "s16.initStr",   &s16.initStr,     "6iS",  ""},
  {'c',  "s16.startStr",  &s16.startStr,    "6sS",  ""},
  {'c',  "s16.takeSamp",  &s16.takeSamp,    "6tS",  "TSSON"},
  {'i',  "s16.pumpMode",  &s16.pumpMode,    "6pM",  "1"},
  {'i',  "s16.timer",     &s16.timer,       "6tr",  "6"},

  // {'b',  "s39.auton",     &s39.auton,       "3an",  "false"},
  // {'i',  "s39.autoInter", &s39.autoInter,   "3aI",  "10"},
  {'c',  "s39.initStr",   &s39.initStr,     "3iS",  ""},
  {'c',  "s39.startStr",  &s39.startStr,    "3sS",  ""},
  {'c',  "s39.takeSamp",  &s39.takeSamp,    "3tS",  "TSSON"},
  //// {'i',  "s39.pumpMode",  &s39.pumpMode,    "3pM",  "1"},
  {'i',  "s39.timer",     &s39.timer,       "3tr",  "3"},

  {'c',  "sys.program",   &sys.program,     "spg",  "LARA"},
  {'c',  "sys.version",   &sys.version,     "sve",  "4.0"},

  {'b',  "utl.ignoreCon", &utl.ignoreCon,   "uiC",  "false"},

  {'b',  "wsp.on",        &wsp.on,          "won",  "false"},
  {'c',  "wsp.spectCmd",  &wsp.spectCmd,    "wsC",  "/bin/spectrogram"},
  {'c',  "wsp.spectFlag", &wsp.spectFlag,   "wsF",  "-v1 -C12 -n512 -o256"},
  {'c',  "wsp.spectLog",  &wsp.spectLog,    "wsL",  "noise"},
  {'c',  "wsp.wisprCmd",  &wsp.wisprCmd,    "wwC",  "/bin/wispr_bw"},
  {'c',  "wsp.wisprFlag", &wsp.wisprFlag,   "wwF",  "-v2 -T8 -b15 -M2 -F2"},
  {'c',  "wsp.wisprLog",  &wsp.wisprLog,    "wwL",  "wispr"},
  {'i',  "wsp.card",      &wsp.card,        "wcd",  "2"},
  {'i',  "wsp.detInt",    &wsp.detInt,      "wdI",  "10"},
  {'i',  "wsp.detMax",    &wsp.detMax,      "wdX",  "10"},
  {'i',  "wsp.diskFree",  &wsp.diskFree,    "wdF",  "5"},
  {'i',  "wsp.dutyM",     &wsp.dutyM,       "wdM",  "30"},
  {'i',  "wsp.phaseH",    &wsp.phaseH,      "wpH",  "24"},
  {'i',  "wsp.spectGain", &wsp.spectGain,   "wsG",  "1"},
  {'i',  "wsp.spectRun",  &wsp.spectRun,    "wsR",  "1"},
  {'i',  "wsp.wisprGain", &wsp.wisprGain,   "wwG",  "1"},

};


///
// read config from CONFIG_FILE
void cfgInit(void) {
  static char *self="cfgInit";
  int r=0;
  char *cfgFileV;
  DBG();
  cfgDefault();
  flogf("\n%s: config file default name '%s'", self, cfg.file);
  cfgFileV = VEEFetchStr( "CFGFILE", "" );
  if (cfgFileV[0]) {
    strcpy(cfg.file, cfgFileV);
    flogf(", VEE(CFGFILE) changes it to '%s'", self, cfg.file);
  }
  r = cfgRead(cfg.file);
  if (r) flogf("\n%s: %d errors in config file %s", self, r, cfg.file);
  cfgVee();
} // configFile

///
// set default parameters defined in cfg.c
// sets: cfg.cnt
// sets: *.*  // all params
void cfgDefault(void) {
  CfgParam *param;
  int i;
  static char *self="cfgDefault";
  cfg.cnt = sizeof(cfgP) / sizeof(CfgParam);
  flogf("\n%s: %d config settings", self, cfg.cnt);
  i=cfg.cnt;
  param=cfgP;
  while (i--) { // note: post decrement
    DBG1("%s=%s", param->var, param->def);
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
// rets: 0=success 1=noEqualSign 2=noMatchName
int cfgString(char *str){
  CfgPtr ptr;
  char *p, *ref, *val, *var, *id, type;
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
  if (p==NULL) return 1;
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
      DBG1("\n(%c) %s=%s", type, var, val);
      return 0; // success
    }
  } // for cfg
  utlErr(cfg_err, "cfgString() no match on name/id");
  flogf( " %s=%s ??", ref, val);
  return 2;                 // name not found
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
static void cfgSet(CfgPtr ptr, char type, char *val ) {
  switch (type) {
  case 'b':     // bool is 0,1 or t,f or T,F or true,false
    if (val[0]=='f' || val[0]=='F' || val[0]=='0')
      *(bool*)ptr.b = false;
    else 
      *(bool*)ptr.b = true;
    break;
  case 'c':
    strcpy((char*)ptr.c, val);
    break;
  case 'f':
    *(float*)ptr.f=atof(val);
    break;
  case 'i':
    *(int*)ptr.i=atoi(val);
    break;
  case 'l':
    *(long*)ptr.l=(long)atoi(val);
    break;
  case 's':
    *(short*)ptr.s=(short)atoi(val);
    break;
  default:
    flogf("\nERR\t| cfgSet() bad type");
  }
} // cfgSet

///
// read cfg strings from a file
// rets: 0=success #=errCount
int cfgRead(char *file) {
  char *buf, *ptr;
  int r=0, fh;
  struct stat finfo;
  //
  DBG0("cfgRead(%s)", file);
  if (stat(file, &finfo) < 0) {
    flogf("\n%s: cannot find file '%s'", file);
    return 0;
  }
  fh = open(file, O_RDONLY);
  // cfg file is not large, read all of it into buf and null terminate
  buf = (char *)malloc(finfo.st_size+2);
  read(fh, buf, finfo.st_size);
  buf[finfo.st_size] = 0;             // note, [x] is last char of malloc(x+1)
  close(fh);
  // parse cfg strings (dos or linux) and return count r
  ptr = strtok(buf, "\r\n");
  while (ptr!=NULL) {
    flogf( "\n\t%s", ptr);
    if (cfgString(ptr)) { flogf(" !! ERR"); r++;}
    ptr = strtok(NULL, "\r\n");
  }
  free(buf);
  return r;
} // cfgRead

///
// read all vee vars, look for *.*=*, try those as settings
void cfgVee(void) {
  VEEVar *vv;
  char *name, *val, cfgstr[128];
  static char *self="cfgVee";
  DBG();
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
      sprintf(val, "=%d", *(bool *)cfgP[i].ptr.b);
      break;
    case 'c':
      sprintf(val, "=%s", (char *)cfgP[i].ptr.c);
      break;
    case 'f':
      sprintf(val, "=%f", *(float *)cfgP[i].ptr.f);
      break;
    case 'i':
      sprintf(val, "=%d", *(int *)cfgP[i].ptr.i);
      break;
    case 'l':
      sprintf(val, "=%ld", *(long *)cfgP[i].ptr.l);
      break;
    case 's':
      sprintf(val, "=%d", *(short *)cfgP[i].ptr.s);
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

