// tst.c
#include<com.h>

/*
typedef struct dirent
{
  uchar d_name[13];		// expanded file name
  uchar d_attr;			// file/entry attributes
  struct tm d_tm;		// modification date in C time
  ulong d_size;			// actual size in bytes
  ushort d_clustsizeF16;	// 2.2x legacy support
  ushort d_stclusterF16;	// 2.2x legacy support
  void *d_link;			// used internally
  ulong d_clustsize;		// cluster size in bytes
  ulong d_stcluster;		// starting cluster
} DIRENT;
 */

int main(void) {
  struct dirent de;
  short err;
  char *path="c:\\test";
  err = DIRFindFirst(path, &de);
  while (err!=dsdEndOfDir) {
    if (err!=0) {
      flogf("unknown dir err\n");
      return -1;
    }
    cprintf("name: %s, size: %ld, attr: %d\n", de.d_name, de.d_attr, de.d_size);
    DIRFindNext(&de);
  }
  return 0;
}
