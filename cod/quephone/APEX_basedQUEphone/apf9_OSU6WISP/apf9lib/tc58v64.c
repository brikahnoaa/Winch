#ifndef TC58V64_H
#define TC58V64_H (0x1000U)

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * $Id: tc58v64.c,v 1.2 2007/04/24 01:43:05 swift Exp $
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
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
 * $Log: tc58v64.c,v $
 * Revision 1.2  2007/04/24 01:43:05  swift
 * Added acknowledgement and funding attribution.
 *
 * Revision 1.1  2006/10/11 23:40:10  swift
 * Low-level API to the TC58V64 flash memory chip.
 *
 * \end{verbatim}
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
#define tc58v64ChangeLog "$RCSfile: tc58v64.c,v $ $Revision: 1.2 $ $Date: 2007/04/24 01:43:05 $"

/* function prototypes */
unsigned int Tc58v64BadBlockCrc(void);
int          Tc58v64BadBlockListInit(void);
int          Tc58v64Busy(void);
int          Tc58v64Erase(unsigned int block);
void         Tc58v64GenEccTbl(void);
unsigned int Tc58v64Id(void);
int          Tc58v64Init(void);
int          Tc58v64IsBadBlock(unsigned int block);
int          Tc58v64PageGet(unsigned int block, unsigned char page, far unsigned char *buf);
int          Tc58v64PageGetNoEcc(unsigned int block, unsigned char page, far unsigned char *buf);
int          Tc58v64PagePut(unsigned int block, unsigned char page, far const unsigned char *buf);
void         Tc58v64Reset(void);
int          Tc58v64WriteConstant(unsigned int block,unsigned char page,unsigned char byte);

/* define the maximum number of bad blocks allowed */
#define Tc58v64BadBlockMax (20)

/* define a static global array to hold the bad-block ids */
extern persistent far unsigned int BadBlock[Tc58v64BadBlockMax];
extern persistent far unsigned int BadBlockCrc;

#endif /* TC58V64_H */

#include <apf9.h>
#include <logger.h>
#include <psd835.h>

#define READ_NORMAL            0x00
#define PROGRAM_PAGE           0x10
#define READ_EXTENDED          0x50
#define ERASE_SETUP            0x60
#define STATUS_READ            0x70
#define SERIAL_DATA_IN         0x80
#define ID_READ                0x90
#define ERASE_START            0xd0
#define RESET                  0xff

#define BYTES_PER_PHYSICAL_PAGE 528
#define BYTES_PER_LOGICAL_PAGE  512
#define PAGES_PER_BLOCK          16
#define BLOCKMAX               1024
#define BADBLOCKEND          0xffff

/* map the Tc58v64 port */
static unsigned char Tc58v64Port @ 0xed0;

/* define a static global array to hold the bad-block ids */
persistent far unsigned int BadBlock[Tc58v64BadBlockMax];
persistent far unsigned int BadBlockCrc;

/* define a static page buffer */
persistent static far unsigned char buf[BYTES_PER_PHYSICAL_PAGE];

/* define a static ECC table */
persistent static far unsigned char EccTbl[256];

/* prototypes for functions with external linkage */
int          Tc58v64BadBlockListPut(void);
int          Tc58v64BadBlockListGet(void);
unsigned int Tc58v64ErrorsCorrectable(void);
int          Tc58v64ErrorsInit(void);
unsigned int Tc58v64ErrorsNoncorrectable(void);
void         Tc58v64IncrementErrorsCorrectable(void);
void         Tc58v64IncrementErrorsNoncorrectable(void);

/* prototypes for functions with static linkage */
static void          CalculateECC(unsigned char *data,unsigned char *ecc1,
                                  unsigned char *ecc2,unsigned char *ecc3);  
static unsigned char CorrectData(unsigned char *data, unsigned char Secc1,
                                 unsigned char Secc2, unsigned char Secc3,
                                 unsigned char ecc1, unsigned char ecc2,
                                 unsigned char ecc3);
static int           DoEcc(unsigned char *page);
static void          Ecc(unsigned char *page);
static void          TransResult(unsigned char reg2, unsigned char reg3,
                                 unsigned char *ecc1, unsigned char *ecc2);

/*------------------------------------------------------------------------*/
/*                                                                        */
/*------------------------------------------------------------------------*/
int Tc58v64IsBadBlock(unsigned int block)
{
   int i,status=0;
   
   for (i=0; i<Tc58v64BadBlockMax; i++)
   {
      if (BadBlock[i]==BADBLOCKEND) break;

      if (BadBlock[i]==block) {status=1; break;}
   }

   return status;
}

