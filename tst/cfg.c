// cfg.c
#include <main.h>

void main(void){
  static char *rets="1=noEqualSign 2=noMatchName";
  char c, str[128];
  int i=0, r=0;
  sysInit();

  flogf("\nQ=quit Z=dump\n");
  while (true) {
    if (cgetq()) {
      c = cgetc();
      if (c=='Q') break;
      if (c=='Z') {
        cfgDump();
        continue;
      }
      if (c=='\r' || c=='\n') {
        flogf("\n");
        str[++i] = 0;
        r = cfgString(str);
        if (r) flogf("\n%d: %s", rets);
        i = 0;
        continue;
      }
      str[++i] = c;
      cputc(c);
    }
  }
}
