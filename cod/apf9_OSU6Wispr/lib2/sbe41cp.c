#ifndef SBE41CP_H
#define SBE41CP_H (0x0008U)

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * $Id: sbe41cp.c,v 1.29 2007/04/24 01:43:29 swift Exp $
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
 * $Log: sbe41cp.c,v $
 * Revision 1.29  2007/04/24 01:43:29  swift
 * Added acknowledgement and funding attribution.
 *
 * Revision 1.28  2007/01/24 17:14:47  swift
 * Implement new formulation of pump-time computations for the Sbe43f.
 *
 * Revision 1.27  2006/11/07 15:18:41  swift
 * Modifications to the way that SeaBird handles calculation of pump-periods
 * for the IDO.
 *
 * Revision 1.26  2006/10/13 18:55:55  swift
 * Modifications to the way that SeaBird handles calculation of pump-periods
 * for the IDO.
 *
 * Revision 1.25  2006/10/12 00:19:03  swift
 * Eliminated unused timeout variable.
 *
 * Revision 1.24  2006/10/11 21:05:12  swift
 * Implemented the ability to configure the SBE41CP to output PTS data during
 * CP mode.
 *
 * Revision 1.23  2006/08/17 21:17:56  swift
 * Modifications to allow better logging control.
 *
 * Revision 1.22  2006/05/15 18:51:03  swift
 * Changed Sbe41cpUploadCP() to accomodate change in resolution of encoded
 * values of bin-averaged CP data.
 *
 * Revision 1.21  2006/04/21 13:45:40  swift
 * Changed copyright attribute.
 *
 * Revision 1.20  2006/02/22 21:39:38  swift
 * Changed the descretization parameterization to eliminate empty deep bins.
 *
 * Revision 1.19  2006/02/03 00:23:27  swift
 * Fixed a buffer-overrun bug in use of pgets().
 *
 * Revision 1.18  2005/10/25 17:02:16  swift
 * Modifications to accomodate the SBE41CP-IDO.
 *
 * Revision 1.17  2005/10/12 20:10:04  swift
 * Refactored SBE41CP configuration to account for presence of IDO.
 *
 * Revision 1.16  2005/08/11 20:14:42  swift
 * Strengthened the pendantic regexs for S,T,P,O.
 *
 * Revision 1.15  2005/08/11 16:31:45  swift
 * Modifications to fix a timing-related bug when requesting a P-only
 * measurement while the ctd is in CP mode.
 *
 * Revision 1.14  2005/07/07 15:07:36  swift
 * Fixed a bug in chat() caused by 1-second time descretization.
 *
 * Revision 1.13  2005/06/14 19:05:59  swift
 * Minor modifications to logging verbosity.
 *
 * Revision 1.12  2005/04/30 22:52:18  swift
 * Implemented an API to the low-power PT sampling feature of the SBE41CP.
 * Added function to query the SBE41CP for its firmware revision.
 *
 * Revision 1.11  2005/01/06 00:46:22  swift
 * Modification to reflect changes to LogEntry() and improved robustness.
 *
 * Revision 1.10  2004/03/20 01:30:37  swift
 * Modifications made to reflect the changes requested for the SBE41CP firmware.
 *
 * Revision 1.9  2004/03/17 01:13:21  swift
 * Tighter implementation of the master-slave model.
 *
 * Revision 1.8  2003/12/20 19:11:26  swift
 * Fixed a bug that ignored negative signs in STP measurements and caused the
 * parse results to be always non-negative.  Elevated Sbe41cpStatus() to be a
 * nonstatitically linked support function.  Added Sbe43fPumpTime() function.
 *
 * Revision 1.7  2003/12/19 23:03:14  swift
 * Fairly major rework of several functions.  The implementation of
 * Sbe41cpStatus() was changed from using regex's to using string matching
 * techniques to match parameter tokens.  Replaced "\r\n" pairs with "\r" to
 * fix parsing problems.
 *
 * Revision 1.6  2003/12/18 22:53:05  swift
 * Unstable major revision prior to reimplementing Sbe41cpStatus().
 *
 * Revision 1.5  2003/10/21 17:59:43  swift
 * Changed TimeOut period of Sbe41cpGetPtso() from 30 seconds to 60 seconds to
 * accomodate SeaBird's change in protocol for the SBE43F oxygen sensor.
 *
 * Revision 1.4  2003/07/16 23:52:23  swift
 * Implement the master/slave model in a more robust way especially when in
 * communications mode.  Also, IEEE definitions of NaN were exploited in order
 * to write and use functions that set and detect NaN and Inf.
 *
 * Revision 1.3  2003/07/05 21:11:13  swift
 * Eliminated 'Sbe41cpLogCal()' because it would require too large of a FIFO
 * buffer for the serial port.  The console serial port of the APF9 is limited
 * to 4800 baud while the CTD serial port operates at 9600 baud.  Since no
 * hardware or software handshaking is implemented then the FIFO has to buffer
 * the differing rates.
 *
 * In 'Sbe41cpInitiateSample()', some of the code was refactored away from being
 * hardware independent toward being hardware dependent.  This was a concession
 * to reliability and robustness that was caused by the slow processing speed
 * of the APF9.  Its prior factorization suffered from too many function calls
 * to low-level hardware dependent primitives.  These calls threw off timing
 * enough to reduce the reliability communications and sequencing of SBE41CP
 * control.
 *
 * Revision 1.2  2003/07/03 22:47:46  swift
 * Major revisions.  This revision is not yet stable.
 *
 * Revision 1.1  2003/06/29 01:36:06  swift
 * Initial revision
 * \end{verbatim}
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
#define sbe41ChangeLog "$RCSfile: sbe41cp.c,v $ $Revision: 1.29 $ $Date: 2007/04/24 01:43:29 $"

#include <serial.h>
#include <stdio.h>

/* function prototypes */
int    Sbe41cpBinAverage(void);
int    Sbe41cpConfig(float PCutOff);
int    Sbe41cpEnablePts(void);
int    Sbe41cpEnterCmdMode(void);
int    Sbe41cpExitCmdMode(void);
int    Sbe41cpFwRev(char *buf,unsigned int bufsize);
int    Sbe41cpGetP(float *p);
int    Sbe41cpGetPt(float *p, float *t);
int    Sbe41cpGetPts(float *p, float *t, float *s);
int    Sbe41cpGetPtso(float *p, float *t, float *s, float *o);
int    Sbe41cpLogCal(void);
int    Sbe41cpSerialNumber(void);
int    Sbe41cpStartCP(time_t FlushSec);
int    Sbe41cpStatus(float *pcutoff, unsigned *serno,
                     unsigned *nsample, unsigned *nbin, time_t *tswait, 
                     float *topint, float *topsize, float *topmax,
                     float *midint, float *midsize, float *midmax,
                     float *botint, float *botsize);
int    Sbe41cpStopCP(void);
int    Sbe41cpTsWait(time_t sec);
int    Sbe41cpUploadCP(FILE *dest);
time_t Sbe43fPumpTime(float p, float t, float Tau1P);
float  Sbe43Tau(void); 

/* define the return states of the SBE41CP API */
extern const char Sbe41cpTooFew;          /* Too few CP samples */
extern const char Sbe41cpChatFail;        /* Failed chat attempt. */
extern const char Sbe41cpNoResponse;      /* No response received from SBE41CP. */
extern const char Sbe41cpRegExceptn;      /* Response received, regexec() exception */
extern const char Sbe41cpRegexFail;       /* response received, regex no-match */
extern const char Sbe41cpNullArg;         /* Null function argument. */
extern const char Sbe41cpFail;            /* General failure */
extern const char Sbe41cpOk;              /* response received, regex match */
extern const char Sbe41cpPedanticFail;    /* response received, pedantic regex no-match */
extern const char Sbe41cpPedanticExceptn; /* response received, pedantic regex exception */

#endif /* SBE41CP_H */

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

#ifdef _XA_
   #include <apf9.h>
#else
   #define WatchDog()
   #define StackOk() 1
#endif /* _XA_ */

/* define the maximum length of the SBE41CP response */
#define MAXLEN 80

/* define a buffer for communications with the CTD serial port */
static char buf[MAXLEN+1];

/* define a nonpedantic regex pattern for a field */
#define FIELD "([^,]*)"

/* define nonpedantic regex pattern for float */
#define FLOAT "[^-+0-9.]*([-+0-9.]+)[^0-9]*"

/* define pedantic regex patterns for P, T, S, and O */
#define P "[ ]+(-?[0-9]{1,4}\\.[0-9]{2})"
#define T "[ ]+(-?[0-9]{1,2}\\.[0-9]{4})"
#define S "[ ]+(-?[0-9]{1,2}\\.[0-9]{4})"
#define O "[ ]+([0-9]{1,5})"
  
/* define the return states of the SBE41CP API */
const char Sbe41cpTooFew          = -6; /* Too few CP samples */
const char Sbe41cpChatFail        = -5; /* Failed chat attempt. */
const char Sbe41cpNoResponse      = -4; /* No response received from SBE41CP. */
const char Sbe41cpRegExceptn      = -3; /* Response received, regexec() exception */
const char Sbe41cpRegexFail       = -2; /* response received, regex no-match */
const char Sbe41cpNullArg         = -1; /* Null function argument. */
const char Sbe41cpFail            =  0; /* General failure */
const char Sbe41cpOk              =  1; /* General success */
const char Sbe41cpPedanticFail    =  2; /* response received, pedantic regex no-match */
const char Sbe41cpPedanticExceptn =  3; /* response received, pedantic regex exception */

/* external function declarations */
extern void Wait(unsigned int millisec);
extern int snprintf(char *str, size_t size, const char *format, ...);

/* prototypes for functions with static linkage */
static int chat(const struct SerialPort *port, const char *cmd,
                const char *expect, time_t sec);

/*------------------------------------------------------------------------*/
/* function to command the SBE41CP to compute bin-averages of samples     */
/*------------------------------------------------------------------------*/
/**
   This function uses the SBE41CP's command mode to compute the bin-averages
   of the samples stored in nvram.  Note: This function does not exit
   command mode upon completion.

      \begin{verbatim}
      output:

         This function returns a positive value on success and a zero or
         negative value on failure.  Here are the possible return values of
         this function:

         Sbe41cpRegexFail.........Response received, regex no-match
         Sbe41cpChatFail..........Execution of configuration commands failed.
         Sbe41cpRegExceptn........Response received but regexec() failed with
                                    an exceptional error.
         Sbe41cpNoResponse........No response received from SBE41CP.
         Sbe41cpNullArg...........Null function argument.
         Sbe41cpFail..............Post-configuration verification failed.
         Sbe41cpOk................Configuration attempt was successful.
                                 
      On success, the normal return value for this function will be 'Sbe41cpOk'.
      \end{verbatim}
*/
int Sbe41cpBinAverage(void)
{
   /* define the logging signature */
   static cc FuncName[] = "Sbe41cpBinAverage()";
   
   /* initialize the return value */
   int status=Sbe41cpNullArg;

   /* pet the watchdog timer */
   WatchDog();
   
   /* stack-check assertion */
   assert(StackOk());
   
   /* SBE41CP serial number is the return value of Sbe41cpEnterCmdMode() */
   if ((status=Sbe41cpEnterCmdMode())>0)
   {
      unsigned int nsample;
      
      /* reinitialize the return value */
      status=Sbe41cpFail;

      /* extract the number of samples from the status request */
      if (Sbe41cpStatus(NULL, NULL, &nsample, NULL, NULL, NULL, 
                        NULL, NULL, NULL, NULL, NULL, NULL, NULL)<=0)
      {
         /* create the message */
         static cc msg[]="Status request failed.\n";

         /* make logentry */
         ConioEnable(); LogEntry(FuncName,msg);
      }

      /* bin averages aren't computed for fewer than 10 samples */
      else if (nsample<10)
      {
         /* create the message */
         static cc format[]="Too few samples: %u.\n";

         /* make logentry */
         ConioEnable(); LogEntry(FuncName,format,nsample);

         /* reinitialize the return value */
         status=Sbe41cpTooFew;
      }
      
      else 
      {
         /* define buffer for output of binaverage command */
         char buf[64];
            
         /* get the current time and define the timeout period */
         const time_t To=time(NULL), TimeOut=3600, timeout=30;
         
         /* execute the command to bin-average the samples */
         if (chat(&ctdio, "binaverage\r", "samples =", 5)>0)
         {
            /* wait for the data to be uploaded to the Ctd fifo */
            while (CtdActiveIo(timeout) && difftime(time(NULL),To)<TimeOut) {}

            /* put the SBE41CP back to sleep and enable console io */
            pputs(&ctdio,"qsr\r",2,""); ConioEnable();

            /* parse the output of the SBE41CP's binaverage command */
            while (pgets(&ctdio, buf, sizeof(buf)-1, 5, "\r\n")>0 &&
                   difftime(time(NULL),To)<TimeOut) 
            {
               /* check for end of SBE41CP's bin-binaverage operations */
               if (!strncmp("done",buf,4))
               {
                  if (debuglevel>=2 || (debugbits&SBE41CP_H))
                  {
                     /* create the message */
                     static cc format[]="Finished averaging %d samples in %0.0f seconds.\n";

                     /* make logentry */
                     LogEntry(FuncName,format,nsample,difftime(time(NULL),To));
                  }
                  
                  break;
               }

               /* log any detected STP range exceptions */
               else if (!strncmp("bad: ",buf,5))
               {
                  /* create the message */
                  static cc format[]="Range exception: %s\n";
     
                  /* make logentry */
                  LogEntry(FuncName,format,buf+5);
               }
            }
            
            /* indicate success */
            status=Sbe41cpOk;
         }
      }
   }

   /* flush the IO buffers for the CTD serial port */
   if (ctdio.ioflush) ctdio.ioflush();
   
   /* put the SBE41 back to sleep */
   if (status<=0 && CtdEnableIo()>0 && chat(&ctdio,"\r","S>",2)>0) Sbe41cpExitCmdMode();

   /* enable console IO */
   CtdDisableIo(); ConioEnable();

   if (status<=0)
   {
      /* create the message */
      static cc msg[]="Attempt to bin-average the SBE41CP continuous profile failed.\n";
 
      /* make logentry */
      LogEntry(FuncName,msg);
   }

   return status;
   
   #undef MaxBufLen
}

