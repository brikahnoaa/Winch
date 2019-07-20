#include <stdio.h>
#include <stdlib.h>
#include <lbt9522.h>
#include <linux.h>
#include <string.h>
#include <logger.h>
#include <defs.p>
#include <limits.h>

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * $Id: Lbt9522IccidMsisdn.c,v 1.2 2007/06/08 02:17:31 swift Exp $
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * RCS Log:
 *
 * $Log: Lbt9522IccidMsisdn.c,v $
 * Revision 1.2  2007/06/08 02:17:31  swift
 * Added specification for maximum log size.
 *
 * Revision 1.1  2007/03/26 21:42:16  swift
 * Added utility to facilitate management of SIM card ICCID/MSISDN numbers.
 *
 *------------------------------------------------------------------------*/
/*                                                                        */
/*------------------------------------------------------------------------*/
int main(int argc,char *argv[])
{
   char iccid[64],msisdn[64];
   int status=1;
   const struct SerialPort *port=&com[0];
   enum CMDS {iccid_,msisdn_,com_,q_,NCMD};
   struct MetaCmd cmd[]={
      {"iccid=","  The 19-digit ICCID (ie., serial number) of the SIM card."},
      {"msisdn="," The 12-digit MSISDN (ie., phone number) of the SIM card."},
      {"com=","    Serial port selection (1-4). [1]"},
      {"-q","      Query the SIM card for the ICCID & MSISDN numbers."}};
   
   debugbits=2; MaxLogSize=LONG_MAX;
      
   /* check for usage query */
   if (argc<2) 
   { 
      fprintf(stderr,"usage: %s %s\n",argv[0],make_usage(cmd,NCMD));
      exit(1);
   }

   /* link the metacommands to the command line arguments */
   link_meta_cmds(cmd,NCMD,argc,argv);

   if (cmd[com_].arg)
   {
      switch (atoi(cmd[com_].arg))
      {
         case 1: {port=&com[0]; break;}
         case 2: {port=&com[1]; break;}
         case 3: {port=&com[2]; break;}
         case 4: {port=&com[3]; break;}
         default:
         {
            fprintf(stderr,"Invalid serial port: %s\n",cmd[com_].arg);
            exit(1);
         }
      }
   }

   if (cmd[q_].arg)
   {
      if (IrModemIccid(port,iccid,msisdn,sizeof(iccid)-1)>0)
      {
         printf("ICCID: %s    MSISDN: %s\n",iccid,msisdn); status=0;
      }
      else {fprintf(stderr,"Attempt to read ICCID/MSISDN from SIM card failed.\n");}
   }
   
   else if (!cmd[iccid_].arg)
   {
      fprintf(stderr,"Missing ICCID number.\n"
              "usage: %s %s\n",argv[0],make_usage(cmd,NCMD));
   }

   else if (!cmd[msisdn_].arg)
   {
      fprintf(stderr,"Missing MSISDN number.\n"
              "usage: %s %s\n",argv[0],make_usage(cmd,NCMD));
   }
   
   else if (IrModemIccidSet(port,cmd[iccid_].arg,cmd[msisdn_].arg)>0)
   {
      if (IrModemIccid(port,iccid,msisdn,sizeof(iccid)-1)>0)
      {
         if (strcmp(cmd[iccid_].arg,iccid))
         {
            fprintf(stderr,"ICCID mismatch when read from SIM card: %s!=%s\n.",
                    cmd[iccid_].arg,iccid);
         }
         
         else if (strcmp(cmd[msisdn_].arg,msisdn)) 
         {
            fprintf(stderr,"MSISDN mismatch when read from SIM card: %s!=%s\n.",
                    cmd[msisdn_].arg,msisdn);
         }

         else
         {
            printf("ICCID(%s) and MSISDN(%s) written to SIM card.\n",
                   cmd[iccid_].arg,cmd[msisdn_].arg); status=0;
         }
      }
      
      else {fprintf(stderr,"Attempt to read ICCID/MSISDN from SIM card failed.\n");}
   }
   
   else {fprintf(stderr,"Attempt to write ICCID/MSISDN to SIM card failed.\n");}
   
   return status;
}
