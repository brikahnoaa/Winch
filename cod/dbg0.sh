sed -i '/DBG0.*()"/s/DBG0("\(.*\)()".*)/static char *self="\1";\
  DBG()/' *.c
