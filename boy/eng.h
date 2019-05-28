// eng.h
#ifndef H_ENG
#define H_ENG

typedef struct EngInfo {
  char *me;
  int log;
} EngInfo;

int engInit(void);
int engLogOpen(void);
int engLogClose(void);

#endif
