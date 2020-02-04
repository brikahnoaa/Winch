//Changed by H. Matsumoto OSU
#include <apf9.h>
#include <logger.h>
#include <control.h>
#include <cmds.h>

/* function prototypes */
int FwStartUp(void);

/* the firmware revision is supplied automatically from the compile command */
//const unsigned long FwRev=0x92009;//HM
const unsigned long FwRev=0x15010616;//YYMMDD HM
//cotrol.c - Changed the wait time from 5000 to 15000 to allow DSP to close file 5/24/10
//const unsigned long FwRev=0x100305b;//YYMMDD HM
//Modified by Haru Matsumoto
//Programs modified
//1)Upload.c - changed to communicate PMEL Rudics (see Upload.c)
//2)quecom.c - added to interface with signal processor board (DSP) by COM1.
//3)cmd.c    - changed to include now modified and additional commands through
//			   console and Iridium interface.
//4)control.c- changed to control DSP
//5)decent.c - changed to add a logic to make the float to ascend
//6)config.c - User function is now to change gain, detection number, DSP pwr ON depth.
//			   Pwd is to Quephone name (Q001 etc).
//7)profile.c- added a few DSP interface commmands.
//8)telemetry.c- minor change in how Iridium logout
//9)upload.c - significant changes on how files are sent and command is received.
// 			   Compliant to the NOAA Rudic protocol, only send one file per connect.
//			   If the file exceeds 20kB, it splits up the files and sends one file
//			   at a time.			   
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * $Id: apf9main.c,v 1.9.2.1 2008/09/11 19:53:00 dbliudnikas Exp $
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Copyright University of Washington.   Written by Dana Swift.
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
 *
 * RCS Log:
 *
 * $Log: apf9main.c,v $
 * Revision 1.9.2.1  2008/09/11 19:53:00  dbliudnikas
 * Replace SBE41 with Seascan TD: comments only
 *
 * Revision 1.9  2007/05/08 18:10:39  swift
 * Added attribution just below the copyright in the main comment section.
 *
 * Revision 1.8  2006/10/13 18:53:09  swift
 * Add conditioning to logstream.
 *
 * Revision 1.7  2006/08/17 21:17:59  swift
 * Modifications to allow better logging control.
 *
 * Revision 1.6  2006/04/21 13:45:42  swift
 * Changed copyright attribute.
 *
 * Revision 1.5  2004/12/29 23:11:26  swift
 * Modified LogEntry() to use strings stored in the CODE segment.  This saves
 * lots of space in the DATA segment and significantly speeds code start-up.
 *
 * Revision 1.4  2004/03/24 00:15:01  swift
 * Added provision for debuglevel to be a persistent variable.
 *
 * Revision 1.3  2003/11/20 18:59:47  swift
 * Added GNU Lesser General Public License to source file.
 *
 * Revision 1.2  2003/09/11 17:15:01  swift
 * Fixed return value of FwStartUp().
 *
 * Revision 1.1  2003/08/31 23:14:38  swift
 * Initial revision
 *
 *========================================================================*/
/* firmware to control an Apex Seascan profiler                           */
/*========================================================================*/
void main(void)
{
   /* run firmware startup code */
   FwStartUp();

   /* check for user request to enter command mode */
   if (CmdModeRequest()) CmdMode();
  
   /* check magnetic switch user interface for mission launch request */
   if (MagSwitchToggled()) MissionLaunch();
   
   /* transfer to mission control agent */
   MissionControlAgent();
}

/*------------------------------------------------------------------------*/
/* function to initialize the mission on program startup                  */
/*------------------------------------------------------------------------*/
/**
   This function initializes the mission at program startup.  A positive
   value is returned on success; otherwise zero is returned.
*/
int FwStartUp(void)
{
   /* define the logging signature */
   static cc FuncName[] = "FwStartUp()";

   int status=0;

   /* initialize APF9 hardware and low-level code */
   Apf9Init();
	
   /* validate the mission currently stored in nonvolatile ram */
   if (MissionValidate()<=0)
   {
      /* create the message */
      static cc msg[]="Attempt to validate mission failed.  "
         "Initializing with default mission.\n";

      /* log the message */
      LogEntry(FuncName,msg); 

      /* initialize the mission with the default mission */
      mission=DefaultMission;
   }
   else {status=1;}

   /* validate the logstream */
   if (fstreamok(logstream)<=0) logstream=NULL;
   
   /* validate the debuglevel */
   if (debuglevel>5) debugbits=2;

   return status;
}