/*------------------------------------------------------------------------*/
/*                                                                        */
/*------------------------------------------------------------------------*/
unsigned int Tc58v64BadBlockCrc(void)
{
   unsigned int i,Crc;

   /* compute the checksum of the list of bad blocks */
   for (Crc=0x5555, i=0; i<Tc58v64BadBlockMax; i++) Crc ^= BadBlock[i];

   return Crc;
}

/*------------------------------------------------------------------------*/
/*                                                                        */
/*------------------------------------------------------------------------*/
int Tc58v64BadBlockListInit(void)
{
   int i,status=0;

   /* verify the bad-block crc */
   if (BadBlockCrc==Tc58v64BadBlockCrc()) status=1;

   /* get the bad-block list from eeprom */
   else if (Tc58v64BadBlockListGet()>0)
   {
      /* check if eeprom contains a valid bad-block list */
      if (BadBlockCrc==Tc58v64BadBlockCrc()) status=1;
      
      else 
      {
         /* initialize the bad-block list */
         for (i=0; i<Tc58v64BadBlockMax; i++) BadBlock[i]=BADBLOCKEND;

         /* write the bad-block list to eeprom */
         Tc58v64BadBlockListPut();
      }
   }

   else 
   {
      /* initialize the bad-block list */
      for (i=0; i<Tc58v64BadBlockMax; i++) BadBlock[i]=BADBLOCKEND;

      /* write the bad-block list to eeprom */
      BadBlockCrc=Tc58v64BadBlockCrc();
   }
   
   return status;
}

/*------------------------------------------------------------------------*/
/* function to test the R/B pin of the TC58V64 flash                      */
/*------------------------------------------------------------------------*/
/**
   This function tests the R/B pin of the TC58V64 to determine if a flash
   operation is in progress.  This function returns a positive value if the
   flash chip is busy; otherwise zero is returned.
*/
int Tc58v64Busy(void)
{
   return ((PORTBRD&FLASH_RB) ? 0 : 1);
}

/*------------------------------------------------------------------------*/
/* function to erase a block of the TC58V64 flash                         */
/*------------------------------------------------------------------------*/
/**
   This function erases an 8KB block of the TC58V64 flash array.  While
   flash can be written one page at a time, erasures must be done in units
   of 8KB blocks.

      \begin{verbatim}
      input:
         block....The block id to erase is specified as an integer in the
                  semi-open range [0-1024).

      output:
         This function returns a positive value on success or zero, on
         failure.  A negative value is returned if an invalid block id is
         specified. 
      \end{verbatim}
*/
int Tc58v64Erase(unsigned int block)
{
   int i,status=-1;

   /* initialize a pointer to the function name for log entries */
   static cc FuncName[]="Tc58v64Erase()";
   
   /* pet the watchdog */
   WatchDog();

   /* validate the block and page counters */
   if (block>=1024)
   {
      /* create the format */
      static cc format[]="Invalid block: %u\n";
      LogEntry(FuncName,format,block);
   }
   
   /* make sure the flash is not busy */
   else if (Tc58v64Busy())
   {
      static cc msg[]="Busy.\n"; LogEntry(FuncName,msg);
      status=0;
   }

   else
   {
      /* compute the address from the block and the page */
      unsigned int address = ((unsigned int)block << 4);

      /* initialize a pointer to the address */
      const unsigned char *q = (unsigned char *)&address;

      /* set command-input mode */
      Psd835PortCSet(FLASH_CLE);
      Psd835PortCClear(FLASH_ALE);
      Psd835PortCClear(FLASH_CE);

      /* send the erase-setup command */
      Tc58v64Port = ERASE_SETUP;

      /* set address-input mode */
      Psd835PortCClear(FLASH_CLE); Psd835PortCSet(FLASH_ALE);

      /* write the address to the TC58V64 flash */
      Tc58v64Port=q[0]; Tc58v64Port=q[1];
      
      /* set command-input mode */
      Psd835PortCSet(FLASH_CLE); Psd835PortCClear(FLASH_ALE);
      
      /* send the erase-start command */
      Tc58v64Port=ERASE_START;

      /* set data-output mode */
      Psd835PortCClear(FLASH_CLE);

      /* wait 50ms or until TC58V64 is not busy */
      for (i=0; Tc58v64Busy() && i<50; i++) {Wait(1);} WaitTicks(10);

      /* set command mode */
      Psd835PortCSet(FLASH_CLE);

      /* send command to read the operation status */
      Tc58v64Port = STATUS_READ;

      /* set data-output mode */
      Psd835PortCClear(FLASH_CLE);

      /* read the status from the flash IO port */
      status=Tc58v64Port;

      /* set standby mode */
      Psd835PortCSet(FLASH_CE);

      /* check the status of the erase operation */
      if (status!=0xc0)
      {
         /* create the format for the logentry */
         static cc format[]="Operation failed: block[%d]  status[0x%02x]\n";
         LogEntry(FuncName,format,block,status);

         /* indicate failure */
         status = 0;
      }

      /* indicate success */
      else status=1;
   }
   
   return(status);
}
   
