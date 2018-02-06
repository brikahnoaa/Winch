// cfg.c
#include <com.h>
#include <cfg.h>

#include <ant.h>
#include <boy.h>
#include <ctd.h>
#include <mpc.h>
#include <ngk.h>
#include <sys.h>
#include <wsp.h>

// 
// static CfgParam cfg[] = array of {id, var, ptr, type}
// scan it for name when updating a cfg
// { "dh", "boy.depth", &boy.depth, 'f'},
//
// &ptr can be any extern var or struct component
// type := bcifls bool char* int float long short
//
static CfgParam cfg[] = {
  { "lng", "ant.gpsLong", &ant.gpsLong, 'c'},
  { "lat", "ant.gpsLat", &ant.gpsLat, 'c'},
  { "suD", "ant.surfaceD", &ant.surfaceD, 'f'},
  { "cuD", "boy.currCheckD", &boy.currCheckD, 'f'},
  { "cuM", "boy.currMax", &boy.currMax, 'f'},
  { "caF", "boy.callFreq", &boy.callFreq, 'i'},
  { "caH", "boy.callHour", &boy.callHour, 'i'},
  { "fiN", "boy.fileNum", &boy.fileNum, 'i'},
  { "fiP", "boy.firstPhase", &boy.firstPhase, 'i'},
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
  strcpy(s, str);
  ref=strtok(s, "=");
  if (ref==NULL) return false;
  val=strtok(NULL, "=");     // rest of string
  if (val==NULL) return false;
  // find matching name
  for (int i=0; i<cfgLen; i++) {
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
