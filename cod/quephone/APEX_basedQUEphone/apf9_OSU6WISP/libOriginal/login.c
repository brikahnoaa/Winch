#ifndef LOGIN_H
#define LOGIN_H

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * $Id: login.c,v 1.2 2007/04/24 01:43:29 swift Exp $
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/* Copyright University of Washington.   Written by Dana Swift.
 *
 * This software was developed at the University of Washington using funds
 * generously provided by the US Office of Naval Research, the National
 * Science Foundation, and NOAA.
 *  
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 2.1 of the License, or (at
 * your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser
 * General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */
/** RCS log of revisions to the C source code.
 *
 * \begin{verbatim}
 * $Log: login.c,v $
 * Password is now platform ID. Change it by i*PQ001 command, which makes 
 * platform ID to Q001. HM Jan 11, 2010
 * 
 * Login changed heavily to incorporate the NOAA buoy-Rudics protocol HM, LM 10/1/2009
 *
 * Revision 1.2  2007/04/24 01:43:29  swift
 * Added acknowledgement and funding attribution.
 *
 * Revision 1.1  2004/12/29 23:11:27  swift
 * Modified LogEntry() to use strings stored in the CODE segment.  This saves
 * lots of space in the DATA segment and significantly speeds code start-up.
 *
 * \end{verbatim}
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
#define loginChangeLog "$RCSfile: login.c,v $  $Revision: 1.2 $   $Date: 2007/04/24 01:43:29 $"
//#define SYSNAME "QUEHQ002"  //HM

#include <serial.h>

/* function prototypes for external functions */
int login(const struct SerialPort *port, const char *user, const char *pwd);
int logout(const struct SerialPort *port);
int Calc_Crc(unsigned char *buf, int cnt);
void itoa(int a, unsigned char *b);

#endif /* LOGIN_H */

#include <ctype.h>
#include <string.h>
#include <chat.h>
#include <expect.h>
#include <logger.h>
#include <apf9.h>

/* define the prompt to set on the host computer */
#define CMD "cmd>"

/* function prototypes */
unsigned int sleep(unsigned int seconds);

/*------------------------------------------------------------------------*/
/* function to log into a remote computer via the serial port             */
/*------------------------------------------------------------------------*/
/**
   This function logs into a remote computer connected via a serial port.
   It reads from the serial port searching for the log-in and password
   prompts.  Each prompt is responded to with a user-specified response
   strings.

      \begin{verbatim}
      input:
         
         port...A structure that contains pointers to machine dependent
                primitive IO functions.  See the comment section of the
                SerialPort structure for details.  The function checks to be
                sure this pointer is not NULL.

         user...The string transmitted when responding to the login prompt.

         pwd....The string transmitted when responding to the password prompt.

      output:

         This function returns a positive number if the login was
         successful.  Zero is returned if the login failed.  A negative
         number is returned if the function arguments are determined to be
         ill-defined. 
      
      \end{verbatim}
*/
int login(const struct SerialPort *port,const char *project_id, const char *platform_id)
{
   /* function name for log entries */
   static cc FuncName[] = "login()";
   
   /* initialize the return value of this function */
   int status = -1;
  
   /* validate the serialport */
   if (!port) 
   {
      /* create the message */
      static cc msg[]="Serial port not ready.\n";

      /* make the logentry */
      LogEntry(FuncName,msg);
   }
 
   /* verify user name */
   else if (!project_id)
   {
      /* create the message */
      static cc msg[]="NULL pointer to the project id.\n";

      /* make the logentry */
      LogEntry(FuncName,msg);
   }

   /* verify the password */
   else if (!platform_id)
   {
      /* create the message */
      static cc msg[]="NULL pointer to the platform id.\n";

      /* make the logentry */
      LogEntry(FuncName,msg);
   }

   /* validate the serial port's putb() function */
   else if (!port->putb) 
   {
      /* create the message */
      static cc msg[]="NULL putb() function for serial port.\n";

      /* make the logentry */
      LogEntry(FuncName,msg);
   }

   else
   {
      /* define the timeout period (sec) */
      const time_t timeout=30;
      
      /* re-initialize the function's return value */
      status=0;

      /* flush the IO buffers as an initialization step */
      if ((status=pflushio(port))<=0)
      {
         /* create the message */
         static cc msg[]="Attempt to flush IO buffers failed.\n";

         /* make the logentry */
         LogEntry(FuncName,msg);
      }

      else
      {
		 	 
         int num_tries = 0;
         int temp_flag = 0;
         
         int crc, crc1, crc2, result;
         char  buf[14];
		 unsigned char proj[8];

         //LogAdd("Platform_id = %s\n", platform_id);
         //sprintf(proj, "QUEH%uc%uc%uc%uc", (unsigned char) platform_id);//This needs to change for every float e.g., 0002, 0003. HM
         sprintf(proj, "QUEH");
		 //Passwd is the platform ID HM (e.g., Q002)
		 proj[4]=platform_id[0];
		 proj[5]=platform_id[1];
		 proj[6]=platform_id[2];
		 proj[7]=platform_id[3];

		 /* make the logentry */
           // LogAdd("SYSNAME = %s\n",proj);//HM Test Remove this 
		 
         crc = Calc_Crc(proj, 8);
         crc1 = crc;
         crc2 = crc;
         
         sprintf(buf,"???%c%c%c%c%c%c%c%c%c%c",(unsigned char)((crc1>>8) & 0x00FF), (unsigned char)(crc2 & 0x00FF),proj[0],proj[1],proj[2],proj[3],proj[4],proj[5],proj[6],proj[7]);
         		
         /* give 8 chances to login before aborting */	
         /* check if carrier-dectect enabled and CD line not asserted */
         if (port->cd && !port->cd())
         {
            /* create the message */
            static cc msg[]="No carrier detected.\n";

            /* make the logentry */
            LogEntry(FuncName,msg);
         }
		 //No need this extra CR HM
         /* write a line-feed to the serial port to initiate the login sequence */
         //if (port->putb('\n')<=0)
         //{
            /* create the message */
         //   static cc msg[]="Attempt to write to serial port failed.\n";

            /* make the logentry */
         //   LogEntry(FuncName,msg);
         //} 
		 // Doubt if this line feed is needed HM
         
		 Wait(1000);
         result=0;
		 buf[13]=0x00;//Null terminator
         while (num_tries < 4 && temp_flag == 0) {
			result=pputs(port,buf, timeout,"");
			//for(i=0;i<13;i++)pputb(port, buf[i],100);//HM
			//Wait(1); //HM //need to fiddle with this wait period.  Still login takes 2 attempts 
            if ( result <= 0) {
              /* create the message */
              static cc msg[]="TX of platform ID failed.\n";
			  /* make the logentry */
              LogEntry(FuncName,msg);
            } else if (expect(port,"ACK", "", 20, "") <= 0) {
              //static cc msg[]="ACK failed.\n";                 
              Wait(1000); //HM
              //LogEntry(FuncName, msg);
              num_tries++;
			  status=0; // No ACK.  Terminate TX.
            } else {
				//pputs(port,"",10,"n\");
				static cc msg[]="ACK rcvd.\n";
				LogEntry(FuncName, msg);
				temp_flag = 1;
            }
         }
         
         /* re-initialize return value */
         if (temp_flag == 1)
         {
            /* create the message */
            static cc msg[]="TX of platform ID (%s) success. \n";

            /* make the logentry */
            LogEntry(FuncName,msg, platform_id);

            /* indicate success */
            status = 1;
         }
      }
   }
   return status;
}

