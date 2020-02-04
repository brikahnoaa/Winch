#ifndef EEPROM_H
#define EEPROM_H (0x01f0U)

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * $Id: eeprom.c,v 1.7 2007/05/08 18:10:39 swift Exp $
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
 */
/** RCS log of revisions to the C source code.
 *
 * \begin{verbatim}
 * $Log: eeprom.c,v $
 * Revision 1.7  2007/05/08 18:10:39  swift
 * Added attribution just below the copyright in the main comment section.
 *
 * Revision 1.6  2006/10/11 20:59:51  swift
 * Integrated the new flashio file system.
 *
 * Revision 1.5  2006/04/21 13:45:42  swift
 * Changed copyright attribute.
 *
 * Revision 1.4  2004/12/29 23:11:27  swift
 * Modified LogEntry() to use strings stored in the CODE segment.  This saves
 * lots of space in the DATA segment and significantly speeds code start-up.
 *
 * Revision 1.3  2004/02/05 23:55:27  swift
 * Cleaved basic eeprom functionality into a library API.
 *
 * Revision 1.2  2003/11/20 18:59:47  swift
 * Added GNU Lesser General Public License to source file.
 *
 * Revision 1.1  2003/09/10 17:50:56  swift
 * Initial revision
 * \end{verbatim}
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
#define eepromChangeLog "$RCSfile: eeprom.c,v $ $Revision: 1.7 $ $Date: 2007/05/08 18:10:39 $"

#include <control.h>

/* function prototypes */
struct MissionParameters *MissionParametersRead(struct MissionParameters *mission);
int          MissionParametersWrite(struct MissionParameters *mission);
int          PrfIdGet(void);
int          PrfIdSet(unsigned int pid);
int          PrfIdIncrement(void);
enum State   StateGet(void);
int          StateSet(enum State state);
int          Tc58v64BadBlockListPut(void);
int          Tc58v64BadBlockListGet(void);
unsigned int Tc58v64ErrorsCorrectable(void);
int          Tc58v64ErrorsInit(void);
unsigned int Tc58v64ErrorsNoncorrectable(void);
void         Tc58v64IncrementErrorsCorrectable(void);
void         Tc58v64IncrementErrorsNoncorrectable(void);

#endif /* EEPROM_H */

#include <apf9.h>
#include <crc16bit.h>
#include <logger.h>
#include <x24c16.h>
#include <tc58v64.h>

#define PrfId                  (0) 
#define MissionAddrPrimary     (2)
#define MissionAddrMirror      (MissionAddrPrimary+sizeof(struct MissionParameters))
#define StateAddrPrimary       (MissionAddrMirror +sizeof(struct MissionParameters))
#define StateAddrMirror        (StateAddrPrimary+2)
#define EepromSize             (16*KB)
#define EepromBadBlockAddr     (EepromSize-(Tc58v64BadBlockMax*sizeof(unsigned int)))
#define EepromBadBlockChecksum (EepromBadBlockAddr-sizeof(unsigned int))
#define EepromCErrorIndx       (EepromBadBlockChecksum-sizeof(unsigned int))
#define EepromErrorIndx        (EepromCErrorIndx-sizeof(unsigned int))

/*------------------------------------------------------------------------*/
/* function to read the mission configuration from EEPROM                 */
/*------------------------------------------------------------------------*/
/**
   This function reads the mission configuration from EEPROM and validates
   it by computing the mission signature and comparing to the signature
   stored in EEPROM.  If the validation fails, a mirror copy of the mission
   configuration is read.  If the mirror copy is also invalid then this
   function returns NULL otherwise this function returns a pointer to the
   mission configuration.
*/
struct MissionParameters *MissionParametersRead(struct MissionParameters *mission)
{
   unsigned int crc,i;
   static struct MissionParameters m;
   unsigned char *byte;

   /* make sure that the pointer to the MissionParameters is valid */
   if (!mission) mission=&m;

   /* read the mission parameters from EEPROM */
   for (byte=(unsigned char *)mission, i=0; i<sizeof(*mission); i++,byte++)
   {
      *byte = EERead(MissionAddrPrimary+i);
   }

