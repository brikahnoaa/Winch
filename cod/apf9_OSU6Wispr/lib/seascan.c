#ifndef SEASCAN_H
#define SEASCAN_H (0x0008U)

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * $Id: seascan.c,v 1.1.2.1 2008/09/11 19:49:30 dbliudnikas Exp $
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/** RCS log of revisions to the C source code.
 *
 * \begin{verbatim}
 * $Log: seascan.c,v $
 * Revision 1.1.2.1  2008/09/11 19:49:30  dbliudnikas
 * Initial version of Seascan TD sensor interface.
 *
 *
 * \end{verbatim}
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
#define seascanChangeLog "$RCSfile: seascan.c,v $ $Revision: 1.1.2.1 $ $Date: 2008/09/11 19:49:30 $"

#include <serial.h>
#include <stdio.h>

/* function prototypes */
int    SeascanEnterCmdMode(char *SerNo, float *p);
int    SeascanExitCmdMode(void);
int    SeascanGetP(float *p);
int    SeascanGetPt(float *p, float *t);
int    SeascanSerialNumber(char *serStr);

/* define the return states of the Seascan API */
extern const char SeascanTooFew;          /* Too few samples */
extern const char SeascanChatFail;        /* Failed chat attempt. */
extern const char SeascanNoResponse;      /* No response received from Seascan. */
extern const char SeascanRegExceptn;      /* Response received, regexec() exception */
extern const char SeascanRegexFail;       /* response received, regex no-match */
extern const char SeascanNullArg;         /* Null function argument. */
extern const char SeascanFail;            /* General failure */
extern const char SeascanOk;              /* response received, regex match */
extern const char SeascanPedanticFail;    /* response received, pedantic regex no-match */
extern const char SeascanPedanticExceptn; /* response received, pedantic regex exception */

/* define the maximum length of the Seascan response */
#define MAXLEN 80

#define ETX  0x03 /* ETX character */
#define NUL  0x00 /* NULL character */
#define LF   0x0a /* LINE FEED character */

#define SERNOSIZE 5 /* size of serial number +1 for null terminate */
#define PTSIZE 7 /* size of temp/pressure + 1 for null terminate */

#endif /* SEASCAN_H */

#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <logger.h>
#include <regex.h>
#include <extract.h>
#include <assert.h>
#include <ctdio.h>
#include <nan.h>
#include <ctype.h>
#include <strtoul.h>
#include <psd835.h>

#ifdef _XA_
   #include <apf9.h>
#else
   #define WatchDog()
   #define StackOk() 1
#endif /* _XA_ */

/* define the return states of the Seascan API */
const char SeascanTooFew          = -6; /* Too few CP samples */
const char SeascanChatFail        = -5; /* Failed chat attempt. */
const char SeascanNoResponse      = -4; /* No response received from Seascan. */
const char SeascanRegExceptn      = -3; /* Response received, regexec() exception */
const char SeascanRegexFail       = -2; /* response received, regex no-match */
const char SeascanNullArg         = -1; /* Null function argument. */
const char SeascanFail            =  0; /* General failure */
const char SeascanOk              =  1; /* General success */
const char SeascanPedanticFail    =  2; /* response received, pedantic regex no-match */
const char SeascanPedanticExceptn =  3; /* response received, pedantic regex exception */

/* external function declarations */
extern void Wait(unsigned int millisec);
extern int snprintf(char *str, size_t size, const char *format, ...);

