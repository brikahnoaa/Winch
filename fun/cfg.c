// cfg.c
#include <com.h>
#include <cfg.h>

bool b;
char c[20];
int i;
float f;
int i;
long l;
short s;

/* 
 * static CfgParam cfg[] = array of {id, var, ptr, type}
 * scan it for name when updating a cfg
 * { "dh", "boy.depth", &boy.depth, 'f'},
 *
 * &ptr can be any extern var or struct component
 * type := bcifls bool char* int float long short
 */
static CfgParam cfg[] = {
  { "bbb", "b", &b, 'b'},
  { "ccc", "c", &c, 'c'},
  { "fff", "f", &f, 'f'},
  { "iii", "i", &i, 'i'},
  { "lll", "l", &l, 'l'},
  { "sss", "s", &s, 's'},
};
static int cfgLen = sizeof(cfg) / sizeof(CfgParam);

/*
 * input line is short or long name, =, value
 * find setVar with id or name, call cfgSet()
 * uses: cfg cfgLen
 */
bool cfgString(char *str){
  char *ref, *val;
  char s[80];
  strcpy(s, str);
  printf("str %s\n", s); //??
  ref=strtok(s, "=");
  if (ref==NULL) return false;
  val=strtok(NULL, "=");     // rest of string
  if (val==NULL) return false;
  printf("ref %s, val %s\n", ref, val); //??
  // find matching name
  for (int i=0; i<cfgLen; i++) {
    if (strcmp(ref, cfg[i].id)==0 || strcmp(ref, cfg[i].var)==0) {
      cfgSet(cfg[i].ptr, cfg[i].type, val);
      return true;
    }
  } // for cfg
  return false;                 // name not found
}

/*
 * convert *val to type and poke into *ptr
 */
static void cfgSet( void *ptr, char type, char *val ) {
  printf("\ncfgSet(%c, %s)", type, val); //??
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
    printf("\nERR\t| bad type"); //??
  }
} // cfgSet

void main(void) {
  printf("cfgLen %d, cfg[1].id %s\n", cfgLen, cfg[1].id);
  cfgString("b=true");
  cfgString("ccc=a b c");
  cfgString("f=01.23");
  cfgString("i=0123");
  cfgString("l=3");
  cfgString("s=4");
  cfgString("q=4");
  printf("\n%d %s %f %d %ld %d \n", b, c, f, i, l, s);
}