/*------------------------------------------------------------------------*/
/* function to configure the SBE41CP                                      */
/*------------------------------------------------------------------------*/
/**
   This function configures the SBE41CP for 2-decibar bins over the whole
   water column.  It does this by entering the SBE41CP's command mode and
   executing commands that set the configuration parameters.

      \begin{verbatim}
      input:
         PCutOff...The continuous profile is automatically halted when the
                   pressure falls below this cut-off pressure.  In addition,
                   the SBE41CP will automatically be powered down.
      
      output:

         This function returns a positive number if the configuration
         attempt was successful.  Zero is returned if a response was
         received but it failed the regex match.  A negative return value
         indicates failure due to an exceptional condition.  Here are the
         possible return values of this function:

         Sbe41cpRegexFail.........Response received, regex no-match
         Sbe41cpChatFail..........Execution of configuration commands failed.
         Sbe41cpRegExceptn........Response received but regexec() failed with
                                    an exceptional error.
         Sbe41cpNoResponse........No response received from SBE41CP.
         Sbe41cpNullArg...........Null function argument.
         Sbe41cpFail..............Post-configuration verification failed.
         Sbe41cpOk................Configuration attempt was successful.
                                 
      On success, the normal return value for this function will be 'Sbe41cpOk'.
      \end{verbatim}
*/
int Sbe41cpConfig(float PCutOff)
{
   /* define the logging signature */
   static cc FuncName[] = "Sbe41cpConfig()";
   
   /* initialize the return value */
   int status=Sbe41cpNullArg;

   /* define the timeout period */
   const time_t TimeOut = 2;
   
   /* pet the watchdog timer */
   WatchDog();
   
   /* stack-check assertion */
   assert(StackOk());

   /* SBE41CP serial number is the return value of Sbe41cpEnterCmdMode() */
   if ((status=Sbe41cpEnterCmdMode())>0)
   {
      #define MaxBufLen 31
      char buf[MaxBufLen+1];
      float topint, topsize, topmax, midint, midsize, midmax, botint, botsize, pcutoff;
         
      /* reinitialize the return value */
      status=Sbe41cpOk;

      /* write the command to set the pressure cutoff */
      snprintf(buf,MaxBufLen,"pcutoff=%0.1f\r",PCutOff);
     
      /* initialize the control parameters of the SBE41CP */
      if (chat(&ctdio, buf,                        "S>", TimeOut)<=0 ||
          chat(&ctdio, "autobinavg=n\r",           "S>", TimeOut)<=0 ||
          chat(&ctdio, "top_bin_interval=2\r",     "S>", TimeOut)<=0 ||
          chat(&ctdio, "top_bin_size=2\r",         "S>", TimeOut)<=0 ||
          chat(&ctdio, "top_bin_max=10\r",         "S>", TimeOut)<=0 ||
          chat(&ctdio, "middle_bin_interval=2\r",  "S>", TimeOut)<=0 ||
          chat(&ctdio, "middle_bin_size=2\r",      "S>", TimeOut)<=0 ||
          chat(&ctdio, "middle_bin_max=20\r",      "S>", TimeOut)<=0 ||
          chat(&ctdio, "bottom_bin_interval=2\r",  "S>", TimeOut)<=0 ||
          chat(&ctdio, "bottom_bin_size=2\r",      "S>", TimeOut)<=0 ||
          chat(&ctdio, "includetransitionbin=n\r", "S>", TimeOut)<=0 ||
          chat(&ctdio, "includenbin=y\r",          "S>", TimeOut)<=0 ||
          chat(&ctdio, "outputpts=n\r",            "S>", TimeOut)<=0  )
      {
         /* create the message */
         static cc msg[]="chat() failed.\n";

         /* log the configuration failure */
         ConioEnable(); LogEntry(FuncName,msg);

         /* indicate failure */
         status=Sbe41cpChatFail;
      }

      /* analyze the query response to verify expected configuration */
      else if ((status=Sbe41cpStatus(&pcutoff, NULL, NULL, NULL, NULL, &topint, &topsize, &topmax,
                                     &midint, &midsize, &midmax, &botint, &botsize))>0)
      { 
         /* verify the configuration parameters */
         if (fabs(pcutoff-PCutOff)>0.1 ||
             fabs(topint-2)>0.1 || fabs(topsize-2)>0.1 || fabs(topmax-10)>0.1 ||
             fabs(midint-2)>0.1 || fabs(midsize-2)>0.1 || fabs(midmax-20)>0.1 ||
             fabs(botint-2)>0.1 || fabs(botsize-2)>0.1 )
            {
               /* create the message */
               static cc msg[]="Configuration failed.\n";
               
               /* log the configuration failure */
               ConioEnable(); LogEntry(FuncName,msg);

               /* indicate failure */
               status=Sbe41cpFail;
            }

         /* log the configuration success */
         else if (debuglevel>=2 || (debugbits&SBE41CP_H))
         {
            /* create the message */
            static cc msg[]="Configuration successful.\n";
  
            /* log the configuration failure */
            ConioEnable(); LogEntry(FuncName,msg);
         }
      }
   }
  
   /* put the SBE41 back to sleep */
   if (CtdEnableIo()>0 && chat(&ctdio,"\r","S>",2)>0) Sbe41cpExitCmdMode();

   /* enable console IO */
   CtdDisableIo(); ConioEnable();

   if (status<=0)
   {
      /* create the message */
      static cc msg[]="Attempt to set up SBE41CP failed.\n";

      /* log the message */
      LogEntry(FuncName,msg);
   }

   return status;
   
   #undef NSUB
   #undef MaxBufLen
}

/*------------------------------------------------------------------------*/
/* function to configure PTS output during CP mode                        */
/*------------------------------------------------------------------------*/
/**
   This function configures the SBE41CP to stream PTS data while in CP mode.

      \begin{verbatim}
      output:

         This function returns a positive number if the configuration
         attempt was successful.  Zero is returned if a response was
         received but it failed the regex match.  A negative return value
         indicates failure due to an exceptional condition.  Here are the
         possible return values of this function:

         Sbe41cpChatFail..........Execution of configuration commands failed.
         Sbe41cpOk................Configuration attempt was successful.
                                 
      On success, the normal return value for this function will be 'Sbe41cpOk'.
      \end{verbatim}
*/
int Sbe41cpEnablePts(void)
{
   /* define the logging signature */
   static cc FuncName[] = "Sbe41cpEnablePts()";
   
   /* initialize the return value */
   int status=Sbe41cpNullArg;
   
   /* pet the watchdog timer */
   WatchDog();
   
   /* stack-check assertion */
   assert(StackOk());
   
   /* SBE41CP serial number is the return value of Sbe41cpEnterCmdMode() */
   if ((status=Sbe41cpEnterCmdMode())>0)
   {
      /* reinitialize the return value */
      status=Sbe41cpOk;
      
      /* initialize the control parameters of the SBE41CP */
      if (chat(&ctdio, "outputpts=y\r","S>", 4)<=0)
      {
         /* create the message */
         static cc msg[]="chat() failed.\n";

         /* log the configuration failure */
         ConioEnable(); LogEntry(FuncName,msg);

         /* indicate failure */
         status=Sbe41cpChatFail;
      }

      /* verify the configuration attempt */
      else if (chat(&ctdio,"ds\r","output is PTS",10)<=0)
      {
         /* create the message */
         static cc msg[]="Failed attempt to verify configuration.\n";
   
         /* log the configuration failure */
         ConioEnable(); LogEntry(FuncName,msg);

         /* indicate failure */
         status=Sbe41cpChatFail;
      }

      /* log the configuration success */
      else if (debuglevel>=2 || (debugbits&SBE41CP_H))
      {
         /* create the message */
         static cc msg[]="PTS output enabled during CP mode.\n";
         
         /* log the configuration failure */
         ConioEnable(); LogEntry(FuncName,msg);
      }
   }
  
   /* put the SBE41 back to sleep */
   if (CtdEnableIo()>0 && chat(&ctdio,"\r","S>",2)>0) Sbe41cpExitCmdMode();

   /* enable console IO */
   CtdDisableIo(); ConioEnable();

   if (status<=0)
   {
      /* create the message */
      static cc msg[]="Failed attempt to enable PTS output during CP mode.\n";

      /* log the message */
      LogEntry(FuncName,msg);
   }

   return status;
   
   #undef NSUB
   #undef MaxBufLen
}

/*------------------------------------------------------------------------*/
/* function to enter the SBE41CP's command mode                           */
/*------------------------------------------------------------------------*/
/**
   This function wakes the SBE41CP and places it in command mode.  It does
   this by asserting the wake pin for 1 full second in order to induce the
   SBE41CP into command mode.  Experience shows that the mode-select line must
   be low when this command is executed or else it initiates a full CTD
   sample.  This will waste energy and throw off timing.

      \begin{verbatim}
      output:

         On success, this function returns a positive number that is the
         SBE41CP serial number if the response matched a regex for the float
         serial number.  Zero is returned if a response was received but it
         failed the regex match.  A negative return value indicates failure
         due to an exceptional condition.  Here are the possible return
         values of this function:
         
         Sbe41cpNoResponse........No response received from SBE41CP.
         Sbe41cpRegExceptn........Response received but regexec() failed with
                                     an exceptional error.
         Sbe41cpNullArg...........Null function argument.
         Sbe41cpRegexFail.........Response received but it did not match the
                                     regex pattern for the serial number.
      \end{verbatim}
*/
int Sbe41cpEnterCmdMode(void)
{ 
   /* define the logging signature */
   static cc FuncName[] = "Sbe41cpEnterCmdMode()";
   
   /* initialize the return value */
   int status = Sbe41cpNullArg;

   /* pet the watchdog */
   WatchDog();

   /* stack-check assertion */
   assert(StackOk());
 
   /* validate the CTD serial ports ioflush() function */
   if (!(ctdio.ioflush))
   {
      /* create the message */
      static cc msg[]="NULL ioflush() function for serial port.\n";

      /* log the message */
      LogEntry(FuncName,msg);
   }

   else
   {
      /* define the number of subexpressions in the regex pattern */
      #define NSUB 1

      /* define objects needed for regex matching */
      regex_t regex; regmatch_t regs[NSUB+1]; int errcode;

      /* define the the nonpedantic pattern that will match a float */
      const char *pattern = "SERIAL NO\\.[ ]*([0-9]{4})";
      
      /* initialize the communications timeout periods */
      time_t To=time(NULL); const time_t TimeOut=30, timeout=2;
    
      /* compile the regex pattern */
      assert(!regcomp(&regex,pattern,REG_EXTENDED|REG_NEWLINE));

      /* protect against segfaults */
      assert(NSUB==regex.re_nsub);

      /* reinitialize the return value */
      status=Sbe41cpNoResponse;

      /* clear the mode-select line and enable IO from CTD serial port */
      CtdClearModePin(); CtdEnableIo(); 

      /* fault tolerance loop - keep trying if ctd is busy */
      for (status=0; status<=0 && difftime(time(NULL),To)<TimeOut;)
      {
         /* initiate the wake-up cycle */
         CtdAssertWakePin(); sleep(1); CtdClearWakePin(); Wait(100); ctdio.iflush(); Wait(100);
    
         /* get the SBE41CP command prompt to confirm that SBE41CP is ready for commands */
         if ((status=chat(&ctdio,"\r","S>",2))>0)
         {
            /* initialize the reference time */
            const time_t To=time(NULL);

            /* flush the IO buffers of the CTD serial port */
            ctdio.iflush();
      
            /* send the command to display status */
            pputs(&ctdio,"ds\r",timeout,"");

            /* get the response */
            while (pgets(&ctdio,buf,MAXLEN,timeout,"\r\n")>0 && difftime(time(NULL),To)<TimeOut)
            {
               /* log the string received from the CTD serial port */
               if (debuglevel>=4)
               {
                  /* create the message */
                  static cc format[]="[%s]\n";
   
                  LogEntry(FuncName,format,buf);
               }
               
               /* check the current response against the regex */
               if (!(errcode=regexec(&regex,buf,regex.re_nsub+1,regs,0)))
               {
                  /* extract the serial number from the response */
                  status = atoi(extract(buf,regs[1].rm_so+1,regs[1].rm_eo-regs[1].rm_so));

                  break;
               }

               /* indicate that the response did not match the regex pattern */
               else  {status = (errcode==REG_NOMATCH) ? Sbe41cpRegexFail : Sbe41cpRegExceptn;}
            }

            /* flush the IO queues */
            chat(&ctdio,"\r","S>",5);
            
            break;
         }

         /* execute the 'stopprofile' command in case the SBE41CP is in CP mode */
         else {pputs(&ctdio,"stopprofile\r",timeout,""); sleep(1);} 
      }
   
      /* clean up the regex pattern buffer */
      regfree(&regex);
   }
   
   return status;

   #undef NSUB
}