/**************************************************************************/
/* The Seascan TD module reponds to a generic address #A<cr> and returns  */
/* its serial number followed by pressure read on power up and a prompt.  */
/* This mode gives access to two commands:                                */
/*   +   Will return a new fast pressure value (NOT IMPLEMENTED)          */
/*   -   Will return a full precision pair of values for P and T          */
/* All data are returned in hex.                                          */
/* The physical interface can also use a different connector (3M ten-point*/
/* header) which separate the transmit and receive lines for communication*/
/* with the external controller, using logic level UART signals.  In this */
/* mode, any character transmitted by the module should be echoed back by */
/* the controller.  The interface lines are as follows:                   */
/*   Pin 1: Shut down (active low, connect to Vbat>V>2V to turn module on)*/
/*   Pin 2: Module transmit line, open collector, needs pull-up           */
/*   Pin 4: Module receive line, should rest high with V= 3 to 15 V       */
/*   Pin 5: Vbat, main supply voltage (3.8 to 15V)                        */
/*   Pin 6: Ground                                                        */
/**************************************************************************/

/**************************************************************************/
/* From a hardware standpoint, the SBE41 CTD interface provides necessary */
/* lines.  Specifically, the CTD_PWR_CTRL line is connected to pin 1 and  */
/* must be asserted (CtdAssertWakePin).  After a power up wait period,    */
/* the ctd serial interface fifos are flushed (ctd.ioflush) to avoid a    */
/* potential break detection.  After exchanging characters (as described  */
/* below, the sensor is powered down (CtdClearWakePin).
/**************************************************************************/

/**************************************************************************/
/* Seascan TD sensor interface is simplified from the SBE41 CTD interface */
/* The serial protocol is as follows:                                     */
/* Enable CTD IO.  Power on the TD.  Wait a bit.  Flush the CTD IO.       */
/* Send serial command "#A<cr>"                                           */
/* Enter loop to read characters from the TD and echo them back.          */
/*    Expected format: <lf><sp>A955<sp>FFFFFF<cr><lf><sp>'>'<etx>         */
/*    where A955 is the serial number                                     */
/*          FFFFFF is the power on pressure in centibars                  */
/*    If string is valid, parse and return the ASCII serial number and    */
/*    the pressure in floating point decibars (/10)                       */
/* Send the PT measurement command: '-'                                   */
/* Enter loop to read characters from the TD and echo them back.          */
/*    Expected format: <sp>FFFFFD<sp>000626<cr><lf><sp>'>'<etx>           */
/*    If string is valid, parse and return                                */
/*    the pressure in floating point decibars (/10) and                   */
/*    the temperature in floating point deg C (/1000)                     */
/* Disable CTD IO.  Power down the TD.  Flush the CTD IO buffers          */
/**************************************************************************/

/**************************************************************************/
/* Notes:                                                                 */
/* 1. Due to the nature of the Seascan interface (character echo), the    */
/*    code is not consistent with existing software.  The inline calls    */
/*    were intentionlly kept simple to aid debugging.                     */
/*                                                                        */
/* 2. The SeascanGetp herein returns the power up pressure and the fast P */
/*    '+' command was not implemented by Seascan.  To be safe, the GetP   */
/*    routine in profile.c (interface to routines herein), should do a    */
/*    SeascanGetPT to ensure a current presssure reading.  
/**************************************************************************/



/* Routine to convert the 6 character hex seascan pressure in 
   centibars to floating point decibars.
*/
float convertSeascanP(char *pStr)
{
   long int pint;
   float p;
   
   /* convert the hex to decimal */
   pint = strtol(pStr,NULL,16);
   /* convert to 2s complement */
   if (pint > 0x7fffff)
   {
      pint = pint - 0x1000000;
   }
   p = pint / 10.0; /* convert centibars to decibars */
   
   return (p);
}

/* Routine to convert the 6 character hex seascan temperature in 
   milli degs C to floating point degs C. 
*/
float convertSeascanT( char *tStr)
{
   long int tint;
   float t;
   
   /* convert the hex to decimal */
   tint = strtol(tStr,NULL,16);
   /* convert to 2s complement */
   if (tint > 0x7fffff)
   {
      tint = tint - 0x1000000;
   }
   t = tint / 1000.0; /* convert milli degs C to degs C */

   
   return (t);
}