/*------------------------------------------------------------------------*/
/* function to read id information from the TC58V64 flash                 */
/*------------------------------------------------------------------------*/
/**
   This function reads the 'maker' and 'device' identifiers from the TC58V64
   flash.  The maker and device identifiers are stored in the MSB,LSB
   (respectively) of the return value.
*/
unsigned int Tc58v64Id(void)
{
   unsigned int id;

   /* pet the watchdog */
   WatchDog();

   /* set command-input mode */
   Psd835PortCSet(FLASH_CLE);
   Psd835PortCClear(FLASH_ALE);
   Psd835PortCClear(FLASH_CE);

   /* give the read command */
   Tc58v64Port=ID_READ;

   /* set address-input mode */
   Psd835PortCClear(FLASH_CLE); Psd835PortCSet(FLASH_ALE);

   /* input zero as the address (see TC58V64 data sheet) */
   Tc58v64Port=0;

   /* set data-output mode */
   Psd835PortCClear(FLASH_ALE);

   /* store the maker in the MSB and the device in the LSB */
   id=Tc58v64Port; id<<=8; id|=Tc58v64Port;

   /* set standby mode */
   Psd835PortCSet(FLASH_CE);

   /* reset the flash */
   Tc58v64Reset();

   return id;
}

/*------------------------------------------------------------------------*/
/* function to initialize the TC58V64 flash                               */
/*------------------------------------------------------------------------*/
/**
   This function initializes the TC58V64 flash by scanning the flash array
   for bad blocks.  The list of bad blocks are stored in EEPROM.  On
   success, this function returns a positive value; otherwise zero is
   returned. 
*/
int Tc58v64Init(void)
{
   /* initialize a pointer to the function name for log entries */
   static cc FuncName[]="Tc58v64Init()", format[]="%s";

   /* define loop variables and function's return value */
   int BadBlockCount,i,block,page,status=1;

   /* pet the watchdog */
   WatchDog();
   
   /* initiate a log entry */
   LogEntry(FuncName,format,"");

   /* initialize the list of bad blocks */
   for (BadBlockCount=0,i=0; i<Tc58v64BadBlockMax; i++) BadBlock[i]=BADBLOCKEND;

   /* execute a pattern test for each block */
   for (block=0; block<BLOCKMAX; block++)
   {
      /* initialize the error to a sentinal value */
      unsigned int error=0xffff;

      /* define the test pattern */
      unsigned char pattern=0x55;
      
      /* erase the block */
      if (Tc58v64Erase(block)>0)
      {
         /* write/read the test pattern for each page */
         for (error=0,page=0; page<PAGES_PER_BLOCK; page++)
         {
            /* write/read the test pattern for the current page */
            if (Tc58v64WriteConstant(block,page,pattern)>0 &&
                Tc58v64PageGetNoEcc(block,page,buf)>0)
            {
               /* verify the test pattern */
               for (i=0; i<BYTES_PER_LOGICAL_PAGE; i++)
               {
                  if (buf[i]!=pattern) {++error;}
               }
            }

            /* set the return value to a sentinal value */
            else error=0x7fff;
         }
      }
      
      /* erase the block */
      if (error!=0xffff && Tc58v64Erase(block)>0)
      {
         /* negate the test pattern; write/read the test pattern for each page */
         for (pattern=~pattern,page=0; page<PAGES_PER_BLOCK; page++)
         {
            /* write/read the test pattern for the current page */
            if (Tc58v64WriteConstant(block,page,pattern)>0 &&
                Tc58v64PageGetNoEcc(block,page,buf)>0)
            {
               /* verify the test pattern */
               for (i=0; i<BYTES_PER_LOGICAL_PAGE; i++)
               {
                  if (buf[i]!=pattern) {++error;}
               }
            }

            /* set the return value to a sentinal value */
            else error=0x7fff;
         }
      }

      /* log the block id */
      if (!error) LogAdd("%u ",block);
      else
      {
         /* log the block id and the number of errors */
         LogAdd("%u[%d errors] ",block,error);

         /* add the block id to the list of bad blocks */
         if (BadBlockCount<Tc58v64BadBlockMax-1) BadBlock[BadBlockCount++]=block;
         else
         {
            LogAdd("Too many bad blocks[%d] - aborted.",++BadBlockCount);
            status=0; break;
         }
      }
   }

   /* terminate the log entry */
   LogAdd("\n");

   /* write the list of bad blocks to EEPROM */
   Tc58v64BadBlockListPut();

   /* initialize the error counters */
   Tc58v64ErrorsInit();

   return status;
}