/*------------------------------------------------------------------------*/
/* function to exit the SBE41CP's command mode                            */
/*------------------------------------------------------------------------*/
/**
   This function sends the SBE41CP a command to exit command mode and power
   down.  Experience shows that the mode-select line must be low when this
   command is executed or else it initiates a full CTD sample.  This will
   waste energy and throw off timing.
   
      \begin{verbatim}
      output:
         This function returns a positive value on success and zero on
         failure.  A negative value indicates an exceptional error.
      \end{verbatim}
*/
int Sbe41cpExitCmdMode(void)
{
   /* define the logging signature */
   static cc FuncName[] = "Sbe41cpExitCmdMode()";
   
   /* initialize the return value */
   int i,status = -1;
   
   /* pet the watchdog */
   WatchDog();

   /* stack-check assertion */
   assert(StackOk());
 
   /* validate the CTD serial ports ioflush() function */
   if (!(ctdio.ioflush))
   {
      /* create the message */
      static cc msg[]="NULL ioflush() function for serial port.\n";

      /* log the message */
      LogEntry(FuncName,msg);
   }
   
   else
   {
      /* initialize the communications timeout periods */
      const time_t To=time(NULL), TimeOut=30, timeout=2;

      /* clear the mode-select line and enable IO from CTD serial port */
      CtdClearModePin(); CtdEnableIo(); 
 
      /* fault tolerance loop - keep trying if ctd is busy */
      for (status=0; status<=0 && difftime(time(NULL),To)<TimeOut;)
      {
         /* flush the CTD's IO buffers */
         ctdio.ioflush();
      
         /* get the SBE41CP command prompt to confirm that SBE41CP is ready for commands */
         if (chat(&ctdio,"\r","S>",timeout)>0)
         {
            for (i=0; i<3; i++)
            {
               /* command the SBE41CP to power down and get the expected response */   
               if (chat(&ctdio,"qsr\r","powering down",timeout)>0) {status=1; break;}
            }
 
            /* exit the fault tolerance loop on success */
            if (status>0) break;
         }

         /* assert the wake pin to initiate a wake-up cycle */
         CtdAssertWakePin();

         /* pause for 1 second */
         sleep(1);

         /* clear the wake pin */
         CtdClearWakePin();
      }
   
      /* disable IO, flush the IO buffers for the CTD serial port */
      CtdDisableIo(); sleep(3); ctdio.ioflush();

      if (status<=0)
      {
         /* create the message */
         static cc msg[]="Command to power down the SBE41CP failed.\n";

         /* log the message */
         LogEntry(FuncName,msg);
      }
   }

   /* enable console IO */
   ConioEnable();
   
   return status;
}

/*------------------------------------------------------------------------*/
/* function to query the SBE41CP for its firmware revision                  */
/*------------------------------------------------------------------------*/
/**
   This function queries the SBE41CP for its firmware revision.  It parses the
   response to a 'ds' command using a regex and extracts the firmware
   revision.  

      \begin{verbatim}
      input:

         size.....This is the maximum number of bytes (including the 0x0
                  string terminator) that will be written into the FwRev
                  function argument.

      output:

         FwRev....The firmware revision string will be written into this
                  buffer.

         On success, this function returns a positive value and stores the
         firmware revision in its function argument.  Zero is returned if a
         response was received but it failed the regex match.  A negative
         return value indicates failure due to an exceptional condition.
         Here are the possible return values of this function:
         
         Sbe41cpNoResponse........No response received from SBE41CP.
         Sbe41cpRegExceptn........Response received but regexec() failed with
                                     an exceptional error.
         Sbe41cpNullArg...........Null function argument.
         Sbe41cpRegexFail.........Response received but it did not match the
                                     regex pattern for the serial number.
      \end{verbatim}
*/
int Sbe41cpFwRev(char *FwRev,unsigned int size)
{
   /* define the logging signature */
   static cc FuncName[] = "Sbe41cpFwRev()";

   int status=Sbe41cpNullArg;

   /* pet the watchdog */
   WatchDog();

   /* stack-check assertion */
   assert(StackOk());

   /* validate the function argument */
   if (!FwRev || !size)
   {
      /* create the message */
      static cc msg[]="NULL function argument.\n";

      /* log the message */
      LogEntry(FuncName,msg);
   }

   /* validate the CTD serial ports ioflush() function */
   else if (!(ctdio.iflush))
   {
      /* create the message */
      static cc msg[]="NULL iflush() function for serial port.\n";

      /* log the message */
      LogEntry(FuncName,msg);
   }

   /* enter SBE41CP command mode */
   else if ((status=Sbe41cpEnterCmdMode())>0)
   {
      /* create a temporary buffer to receive the SBE41CP response string */
      char buf[64];

      /* initialize the return values */
      status=Sbe41cpNoResponse; FwRev[0]=0;
      
      /* get the SBE41CP command prompt to confirm that SBE41CP is ready for commands */
      if (chat(&ctdio,"ds\r","SBE 41CP",2)>0 && pgets(&ctdio,buf,sizeof(buf)-1,3,"\r\n")>0)
      {
         /* define the number of subexpressions in the regex pattern */
         #define NSUB 1
          
         /* define objects needed for regex matching */
         regex_t regex; regmatch_t regs[NSUB+1]; int errcode;

         /* define the the nonpedantic pattern that will match a float */
         const char *pattern = "UW.*[ ]+V[ ]+([^ ]+)";

         /* compile the regex pattern */
         assert(!regcomp(&regex,pattern,REG_EXTENDED|REG_NEWLINE));

         /* protect against segfaults */
         assert(regex.re_nsub==NSUB);

         /* check if the current line matches the regex */
         if (!(errcode=regexec(&regex,buf,regex.re_nsub+1,regs,0)))
         {
            /* determine the number of bytes to copy */
            unsigned int n = regs[1].rm_eo-regs[1].rm_so; if(n>=size) n=size-1;

            /* copy the firmware revision from the SBE41CP response */
            strncpy(FwRev,buf+regs[1].rm_so,n); FwRev[n]=0;

            /* indicate success */
            status=Sbe41cpOk;
         }

         /* indicate that the response did not match the regex pattern */
         else  {status = (errcode==REG_NOMATCH) ? Sbe41cpRegexFail : Sbe41cpRegExceptn;}
         
         /* clean up the regex pattern buffer */
         regfree(&regex); 
      }
   }
   
   /* put the SBE41CP back to sleep */
   if (CtdEnableIo()>0 && chat(&ctdio,"\r","S>",2)>0) Sbe41cpExitCmdMode();
   
   /* enable console IO */
   ConioEnable();
   
   return status;
}

/*------------------------------------------------------------------------*/
/* function to get one pressure measurement from the SBE41CP              */
/*------------------------------------------------------------------------*/
/**
   This function queries the SBE41CP CTD for a single pressure measurement.

      \begin{verbatim}
      input:
         ctdio....A structure that contains pointers to machine dependent
                  primitive IO functions.  See the comment section of the
                  SerialPort structure for details.  The function checks to
                  be sure this pointer is not NULL.

      output:
         p........This is where the pressure will be stored when the
                  function returns.  If an error occurs, this is set to NaN
                  (according to IEEE floating point format).

         This function returns a positive number if the response received
         from the CTD serial port matched (for each of p, t, and s) a regex
         for a float.  Zero is returned if a response was received but it
         failed the regex match.  A negative return value indicates failure
         due to an exceptional condition.  Here are the possible return
         values of this function:
         
         Sbe41cpNoResponse........No response received from SBE41CP.
         Sbe41cpRegExceptn........Response received but regexec() failed with
                                     an exceptional error.
         Sbe41cpNullArg...........Null function argument.
         Sbe41cpRegexFail.........Response received but it did not match the
                                     regex pattern for a float.
         Sbe41cpOk................Response received that matched the pedantic
                                     regex pattern.
         Sbe41cpPedanticFail......Response received that matched a float but
                                     failed to match the pedantic regex pattern.
         Sbe41cpPedanticExceptn...Response received that matched a float but
                                     the pedantic regex experienced an exception.
                                 
      On success, the normal return value for this function will be 'Sbe41cpOk'.
      \end{verbatim}
*/
int Sbe41cpGetP(float *p)
{
   /* define the logging signature */
   static cc FuncName[] = "Sbe41cpGetP()";
   
   int status=Sbe41cpNullArg;

   /* pet the watchdog */
   WatchDog();

   /* stack-check assertion */
   assert(StackOk());

   /* make sure that floats are 4 bytes in length */
   assert(sizeof(float)==4);
   
   /* validate the function argument */
   if (!p) 
   {
      /* create the message */
      static cc msg[]="NULL function argument.\n";

      /* log the message */
      LogEntry(FuncName,msg);
   }

   /* validate the CTD serial ports ioflush() function */
   else if (!(ctdio.iflush))
   {
      /* create the message */
      static cc msg[]="NULL iflush() function for serial port.\n";

      /* log the message */
      LogEntry(FuncName,msg);
   }
 
   else
   {
      int errcode;

      /* reinitialize the return values */
      status=Sbe41cpOk;

      /* initialize the return value of 'p' to IEEE NaN */
      *p = nan();
      
      /* activate the SBE41CP with the hardware control lines */
      errcode=CtdPSample(buf,MAXLEN);

      /* log the data received from the SBE41CP */
      if (debuglevel>=4)
      {
         /* create the message */
         static cc format[]="Received: [%s]\n";
 
         /* log the message */
         LogEntry(FuncName,format,buf);
      }
      
      /* check if an error was detected */
      if (errcode<=0)
      {
         /* create the message */
         static cc format[]="No response from SBE41CP.\n";
 
         /* log the message */
         LogEntry(FuncName,format);

         /* indicate failure */
         status=Sbe41cpNoResponse;
      }

      /* subject the SBE41CP response to lexical analysis */
      else
      {
         /* define the number of subexpressions in the regex pattern */
         #define NSUB 1

         /* define objects needed for regex matching */
         regex_t regex; regmatch_t regs[NSUB+1]; int errcode;

         /* define the the nonpedantic pattern that will match a float */
         const char *pattern = "^" FLOAT;
         
         /* compile the regex pattern */
         assert(!regcomp(&regex,pattern,REG_EXTENDED|REG_NEWLINE));

         /* protect against segfaults */
         assert(NSUB==regex.re_nsub);

         /* check if the current line matches the regex */
         if (!(errcode=regexec(&regex,buf,regex.re_nsub+1,regs,0)))
         {
            /* extract the pressure from the buffer */
            *p = atof(extract(buf,regs[1].rm_so+1,regs[1].rm_eo-regs[1].rm_so));
               
            /* define the pedantic form of the expected response */
            pattern = "^" P;

            /* jettison the nonpedantic regex */
            regfree(&regex);

            /* compile the regex pattern */
            assert(!regcomp(&regex,pattern,REG_EXTENDED|REG_NEWLINE));

            /* protect against segfaults */
            assert(NSUB==regex.re_nsub);

            /* check if the response matches exactly the expected form */
            if ((errcode=regexec(&regex,buf,regex.re_nsub+1,regs,0)))
            {
               /* create the message */
               static cc format[]="Violation of pedantic regex: \"%s\\r\\n\"\n";

               /* log the message */
                LogEntry(FuncName,format,buf);

               /* reinitialize the return value */
               status = (errcode==REG_NOMATCH) ? Sbe41cpPedanticFail : Sbe41cpPedanticExceptn;
            }
         }

         else
         {
            /* create the message */
            static cc format[]="Violation of nonpedantic regex: [%s\\r\\n]\n";

            /* log the message */
            LogEntry(FuncName,format,buf);
              
            /* indicate that the response from the SBE41CP violated even the nonpedantic regex */
            status = (errcode==REG_NOMATCH) ? Sbe41cpRegexFail : Sbe41cpRegExceptn; 
         }
         
         /* clean up the regex pattern buffer */
         regfree(&regex); 
      }
   }

   return status;
   
   #undef NSUB
}

