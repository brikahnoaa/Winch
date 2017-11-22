// settings and initial values
// 
// struct seti used in settings is a dynamic structure, not compile time
//

#include <stdio.h>
#include <stdlib.h>
void main(){}
// #include <common.h>
// #include <settings.h>

typedef struct setiNode {
  char *shortName;
  char *longName;
  void *varPtr;
  char *valueStr;
  char typeCode;
  struct setiNode *next;
} setiNode;

setiNode *settings = NULL;

setiNode *settingPush(
  setiNode *head,
  char *shortName,
  char *longName,
  void *varPtr,
  char *valueStr,
  char typeCode
  );
void settingUpdate(
  setiNode *s,
  char *shortName,
  char *longName,
  void *varPtr,
  char *valueStr,
  char typeCode
  );


/*
 * create a new setiNode at head of (global) settings linklist
 *   settings=settingPush(settings, "ti", "testi", &test.i, "11", 'i');
 */
setiNode *settingPush(  
  setiNode *head,
  char *shortName,
  char *longName,
  void *varPtr,
  char *valueStr,
  char typeCode
  ) {
  setiNode *s;
  s=malloc(sizeof(setiNode));
  s->next=head;
  //
  settingUpdate(s, shortName, longName, varPtr, valueStr, typeCode);
  return s;
} // settingPush

/*
 * convert valueStr to typeCode and poke into varPtr
 * used by settingPush and settingRead
 */
void settingUpdate(
  setiNode *s,
  char *shortName,
  char *longName,
  void *varPtr,
  char *valueStr,
  char typeCode
  ) {
  s->longName=longName;
  s->shortName=shortName;
  s->varPtr=varPtr;
  s->valueStr=valueStr;
  s->typeCode=typeCode;
  switch (typeCode) {
  case 'i':
    *(int*)varPtr=atoi(valueStr);
    break;
  case 'l':
    *(long*)varPtr=(long)atoi(valueStr);
    break;
  case 's':
    *(short*)varPtr=(short)atoi(valueStr);
    break;
  case 'f':
    *(float*)varPtr=atof(valueStr);
    break;
  case 'c':
    *(char**)varPtr=valueStr;
    break;
  }
} // settingUpdate