   /* compute the mission signature */
   crc = Crc16Bit((unsigned char *)mission, sizeof(*mission)-sizeof(mission->crc));

   /* validate the primary mission parameters */
   if (crc!=mission->crc)
   {
      /* primary mission parameters are not valid; read the mirror */
      for (byte=(unsigned char *)mission, i=0; i<sizeof(*mission); i++,byte++)
      {
         *byte = EERead(MissionAddrMirror+i);
      }

      /* compute the mission signature */
      crc = Crc16Bit((unsigned char *)mission, sizeof(*mission)-sizeof(mission->crc));
   }

   /* validate the mission parameters */
   return (crc==mission->crc) ? mission : NULL;
}

/*------------------------------------------------------------------------*/
/* function to write the mission configuration to EEPROM                  */
/*------------------------------------------------------------------------*/
/**
   This function writes the mission configuration to EEPROM.  This function
   returns a positive value if successful.  A negative value indicates a
   NULL function argument.
*/
int MissionParametersWrite(struct MissionParameters *mission)
{
   /* define the logging signature */
   static cc FuncName[] = "MissionParametersWrite()";

   /* initialize the return value */
   int status=-1;

   /* validate the function parameter */
   if (!mission)
   {
       /* create the message */
      static cc msg[]="NULL function argument.\n";

      /* make the logentry */
      LogEntry(FuncName,msg);
   }
   
   else
   {
      unsigned int i;
      unsigned char *byte;

      /* compute the mission signature */
      mission->crc = Crc16Bit((unsigned char *)mission, sizeof(*mission)-sizeof(mission->crc));

      /* write the mission configuration to the primary location */
      for (byte=(unsigned char *)mission, i=0; i<sizeof(*mission); i++,byte++)
      {
         EEWrite(*byte,MissionAddrPrimary+i);
      }
      
      /* write the mission configuration to the mirror location */
      for (byte=(unsigned char *)mission, i=0; i<sizeof(*mission); i++,byte++)
      {
         EEWrite(*byte,MissionAddrMirror+i);
      }

      status=1;
    }

   return status;
}

/*------------------------------------------------------------------------*/
/* function to read the profile id from EEPROM                            */
/*------------------------------------------------------------------------*/
/**
   This function reads and returns the profile id from EEPROM.
*/
int PrfIdGet(void)
{
   /* read the profile id from EEPROM */
   int pid = EEReadWord(PrfId);

   /* make sure the profile id is nonnegative */
   if (pid<0) pid=0;
      
   return pid;
}

/*------------------------------------------------------------------------*/
/* function to set the profile id                                         */
/*------------------------------------------------------------------------*/
/**
   This function sets the profile counter to a specified value and stores it
   in EEPROM.  A positive value is returned on success; zero is returned
   otherwise. 
*/
int PrfIdSet(unsigned int pid)
{
   int status=1;

   /* set the profile id */
   EEWriteWord(pid,PrfId);

   return status;
}

/*------------------------------------------------------------------------*/
/* function to increment the profile id                                   */
/*------------------------------------------------------------------------*/
/**
   This function increments the profile counter stored in EEPROM.  A
   positive value is returned on success; zero is returned otherwise.
*/
int PrfIdIncrement(void)
{
   int status=1;

   /* read the profile id from EEPROM */
   int pid = EEReadWord(PrfId);

   /* increment the profile counter */
   pid++;

   /* write the profile counter back to EEPROM */
   EEWriteWord(pid,PrfId);
   
   return status;
}

/*------------------------------------------------------------------------*/
/* function to read the mission state from EEPROM                         */
/*------------------------------------------------------------------------*/
/**
   This function reads and returns the mission state variable from EEPROM.
   The primary and mirror copy are read and validation criteria are
   applied.  If the validation criteria are satisfied then the mission state
   is returned.  If the validate criteria are violated then an UNDEFINED
   state is returned.
*/
enum State StateGet(void)
{
   /* define the logging signature */
   static cc FuncName[] = "StateGet()";