/*------------------------------------------------------------------------*/
/* function to read a page from TC58V64 flash                             */
/*------------------------------------------------------------------------*/
/**
   This function reads a page from TC58V64 flash and applies an
   error-correction code to validate accuracy.  This function stores a page
   in the TC58V64 flash.

      \begin{verbatim}
      input:
         block.....The flash array is segmented into 8KB blocks.  This
                   argument specifies the block for the read operation. This
                   argument must be in the semi-open range [0, 1024).
         page......Each 8KB block of the flash array is segmented into
                   sixteen 512-byte pages.  This argument specifies the page
                   within the block for the read operation.  This argument
                   must be in the semi-open range [0, 16).
         buf.......The buffer into which the page will be written.
                   
      output:
         On success, this function returns a positive value; Zero indicates
         failure.  A negative value is returned if the function arguments
         are invalid.  The specified block must be in the semi-open range
         [0, 1024) and the page must be in the semi-open range [0, 16).
      \end{verbatim}
*/
int Tc58v64PageGet(unsigned int block, unsigned char page, far unsigned char *buf)
{
   /* initialize a pointer to the function name for log entries */
   static cc FuncName[]="Tc58v64PageGet()";

   int i,status=-1;

   /* pet the watchdog */
   WatchDog();

   /* validate the function arguments */
   if (!buf)
   {
      static cc msg[]="NULL function parameter.\n";
      LogEntry(FuncName,msg);
   }
   
   /* validate the block and page counters */
   else if (block>=1024 || page>=16)
   {
      /* create the format for the logentry */
      static cc format[]="Invalid block/page: %u/%u\n";
      LogEntry(FuncName,format,block,page);
         
      /* fill the buffer with NULL characters */      
      if (buf) {for(i=0; i<BYTES_PER_LOGICAL_PAGE; i++) {buf[i]=0x00;}}
   }

   /* make sure the flash is not busy */
   else if (Tc58v64Busy())
   {
      static cc msg[]="Busy.\n"; LogEntry(FuncName,msg);
      status=0;
   }
   
   else
   {
      /* compute the page address */
      unsigned long address = ((long)block << 12) | ((long)page << 8);
   
      /* initialize a pointer to the address */
      unsigned char *q = (unsigned char *)&address;
         
      /* define a page buffer */
      unsigned char pbuf[BYTES_PER_PHYSICAL_PAGE];

      /* set command-input mode */
      Psd835PortCSet(FLASH_CLE);
      Psd835PortCClear(FLASH_ALE);
      Psd835PortCClear(FLASH_CE);
   
      /* send the command for serial input */
      Tc58v64Port = READ_NORMAL;

      /* set address-input mode */
      Psd835PortCClear(FLASH_CLE); Psd835PortCSet(FLASH_ALE);
   
      /* write the address to the TC58V64 flash */
      Tc58v64Port=q[0]; Tc58v64Port=q[1]; Tc58v64Port=q[2];
   
      /* set data input mode */
      Psd835PortCClear(FLASH_ALE); WaitTicks(10);

      /* read the data from flash */
      for (i=0; i<BYTES_PER_PHYSICAL_PAGE; i++) pbuf[i] = Tc58v64Port; WaitTicks(10);

      /* set standby mode */
      Psd835PortCSet(FLASH_CE);

      /* execute error-correcting algorithm to buffer */
      status=DoEcc(pbuf);
      
      /* copy the page to the local buffer and compute ECC data */
      for(i=0; i<BYTES_PER_LOGICAL_PAGE; i++) buf[i] = pbuf[i];
   }
   
   return status;
}