void itoa(int a, unsigned char *b) {
   b[0] = (a & 0xFF00) >> 8;
   b[1] = (a & 0x00FF);
}

/*------------------------------------------------------------------------*/
/* function to logout of the remote computer                              */
/*------------------------------------------------------------------------*/
/**
   This function logs out of the remote computer attached to the serial
   port.  Unfortunately, there is no way to verify that the logout was
   actually successful.

      \begin{verbatim}
      input:
               
         port...A structure that contains pointers to machine dependent
                primitive IO functions.  See the comment section of the
                SerialPort structure for details.  The function checks to be
                sure this pointer is not NULL.

      output:

         This function returns a positive number if successful.  Zero is
         returned if the attempt failed.  A negative number is returned if
         the function argument was determined to be ill-defined.
      
      \end{verbatim}
*/
int logout(const struct SerialPort *port)
{
   /* function name for log entries */
   static cc FuncName[] = "logout()";

   /* initialize return value */
   int status = -1;
   
   /* validate the serialport */
   if (!port)
   {
      /* create the message */
      static cc msg[]="Serial port not ready.\n";

      /* make the logentry */
      LogEntry(FuncName,msg);
   }

   /* validate the serial port's pputb() function */
   else if (!port->putb)
   {
      /* create the message */
      static cc msg[]="NULL pputb() function for serial port.\n";

      /* make the logentry */
      LogEntry(FuncName,msg);
   }
   
   /* write the logout command */
   else
   {
      int i;

      /* define the timeout period (sec) */
      const time_t timeout=5;

      for (status=0,i=0; i<3; i++)
      {
         /* check if carrier-dectect enabled and CD line not asserted */
         if (port->cd && !port->cd())
         {
            /* create the message */
            static cc msg[]="No carrier detected.\n";

            /* make the logentry */
            LogEntry(FuncName,msg);

            /* indicate failure */
            status=0; break;
         }
         
         if (chat(port,"\003\003\003\003\003\r",CMD,timeout,"\n")<=0)
         {
            /* create the message */
            static cc msg[]="Can't get command prompt.\n";

            /* make the logentry */
            LogEntry(FuncName,msg);
         }
         
         if (chat(port,"exit\r","logout",timeout,"\n")<=0)
         {
            /* create the message */
            static cc msg[]="Attempt to log-out failed.\n";

            /* make the logentry */
            LogEntry(FuncName,msg);
         }
         else
         {
            /* create the message */
            static cc msg[]="Log-out successful.\n";

            /* make the logentry */
            LogEntry(FuncName,msg);

            /* indicate success */
            status=1; sleep(1); break;
         }
      }
   }
   
   return status;
}

int Calc_Crc(unsigned char *buf, int cnt)
	/* calculate 16-bit CRC of contents of buf */
{
	long accum;
	int i, j;
	accum=0x00000000;
   
	if(cnt <= 0) return 0;
	while( cnt-- )
	{
		accum |= *buf++ & 0xFF;
		for( i=0; i<8; i++ )
		{
			accum <<= 1;
			if(accum & 0x01000000)  accum ^= 0x00102100;
		}
	}
	/* The next 2 lines forces compatibility with XMODEM CRC */
	for(j = 0; j<2; j++)
	{
		accum |= 0 & 0xFF;
		for( i=0; i<8; i++ )
		{
			accum <<= 1;
			if(accum & 0x01000000)  accum ^= 0x00102100;
		}
	}
	//cprintf("%x%x\n",(char)((accum >>16)  & 0x000000FF),(char)((accum >>8) & 0x000000FF));
	//RTCDelayMicroSeconds(100000L);
	return( accum >> 8 );
	}