/*------------------------------------------------------------------------*/
/* function to get a low-power PT sample from the SBE41CP                 */
/*------------------------------------------------------------------------*/
/**
   This function queries the SBE41CP CTD for a low-power PT measurement.

      \begin{verbatim}
      input:
         ctdio....A structure that contains pointers to machine dependent
                  primitive IO functions.  See the comment section of the
                  SerialPort structure for details.  The function checks to
                  be sure this pointer is not NULL.

      output:
      
         p........This is where the pressure will be stored when the
                  function returns.  If an error occurs, this is set to NaN
                  (according to IEEE floating point format).
      
         t........This is where the temperature will be stored when the
                  function returns.  If an error occurs, this is set to NaN
                  (according to IEEE floating point format).

         This function returns a positive number if the response received
         from the CTD serial port matched (for each of p and t) a regex for
         a float.  Zero is returned if a response was received but it failed
         the regex match.  A negative return value indicates failure due to
         an exceptional condition.  Here are the possible return values of
         this function:
         
         Sbe41cpNoResponse........No response received from SBE41CP.
         Sbe41cpRegExceptn........Response received but regexec() failed with
                                  an exceptional error.
         Sbe41cpNullArg...........Null function argument.
         Sbe41cpRegexFail.........Response received but it did not match (for
                                  each of p, t, and s) the regex pattern for a
                                  float.
         Sbe41cpOk................Response received that matched the pedantic
                                  regex pattern.
         Sbe41cpPedanticFail......Response received that matched a float but
                                  failed to match the pedantic regex pattern.
         Sbe41cpPedanticExceptn...Response received that matched a float but
                                  the pedantic regex experienced an exception.
                                 
      On success, the normal return value for this function will be 'Sbe41cpOk'.
      \end{verbatim}
*/
int Sbe41cpGetPt(float *p, float *t)
{
   /* define the logging signature */
   static cc FuncName[] = "Sbe41cpGetPt()";

   int status=Sbe41cpNullArg;

   /* pet the watchdog timer */
   WatchDog();
      
   /* stack-check assertion */
   assert(StackOk());

   /* make sure that floats are 4 bytes in length */
   assert(sizeof(float)==4);
   
   /* validate the function argument */
   if (!p || !t)
   {
      /* create the message */
      static cc msg[]="NULL function argument(s).\n";

      /* log the message */
      LogEntry(FuncName,msg);
   }

   /* validate the CTD serial ports ioflush() function */
   else if (!(ctdio.ioflush))
   {
      /* create the message */
      static cc msg[]="NULL ioflush() function for serial port.\n";

      /* log the message */
      LogEntry(FuncName,msg);
   }

   else
   {
      int errcode;

      /* reinitialize the return values */
      status=Sbe41cpOk;

      /* initialize the return value of 'p' and 't' to IEEE NaN */
      *p=nan(); *t=nan();

      /* activate the SBE41CP with the hardware control lines */
      errcode=CtdPtSample(buf,MAXLEN);

      /* log the data received from the SBE41CP */
      if (debuglevel>=4 || (debugbits&SBE41CP_H))
      {
         /* create the message */
         static cc format[]="Received: [%s]\n";

         /* log the message */
         LogEntry(FuncName,format,buf);
      }
      
      /* check if an error was detected */
      if (errcode<=0)
      {
         /* create the message */
         static cc msg[]="No response from SBE41CP.\n";
         
         /* log the message */
         LogEntry(FuncName,msg);

         /* indicate failure */
         status=Sbe41cpNoResponse;
      }
      
      /* subject the SBE41CP response to lexical analysis */
      else
      {
         /* define the number of subexpressions in the regex pattern */
         #define NSUB 2
          
         /* define objects needed for regex matching */
         regex_t regex; regmatch_t regs[NSUB+1]; int errcode;

         /* define the the nonpedantic pattern that will match a float */
         const char *pattern = "^" FIELD "," FIELD;
         
         /* compile the regex pattern */
         assert(!regcomp(&regex,pattern,REG_EXTENDED|REG_NEWLINE));

         /* protect against segfaults */
         assert(regex.re_nsub==NSUB);

         /* log the string received from the CTD */
         if (debuglevel>=4 || (debugbits&SBE41CP_H))
         {
            /* create the message */
            static cc format[]="[%s]\n";
         
            /* log the message */
            LogEntry(FuncName,format,buf);
         }
         
         /* check if the current line matches the regex */
         if (!(errcode=regexec(&regex,buf,regex.re_nsub+1,regs,0)))
         {
            /* initialize pointers to the P,T fields */
            const char *pbuf=0, *tbuf=0;

            /* define the pedantic form of the expected response */
            pattern = "^" P "," T; 

            /* jettison the nonpedantic regex */
            regfree(&regex);

            /* compile the pedantic regex pattern */
            assert(!regcomp(&regex,pattern,REG_NOSUB|REG_EXTENDED|REG_NEWLINE));

            /* check if the response matches exactly the expected form */
            if ((errcode=regexec(&regex,buf,0,0,0)))
            {
               /* create the message */
               static cc format[]="Violation of pedantic regex: [%s\\r\\n]\n";
               
               /* log the message */
               LogEntry(FuncName,format,buf);

               /* reinitialize the return value */
               status = (errcode==REG_NOMATCH) ? Sbe41cpPedanticFail : Sbe41cpPedanticExceptn;
            }

            /* segment the buffer into separate P and T fields */
            if (regs[1].rm_so>=0 && regs[1].rm_eo>=0) {pbuf=buf+regs[1].rm_so; buf[regs[1].rm_eo]=0;}
            if (regs[2].rm_so>=0 && regs[2].rm_eo>=0) {tbuf=buf+regs[2].rm_so; buf[regs[2].rm_eo]=0;}

            /* jettison the pedantic regex */
            regfree(&regex);

            /* compile the regex pattern for a float */
            assert(!regcomp(&regex,FLOAT,REG_EXTENDED|REG_NEWLINE));

            /* protect against segfaults */
            assert(regex.re_nsub==1);

            /* check the pressure-field against a nonpedantic float regex pattern */
            if (pbuf && !regexec(&regex,pbuf,regex.re_nsub+1,regs,0)) 
            {
               /* extract pressure from the buffer */
               *p = atof(extract(pbuf,regs[1].rm_so+1,regs[1].rm_eo-regs[1].rm_so));
            }

            /* check the temperature-field against a nonpedantic float regex pattern */
            if (tbuf && !regexec(&regex,tbuf,regex.re_nsub+1,regs,0))
            {
               /* extract temperature from the buffer */               
               *t = atof(extract(tbuf,regs[1].rm_so+1,regs[1].rm_eo-regs[1].rm_so));
            }
         }
         
         else
         {
            /* create the message */
            static cc format[]="Violation of 2-fields regex: [%s\\r\\n]\n";

            /* make logentry */
            LogEntry(FuncName,format,buf);
            
            /* indicate that the response from the SBE41CP violated even the nonpedantic regex */
            status = (errcode==REG_NOMATCH) ? Sbe41cpRegexFail : Sbe41cpRegExceptn; 
         }
         
         /* clean up the regex pattern buffer */
         regfree(&regex); 
      }
   }
   
   return status;
   
   #undef NSUB 
}

/*------------------------------------------------------------------------*/
/* function to get a full PTS sample from the SBE41CP                     */
/*------------------------------------------------------------------------*/
/**
   This function queries the SBE41CP CTD for a full PTS measurement.

      \begin{verbatim}
      input:
         ctdio....A structure that contains pointers to machine dependent
                  primitive IO functions.  See the comment section of the
                  SerialPort structure for details.  The function checks to
                  be sure this pointer is not NULL.

      output:
      
         p........This is where the pressure will be stored when the
                  function returns.  If an error occurs, this is set to NaN
                  (according to IEEE floating point format).
      
         t........This is where the temperature will be stored when the
                  function returns.  If an error occurs, this is set to NaN
                  (according to IEEE floating point format).
      
         s........This is where the salinity will be stored when the
                  function returns.  If an error occurs, this is set to NaN
                  (according to IEEE floating point format).

         This function returns a positive number if the response received
         from the CTD serial port matched (for each of p, t, and s) a regex
         for a float.  Zero is returned if a response was received but it
         failed the regex match.  A negative return value indicates failure
         due to an exceptional condition.  Here are the possible return
         values of this function:
         
         Sbe41cpNoResponse........No response received from SBE41CP.
         Sbe41cpRegExceptn........Response received but regexec() failed with
                                     an exceptional error.
         Sbe41cpNullArg...........Null function argument.
         Sbe41cpRegexFail.........Response received but it did not match (for
                                     each of p, t, and s) the regex pattern for a
                                     float.
         Sbe41cpOk................Response received that matched the pedantic
                                     regex pattern.
         Sbe41cpPedanticFail......Response received that matched a float but
                                     failed to match the pedantic regex pattern.
         Sbe41cpPedanticExceptn...Response received that matched a float but
                                     the pedantic regex experienced an exception.
                                 
      On success, the normal return value for this function will be 'Sbe41cpOk'.
      \end{verbatim}
*/
int Sbe41cpGetPts(float *p, float *t, float *s)
{
   /* define the logging signature */
   static cc FuncName[] = "Sbe41cpGetPts()";
   
   int status=Sbe41cpNullArg;

   /* pet the watchdog timer */
   WatchDog();
      
   /* stack-check assertion */
   assert(StackOk());

   /* make sure that floats are 4 bytes in length */
   assert(sizeof(float)==4);
   
   /* validate the function argument */
   if (!p || !t || !s)
   {
      /* create the message */
      static cc msg[]="NULL function argument(s).\n";

      /* log the message */
      LogEntry(FuncName,msg);
   }

   /* validate the CTD serial ports ioflush() function */
   else if (!(ctdio.ioflush))
   {
      /* create the message */
      static cc msg[]="NULL ioflush() function for serial port.\n";

      /* log the message */
      LogEntry(FuncName,msg);
   }

   else
   {
      int errcode;
      
      /* initialize the communications timeout period */
      const time_t timeout=70;

      /* reinitialize the return values */
      status=Sbe41cpOk;

      /* initialize the return value of 'p', 't', and 's' to IEEE NaN */
      *p=nan(); *t=nan(); *s=nan();
 
      /* activate the SBE41CP with the hardware control lines */
      errcode=CtdPtsSample(buf,MAXLEN,timeout);

      /* log the data received from the SBE41CP */
      if (debuglevel>=4 || (debugbits&SBE41CP_H))
      {
         /* create the message */
         static cc format[]="Received: [%s]\n";

         /* make logentry */
         LogEntry(FuncName,format,buf);
      }
      
      /* check if an error was detected */
      if (errcode<=0)
      {
         /* create the message */
         static cc msg[]="No response from SBE41CP.\n";

         /* make logentry */
         LogEntry(FuncName,msg);

         /* indicate failure */
         status=Sbe41cpNoResponse;
      }
      
      /* subject the SBE41CP response to lexical analysis */
      else
      {
         /* define the number of subexpressions in the regex pattern */
         #define NSUB 3
          
         /* define objects needed for regex matching */
         regex_t regex; regmatch_t regs[NSUB+1]; int errcode;

         /* define the the nonpedantic pattern that will match a float */
         const char *pattern = "^" FIELD "," FIELD "," FIELD;
         
         /* compile the regex pattern */
         assert(!regcomp(&regex,pattern,REG_EXTENDED|REG_NEWLINE));

         /* protect against segfaults */
         assert(regex.re_nsub==NSUB);
         
         /* check if the current line matches the regex */
         if (!(errcode=regexec(&regex,buf,regex.re_nsub+1,regs,0)))
         {
            /* initialize pointers to the P,T,S fields */
            const char *pbuf=0, *tbuf=0, *sbuf=0;

            /* define the pedantic form of the expected response */
            pattern = "^" P "," T "," S "$"; 

            /* jettison the nonpedantic regex */
            regfree(&regex);

            /* compile the pedantic regex pattern */
            assert(!regcomp(&regex,pattern,REG_NOSUB|REG_EXTENDED|REG_NEWLINE));

            /* check if the response matches exactly the expected form */
            if ((errcode=regexec(&regex,buf,0,0,0)))
            {
               /* create the message */
               static cc format[]="Violation of pedantic regex: [%s\\r\\n]\n";

               /* make logentry */
               LogEntry(FuncName,format,buf);

               /* reinitialize the return value */
               status = (errcode==REG_NOMATCH) ? Sbe41cpPedanticFail : Sbe41cpPedanticExceptn;
            }

            /* segment the buffer into separate P,T, and S fields */
            if (regs[1].rm_so>=0 && regs[1].rm_eo>=0) {pbuf=buf+regs[1].rm_so; buf[regs[1].rm_eo]=0;}
            if (regs[2].rm_so>=0 && regs[2].rm_eo>=0) {tbuf=buf+regs[2].rm_so; buf[regs[2].rm_eo]=0;}
            if (regs[3].rm_so>=0 && regs[3].rm_eo>=0) {sbuf=buf+regs[3].rm_so; buf[regs[3].rm_eo]=0;}

            /* jettison the pedantic regex */
            regfree(&regex);

            /* compile the regex pattern for a float */
            assert(!regcomp(&regex,FLOAT,REG_EXTENDED|REG_NEWLINE));

            /* protect against segfaults */
            assert(regex.re_nsub==1);

            /* check the pressure-field against a nonpedantic float regex pattern */
            if (pbuf && !regexec(&regex,pbuf,regex.re_nsub+1,regs,0)) 
            {
               /* extract pressure from the buffer */
               *p = atof(extract(pbuf,regs[1].rm_so+1,regs[1].rm_eo-regs[1].rm_so));
            }

            /* check the temperature-field against a nonpedantic float regex pattern */
            if (tbuf && !regexec(&regex,tbuf,regex.re_nsub+1,regs,0))
            {
               /* extract temperature from the buffer */               
               *t = atof(extract(tbuf,regs[1].rm_so+1,regs[1].rm_eo-regs[1].rm_so));
            }
            
            /* check the salinity-field against a nonpedantic float regex pattern */
            if (sbuf && !regexec(&regex,sbuf,regex.re_nsub+1,regs,0))
            {
               /* extract salinity from the buffer */               
               *s = atof(extract(sbuf,regs[1].rm_so+1,regs[1].rm_eo-regs[1].rm_so));
            }
         }
         
         else
         {
            /* create the message */
            static cc format[]="Violation of 3-fields regex: [%s\\r\\n]\n";

            /* log the message */
            LogEntry(FuncName,format,buf);
            
            /* indicate that the response from the SBE41CP violated even the nonpedantic regex */
            status = (errcode==REG_NOMATCH) ? Sbe41cpRegexFail : Sbe41cpRegExceptn; 
         }
         
         /* clean up the regex pattern buffer */
         regfree(&regex); 
      }
   }
   
   return status;
   
   #undef NSUB 
}