/*------------------------------------------------------------------------*/
/* function to read a page of TC58V64 flash (no ECC)                      */
/*------------------------------------------------------------------------*/
/**
   This function reads one page from TC58V64 flash into a buffer without
   error correction.  
   
      \begin{verbatim}
      input:
         block.....The flash array is segmented into 8KB blocks.  This
                   argument specifies the block for the read operation. This
                   argument must be in the semi-open range [0, 1024).
         page......Each 8KB block of the flash array is segmented into
                   sixteen 512-byte pages.  This argument specifies the page
                   within the block for the read operation.  This argument
                   must be in the semi-open range [0, 16).
                   
      output:
         buf...The buffer into which the page will be written.

      On success, this function returns a positive value; Zero indicates
      failure.  A negative value is returned if the function arguments are
      invalid.  The specified block must be in the semi-open range [0, 1024)
      and the page must be in the semi-open range [0, 16).
      \end{verbatim}
*/
int Tc58v64PageGetNoEcc(unsigned int block, unsigned char page, far unsigned char *buf)
{
   /* initialize the function's return value */
   int i, status=-1;

   /* initialize a pointer to the function name for log entries */
   static cc FuncName[]="Tc58v64PageGetNoEcc()";

   /* pet the watchdog */
   WatchDog();

   /* validate the function arguments */
   if (!buf)
   {
      static cc msg[]="NULL function parameter.\n";
      LogEntry(FuncName,msg);
   }
   
   /* validate the block and page counters */
   else if (block>=1024 || page>=16)
   {
      /* create the format for the log entry */
      static cc format[]="Invalid block/page: %u/%u\n";
      
      /* log the error */
      LogEntry(FuncName,format,block,page);
         
      /* fill the buffer with NULL characters */      
      if (buf) {for(i=0; i<BYTES_PER_LOGICAL_PAGE; i++) {buf[i]=0x00;}}
   }
   
   /* make sure the flash is not busy */
   else if (Tc58v64Busy())
   {
      static cc msg[]="Busy.\n"; LogEntry(FuncName,msg);
      status=0; 
   }

   else
   {
      /* compute the address from the block and the page */
      unsigned long address = ((unsigned long)block << 12) | ((unsigned long)page << 8);

      /* initialize a pointer to the address */
      const unsigned char *q = (unsigned char *)&address;
      
      /* set command-input mode */
      Psd835PortCSet(FLASH_CLE);
      Psd835PortCClear(FLASH_ALE);
      Psd835PortCClear(FLASH_CE);

      /* give command for Read Mode (1) */
      Tc58v64Port = READ_NORMAL;

      /* set address-input mode */
      Psd835PortCClear(FLASH_CLE); Psd835PortCSet(FLASH_ALE);

      /* write the address to the TC58V64 flash */
      Tc58v64Port=q[0]; Tc58v64Port=q[1]; Tc58v64Port=q[2];

      /* set data-input mode */
      Psd835PortCClear(FLASH_ALE); WaitTicks(10);

      /* read the page into the buffer */
      for(i=0; i<BYTES_PER_LOGICAL_PAGE; i++) {buf[i]=Tc58v64Port;}

      /* set standby mode */ 
      WaitTicks(10); Psd835PortCSet(FLASH_CE);

      /* initialize the function's return value */
      status=1;
   }
   
   return status;
}

/*------------------------------------------------------------------------*/
/* function to store a page in flash                                      */
/*------------------------------------------------------------------------*/
/**
   This function stores a page in the TC58V64 flash array.  Six bytes of
   error-correcting code are computed and stored with each page.

      \begin{verbatim}
      input:
         block.....The flash array is segmented into 8KB blocks.  This
                   argument specifies the block for the read operation. This
                   argument must be in the semi-open range [0, 1024).
         page......Each 8KB block of the flash array is segmented into
                   sixteen 512-byte pages.  This argument specifies the page
                   within the block for the read operation.  This argument
                   must be in the semi-open range [0, 16).
         buf.......The buffer into which the page will be written.
                   
      output:
         On success, this function returns a positive value; Zero indicates
         failure.  A negative value is returned if the function arguments
         are invalid.  The specified block must be in the semi-open range
         [0, 1024) and the page must be in the semi-open range [0, 16).
      \end{verbatim}
*/
int Tc58v64PagePut(unsigned int block, unsigned char page, far const unsigned char *buf)
{
   /* initialize a pointer to the function name for log entries */
   static cc FuncName[]="Tc58v64PagePut()";

   /* initialize the return value */
   unsigned int i,status=-1;

   /* pet the watchdog */
   WatchDog();

   /* validate the function arguments */
   if (!buf)
   {
      static cc msg[]="NULL function parameter.\n";
      LogEntry(FuncName,msg);
   }
   
   /* validate the block and page counters */
   else if (block>=1024 || page>=16)
   {
      static cc format[]="Invalid block/page: %u/%u\n";
      LogEntry(FuncName,format,block,page);
   }
   
   /* make sure the flash is not busy */
   else if (Tc58v64Busy()) 
   {
      static cc msg[]="Busy.\n"; LogEntry(FuncName,msg);
      status=0;
   }

   else
   {
      /* compute the page address */
      unsigned long int address = ((long)block << 12) | ((long)page << 8);

      /* initialize a pointer to the address */
      unsigned char *q = (unsigned char *)&address;
   
      /* define a page buffer */
      unsigned char pbuf[BYTES_PER_PHYSICAL_PAGE];

      /* copy the page to the local buffer and compute ECC data */
      for(i=0; i<BYTES_PER_LOGICAL_PAGE; i++) pbuf[i] = buf[i]; Ecc(pbuf);

      /* set command-input mode */
      Psd835PortCSet(FLASH_CLE);
      Psd835PortCClear(FLASH_ALE);
      Psd835PortCClear(FLASH_CE);

      /* send the command for serial input */
      Tc58v64Port = SERIAL_DATA_IN;

      /* set address-input mode */
      Psd835PortCClear(FLASH_CLE); Psd835PortCSet(FLASH_ALE);

      /* write the address to the TC58V64 flash */
      Tc58v64Port=q[0]; Tc58v64Port=q[1]; Tc58v64Port=q[2];

      /* set data input mode */
      Psd835PortCClear(FLASH_ALE);

      /* load the data in the flash's buffer register */
      for(i=0; i<BYTES_PER_PHYSICAL_PAGE; i++) Tc58v64Port = pbuf[i];

      /* set command-input mode */
      Psd835PortCSet(FLASH_CLE); Psd835PortCClear(FLASH_ALE);
   
      /* send the command to program the page */
      Tc58v64Port = PROGRAM_PAGE; Wait(2);

      /* set the command to read the status */
      Tc58v64Port = STATUS_READ;

      /* set data output mode */
      Psd835PortCClear(FLASH_CLE);

      /* read the status from the IO port */
      status = Tc58v64Port;

      /* set standby mode */
      Psd835PortCSet(FLASH_CE);
   
      /* check the status of the write operation */
      if (status!=0xc0)
      {
         /* create the format for the logentry */
         static cc format[]="Flash operation failed: block[%d] status[0x%02x]\n";
         LogEntry(FuncName,format,block,status);
         
         /* indicate failure */
         status = 0;
      }

      /* indicate success */
      else status=1;
   }

   return status;
}