   /* initialize the state variable */
   enum State state = UNDEFINED;

   /* read the state variable from eeprom */
   enum State primary = EEReadWord(StateAddrPrimary);

   /* read the state mirror from eeprom */
   enum State mirror = EEReadWord(StateAddrMirror);

   /* check validation criteria for the state variable */
   if (primary>UNDEFINED && primary<EOS && primary==mirror) state=primary;

   /* log the failure */
   else
   {
      /* create the message */
      static cc msg[]="Attempt to validate state variable failed.\n";

      /* make the logentry */
      LogEntry(FuncName,msg);
   }
   
   return state;
}

/*------------------------------------------------------------------------*/
/* function to write the mission state to EEPROM                          */
/*------------------------------------------------------------------------*/
/**
   This function writes the mission state variable to EEPROM.  The state is
   then reread and compared as a validation step.  If successful, this
   function returns a positive value.  A return value of zero indicates that
   the read/verification failed.  A negative return value indicates that an
   invalid state was passed to this function.
*/
int StateSet(enum State state)
{
   /* define the logging signature */
   static cc FuncName[] = "StateSet()";

   int status=-1;

   /* validate the state variable */
   if (state>UNDEFINED && state<EOS) 
   {
      /* reinitialize the return value */
      status=1;

      /* write the state variable to the primary location */
      EEWriteWord(state,StateAddrPrimary);

      /* write the state variable to the mirror location */
      EEWriteWord(state,StateAddrMirror);

      /* read the state variable back from the primary location */
      if (EEReadWord(StateAddrPrimary)!=state ||
          EEReadWord(StateAddrMirror)!=state)
      {
         /* create the message */
         static cc msg[]="Verification of state variable failed.\n";

         /* make the logentry */
         LogEntry(FuncName,msg);

         /* indicate failure */
         status=0;
      }
   } 

   return status;
}

/*------------------------------------------------------------------------*/
/* function to read the flash bad-block list to EEPROM                   */
/*------------------------------------------------------------------------*/
/**
   This function reads the flash's bad-block list to EEPROM.  On success, a
   positive value is returned; otherwise zero is returned.
*/
int Tc58v64BadBlockListGet(void)
{
   /* initialize a pointer to the function name for log entries */
   static cc FuncName[]="Tc58v64BadBlockListGet()";

   /* initialize the return value */
   int i,status=1;

   /* initialize the start-address of the EEPROM block */
   unsigned int address=EepromBadBlockAddr;
   
   /* pet the watchdog */
   WatchDog();
   
   /* read the bad-block checksum EEPROM */
   BadBlockCrc=EEReadWord(EepromBadBlockChecksum);

   /* make a log-entry of the bad block list */
   if (debuglevel>=3 || BadBlock[i]!=0xffff)
   {
      /* create the format */
      static cc format[]="Reading bad-block check-sum[0x%04x] "
         "from EEPROM address 0x%04x\n";
      
      /* make a log-entry of the bad block list */
      LogEntry(FuncName,format,BadBlockCrc,EepromBadBlockChecksum);
   }

   /* loop over the bad-block list */
   for (i=0; i<Tc58v64BadBlockMax; i++,address+=sizeof(unsigned int))
   {
      /* read the current bad-block from EEPROM */
      BadBlock[i]=EEReadWord(address);

      /* make a log-entry of the bad block list */
      if (debuglevel>=3 || BadBlock[i]!=0xffff)
      {
         /* create the format */
         static cc format[]="Reading bad-block id[0x%04x] "
            "from EEPROM address 0x%04x\n";

         /* make a log-entry of the bad block list */
         LogEntry(FuncName,format,BadBlock[i],address);
      }
   }

   return status;
}