/*------------------------------------------------------------------------*/
/* function to get a full PTSO sample from the SBE41CP/43                 */
/*------------------------------------------------------------------------*/
/**
   This function queries the SBE41CP/43 CTD for a full PTSO measurement.

      \begin{verbatim}
      input:
         ctdio....A structure that contains pointers to machine dependent
                  primitive IO functions.  See the comment section of the
                  SerialPort structure for details.  The function checks to
                  be sure this pointer is not NULL.

      output:
      
         p........This is where the pressure will be stored when the
                  function returns.  If an error occurs, this is set to NaN
                  (according to IEEE floating point format).
      
         t........This is where the temperature will be stored when the
                  function returns.  If an error occurs, this is set to NaN
                  (according to IEEE floating point format).
      
         s........This is where the salinity will be stored when the
                  function returns.  If an error occurs, this is set to NaN
                  (according to IEEE floating point format).
      
         o........This is where the oxygen frequency will be stored when the
                  function returns.  If an error occurs, this is set to NaN
                  (according to IEEE floating point format).

         This function returns a positive number if the response received
         from the CTD serial port matched (for each of p, t, and s) a regex
         for a float.  Zero is returned if a response was received but it
         failed the regex match.  A negative return value indicates failure
         due to an exceptional condition.  Here are the possible return
         values of this function:
         
         Sbe41cpNoResponse........No response received from SBE41CP.
         Sbe41cpRegExceptn........Response received but regexec() failed with
                                     an exceptional error.
         Sbe41cpNullArg...........Null function argument.
         Sbe41cpRegexFail.........Response received but it did not match (for
                                     each of p, t, and s) the regex pattern for a
                                     float.
         Sbe41cpOk................Response received that matched the pedantic
                                     regex pattern.
         Sbe41cpPedanticFail......Response received that matched a float but
                                     failed to match the pedantic regex pattern.
         Sbe41cpPedanticExceptn...Response received that matched a float but
                                     the pedantic regex experienced an exception.
                                 
      On success, the normal return value for this function will be 'Sbe41cpOk'.
      \end{verbatim}
*/
int Sbe41cpGetPtso(float *p, float *t, float *s, float *o)
{
   /* define the logging signature */
   static cc FuncName[] = "Sbe41cpGetPtso()";
   
   int status=Sbe41cpNullArg; 

   /* pet the watchdog timer */
   WatchDog();
     
   /* stack-check assertion */
   assert(StackOk());

   /* make sure that floats are 4 bytes in length */
   assert(sizeof(float)==4);

   /* validate the function argument */
   if (!p || !t || !s || !o)
   {
      /* create the message */
      static cc msg[]="NULL function argument(s).\n";

      /* log the message */
      LogEntry(FuncName,msg);
   }

   /* validate the CTD serial ports ioflush() function */
   else if (!(ctdio.ioflush)) 
   {
      /* create the message */
      static cc msg[]="NULL ioflush() function for serial port.\n";

      /* log the message */
      LogEntry(FuncName,msg);
   }

   else
   {
      int errcode;
      
      /* initialize the communications timeout period */
      const time_t timeout=180;

      /* reinitialize the return values */
      status=Sbe41cpOk;

      /* initialize the return value of 'p', 't', 's', and 'o' to IEEE NaN */
      *p=nan(); *t=nan(); *s=nan(); *o=nan();

      /* activate the SBE41CP with the hardware control lines */
      errcode=CtdPtsSample(buf,MAXLEN,timeout);

      /* log the data received from the SBE41CP */
      if (debuglevel>=4 || (debugbits&SBE41CP_H))
      {
         /* create the message */
         static cc format[]="Received: [%s]\n";

         /* make logentry */
         LogEntry(FuncName,format,buf);
      }
      
      /* check if an error was detected */
      if (errcode<=0)
      {
        /* create the message */
         static cc msg[]="No response from SBE41CP.\n";

         /* make logentry */
         LogEntry(FuncName,msg);

         /* indicate failure */
         status=Sbe41cpNoResponse;
      }
      
      /* subject the SBE41CP response to lexical analysis */
      else
      {
         /* define the number of subexpressions in the regex pattern */
         #define NSUB 4
          
         /* define objects needed for regex matching */
         regex_t regex; regmatch_t regs[NSUB+1]; int errcode;

         /* define the the nonpedantic pattern that will match a float */
         const char *pattern = "^" FIELD "," FIELD "," FIELD "," FIELD;

         /* compile the regex pattern */
         assert(!regcomp(&regex,pattern,REG_EXTENDED|REG_NEWLINE));

         /* protect against segfaults */
         assert(regex.re_nsub==NSUB);
         
         /* check if the current line matches the regex */
         if (!(errcode=regexec(&regex,buf,regex.re_nsub+1,regs,0)))
         {
            const char *pbuf=0, *tbuf=0, *sbuf=0, *obuf=0;

            /* define the pedantic form of the expected response */
            pattern = "^" P "," T "," S "," O "$"; 

            /* jettison the nonpedantic regex */
            regfree(&regex);

            /* compile the regex pattern */
            assert(!regcomp(&regex,pattern,REG_NOSUB|REG_EXTENDED|REG_NEWLINE));

            /* check if the response matches exactly the expected form */
            if ((errcode=regexec(&regex,buf,0,0,0)))
            {
               /* create the message */
               static cc format[]="Violation of pedantic regex: [%s\\r\\n]\n";

               /* log the message */
               LogEntry(FuncName,format,buf);

               /* reinitialize the return value */
               status = (errcode==REG_NOMATCH) ? Sbe41cpPedanticFail : Sbe41cpPedanticExceptn;
            }

            /* segment the buffer into separate P,T, S, and O fields */
            if (regs[1].rm_so>=0 && regs[1].rm_eo>=0) {pbuf=buf+regs[1].rm_so; buf[regs[1].rm_eo]=0;}
            if (regs[2].rm_so>=0 && regs[2].rm_eo>=0) {tbuf=buf+regs[2].rm_so; buf[regs[2].rm_eo]=0;}
            if (regs[3].rm_so>=0 && regs[3].rm_eo>=0) {sbuf=buf+regs[3].rm_so; buf[regs[3].rm_eo]=0;}
            if (regs[4].rm_so>=0 && regs[4].rm_eo>=0) {obuf=buf+regs[4].rm_so; buf[regs[4].rm_eo]=0;}

            /* jettison the pedantic regex */
            regfree(&regex);

            /* compile the regex pattern for a float */
            assert(!regcomp(&regex,FLOAT,REG_EXTENDED|REG_NEWLINE));

            /* protect against segfaults */
            assert(regex.re_nsub==1);

            /* check the pressure-field against a nonpedantic float regex pattern */
            if (pbuf && !regexec(&regex,pbuf,regex.re_nsub+1,regs,0)) 
            {
               /* extract pressure from the buffer */
               *p = atof(extract(pbuf,regs[1].rm_so+1,regs[1].rm_eo-regs[1].rm_so));
            }

            /* check the temperature-field against a nonpedantic float regex pattern */
            if (tbuf && !regexec(&regex,tbuf,regex.re_nsub+1,regs,0))
            {
               /* extract temperature from the buffer */               
               *t = atof(extract(tbuf,regs[1].rm_so+1,regs[1].rm_eo-regs[1].rm_so));
            }

            /* check the salinity-field against a nonpedantic float regex pattern */
            if (sbuf && !regexec(&regex,sbuf,regex.re_nsub+1,regs,0))
            {
               /* extract salinity from the buffer */               
               *s = atof(extract(sbuf,regs[1].rm_so+1,regs[1].rm_eo-regs[1].rm_so));
            }

            /* check the oxygen-field against a nonpedantic float regex pattern */
            if (obuf && !regexec(&regex,obuf,regex.re_nsub+1,regs,0))
            {
               /* extract oxygen from the buffer */               
               *o = atof(extract(obuf,regs[1].rm_so+1,regs[1].rm_eo-regs[1].rm_so));
            }
         }
         
         else
         {
            /* create the message */
            static cc format[]="Violation of 4-fields regex: [%s\\r\\n]\n";

            /* log the message */
            LogEntry(FuncName,format,buf);
            
            /* indicate that the response from the SBE41CP violated even the nonpedantic regex */
            status = (errcode==REG_NOMATCH) ? Sbe41cpRegexFail : Sbe41cpRegExceptn; 
         }
         
         /* clean up the regex pattern buffer */
         regfree(&regex); 
      }
      
      /* disable communications via the CTD serial port */
      CtdDisableIo(); ConioEnable();
   }
   
   return status;
   
   #undef NSUB 
}

/*------------------------------------------------------------------------*/
/* function to log the SBE41's calibration coefficents                    */
/*------------------------------------------------------------------------*/
/**
   This function uses the SBE41 communications mode to log its calibration
   coefficients via the SBE41 'dc' command.
   
      \begin{verbatim}
      input:
         ctdio....A structure that contains pointers to machine dependent
                  primitive IO functions.  See the comment section of the
                  SerialPort structure for details.  The function checks to
                  be sure this pointer is not NULL.

      output:
      
         This function returns a positive number if a response was received
         from the CTD serial port.  Zero is returned if an unexpected
         response was received.  A negative return value indicates failure
         due to an exceptional condition.  Here are the possible return
         values of this function:
         
         Sbe41cpNoResponse........No response received from SBE41.
         Sbe41cpRegExceptn........Response received but regexec() failed with
                                     an exceptional error.
         Sbe41cpNullArg...........Null function argument.
         Sbe41cpRegexFail.........Response received but it did not match the
                                     regex pattern for the serial number.
         Sbe41cpOk................Response received.
                                 
      On success, the normal return value for this function will be 'Sbe41cpOk'.
      \end{verbatim}
*/
int Sbe41cpLogCal(void)
{
   /* define the logging signature */
   static cc FuncName[] = "Sbe41cpLogCal()";
   
   /* initialize the return value */
   int status=Sbe41cpNullArg;
      
   /* pet the watchdog timer */
   WatchDog();
     
   /* stack-check assertion */
   assert(StackOk());

   /* enter SBE41 command mode */
   if ((status=Sbe41cpEnterCmdMode())>0)
   {
      /* define the timeout period for communications mode */
      const time_t To=time(NULL), timeout=2, TimeOut=60;

      /* reinitialize the return value */
      status = Sbe41cpOk;

      /* flush the Rx queue of the ctd serial port */
      if (ctdio.iflush) ctdio.iflush();
      
      /* send the command to display calibration coefficients */
      pputs(&ctdio,"dc\r",timeout,"");

      /* wait for the data to be uploaded to the Ctd fifo */
      while (CtdActiveIo(timeout) && difftime(time(NULL),To)<TimeOut/2) {}
       
      /* enable console io */
      ConioEnable();

      /* read the SBE41 response from the serial port */
      while (pgets(&ctdio,buf,MAXLEN,timeout,"\r\n")>0 && difftime(time(NULL),To)<TimeOut)
      {
         /* look for end of calibration coefficient display */
         if (!strncmp("S>",buf,2)) break;
         
         /* ignore "dc" and log the current coefficients */
         if (strncmp("dc",buf,2))
         {
            /* create the message */
            static cc format[]="%s\n";

            /* make logentry */
            LogEntry(FuncName,format,buf);
         }
      }
   }
   else
   {
      /* create the message */
      static cc format[]="Attempt to enter command mode failed [errcode: %d] - aborting.\n";

      /* make logentry */
      LogEntry(FuncName,format,status); 
   }
   
   /* put the SBE41 back to sleep */
   if (CtdEnableIo()>0 && chat(&ctdio,"\r","S>",2)>0) Sbe41cpExitCmdMode();

   /* enable console IO */
   CtdDisableIo(); ConioEnable();
   
   return status;
}