/*------------------------------------------------------------------------*/
/* function to reset the TC58V64 flash                                    */
/*------------------------------------------------------------------------*/
/**
   This function resets the TC58V64 flash.
*/
void Tc58v64Reset(void)
{
   /* pet the watchdog */
   WatchDog();

   /* set command-input mode */
   Psd835PortCSet(FLASH_CLE);
   Psd835PortCClear(FLASH_ALE);
   Psd835PortCClear(FLASH_CE);

   /* write the reset command */
   Tc58v64Port = RESET;

   /* set standby mode */
   Psd835PortCSet(FLASH_CE);

   /* give the flash time to reset */
   Wait(1);
} 
   
/*------------------------------------------------------------------------*/
/* function to write a constant to a page of the TC58V64 flash            */
/*------------------------------------------------------------------------*/
/**
   This function writes a constant test pattern to a page of the TC58V64
   flash array.
   
      \begin{verbatim}
      input:
         block.....The flash array is segmented into 8KB blocks.  This
                   argument specifies the block for the test pattern.  This
                   argument must be in the semi-open range [0, 1024).
         page......Each 8KB block of the flash array is segmented into
                   sixteen 512-byte pages.  This argument specifies the page
                   within the block for the test pattern.  This argument
                   must be in the semi-open range [0, 16).
         byte......This is the 8-bit test pattern that will be written to
                   each element of the page.
                   
      output:
         On success, this function returns a positive value; Zero indicates
         failure.  A negative value is returned if the function arguments
         are invalid.  The specified block must be in the semi-open range
         [0, 1024) and the page must be in the semi-open range [0, 16).
      \end{verbatim}
*/
int Tc58v64WriteConstant(unsigned int block,unsigned char page,unsigned char byte)
{
   int i, status=-1;

   /* initialize a pointer to the function name for log entries */
   static cc FuncName[]="Tc58v64WriteConstant()";

   /* pet the watchdog */
   WatchDog();
   
   /* validate the function arguments */
   if (!buf)
   {
      static cc msg[]="NULL function parameter.\n";
      LogEntry(FuncName,msg);
   }
   
   /* validate the block and page counters */
   else if (block>=1024 || page>=16)
   {
      static cc format[]="Invalid block/page: %u/%u\n";
      LogEntry(FuncName,format,block,page);
   }
   
   /* make sure the flash is not busy */
   else if (Tc58v64Busy())
   {
      static cc msg[]="Busy.\n"; LogEntry(FuncName,msg);
      status=0;
   }

   else
   {
      /* compute the address from the block and the page */
      unsigned long address = ((unsigned long)block << 12) | ((unsigned long)page << 8);

      /* initialize a pointer to the address */
      const unsigned char *q = (unsigned char *)&address;
      
      /* set command-input mode */
      Psd835PortCSet(FLASH_CLE);
      Psd835PortCClear(FLASH_ALE);
      Psd835PortCClear(FLASH_CE);

      /* write the command */
      Tc58v64Port=SERIAL_DATA_IN;

      /* set address-input mode */
      Psd835PortCClear(FLASH_CLE); Psd835PortCSet(FLASH_ALE);

      /* write the address to the TC58V64 flash */
      Tc58v64Port=q[0]; Tc58v64Port=q[1]; Tc58v64Port=q[2];

      /* set data-input mode */
      Psd835PortCClear(FLASH_ALE);

      /* write the test pattern TC58V64's internal buffer */
      for (i=0; i<BYTES_PER_PHYSICAL_PAGE; i++) Tc58v64Port=byte;

      /* set command-input mode */
      Psd835PortCSet(FLASH_CLE); Psd835PortCClear(FLASH_ALE);

      /* transfer the data to the page */
      Tc58v64Port=PROGRAM_PAGE; Wait(10);

      /* request the status of the program operation */
      Tc58v64Port=STATUS_READ;

      /* set data-output mode */
      Psd835PortCClear(FLASH_CLE);

      /* read the status from the TC58V64 ioport */
      status = Tc58v64Port;

      /* set standby mode */
      Psd835PortCSet(FLASH_CE);

      /* set the function's return value */
      status = (status==0xc0) ? 1 : 0;
   }
   
   return status;
}

