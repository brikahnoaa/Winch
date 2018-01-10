// settings.c

#include <common.h>
#include <settings.h>

#include <ant.h>
#include <boy.h>
#include <ctd.h>
#include <dat.h>
#include <mpc.h>
#include <ngk.h>
#include <sys.h>

/* 
 * static setVar settings[] = array of {shortname, longname, ptr, type}
 * scan it for name when updating a setting
 * { "dh", "boy.depth", &boy.depth, 'f'},
 *
 * try to use first,last char for shortname
 * use variable name for longname
 * &ptr can be any extern var or struct component
 * type := bool char* int float long short
 */
static setVar settings[] = {
  { "pD", "boy.platformID", &boy.platformID, 'c'},
  { "fm", "boy.filenum", &boy.filenum, 'f'},
  { "ms", "boy.maxStarts", &boy.maxStarts, 's'},
  { "on", "boy.on", &boy.on, 'b'},
  { "pD", "boy.projID", &boy.projID, 'c'},
  { "", "", NULL, 0},
};

static void settingUpdate( setVar *s, char *value );

/*
 * input line is short or long name, =, value
 * find setVar with name and set value
 * :: changes str
 */
bool settingString(char *str){
  // global settings
  setVar *s = settings;
  char *name, *val;
  //
  name=strtok(str, "=");
  val=strtok(NULL, "=");     // rest of string
  // find matching name
  while (s->varPtr != NULL) {
    if (strcmp(name, s->shortName)==0 || strcmp(name, s->longName)==0) {
      settingUpdate(s, val);
      return true;
    }
    s++;
  }
  return false;                 // name not found
}

/*
 * convert value to typeCode and poke into varPtr
 * used by settingPush and settingRead
 */
void settingUpdate(setVar *s, char *value ) {
  void *varPtr=s->varPtr;
  //
  switch (s->typeCode) {
  case 'b':     // bool is a char
    *(bool*)varPtr=(bool)atoi(value);
    break;
  case 'c':
    strcpy((char*)varPtr, value);
    break;
  case 'f':
    *(float*)varPtr=atof(value);
    break;
  case 'i':
    *(int*)varPtr=atoi(value);
    break;
  case 'l':
    *(long*)varPtr=(long)atoi(value);
    break;
  case 's':
    *(short*)varPtr=(short)atoi(value);
    break;
  }
} // settingUpdate