/* Routine to parse the command mode response string for serial number and power up pressure */
int SeascanParseSerP( char *rspStr, int rspLen, char *serStr, float *p )
{
   int i;
   char pStr[PTSIZE];
   int status = SeascanOk, pStatus = SeascanOk, sStatus = SeascanOk;

   /* define the logging signature */
   static cc FuncName[] = "SeascanParseSerP()";
 
   if (debuglevel>=4)
   {
      static cc format[]="SerP Rx String [%d %s]\n";
      LogEntry(FuncName,format,rspLen, rspStr);
   }
   
   /* <lf><sp>A955<sp>FFFFFF<cr><lf><sp>'>'*/   
   /* Data format is fixed */
   /* Make sure at least 14 characters (containing P and T are sent */
   /* Copy the 6 character P and T and convert: 2's complement, units */
   if (rspLen < PTSIZE+SERNOSIZE+2)
   {
      status = SeascanTooFew; /* Not enough returned data */
      *p = nan();
       serStr[0] = NUL;
   }
   else
   {
       /* copy 4 character serial number from string at 3rd location */
       strncpy( serStr, rspStr+2,SERNOSIZE-1);
       serStr[SERNOSIZE-1] = NUL;
       
       /* copy 6 character pressure from string at 7th location */
       strncpy( pStr, rspStr+7,PTSIZE-1);
       pStr[PTSIZE-1] = NUL;
       
      if (debuglevel>=4)
      {
         static cc format[]="Seascan Serial Number [%s] \n";   
         LogEntry(FuncName,format, serStr);
      }

     *p=nan();
      for (i=0; i<PTSIZE-1; i++)
      {
         if (!isxdigit(pStr[i]))
            pStatus = SeascanRegexFail;
      }
      if (pStatus == SeascanOk)
      {
         /* convert the ascii data to a float */
         *p = convertSeascanP(pStr);
         if (debuglevel>=4)
         {
            static cc format[]="Seascan P [%s %1.2f] \n";   
            LogEntry(FuncName,format, pStr, *p);
         }
      }
       else pStatus = SeascanRegexFail;               
   }
 
   /* Set overall status if not already set and pressure or temperature string failure */
   if ( status == SeascanOk &&
       (pStatus == SeascanRegexFail || sStatus == SeascanRegexFail ) )
       status = SeascanRegexFail;   
   
   return (status); 
}

/* Routine to parse the PT response string for pressure and temperature */
int SeascanParsePT( char *ptStr, int ptLen, float *p, float *t)
{
   int i;
   char pStr[PTSIZE], tStr[PTSIZE];
   int status = SeascanOk, pStatus = SeascanOk, tStatus = SeascanOk;

   /* define the logging signature */
   static cc FuncName[] = "SeascanParsePT()";
      
   if (debuglevel>=4)
   {
      static cc format[]="Rx PT String [%d %s]\n";
      LogEntry(FuncName,format,ptLen, ptStr);
   }
   
   /* <sp>FFFFFD<sp>000626<cr><lf><sp>'>'<etx>  */
   /* Data format is fixed */
   /* Make sure at least 14 characters (containing P and T are sent */
   /* Copy the 6 character P and T and convert: 2's complement, units */
   if (ptLen < PTSIZE*2)
   {
      status = SeascanTooFew; /* Not enough returned data */
      *p = nan();
      *t = nan();
   }
   else
   {
       /* copy 6 character pressure from string at 2nd location */
       strncpy( pStr, ptStr+1,PTSIZE-1);
       pStr[PTSIZE-1] = NUL;
       /* copy 6 character temperature from string at 8th location */
       strncpy( tStr, ptStr+8,PTSIZE-1);
       tStr[PTSIZE-1] = NUL;
       
       if (debuglevel>=4)
       {
          static cc format[]="P T String [%s %s]\n";
          LogEntry(FuncName,format, pStr, tStr);
       }
       
       /* check if data is hex and if so, send to get translated */
       *p=nan();
       for (i=0; i<PTSIZE-1; i++)
       {
          if (!isxdigit(pStr[i]))
             pStatus = SeascanRegexFail;
       }
       if (pStatus == SeascanOk)
       {
         /* convert the ascii data to a float */
         *p = convertSeascanP(pStr);
          if (debuglevel>=4)
          {
             static cc format[]="Seascan P [%s %1.2f] \n";   
             LogEntry(FuncName,format, pStr, *p);
          }
       }
       else pStatus = SeascanRegexFail;
 
       /* check if data is hex and if so, send to get translated */
       *t=nan();
       for (i=0; i<PTSIZE-1; i++)
       {
          if (!isxdigit(tStr[i]))
             tStatus = SeascanRegexFail;
       }
       if (tStatus == SeascanOk)
       {
         /* convert the ascii data to a float */
         *t = convertSeascanT(tStr);
          if (debuglevel>=4)
          {
             static cc format[]="Seascan T [%s %1.2f] \n";   
             LogEntry(FuncName,format, tStr, *t);
          }
      } 
      else pStatus = SeascanRegexFail;               
   }
    
   /* Set overall status if not already set and pressure or temperature string failure */
   if ( status == SeascanOk &&
       (pStatus == SeascanRegexFail || tStatus == SeascanRegexFail ) )
       status = SeascanRegexFail;   
   
   return (status); 
}

