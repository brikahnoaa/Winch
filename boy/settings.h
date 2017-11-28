typedef struct setVar {
  char *shortName;
  char *longName;
  void *varPtr;
  char typeCode;                // b, c, f, i, l, s
} setVar;

bool settingString(char *str);

