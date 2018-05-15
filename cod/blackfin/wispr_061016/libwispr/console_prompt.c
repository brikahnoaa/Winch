/*
 */

#include <stdio.h>
#include <getopt.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>
#include <sys/syscall.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>


int main(int argc, char **argv)
{
	FILE *fp;
	int fd;
	int t, c, flags;
	int count, timeout=0;

        while ((t = getopt(argc, argv, "t:")) != EOF) {
           switch (t) {
              case 't': // number of adc samples
                  timeout = atoi(optarg);
                  break;
           }
        }

	fd = open("/dev/console", O_RDWR|O_NONBLOCK);
	fp = fdopen(fd, "w+");

	count = -1;
	//while((c = getchar()) != '\n') {
	while(count < timeout) {
	   c = fgetc(fp); //getchar();
	   if(c>0) putchar(c);
	   if(c == '\n') break;
	   if(timeout) {
	      //if(count >= timeout) break;
	      usleep(1);
	      count = count + 1;
	      //fprintf(stderr, "c = %d, count = %d\n", c, count);
	   }
	}
	//putchar('\n');

	fclose(fp);
	//fcntl(0, F_SETFL, flags);
	if(timeout == count) return(-1);
	return(1);
}