/*------------------------------------------------------------------------*/
/* function to enter the Seascan's command mode                           */
/*------------------------------------------------------------------------*/
/**
   This function wakes the Seascan and places it in command mode.  It does
   this by powering up the seascan, sending the #A command, and echoing the
   each character back until ETX is read (or timeout).  On command, the 
   Seascan serial number and power up pressure are returned automatically.

      \begin{verbatim}
      output:

 
         size.....This is the number of bytes (including the 0x0
                  string terminator) ritten into the SerNo
                  function argument.

         SerNo....The Serial Number string will be written into this
                  buffer.
         
         p........Pointer to the power up pressure.
 
         This function returns status.
         Here are the possible return values of this function:
         SeascanOk................No error.
         SeascanNoResponse........No response received from Seascan.
         SeascanRegExceptn........Response received but regexec() failed with
                                     an exceptional error.
         SeascanNullArg...........Null function argument.
         SeascanRegexFail.........Response received but it did not match the
                                     regex pattern for the serial number.
      \end{verbatim}
*/
int SeascanEnterCmdMode(char *serNo, float *p)
{ 
   char rspStr[MAXLEN+1];  /* store the characters into a response string */
   int rspLen = 0;
   int status = SeascanOk, stat;
   unsigned char ibyte;
   int n=0, nMax=200;
   int ntrys=0, ntrysMax=3;

   /* pet the watchdog */
   WatchDog();

   /* stack-check assertion */
   assert(StackOk());

   /* assert that buffer-flushing services exist on ctdio */
   assert(ctdio.ioflush());
   
   /*Enable Seascan TD IO */
   CtdEnableIo();
   
   /* Power on the Seascan TD */
   CtdAssertWakePin();
      
   Wait(1000);
   /* Flush the IO queues again in case break received */
   if (ctdio.ioflush) {Wait(10); ctdio.ioflush(); Wait(10);}
         
   /* Retry getting a valid response from the Seascan */
   /* Detected breaks should be cleared by io flush */
   while (ntrys < ntrysMax )
   {
      //if (ntrys>0 && debuglevel>=2)  putchar_CRLF('Z'); /*debug output*/
      ntrys++;
 
      /* Put the seabird in cmd mode */
      ctdio.putb('#'); Wait(10); 
      ctdio.putb('A'); Wait(10); 
      ctdio.putb('\r'); Wait(10);    
   
      /* Read characters and echo back until ETX is detected */
      /* Loop iteration and wait makes > 2 second response time */
      for (n=0; n<nMax; n++)
      {
         if ( (stat = ctdio.getb(&ibyte)) >0 )
         {
            /* Check if expected 1st char LF not received, i.e. break which has been seen */
            if (rspLen ==0 && ibyte != LF ) 
            {
               /* Toggle the power lines, flush the io buffer and retry */
               CtdClearWakePin();
               Wait (100);
               CtdAssertWakePin();
               Wait (100);
               ctdio.ioflush();
               //if (debuglevel>=2) { putchar_CRLF('X');putchar_CRLF(ibyte); }
               n=nMax+1;
               break;
             }
             else
             {
               /* got a byte, echo the byte to the Seascan TD */
               ctdio.putb(ibyte); 
               /* If etx, terminate the string and break out of loop */
               if (ibyte == ETX)  
               { 
                  rspStr[rspLen]=NUL; 
                  ntrys=ntrysMax+1; break;
               }
               /* store the byte in the response string */
               rspStr[rspLen] = ibyte; if (rspLen<MAXLEN) rspLen++;
            }
         }
         Wait(10);
      }
   }
   
   /* Make size real, not 0base index */
   if (rspLen>0) rspLen++;
       
   /* If got at least one character and havent't exceed buffer, check for data */
   if (rspLen==0)
   {
      status = SeascanNoResponse;
   }
   else if (rspLen>MAXLEN)
   {
      status = SeascanChatFail;
   }
   else
   {
      /* Parse the response string for serial number and power up pressure */
      status = SeascanParseSerP( rspStr, rspLen, serNo, p);
   }
      
   return status;
 }