/*------------------------------------------------------------------------*/
/* function to query an SBE41CP for its serial number                     */
/*------------------------------------------------------------------------*/
/**
   This function queries the SBE41CP for its serial number. 

      \begin{verbatim}
      input:
         ctdio....A structure that contains pointers to machine dependent
                  primitive IO functions.  See the comment section of the
                  SerialPort structure for details.  The function checks to
                  be sure this pointer is not NULL.

      output:
      
         This function returns a positive number if the response received
         from the CTD serial port matched (for each of p, t, and s) a regex
         for a 4-digit integer.  Zero is returned if a response was received
         but it failed the regex match.  A negative return value indicates
         failure due to an exceptional condition.  Here are the possible
         return values of this function:
         
         Sbe41cpNoResponse........No response received from SBE41CP.
         Sbe41cpRegExceptn........Response received but regexec() failed with
                                     an exceptional error.
         Sbe41cpNullArg...........Null function argument.
         Sbe41cpRegexFail.........Response received but it did not matchthe
                                     regex pattern for a 4-digit integer.
         Sbe41cpOk................Response received that matched the pedantic
                                     regex pattern.
                                 
      On success, the normal return value for this function will be 'Sbe41cpOk'.
      \end{verbatim}
*/
int Sbe41cpSerialNumber(void)
{
   /* define the logging signature */
   static cc FuncName[] = "Sbe41cpSerialNumber()";
   
   /* initialize the return value */
   int status=Sbe41cpNullArg;

   /* pet the watchdog timer */
   WatchDog();
   
   /* stack-check assertion */
   assert(StackOk());

   /* SBE41CP serial number is the return value of Sbe41cpEnterCmdMode() */
   if ((status=Sbe41cpEnterCmdMode())<=0)
   {
      /* create the message */
      static cc msg[]="Attempt to query SBE41CP serial number failed.\n";

      /* log the message */
      LogEntry(FuncName,msg);
   }
      
   /* exit the SBE41CP's command mode */
   Sbe41cpExitCmdMode();

   return status;
}

/*------------------------------------------------------------------------*/
/* function to start the continuous profiling mode of the SBE41CP         */
/*------------------------------------------------------------------------*/
/**
   This function initiates the continuous profiling mode of the SBE41CP.

      \begin{verbatim}
      input:
         FlushSec...This specifies the length of time (seconds) that the
                    pump is allowed to flush the conductivity cell before
                    data acquisition is started.
      
      output:

         This function returns a positive number if continuous profile was
         successfully initiated.  Zero is usually returned if the attempt
         failed.  A negative return value indicates failure due to an
         exceptional condition.  Here are the possible return values of this
         function:

         Sbe41cpRegExceptn........Response received but regexec() failed with
                                    an exceptional error.
         Sbe41cpNoResponse........No response received from SBE41CP.
         Sbe41cpNullArg...........Null function argument.
         Sbe41cpRegexFail.........Response received but it did not match the
                                    expected regex pattern.
         Sbe41cpFail..............Attempt to start profile failed.
         Sbe41cpOk................Attempt to start profile was successful.
                                 
      On success, the normal return value for this function will be 'Sbe41cpOk'.
      \end{verbatim}
*/
int Sbe41cpStartCP(time_t FlushSec)
{
   /* define the logging signature */
   static cc FuncName[] = "Sbe41cpStartCP()";
   
   /* initialize the return value */
   int i,status=Sbe41cpNullArg;

   /* define the timeout period */
   const time_t TimeOut = 15;
   
   /* pet the watchdog timer */
   WatchDog();
   
   /* stack-check assertion */
   assert(StackOk());

   /* make sure the pre-profile flush period is reasonable */
   if (FlushSec<10) FlushSec=10; else if (FlushSec>60) FlushSec=60;
   
   /* SBE41CP serial number is the return value of Sbe41cpEnterCmdMode() */
   if ((status=Sbe41cpEnterCmdMode())>0)
   {
      /* define the minimum sample rate */
      const float SecPerSmpl=1.2;

      /* compute the number of samples in the flush period */
      int NSamples=(int)(((float)FlushSec)/SecPerSmpl);
      
      /* create the command to start the profile */
      char buf[32]; snprintf(buf,31,"startprofile%d\r",NSamples);

      /* retry loop */
      for (status=Sbe41cpFail, i=0; i<3; i++)
      {
         /* enable IO from CTD serial port */   
         CtdEnableIo(); 

         /* execute the command to start the continuous profile */
         if (chat(&ctdio,buf,"profile started",TimeOut)>0) 
         {
            /* pause long enough to flush the conductivity cell */
            sleep(FlushSec+5);

            /* log the start of the continuous profile */
            if (debuglevel>=2 || (debugbits&SBE41CP_H))
            {
               /* create the message */
               static cc msg[]="Continuous profile started.\n";

               /* log the message */
               ConioEnable(); LogEntry(FuncName,msg);
            }
            
            /* indicate success */
            status=Sbe41cpOk; break;
         }
      }
   }
      
   /* exit the SBE41CP's command mode */
   else Sbe41cpExitCmdMode();

   /* enable console io */
   CtdDisableIo(); ConioEnable();
 
   if (status<=0)
   {
      /* create the message */
      static cc msg[]="Attempt to start the SBE41CP continuous profile failed.\n";

      /* log the message */
      LogEntry(FuncName,msg);
   }

   return status;
}

/*------------------------------------------------------------------------*/
/* function to query the SBE41CP for configuration/status parameters      */
/*------------------------------------------------------------------------*/
/**
   This function executes an SBE41CP status query and then extracts various
   configuration/status parameters from the response.
   
     \begin{verbatim}
     output:
        serno.....The serial number of the SBE41CP.
        nsample...The number of samples presently stored in the SBE41CP nvram.
        nbin......The number of bins created by the bin-averaging scheme.
        tswait....The prelude period (seconds) before samples are collected.
        topint....The pressure bin interval (dbar) for the shallow sampling regime.
        topsize...The pressure bin size (dbar) for the shallow sampling regime.
        topmax....The maximum pressure (dbar) for the shallow sampling regime.
        midint....The pressure bin interval (dbar) for the mid-depth sampling regime.
        midsize...The pressure bin size (dbar) for the mid-depth sampling regime.
        midmax....The maximum pressure (dbar) for the mid-depth sampling regime.
        botint....The pressure bin interval (dbar) for the deep sampling regime.
        botsize...The pressure bin size (dbar) for the deep sampling regime.
        pcutoff...The cut-off pressure where the SBE41CP pump is shut down
                     in order to avoid contamination by surface film.

     This function returns a positive return value on success and a zero or
     negative value on failure.  Here are the possible return values of this
         function:

         Sbe41cpRegExceptn........Response received but regexec() failed with
                                    an exceptional error.
         Sbe41cpNoResponse........No response received from SBE41CP.
         Sbe41cpNullArg...........Null function argument.
         Sbe41cpFail..............Attempt to start profile failed.
         Sbe41cpOk................Attempt to start profile was successful.
                                 
      On success, the normal return value for this function will be 'Sbe41cpOk'.
   \end{verbatim}
*/
int Sbe41cpStatus(float *pcutoff, unsigned *serno,
                  unsigned *nsample, unsigned *nbin, time_t *tswait, 
                  float *topint, float *topsize, float *topmax,
                  float *midint, float *midsize, float *midmax,
                  float *botint, float *botsize)
{
   #define MaxBufLen 79
   char *p,buf[MaxBufLen+1];

   /* define the parameter tokens */
   const char *const token[]=
   {
      "SERIAL NO.",
      "stop profile when pressure is less than =",
      "number of samples =",
      "number of bins =",
      "top bin interval =",
      "top bin size =",
      "top bin max =",
      "middle bin interval =",
      "middle bin size =",
      "middle bin max =",
      "bottom bin interval =",
      "bottom bin size =",
      "take sample wait time =",
   };

   /* initialize the return value */
   int i,status=Sbe41cpNoResponse;

   /* define the timeout period */
   const time_t TimeOut = 2;

   /* pet the watchdog timer */
   WatchDog();
   
   /* stack-check assertion */
   assert(StackOk());

   /* initialize the function parameters */
   if (serno)   *serno   = (unsigned)(-1);
   if (nsample) *nsample = (unsigned)(-1);
   if (nbin)    *nbin    = (unsigned)(-1);
   if (tswait)  *tswait  = (unsigned)(-1);
   if (topint)  *topint  = nan();
   if (topsize) *topsize = nan(); 
   if (topmax)  *topmax  = nan(); 
   if (midint)  *midint  = nan(); 
   if (midsize) *midsize = nan(); 
   if (midmax)  *midmax  = nan(); 
   if (botint)  *botint  = nan(); 
   if (botsize) *botsize = nan(); 
   if (pcutoff) *pcutoff = nan();

   for (i=0; i<3 && status!=Sbe41cpOk; i++)
   {
      /* flush the IO queues */
      if (ctdio.ioflush) {Wait(10); ctdio.ioflush(); Wait(10);}

      if (chat(&ctdio,"\r","S>",2)>0)
      {
         /* query the SBE41CP for its current configuration */
         pputs(&ctdio,"ds\r",TimeOut,"");

         /* analyze the query response to verify expected configuration */
         while (pgets(&ctdio,buf,MaxBufLen,TimeOut,"\r\n")>0)
         {
            if      (serno   && (p=strstr(buf,token[ 0]))) {*serno   = atoi(p+strlen(token[ 0]));} 
            else if (pcutoff && (p=strstr(buf,token[ 1]))) {*pcutoff = atof(p+strlen(token[ 1]));}
            else if (nsample && (p=strstr(buf,token[ 2]))) {*nsample = atoi(p+strlen(token[ 2]));}
            else if (nbin    && (p=strstr(buf,token[ 3]))) {*nbin    = atoi(p+strlen(token[ 3]));} 
            else if (topint  && (p=strstr(buf,token[ 4]))) {*topint  = atof(p+strlen(token[ 4]));} 
            else if (topsize && (p=strstr(buf,token[ 5]))) {*topsize = atof(p+strlen(token[ 5]));} 
            else if (topmax  && (p=strstr(buf,token[ 6]))) {*topmax  = atof(p+strlen(token[ 6]));} 
            else if (midint  && (p=strstr(buf,token[ 7]))) {*midint  = atof(p+strlen(token[ 7]));} 
            else if (midsize && (p=strstr(buf,token[ 8]))) {*midsize = atof(p+strlen(token[ 8]));} 
            else if (midmax  && (p=strstr(buf,token[ 9]))) {*midmax  = atof(p+strlen(token[ 9]));} 
            else if (botint  && (p=strstr(buf,token[10]))) {*botint  = atof(p+strlen(token[10]));} 
            else if (botsize && (p=strstr(buf,token[11]))) {*botsize = atof(p+strlen(token[11]));} 
            else if (tswait  && (p=strstr(buf,token[12]))) {*tswait  = atol(p+strlen(token[12]));} 
            
            if (strstr(buf,"take sample wait time")) break;
            
            status=Sbe41cpOk;
         }
      }
   }
   
   /* validate each requested parameter */
   if (status>0)
   {
      if (serno   && (*serno   == (unsigned)(-1))) status=Sbe41cpFail;
      if (nsample && (*nsample == (unsigned)(-1))) status=Sbe41cpFail;
      if (nbin    && (*nbin    == (unsigned)(-1))) status=Sbe41cpFail;
      if (tswait  && (*tswait  == (unsigned)(-1))) status=Sbe41cpFail;
      if (topint  && isnan(*topint))               status=Sbe41cpFail;
      if (topsize && isnan(*topsize))              status=Sbe41cpFail; 
      if (topmax  && isnan(*topmax))               status=Sbe41cpFail; 
      if (midint  && isnan(*midint))               status=Sbe41cpFail; 
      if (midsize && isnan(*midsize))              status=Sbe41cpFail; 
      if (midmax  && isnan(*midmax))               status=Sbe41cpFail; 
      if (botint  && isnan(*botint))               status=Sbe41cpFail; 
      if (botsize && isnan(*botsize))              status=Sbe41cpFail; 
      if (pcutoff && isnan(*pcutoff))              status=Sbe41cpFail;
      Wait(100);
   }
   
   return status;

   #undef MaxBufLen
}