#define BIT7	0x80
#define BIT6	0x40
#define BIT5	0x20
#define BIT4	0x10
#define BIT3	0x08
#define BIT2	0x04
#define BIT1	0x02
#define BIT0	0x01
#define BIT1BIT0 0x03
#define CORRECTABLE 0x00555554L
#define BIT23	0x00800000L

/*------------------------------------------------------------------------*/
/* function to generate the ECC table                                     */
/*------------------------------------------------------------------------*/
/**
   This function generates the ECC table and was written by John Backes at
   SeaBird, Inc.
*/
void Tc58v64GenEccTbl(void)
{									
   unsigned int i,j;
   unsigned int mask;
   unsigned char dall;
   unsigned char cp0,cp1,cp2,cp3,cp4,cp5;

	for (i=0;i<256;++i) EccTbl[i] = 0;

	for (i=0;i<256;++i) 
   {
		mask = BIT0;

		dall = cp0 = cp1 = cp2 = cp3 = cp4 = cp5 = 0;

		for (j=0;j<7;++j)
      {
			if ((unsigned char)i & BIT0) { ++dall; ++cp0; ++cp2; ++cp4;}
			if ((unsigned char)i & BIT1) { ++dall; ++cp1; ++cp2; ++cp4;}
			if ((unsigned char)i & BIT2) { ++dall; ++cp0; ++cp3; ++cp4;}
			if ((unsigned char)i & BIT3) { ++dall; ++cp1; ++cp3; ++cp4;}
			if ((unsigned char)i & BIT4) { ++dall; ++cp0; ++cp2; ++cp5;}
			if ((unsigned char)i & BIT5) { ++dall; ++cp1; ++cp2; ++cp5;}
			if ((unsigned char)i & BIT6) { ++dall; ++cp0; ++cp3; ++cp5;}
			if ((unsigned char)i & BIT7) { ++dall; ++cp1; ++cp3; ++cp5;}
      }
		if (dall & BIT0) EccTbl[i] |= BIT6;
		if (cp5 & BIT0) EccTbl[i] |= BIT5;
		if (cp4 & BIT0) EccTbl[i] |= BIT4;
		if (cp3 & BIT0) EccTbl[i] |= BIT3;
		if (cp2 & BIT0) EccTbl[i] |= BIT2;
		if (cp1 & BIT0) EccTbl[i] |= BIT1;
		if (cp0 & BIT0) EccTbl[i] |= BIT0;
	}
}

/*------------------------------------------------------------------------*/
/* function to compute 3 bytes of error correcting code                   */
/*------------------------------------------------------------------------*/
/**
   This function computes three bytes of error correcting code for 256 bytes
   of data.  It was written by John Backes at SeaBird and is used here in
   black-box fashion.
*/
static void CalculateECC(unsigned char *data,unsigned char *ecc1,
                         unsigned char *ecc2,unsigned char *ecc3)  
{
   unsigned int i;
   unsigned char a;
   unsigned char reg1,reg2,reg3;

   reg1 = reg2 = reg3 = 0;

   for (i=0;i<256;++i)
	{
      a = EccTbl[data[i]];
      reg1 ^= (a & 0x3f);
      if ((a & BIT6) != 0)
		{
         reg3 ^= (unsigned char)i;
         reg2 ^=~ ((unsigned char)i);
		}
	}
   TransResult(reg2,reg3,ecc1,ecc2);
   *ecc1 =~ (*ecc1);
   *ecc2 =~ (*ecc2);
   *ecc3 = ((~reg1) << 2) | BIT1BIT0;
}

/*------------------------------------------------------------------------*/
/* quintessential black box function                                      */
/*------------------------------------------------------------------------*/
/**
   This function participates in the ECC computation.  It was written by John
   Backes at SeaBird and is used here in black-box fashion.
*/
static void TransResult(unsigned char reg2, unsigned char reg3,
                        unsigned char *ecc1, unsigned char *ecc2)
{
   unsigned char a,b,i;

   a = BIT7;
   b = BIT7;
   *ecc1 = *ecc2 = 0;
   for (i=0;i<4;++i)
	{
      if ((reg3 & a) != 0) *ecc1 |= b;
      b = b >> 1;
      if ((reg2 & a) !=0) *ecc1 |= b;
      b = b >> 1;
      a = a >> 1;
	}
   b = BIT7;
   for (i=0;i<4;++i)
	{
      if ((reg3 & a) != 0) *ecc2 |= b;
      b = b >> 1;
      if ((reg2 & a) !=0) *ecc2 |= b;
      b = b >> 1;
      a = a >> 1;
	}
}