/*------------------------------------------------------------------------*/
/* function to exit the Seascan's command mode                            */
/*------------------------------------------------------------------------*/
/**
   This function powers down the Seascan.   
      \begin{verbatim}
      output:
         This function always returns a positive value on success.
         No failure detection available.
      \end{verbatim}
*/
int SeascanExitCmdMode(void)
{
   /* define the logging signature */
   //static cc FuncName[] = "SeascanExitCmdMode()";
   
   /* initialize the return value */
   int status = SeascanOk;
   
   /* pet the watchdog */
   WatchDog(); /* pet the watchdog */

   assert(StackOk()); /* stack-check assertion */

   CtdClearWakePin(); /* Power off the Seascan TD */

   /* disable IO, flush the IO buffers for the CTD serial port */
   CtdDisableIo();  Wait(10); ctdio.ioflush(); 

   
   return status;
}


/*------------------------------------------------------------------------*/
/* function to return the Seascan Power Up Pressure only                  */
/*------------------------------------------------------------------------*/
/**
   This function returns the Power up pressure returned by the Seascan
   when entering the #A command mode.  This is done by calling the the Seascan
   wakeup function which automatically returns the serial number and pressure.
   The serial number is ignored.  
   The wakeup functions return status is also returned herein.
      \begin{verbatim}
      input:
         p.....Pointer to returned power up pressure
      
      output:
         p.....power up pressure returned at p
 
         This function returns status.
         Here are the possible return values of this function:
         SeascanOk................No error.
         SeascanNoResponse........No response received from Seascan.
         SeascanRegExceptn........Response received but regexec() failed with
                                     an exceptional error.
         SeascanNullArg...........Null function argument.
         SeascanRegexFail.........Response received but it did not match the
                                     regex pattern for the serial number.
      \end{verbatim}
*/
int SeascanGetP(float *p)
{
   int status=SeascanOk, statusExit=SeascanOk;
   char serStr[SERNOSIZE];
   
   /* Wake up the Seascan.  Enter command mode.  */
   status = SeascanEnterCmdMode(serStr, p);   
   /* Power down the Seascan  */   
   statusExit = SeascanExitCmdMode();
   
   return(status); /* ignore exit status */
}