/*------------------------------------------------------------------------*/
/* function to stop the continuous profiling mode of the SBE41CP          */
/*------------------------------------------------------------------------*/
/**
   This function stops the continuous profiling mode of the SBE41CP.  The
   mode-select line is set for p-only mode to prevent the pump motor from
   running if continuous mode has already been deactivated (eg.,
   automatically by the SBE41CP).

      \begin{verbatim}
      output:

         This function returns a positive number if continuous profile was
         successfully stopped.  Zero is usually returned if the attempt
         failed.  A negative return value indicates failure due to an
         exceptional condition.  Here are the possible return values of this
         function:

         Sbe41cpNullArg...........Null function argument.
         Sbe41cpFail..............Attempt to stop profile failed.
         Sbe41cpOk................Attempt to stop profile was successful.
                                 
      On success, the normal return value for this function will be 'Sbe41cpOk'.
      \end{verbatim}
*/
int Sbe41cpStopCP(void)
{
   /* define the logging signature */
   static cc FuncName[] = "Sbe41cpStopCP()";
   
   /* initialize the return value */
   int i,status=Sbe41cpNullArg;

   /* define the timeout period */
   const time_t TimeOut = 5;
   
   /* pet the watchdog timer */
   WatchDog();
   
   /* stack-check assertion */
   assert(StackOk());

   /* clear the mode-select line */
   CtdClearModePin(); 

   /* retry loop */
   for (status=Sbe41cpFail, i=0; i<3; i++)
   {
      /* enable IO from CTD serial port */   
      CtdEnableIo(); ctdio.putb('\r'); Wait(10);
  
      /* execute the command to stop the continuous profile */
      if (chat(&ctdio,"stopprofile\r","profile stopped",TimeOut)>0) 
      {
         /* enable console IO */
         ConioEnable();

         /* log the end of the continuous profile */
         if (debuglevel>=2 || (debugbits&SBE41CP_H))
         {
            /* create the message */
            static cc msg[]="Continuous profile stopped.\n";

            /* make logentry */
            LogEntry(FuncName,msg);
         }
         
         /* enable IO from CTD serial port */   
         CtdEnableIo(); 
 
         /* exit the SBE41CP's command mode */ 
         sleep(2); if (chat(&ctdio,"\r","S>",2)>0) Sbe41cpExitCmdMode();

         /* indicate success */
         status=Sbe41cpOk; break;
      }
   }

   /* enable console IO */
   CtdDisableIo(); ConioEnable();

   if (status<=0)
   {
      /* create the message */
      static cc msg[]="Attempt to stop SBE41CP continuous profile failed.\n";
      
      /* make logentry */
      LogEntry(FuncName,msg);
   }

   return status;
}

/*------------------------------------------------------------------------*/
/* function to configure the presample pump period of the SBE41CP         */
/*------------------------------------------------------------------------*/
/**
   This function configures the presample pumping period of the SBE41CP.  It
   does this by entering the SBE41CP's command mode and executing the
   SBE41CP's 'tswait' command.  This configuration parameter applies only to
   the spot-sampled PTS mode on SBE41CPs without an oxygen sensor.  The
   tswait command determines the number of seconds after pump activation
   before a full PTS sample is collected.  

      \begin{verbatim}
      input:
         sec...The number of seconds after pump activation before the full
               PTS sample is collected.
      
      output:

         This function returns a positive number if the configuration
         attempt was successful.  Zero is returned if a response was
         received but it failed the regex match.  A negative return value
         indicates failure due to an exceptional condition.  Here are the
         possible return values of this function:

         Sbe41cpRegexFail.........Response received, regex no-match
         Sbe41cpChatFail..........Execution of configuration commands failed.
         Sbe41cpRegExceptn........Response received but regexec() failed with
                                    an exceptional error.
         Sbe41cpNoResponse........No response received from SBE41CP.
         Sbe41cpNullArg...........Null function argument.
         Sbe41cpFail..............Post-configuration verification failed.
         Sbe41cpOk................Configuration attempt was successful.
                                 
      On success, the normal return value for this function will be 'Sbe41cpOk'.
      \end{verbatim}
*/
int Sbe41cpTsWait(time_t sec)
{
   /* define the logging signature */
   static cc FuncName[] = "Sbe41cpTsWait()";
   
   /* initialize the return value */
   int status=Sbe41cpNullArg;

   /* define the timeout period */
   const time_t TimeOut = 2;
   
   /* pet the watchdog timer */
   WatchDog();
   
   /* stack-check assertion */
   assert(StackOk());

   /* make sure the wait period is well conditioned */
   if (sec<2) sec=2; else if (sec>60) sec=60;
   
   /* SBE41CP serial number is the return value of Sbe41cpEnterCmdMode() */
   if ((status=Sbe41cpEnterCmdMode())>0)
   {
      #define MaxBufLen 31
      char buf[MaxBufLen+1];
      
      time_t tswait;
         
      /* reinitialize the return value */
      status=Sbe41cpOk;

      /* write the command to set the pressure cutoff */
      snprintf(buf,MaxBufLen,"tswait=%ld\r",sec);
      
      /* initialize the control parameters of the SBE41CP */
      if (chat(&ctdio, buf,"S>", TimeOut)<=0)
      {
         /* create the message */
         static cc msg[]="chat() failed.\n";

         /* log the configuration failure */
         ConioEnable(); LogEntry(FuncName,msg);

         /* indicate failure */
         status=Sbe41cpChatFail;
      }

      /* analyze the query response to verify expected configuration */
      else if ((status=Sbe41cpStatus(NULL, NULL, NULL, NULL, &tswait, NULL, NULL,
                                     NULL, NULL, NULL, NULL, NULL, NULL))>0)
      { 
         /* verify the configuration parameters */
         if (tswait!=sec)
         {
            /* create the message */
            static cc msg[]="Configuration failed.\n";
               
            /* log the configuration failure */
            ConioEnable(); LogEntry(FuncName,msg);

            /* indicate failure */
            status=Sbe41cpFail;
         }

         /* log the configuration success */
         else if (debuglevel>=2 || (debugbits&SBE41CP_H))
         {
            /* create the message */
            static cc msg[]="Configuration successful.\n";
  
            /* log the configuration failure */
            ConioEnable(); LogEntry(FuncName,msg);
         }
      }
   }
  
   /* put the SBE41 back to sleep */
   if (CtdEnableIo()>0 && chat(&ctdio,"\r","S>",2)>0) Sbe41cpExitCmdMode();

   /* enable console IO */
   CtdDisableIo(); ConioEnable();

   if (status<=0)
   {
      /* create the message */
      static cc msg[]="Attempt to set up SBE41CP failed.\n";

      /* log the message */
      LogEntry(FuncName,msg);
   }

   return status;
   
   #undef NSUB
   #undef MaxBufLen
}

/*------------------------------------------------------------------------*/
/* function to upload the continuous profile from the SBE41CP             */
/*------------------------------------------------------------------------*/
/**
   This function uploads the continuous profile from the SBE41CP and writes
   it to a file.  

      \begin{verbatim}
      input:
         dest....The file to which the profile will be written.
         
      output:

         This function returns a positive number if continuous profile was
         successfully stopped.  Zero is usually returned if the attempt
         failed.  A negative return value indicates failure due to an
         exceptional condition.  Here are the possible return values of this
         function:

         Sbe41cpRegexFail.........Response received, regex no-match
         Sbe41cpChatFail..........Execution of configuration commands failed.
         Sbe41cpRegExceptn........Response received but regexec() failed with
                                    an exceptional error.
         Sbe41cpNoResponse........No response received from SBE41CP.
         Sbe41cpNullArg...........Null function argument.
         Sbe41cpFail..............General failure.
         Sbe41cpOk................Configuration attempt was successful.

      On success, the normal return value for this function will be 'Sbe41cpOk'.
      \end{verbatim}
*/
int Sbe41cpUploadCP(FILE *dest)
{
   /* define the logging signature */
   static cc FuncName[] = "Sbe41cpUploadCP()";

   #ifdef IDO        
      #define ZEROS "000000000000000000"
   #else
      #define ZEROS "00000000000000"
   #endif
   
   #define MaxStrLen 80
   char buf[MaxStrLen+1];
   unsigned n,zeros,serno,nsample,nbin;
   const int NRetry = 2;
   
   /* initialize the return value */
   int i, status=Sbe41cpNullArg;
   
   /* pet the watchdog timer */
   WatchDog();
   
   /* stack-check assertion */
   assert(StackOk());

   for (i=0; i<NRetry && status<=0; i++)
   {
      /* execute commands to bin-average the samples */
      if ((status=Sbe41cpBinAverage())>0)
      {
         /* reinitialize return value */
         status=Sbe41cpFail; 

         /* flush the CTD IO buffers */
         if (ctdio.ioflush) ctdio.ioflush();
         
         /* make sure that command mode is still active */
         CtdEnableIo(); Sbe41cpEnterCmdMode();
                  
         /* query for the serial number and number of samples \& bins */
         if (Sbe41cpStatus(NULL,&serno,&nsample,&nbin,NULL,NULL,
                           NULL,NULL,NULL,NULL,NULL,NULL,NULL)<=0)
         {
            /* create the message */
            static cc msg[]="Status request failed.\n";

            /* log the message */
            LogEntry(FuncName,msg);
         }

         else
         {
            /* log the number of samples and bins */
            if (debuglevel>=2 || (debugbits&SBE41CP_H))
            {
               /* create the message */
               static cc format[]="Sbe41cpSerNo[%04u] NSample[%u] NBin[%u]\n";

               /* log the message */
               ConioEnable(); LogEntry(FuncName,format,serno,nsample,nbin); 
            }
            
            /* bin averages aren't computed for fewer than 10 samples */
            if (nsample<10)
            {
               /* create the message */
               static cc msg[]="Too few samples.\n";

               /* log the message */
               LogEntry(FuncName,msg);
         
               /* reinitialize the return value */
               status=Sbe41cpTooFew;
            }

            /* make sure that bins have been defined */
            else if (!nbin)
            {
               /* create the message */
               static cc msg[]="No bins defined; execute Sbe41cpBinAverage().\n";
               
               /* log the message */
               LogEntry(FuncName,msg);
            }

            /* flush the CTD IO queues */
            else if (!ctdio.ioflush || ctdio.ioflush()<=0)
            {
               /* create the message */
               static cc msg[]="Unable to flush CTD IO buffers.\n";
               
               /* log the message */
               LogEntry(FuncName,msg);
            }

            /* execute the command to upload the hex data */         
            else if (chat(&ctdio,"dah\r","dah\r\n",2)<=0)
            {
               /* create the message */
               static cc msg[]="Unable to upload data from SBE41CP.\n";
               
               /* log the message */
               LogEntry(FuncName,msg);
            }
            
            else
            {
               /* define quantities to implement time-out protection */
               const time_t timeout=15, TimeOut=900;  time_t To=time(NULL);
                        
               /* wait for the data to be uploaded to the Ctd fifo */
               while (CtdActiveIo(timeout) && difftime(time(NULL),To)<TimeOut) {}
      
               /* send the command to shutdown the SBE41CP */
               pputs(&ctdio,"qsr",2,"\r"); 

               /* reinitialize return value */
               ConioEnable();
         
               /* write the time-stamp, nmuber of samples, and number of bins */
               if (dest)
               { 
                  char buf[32]; time_t To=time(NULL);
                  strftime(buf,31,"%b %d %Y %H:%M:%S",gmtime(&To));
                  fprintf(dest,"# %s Sbe41cpSerNo[%04u] NSample[%u] NBin[%u]\n",
                          buf,serno,nsample,nbin);
               }
               
               /* loop through the uploaded lines */
               for (To=time(NULL), zeros=0 ,n=0; pgets(&ctdio,buf,MaxStrLen,5,"\r\n")>0 &&
                       difftime(time(NULL),To)<TimeOut; n++)
               {
                  /* check if upload is complete */
                  if (strstr(buf,"upload complete")) {status=Sbe41cpOk; break;}

                  /* check if the current line is all zeros */
                  else if (!strcmp(buf,ZEROS)) zeros++;
         
                  else
                  {
                     /* write the current uploaded line(s) to the output file */
                     if (dest)
                     {
                        if (zeros>0) {fprintf(dest,ZEROS"[%d]\n",zeros);}
                        fprintf(dest,"%s\n",buf);
                     }
                     
                     /* log the current uploaded line(s) */
                     if (debuglevel>=4) 
                     {
                        /* create the message */
                        static cc format[]="%s\n";
                        
                        if (zeros>0)
                        {
                           /* create the message */
                           static cc format[]=ZEROS"[%d]\n";

                           /* log the message */
                           LogEntry(FuncName,format,zeros);
                        }

                        /* log the buffer */
                        LogEntry(FuncName,format,buf);
                     }
                     
                     /* reinitialize the zeros-counter */
                     zeros=0; 
                  }
               }

               /* write the remaining uploaded lines to the output file */
               if (dest && zeros>0) {fprintf(dest,ZEROS"[%d]\n",zeros);}

               /* log the remaining uploaded lines */
               if (debuglevel>=4 && zeros>0)
               {
                  /* create the message */
                  static cc format[]=ZEROS"[%d]\n";
                  
                  /* log the message */
                  LogEntry(FuncName,format,zeros);
               }

               fflush(dest);
            }
         }
         
         /* check to see if too few samples were collected in CP mode */
         if (status==Sbe41cpTooFew) {break;}

         /* check for other failures */
         else if (status<=0 && i<(NRetry-1))
         {
            /* create the message */
            static cc format[]="Upload[%d] failed [errcode: %d].  "
               "Waiting 3 minutes before retrying.\n";
            
            /* log the message */
            ConioEnable(); LogEntry(FuncName,format,i,status);

            /* pause for retry */
            sleep(180);
         }
      }
   }

   /* check for command prompt and put the SBE41CP back to sleep */
   if (status<0 && CtdEnableIo()>0 && chat(&ctdio,"\r","S>",2)) Sbe41cpExitCmdMode();

   /* enable IO from CTD serial port */   
   CtdDisableIo(); ConioEnable(); 

   if (status<=0)
   {
      /* create the message */
      static cc format[]="Upload failed [errcode: %d].\n";

      /* log the message */
      LogEntry(FuncName,format,status); 
   }
   
   else if (debuglevel>=2 || (debugbits&SBE41CP_H))
   {
      /* create the message */
      static cc format[]="Continuous profile uploaded [%d lines].\n";

      /* log the message */
      LogEntry(FuncName,format,n);
   }
   
   return status;
   
   #undef MaxStrLen
}

