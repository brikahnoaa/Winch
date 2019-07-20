#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

void PrintUsage(void);

/*========================================================================*/
/*                                                                        */
/*========================================================================*/
int main(int argc,char *argv[])
{
   char fwrev[32];
   time_t t=time(NULL);
   struct tm *tm=gmtime(&t);
   int c;
   
   while ((c = getopt(argc, argv, "eho?")) != -1)
   {  
      switch (c)
      {
         /* check if the FwRev should specify an even day of the month */
         case 'e':
         {
            if ((tm->tm_mday&0x0001))
            {
               if (tm->tm_mon==1 && tm->tm_mday>=29) tm->tm_mday=28;
               else if (tm->tm_mday>=31) tm->tm_mday=30;
               else tm->tm_mday+=1;
            }
            break;
         }

         case '?':
         case 'h': {PrintUsage(); break;}
         
         /* check if the FwRev should specify an odd day of the month */
         case 'o':
         {
            if (!(tm->tm_mday&0x0001))
            {
               if (tm->tm_mon==1 && tm->tm_mday>=28) tm->tm_mday=27;
               else if (tm->tm_mday>=30) tm->tm_mday=29;
               else tm->tm_mday+=1;
            }
            break;
         }
      }
   }
   
   t=mktime(tm); tm=localtime(&t);

   strftime(fwrev,sizeof(fwrev),"%m%d%y",tm);

   printf("%s\n",fwrev);

   return 0;
}

/*------------------------------------------------------------------------*/
/*                                                                        */
/*------------------------------------------------------------------------*/
void PrintUsage(void)
{
   printf("usage:FwRev -e -h -o -?\n"
          "   -e     FwRev should specify an even day of the month.\n"
          "   -o     FwRev should specify an odd day of the month.\n"
          "   -h,-?  Print this usage summary.\n"
          "Note: The FwRev will specify today's date if -o or -e\n"
                 "options are not specified.\n\n");

   exit(0);
}