/*------------------------------------------------------------------------*/
/* function to write the flash bad-block list to EEPROM                   */
/*------------------------------------------------------------------------*/
/**
   This function writes the flash's bad-block list to EEPROM.  On success, a
   positive value is returned; otherwise zero is returned.
*/
int Tc58v64BadBlockListPut(void)
{
   /* initialize a pointer to the function name for log entries */
   static cc FuncName[]="Tc58v64BadBlockListPut()";

   /* initialize a format string for the bad-block check-sum */
   static cc format[]="Writing bad-block check-sum[0x%04x] to EEPROM address 0x%04x\n";

   /* initialize the return value */
   int i,status=1;

   /* initialize the start-address of the EEPROM block */
   unsigned int address=EepromBadBlockAddr;
   
   /* pet the watchdog */
   WatchDog();

   /* write the checksum into the last element of the bad-block list*/
   BadBlockCrc=Tc58v64BadBlockCrc();

   /* make a log-entry of the bad block list */
   LogEntry(FuncName,format,BadBlockCrc,EepromBadBlockChecksum);
         
   /* write the current bad-block to EEPROM */
   EEWriteWord(BadBlockCrc,EepromBadBlockChecksum);
 
   /* loop over the bad-block list */
   for (i=0; i<Tc58v64BadBlockMax; i++,address+=sizeof(unsigned int))
   {
      /* make a log-entry of the bad block list */
      if (BadBlock[i]!=0xffff || debuglevel>=3 || debugbits&EEPROM_H)
      {
         static cc format[]="Writing bad-block id[0x%04x] to EEPROM address 0x%04x\n";
         LogEntry(FuncName,format,BadBlock[i],address);
      }
      
      /* write the current bad-block to EEPROM */
      EEWriteWord(BadBlock[i],address);
   }

   return status;
}

/*------------------------------------------------------------------------*/
/* function to return the number of correctable errors recorded           */
/*------------------------------------------------------------------------*/
/**
   This function returns the number of correctable errors recorded in
   EEPROM since the last time that Tc58v64ErrorsInit() was executed.
*/
unsigned int Tc58v64ErrorsCorrectable(void)
{
   /* read the number of correctable errors from EEPROM */
   unsigned int err = EEReadWord(EepromCErrorIndx);

   return err;
}

/*------------------------------------------------------------------------*/
/* function to initialize the number of errors in EEPROM                  */
/*------------------------------------------------------------------------*/
/**
   This function initializes to zero the elements of EEPROM used to record
   the number of correctable and noncorrectable errors.  On success, this
   function returns a positive value, failure is indicated with a return
   value of zero, and a negative return value indicates an exceptional
   condition.
*/
int Tc58v64ErrorsInit(void)
{
   /* initialize the number of correctable errors in EEPROM */
   EEWriteWord(0, EepromCErrorIndx);
   
   /* initialize the number of noncorrectable errors in EEPROM */
   EEWriteWord(0, EepromErrorIndx);

   return 1;
}

/*------------------------------------------------------------------------*/
/* function to return the number of noncorrectable errors recorded        */
/*------------------------------------------------------------------------*/
/**
   This function returns the number of noncorrectable errors recorded in
   EEPROM since the last time that Tc58v64ErrorsInit() was executed.
*/
unsigned int Tc58v64ErrorsNoncorrectable(void)
{
   /* read the number of noncorrectable errors from EEPROM */
   unsigned int err = EEReadWord(EepromErrorIndx);

   return err;
}

/*------------------------------------------------------------------------*/
/*                                                                        */
/*------------------------------------------------------------------------*/
void Tc58v64IncrementErrorsCorrectable(void)
{
   /* read the number of correctable errors from EEPROM */
   unsigned int err = EEReadWord(EepromCErrorIndx);

   /* store updated number of correctable errors in EEPROM */
   EEWriteWord(++err, EepromCErrorIndx);
}

/*------------------------------------------------------------------------*/
/*                                                                        */
/*------------------------------------------------------------------------*/
void Tc58v64IncrementErrorsNoncorrectable(void)
{
   /* read the number of noncorrectable errors from EEPROM */
   unsigned int err = EEReadWord(EepromErrorIndx);

   /* store updated number of noncorrectable errors in EEPROM */
   EEWriteWord(++err, EepromErrorIndx);
}