/*------------------------------------------------------------------------*/
/* function to compute the total pumping time of the SBE41/43F            */
/*------------------------------------------------------------------------*/
/**
   This function computes the total pumping time for the adaptive pumping
   scheme that SeaBird uses for the SBE41/43F.  The response time scale of
   the SBE43F is pressure and temperature dependent  The documentation used to
   write this function was the C source code for the main() function of the
   SBE41/43F's firmware (provided by Norge).
  
      \begin{verbatim}
      input:
         p ....... The insitu pressure (dbars).
         t ....... The insitu temperture (C).
         Tau1P ... The time-scale of the response of the IDO sensor at
                     atmospheric pressure and 20C.
         
      output:
         This function returns the total pumping time (seconds) for the
         SBE41/43F.  The following table shows a few values.

                           [Tau-1P-20C=1.00sec]
         Pres |          T e m p e r a t u r e (C)  
         dbar |  -2   0   2   4   8  12  16  20  24  28  30  36
         -----+------------------------------------------------
            0 |  38  35  32  30  25  21  18  15  13  11  11  11
          100 |  39  36  33  30  25  22  18  15  13  11  11  11
          200 |  39  36  33  31  26  22  19  16  13  11  11  11
          300 |  40  37  34  31  26  22  19  16  14  11  11  11
          400 |  41  38  35  32  27  23  19  16  14  12  11  11
          500 |  42  38  35  33  28  23  20  17  14  12  11  11
          600 |  43  39  36  33  28  24  20  17  14  12  11  11
          700 |  44  40  37  34  29  24  21  17  15  12  11  11
          800 |  44  41  38  35  29  25  21  18  15  13  12  11
          900 |  45  42  38  35  30  25  21  18  15  13  12  11
         1000 |  46  42  39  36  30  26  22  18  16  13  12  11
         1100 |  47  43  40  37  31  26  22  19  16  13  12  11
         1200 |  48  44  41  37  32  27  23  19  16  14  13  11
         1300 |  49  45  41  38  32  27  23  20  17  14  13  11
         1400 |  50  46  42  39  33  28  24  20  17  14  13  11
         1500 |  51  47  43  40  34  28  24  20  17  15  13  11
         1600 |  52  48  44  40  34  29  24  21  18  15  14  11
         1700 |  53  49  45  41  35  30  25  21  18  15  14  11
         1800 |  54  50  46  42  36  30  25  22  18  15  14  11
         1900 |  55  51  47  43  36  31  26  22  19  16  14  11
         2000 |  56  52  48  44  37  31  26  22  19  16  15  11

                           [Tau-1P-20C=4.00sec]
         Pres |          T e m p e r a t u r e (C)  
         dbar |  -2   0   2   4   8  12  16  20  24  28  30  36
         -----+------------------------------------------------
            0 |  55  51  47  43  36  31  26  22  19  16  14  11
          100 |  56  52  48  44  37  31  27  22  19  16  15  11
          200 |  57  53  49  45  38  32  27  23  19  16  15  12
          300 |  59  54  49  46  39  33  28  23  20  17  15  12
          400 |  60  55  50  46  39  33  28  24  20  17  16  12
          500 |  61  56  51  47  40  34  29  24  21  17  16  12
          600 |  62  57  53  48  41  35  29  25  21  18  16  13
          700 |  63  58  54  49  42  35  30  25  21  18  17  13
          800 |  65  59  55  50  42  36  30  26  22  18  17  13
          900 |  66  61  56  51  43  37  31  26  22  19  17  13
         1000 |  67  62  57  52  44  37  32  27  23  19  18  14
         1100 |  68  63  58  53  45  38  32  27  23  20  18  14
         1200 |  70  64  59  54  46  39  33  28  24  20  18  14
         1300 |  71  65  60  55  47  40  34  28  24  20  19  15
         1400 |  73  67  61  57  48  40  34  29  25  21  19  15
         1500 |  74  68  63  58  49  41  35  30  25  21  19  15
         1600 |  76  69  64  59  50  42  36  30  25  22  20  15
         1700 |  77  71  65  60  51  43  36  31  26  22  20  16
         1800 |  79  72  66  61  52  44  37  31  27  22  21  16
         1900 |  80  74  68  62  53  45  38  32  27  23  21  16
         2000 |  82  75  69  64  54  46  39  33  28  23  21  17
      \end{verbatim}
*/
time_t Sbe43fPumpTime(float p, float t, float Tau1P)
{
   float Tau;
   time_t PumpTime;

   /* define the temperature coefficients (ref: SWJ16:259, p289) */
   const float Tcor = -4.1776e-2;

   /* define the pressure coefficient (ref: SWJ16:259, p289) */
   const float Pcor = 1.964e-4;

   /* define the SBE43F response timescale at 20C, 0dbar */
   if (Tau1P<=1 || Tau1P>10) Tau1P = 2.75;

   /* make sure the input temperature is well conditioned */
   if (t<-2 || t>40) t=4;

   /* make sure the input pressure is well conditioned */
   if (p<0 || p>2500) p=2000;

   /* compute the SBE43F response timescale at t,p (ref: SWJ16:259, p289) */
   Tau = Tau1P*exp(Pcor*p)*exp(Tcor*(t-20));

   /* make sure the response timescale is well conditioned */
   if (Tau<2.0) Tau=2.0; else if (Tau>30.0) Tau=30.0;

   /* compute the total pumping time of the SBE41/SBE43F */
   PumpTime = (time_t)(5.5*Tau + 0.5);
   
   return PumpTime;
}

/*------------------------------------------------------------------------*/
/* function to query the SBE41cp for the SBE43i timescale (Tau)           */
/*------------------------------------------------------------------------*/
/**
   This function queries the SBE41cp for the diffusion time scale of the
   SBE43i oxygen sensor.  
*/
float Sbe43Tau(void) 
{
   #define MaxBufLen 79
   char *p,buf[MaxBufLen+1];
   float Tau=nan();
   
   const char *token="PTAU_20 =";
   
   /* initialize the return value */
   int i,status=Sbe41cpNoResponse;

   /* define the timeout period */
   const time_t TimeOut = 2;

   /* pet the watchdog timer */
   WatchDog();
   
   /* stack-check assertion */
   assert(StackOk());

   for (i=0; i<3 && status!=Sbe41cpOk; i++)
   {
      /* flush the IO queues */
      if (ctdio.ioflush) {ctdio.ioflush();}

      if (chat(&ctdio,"\r","S>",2)>0)
      {
         /* query the SBE41CP for its current configuration */
         pputs(&ctdio,"dc\r",TimeOut,"");

         /* analyze the query response to verify expected configuration */
         while (pgets(&ctdio,buf,MaxBufLen,TimeOut,"\r\n")>0)
         {
            if ((p=strstr(buf,token)))
            {
               Tau = atoi(p+strlen(token));
               status=Sbe41cpOk;
               break;
            } 
         }
      }
   }
   
   return status;

   #undef MaxBufLen
}

/*------------------------------------------------------------------------*/
/* function to negotiate commands                                         */
/*------------------------------------------------------------------------*/
/**
   This function transmits a command string to the serial port and verifies
   and expected response.  The command string should not include that
   termination character (\r), as this function transmits the termination
   character after the command string is transmitted.  The command string
   may include wait-characters (~) to make the processing pause as needed.
   The command string is processed each byte in turn.  Each time a
   wait-character is encountered, processing is halted for one wait-period
   (1 sec) and then processing is resumed.
   
      \begin{verbatim}
      input:

         port.......A structure that contains pointers to machine dependent
                    primitive IO functions.  See the comment section of the
                    SerialPort structure for details.  The function checks
                    to be sure this pointer is not NULL.

         cmd........The command string to transmit.

         expect.....The expected response to the command string.

         sec........The number of seconds this function will attempt to
                    match the prompt-string.

      output:

         This function returns a positive number if the exchange was
         successful.  Zero is returned if the exchange failed.  A negative
         number is returned if the function parameters were determined to be
         ill-defined. 
         
      \end{verbatim}

   written by Dana Swift
*/
static int chat(const struct SerialPort *port, const char *cmd,
                const char *expect, time_t sec)
{
   /* define the logging signature */
   static cc FuncName[] = "sbe41cp.c:chat()";
   
   int status = -1;

   /* enable console IO */
   ConioEnable();
   
   /* verify the serialport */
   if (!port)
   {
      /* create the message */
      static cc msg[]="Serial port not ready.\n";

      /* log the message */
      LogEntry(FuncName,msg);
   }

   /* verify the cmd */
   else if (!cmd) 
   {
      /* create the message */
      static cc msg[]="NULL pointer to the command string.\n";

      /* log the message */
      LogEntry(FuncName,msg);
   }

   /* verify the expect string */
   else if (!expect)
   {
      /* create the message */
      static cc msg[]="NULL pointer to the expect string.\n";

      /* log the message */
      LogEntry(FuncName,msg);
   }

   /* verify the serial port's putb() function */
   else if (!port->putb)
   {
      /* create the message */
      static cc msg[]="NULL putb() function for serial port.\n";

      /* log the message */
      LogEntry(FuncName,msg);
   }

   /* verify the serial port's getb() function */
   else if (!port->getb)
   {
      /* create the message */
      static cc msg[]="NULL getb() function for serial port.\n";

      /* log the message */
      LogEntry(FuncName,msg);
   }

   /* verify the timeout period */
   else if (sec<=0)
   {
      /* create the message */
      static cc format[]="Invalid time-out period: %ld\n";

      /* log the message */
      LogEntry(FuncName,format,sec);
   }
   
   /* flush the IO buffers prior to sending the command string */
   else if (pflushio(port)<=0)
   {
      /* create the message */
      static cc msg[]="Attempt to flush IO buffers failed.";

      /* log the message */
      LogEntry(FuncName,msg);
   }
   
   else
   {
      /* compute the length of the command string */
      int i,len=strlen(cmd);

      /* work around a time descretization problem */
      if (sec==1) sec=2;

      CtdEnableIo(); Wait(50);
      
      /* transmit the command to the serial port */
      for (status=0, i=0; i<len; i++)
      {
         if (port->putb(cmd[i])<=0)
         {
            /* create the message */
            static cc format[]="Attempt to send command string (%s) failed.\n";

            /* log the message */
            ConioEnable(); LogEntry(FuncName,format,cmd);
            
            goto Err;
         }
      }
      
      /* seek the expect string in the modem response */
      if (*expect)
      {
         unsigned char byte;

         /* get the reference time */
         time_t Tnow,To=time(NULL);
         
         /* compute the length of the prompt string */
         int len=strlen(expect);
      
         /* define the index of prompt string */
         i=0;
         
         do 
         {
            /* read the next byte from the serial port */
            if (pgetb(port,&byte,1)>0)
            {
               /* check if the current byte matches the expected byte from the prompt */
               if (byte==expect[i]) {i++;} else i=0;

               /* the expect-string has been found if the index (i) matches its length */
               if (i>=len) {status=1; break;}
            }
            
            /* get the current time */
            Tnow=time(NULL);
         }

         /* check the termination conditions */
         while (Tnow>=0 && To>=0 && difftime(Tnow,To)<sec);

         /* write the response string if the prompt was found */
         if (status<=0)
         {
            /* create the message */
            static cc format[]="Expected string [%s] not received.\n";

            /* log the message */
            ConioEnable(); LogEntry(FuncName,format,expect);
         }
         
         /* report a successful chat session */
         else if (debuglevel>=3 || (debugbits&SBE41CP_H))
         {
            /* create the message */
            static cc format[]="Expected response [%s] received.\n";

            /* log the message */
            ConioEnable(); LogEntry(FuncName,format,expect);
         }

         /* wait 100msec before proceeding */
         if (status>0) Wait(50);
      }
      else status=1;
   }

   Err: /* collection point for errors */

   CtdEnableIo();
   
   return status;
}