/*------------------------------------------------------------------------*/
/* function to correct errors in a buffer                                 */
/*------------------------------------------------------------------------*/
/**
   This function corrects errors in a page.  It was written by John Backes
   at SeaBird and is used here in black-box fashion.
*/
static unsigned char CorrectData(unsigned char *data,
                                 unsigned char Secc1,unsigned char Secc2,unsigned char Secc3,
                                 unsigned char ecc1,unsigned char ecc2,unsigned char ecc3)
{
   unsigned long l,d;
   unsigned int i;
   unsigned char d1,d2,d3,a,add,b,abit;

   d1 = ecc1^Secc1;
   d2 = ecc2^Secc2;
   d3 = ecc3^Secc3;

   d = ((unsigned long)d1<<16) + ((unsigned long)d2<<8) + (unsigned long)d3;

   /* check for no-error condition */
   if (d == 0) return(0);								

   /* check for a correctable error */
   if (((d ^ (d >> 1)) & CORRECTABLE) == CORRECTABLE)
	{ 
      l = BIT23;
      add = 0;
      a = BIT7;
      for (i=0;i<8;++i)
		{
         if ((d & l) !=0) add |= a;
         l>>=2;
         a>>=1;
		}
      abit = 0;
      b = BIT2;
      for (i=0;i<3; ++i)
		{
         if((d & l) != 0) abit |= b;
         l>>=2;
         b>>=1;
		}
      b = BIT0;	
      data[add]^=(b << abit); // correct the data
      return(1);
	}
   i = 0;
   d &= 0x00ffffffL;
   while (d)
	{
      if (d & BIT0) ++i;
      d>>=1;
	}
   if (i == 1)
	{
      Secc1 = ecc1;
      Secc2 = ecc2;
      Secc3 = ecc3;
      return(2);
	}

   /* indicate uncorrectable error */
   return(3);			
}

/*------------------------------------------------------------------------*/
/* function to compute 6 bytes of error correction code                   */
/*------------------------------------------------------------------------*/
/**
   This function computes three bytes of error correction code for each of
   two 256-byte half-pages.  
*/
static void Ecc(unsigned char *page)
{
   int i;
   
   /* compute 3 bytes of error correction code for the first half-page */
   CalculateECC(page,&page[512],&page[513],&page[514]);
   
   /* compute 3 bytes of error correction code for the second half-page */
   CalculateECC(&page[256],&page[515],&page[516],&page[517]);

   /* initialize the remainder of the page */
   for (i=518; i<BYTES_PER_PHYSICAL_PAGE; i++) page[i]=0;
}

/*------------------------------------------------------------------------*/
/* function to execute the error correction algorithm for a page of data  */
/*------------------------------------------------------------------------*/
/**
   This function executes the error correction algorithm for a page of data.
   A cumulative record of the number of correctable and uncorrectable errors
   is maintained in EEPROM.  This function returns a positive value if the
   page was successfully validated or corrected and zero is returned if
   uncorrectable errors were detected.  A negative return value indicates a
   NULL function argument.
*/
static int DoEcc(unsigned char *page)
{
   int status=-1;

   /* validate the function parameter */
   if (page)
   {
      int error0,error1;
      unsigned char ecc1, ecc2, ecc3;

      /* reinitialize the return value */
      status=1;
   
      /* compute 3 error correction bytes for the first half-page */
      CalculateECC(page, &ecc1, &ecc2, &ecc3);

      /* apply the error correction algorithm to the first half-page */
      error0 = CorrectData(page,page[512],page[513],page[514],ecc1,ecc2,ecc3);
      
      /* compute 3 error correction bytes for the second half-page */
      CalculateECC(&page[256],&ecc1,&ecc2,&ecc3);

      /* apply the error correction algorithm to the second half-page */
      error1 = CorrectData(&page[256],page[515],page[516],page[517],ecc1,ecc2,ecc3);

      /* check for noncorrectable errors */
      if ((error0 == 3) || (error1 == 3))
      {
         /* increment the number of noncorrectable errors */
         Tc58v64IncrementErrorsNoncorrectable();

         /* indicate failure */
         status=0;
      }

      /* check for correctable errors */
      else if (error0==1 || error1==1 || error0==2 || error1==2)
      {
         /* increment the number of correctable errors */
         Tc58v64IncrementErrorsCorrectable();
      }
   }

   return status;
}