/*------------------------------------------------------------------------*/
/* function to return the Seascan Pressure/Temperature Sample             */
/*------------------------------------------------------------------------*/
/**
   This function returns the Seascan pressure and temperature sample.
   The wakeup function puts the sensor into command mode so that the '-'
   command can be issued in order to get the subsequent returned PT values.
   The command functions return status is also returned herein.
      \begin{verbatim}
      input:
         p.....Pointer to returned pressure
         t.....Pointer to returned temperature
      
      output:
         p.....pressure returned at p
         t.....temperature returned at t
 
         This function returns status.
         Here are the possible return values of this function:
         SeascanOk................No error.
         SeascanNoResponse........No response received from Seascan.
         SeascanRegExceptn........Response received but regexec() failed with
                                     an exceptional error.
         SeascanNullArg...........Null function argument.
         SeascanRegexFail.........Response received but it did not match the
                                     regex pattern for the serial number.
      \end{verbatim}
*/
int SeascanGetPt(float *p, float *t)
{
   int status = SeascanOk, statusExit = SeascanOk,stat;
   char serStr[SERNOSIZE];
   char rspStr[MAXLEN+1];
   int rspLen = 0; 
   float pup;            /* power up pressure */
   unsigned char ibyte;
   int n=0, nMax=200;
   
   *p = nan();
   *t = nan();
     
   /* Wake up the Seascan.  Enter command mode.  */
   status = SeascanEnterCmdMode(serStr, &pup);
   
   if (status==SeascanOk)
   {
      /* Send the "-" command to get a pressure and temperature measurement */
      ctdio.putb('-'); Wait(10);
      
      /* Read characters and echo back until ETX is detected */
      /* Loop iteration and wait makes > 2 second response time */
      for (n=0; n<nMax; n++)
      {
         if ( (stat = ctdio.getb(&ibyte)) >0 )
         {
            /* got a byte, echo the byte to the Seascan TD */
            ctdio.putb(ibyte); 
            /* If etx, terminate the string and break out of loop */
            if (ibyte == ETX)  { rspStr[rspLen]=NUL; break;}
            /* store the byte in the response string */
            rspStr[rspLen] = ibyte; if (rspLen<MAXLEN) rspLen++;
          }
          Wait(10);
      }
            
      /* Make size real, not 0-base index */
      if (rspLen>0) rspLen++;
       
      /* If got at least one character and havent't exceed buffer, check for data */
      if (rspLen==0)
      {
         status = SeascanNoResponse;
      }
      else if (rspLen>MAXLEN)
      {
         status = SeascanChatFail;
      }
      else
      {
         /* Parse the PT response string for pressure and temperature */
         status = SeascanParsePT( rspStr, rspLen, p, t);
      }
   }
   
   /* Power down the Seascan.  */
   statusExit = SeascanExitCmdMode();
   
   return ( status ); /* ignore exit status */
}

/*------------------------------------------------------------------------*/
/* function to return the Seascan TD serial number                        */
/*------------------------------------------------------------------------*/
/**
   This function returns the TD serial number by calling the the Seascan
   wakeup function which automatically returns the serial number and pressure.
   The pressure is ignored.  
   The wakeup functions return status is also returned herein.
      \begin{verbatim}
      input:
         SerNo....Pointer to the Serial Number string to be written.
 
      output:
         SerNo....The Serial Number string will be written into this
                  buffer.
 
         This function returns status.
         Here are the possible return values of this function:
         SeascanOk................No error.
         SeascanNoResponse........No response received from Seascan.
         SeascanRegExceptn........Response received but regexec() failed with
                                     an exceptional error.
         SeascanNullArg...........Null function argument.
         SeascanRegexFail.........Response received but it did not match the
                                     regex pattern for the serial number.
      \end{verbatim}
*/
int SeascanSerialNumber(char *SerNo)
{
   int status = SeascanOk, statusExit = SeascanOk;
   float pup;            /* power up pressure */

   /* Wake up the Seascan.  Enter command mode.  */
   status = SeascanEnterCmdMode(SerNo, &pup); 
  /* Power down the Seascan  */   
   statusExit = SeascanExitCmdMode();
   
   return(status); /* ignore exit status */
}





