// settings and initial values
// 
// global setiNode *settings = linked list of name, ptr, type, value
// scan it for name when updating a setting

/* test */
#include <stdio.h>
#include <stdlib.h>
typedef char bool;              // pico thing
#include <string.h>
void testSettings();
void main(){ testSettings(); }
/* test */

/* test
#include <common.h>
#include <settings.h>
 */
#include <global.h>

typedef struct setiNode {
  struct setiNode *next;
  char *shortName;
  char *longName;
  void *varPtr;
  char typeCode;                // i, s, l, f, c
} setiNode;

setiNode *settings = NULL;

setiNode *settingPush(
    setiNode *head,
    char *shortName,
    char *longName,
    void *varPtr,
    char typeCode);
void settingUpdate( setiNode *s, char *valueStr);

/*
 * create a new setiNode at head of (global) settings linklist
 * settings=settingPush(settings, "ti", "testi", &test.i, 'i', "11");
 * :: return new head
 */
setiNode *settingPush(  
    setiNode *head,
    char *shortName,
    char *longName,
    void *varPtr,
    char typeCode) {
  setiNode *s;
  s=malloc(sizeof(setiNode));   // never freed
  s->shortName=shortName;
  s->longName=longName;
  s->varPtr=varPtr;
  s->typeCode=typeCode;
  s->next=head;
  return s;
} // settingPush

/*
 * input line is short or long name, =, value
 * find setiNode with name and set value
 * :: changes str
 */
int settingRead(setiNode *s, char *str){
  char *name, *val;
  //
  name=strtok(str, "=");
  val=strtok(NULL, "=");     // rest of string
  while (s) {               // scan settings list
    if (strstr(name, s->shortName) || strstr(name, s->longName)) {
      settingUpdate(s, val);
      return(0);
    }
    s=s->next;
  }
  return 1;                 // name not found
}

/*
 * convert valueStr to typeCode and poke into varPtr
 * used by settingPush and settingRead
 */
void settingUpdate( setiNode *s, char *valueStr) {
  void *varPtr=s->varPtr;
  switch (s->typeCode) {
  case 'b':     // bool is a char
    *(bool*)varPtr=(bool)atoi(valueStr);
    break;
  case 'c':
    strcpy((char*)varPtr, valueStr);
    break;
  case 'f':
    *(float*)varPtr=atof(valueStr);
    break;
  case 'i':
    *(int*)varPtr=atoi(valueStr);
    break;
  case 'l':
    *(long*)varPtr=(long)atoi(valueStr);
    break;
  case 's':
    *(short*)varPtr=(short)atoi(valueStr);
    break;
  }
} // settingUpdate

/*
 * used during devo test
 */
void testSettings(){
  struct {bool b; int i; short int s; float f; char c[8];
    } test = {0, 1, 2, 3.0, "4"};
  char str[32];
  printf ( "%d, %hd, %.1f, %s\n", test.i, test.s, test.f, test.c );
  settings=settingPush(settings, 
    "tf", "test.f", &test.f, 'f');
  settings=settingPush(settings, 
    "tc", "test.c", &test.c, 'c');
  settings=settingPush(settings, 
    "ti", "test.i", &test.i, 'i');
  settings=settingPush(settings, 
    "ts", "test.s", &test.s, 's');
  settings=settingPush(settings, 
    "tb", "test.b", &test.b, 'b');
  // update
  settingRead(settings, strcpy( str, "test.s=22"));
  settingRead(settings, strcpy( str, "tc=abc def"));
  printf ( "%d, %hd, %.1f, %s\n", test.i, test.s, test.f, test.c );
}

/*
 * set up all the possible settings here
 * reading from files or download is only for updates
 * s=settingPush(s,    // description
 *   "ti", "test.i", &test.i, 'i', "11");
 * :: return setting linklist
 */
setiNode *initSettings() {
  setiNode *s=NULL;
  s=settingPush(s,    // init value is a sanity check?
    "gg", "gpsLong", &ant.gpsLong, 'c');
  return s;
} // initSettings
