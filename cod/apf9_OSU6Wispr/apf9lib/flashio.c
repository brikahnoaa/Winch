#ifndef FLASHIO_H
#define FLASHIO_H (0x1000U)

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * $Id: flashio.c,v 1.2 2007/04/24 01:43:05 swift Exp $
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
 * $Log: flashio.c,v $
 * Revision 1.2  2007/04/24 01:43:05  swift
 * Added acknowledgement and funding attribution.
 *
 * Revision 1.1  2006/10/11 23:35:12  swift
 * Implement new flashio file system.
 *
 * \end{verbatim}
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
#define flashioChangeLog "$RCSfile: flashio.c,v $ $Revision: 1.2 $ $Date: 2007/04/24 01:43:05 $"

#include <stdio.h>

#define KB            (1L<<10)
#define MB            (1L<<20)
#define FLASHSIZE     (8*MB)
#define FILESIZE      (64*KB)
#define BLOCKSIZE     (8*KB)
#define PAGESIZE      (512U)
#define MAXFILES      (FLASHSIZE/FILESIZE)
#define BUFSIZE       (FILESIZE-PAGESIZE)
#define NFIOBLK       (NFIO)

#define BUSY   (-6)
#define FEMPTY (-5)
#define BADLEN (-4)
#define WFAIL  (-3)
#define RFAIL  (-2)
#define BADARG (-1)
#define FAIL    (0)
#define FTRUNC  (0)
#define OK      (1)

/* define the structure for file control blocks */
typedef far struct
{
   int    fid;
   size_t fpos;
   size_t len;
   char   fname[FILENAME_MAX+1];
   unsigned char buf[BUFSIZE];
} FioBlk;

/* define file control blocks */
extern persistent FioBlk fioblk_[NFIOBLK];

/* function prototypes */
int    fidnext(void);
int    fioFind(const char *fname);
int    fioFormat(void);
int    fioInit(void);
int    fioGet(int fid,FioBlk *fioblk);
size_t fioLen(int fid);
int    fioName(int fid,char *fname,size_t bufsize);
int    fioPut(const FioBlk *fioblk);
int    fioRename(int fid,const char *fname);
int    fioUnlink(int fid);
int    fioWipe(int fid);

#endif /* FLASHIO_H */

#include <apf9.h>
#include <tc58v64.h>
#include <logger.h>
#include <fstring.h>
#include <string.h>

/* define file control blocks */
persistent FioBlk fioblk_[NFIOBLK];

/* define a scratch page buffer */
static far unsigned char pagebuf[PAGESIZE];

#define BLOCKSPERFILE (FILESIZE/BLOCKSIZE)

/*------------------------------------------------------------------------*/
/* function to find the next available flash file folder                  */
/*------------------------------------------------------------------------*/
/**
   This function loops through each file folder in the flash file system in
   search of the next available folder.  On success, this function returns
   the file-id (fid) of the next available folder.  On failure, this
   function returns EOF.
*/
int fidnext(void) 
{
   /* initialize the return value */
   int i,k,fid=EOF;
   
   /* pet the watchdog */
   WatchDog();
   
   /* loop through each folder in the flash file system */
   for (i=0; i<MAXFILES; i++)
   {
      /* create a buffer to hold the file name */
      char fname[FILENAME_MAX+1];

      /* check if the file-id is already in use in an open file */
      for (k=0; k<NFIOBLK; k++) {if (fioblk_[k].fid==i) break;} if (k!=NFIOBLK) continue;

      /* file is available if the current file does not have a valid file name */
      if (!fioName(i,fname,sizeof(fname))) {fid=i; break;}
   }
   
   return fid;
}

/*------------------------------------------------------------------------*/
/* function to find a filename within the filesystem                      */
/*------------------------------------------------------------------------*/
/**
   This function searches the header page of each flash file folder for a
   file with a specified name and returns the flash file system's fid.

      \begin{verbatim}
      input:
         fname....A buffer containing the name of the file to search for.
                  Provision for wildcards or regexs are not implemented.

      output:
         On success, this function returns the file-id (fid) of the file
         folder where the specified file can be found in flash.  On failure,
         this function returns EOF.
      \end{verbatim}
*/
int fioFind(const char *fname)
{
   /* function name for log entries */
   static cc FuncName[] = "fioFind()";

   /* initialize the return value */
   int i,fid=EOF;
   
   /* pet the watchdog */
   WatchDog();

   /* validate the function arguments */
   if (!fname || fnameok(fname)<=0)
   {
      static cc msg[]="Invalid function parameter.\n";
      LogEntry(FuncName,msg); 
   }

   /* loop through each folder in the flash file system */
   else for (i=0; i<MAXFILES; i++)
   {
      /* create a buffer to hold the file name */
      char buf[FILENAME_MAX+1];

      /* get the filename in the current file folder */
      if (fioName(i,buf,sizeof(buf))>0)
      {
         /* check if the file folder contains a matching filename */
         if (!strncmp(buf,fname,FILENAME_MAX)) {fid=i; break;}
      }
   }

   return fid;
}

/*------------------------------------------------------------------------*/
/* function to format the flash memory                                    */
/*------------------------------------------------------------------------*/
/**
   This function formats flash memory by repeatedly calling fioWipe() for
   each (static) file block.  This function returns a positive value on
   success or zero on failure.
*/
int fioFormat(void)
{
   int fid,status=OK;
   
   /* pet the watchdog */
   WatchDog();

   for (fid=0; fid<MAXFILES; fid++) if (fioWipe(fid)<0) status=FAIL;

   return status;
}

/*------------------------------------------------------------------------*/
/* function to read a file from the flash file system                     */
/*------------------------------------------------------------------------*/
/**
   This function reads a specified file from the flash file system into a
   FioBlk object.  

      \begin{verbatim}
      input:
         fid....The file id of the folder in the flash file sysytem that
                contains the file to be read.  The fid must be in the
                semiopen range [0-1024).

      output:
         fioblk....The FioBlk object into which the file is read.

      This function returns one of the following integer values:
         BADARG: One of the function's arguments was invalid.
         OK:     The file was successfully read.
         RFAIL:  Attempt to read one or more pages from flash failed.
         BADLEN: An invalid file length was read from flash.
         FEMPTY: The file folder identified by the specified fid was empty.
         FTRUNC: The file read from flash was truncated.
      \end{verbatim}
*/
int fioGet(int fid,FioBlk *fioblk)
{
   /* function name for log entries */
   static cc FuncName[] = "fioGet()";

   /* initialize the return value */
   int status=BADARG;
   
   /* pet the watchdog */
   WatchDog();
 
   /* validate the function arguments */
   if (fid<0 || fid>=MAXFILES)
   {
      static cc format[]="Invalid fid: %d\n";
      LogEntry(FuncName,format,fid);
   }
  
   /* validate the function arguments */
   else if (!fioblk)
   {
      static cc msg[]="Invalid FioBlk.\n";
      LogEntry(FuncName,msg);
   }

   else
   {
      unsigned int i,j,lpage;

      /* reinitialize the return value */
      status=OK;
      
      /* initialize the FioBlk elements */
      fioblk->fid=fid; fioblk->fpos=0; fioblk->len=0; fioblk->fname[0]=0;

      /* validate the bad-block list */
      if (Tc58v64BadBlockListInit()<=0)
      {
         static cc msg[]="Warning: Flash ram has not been initialized.\n";
         LogEntry(FuncName,msg);
      }

      /* loop through each block of the file folder */
      for (lpage=0, i=0; i<BLOCKSPERFILE; i++)
      {
         /* compute the flash block-id */
         unsigned int page,block = fid*BLOCKSPERFILE + i;

         /* check if the block is in the bad-block table */
         if (!Tc58v64IsBadBlock(block))
         {
            /* loop over each page in the current block */
            for (page=0; page<16; page++)
            {
               /* compute the location of the page buffer in the FioBlk */
               far unsigned char *buf = fioblk->buf + lpage*PAGESIZE;

               /* initialize the page buffer to zeros */
               for (j=0; j<PAGESIZE; j++) buf[j]=0;
               
               /* read the first page of the block */
               if (Tc58v64PageGet(block,page,buf)<=0)
               {
                  static cc format[]="Attempt failed to read block:%u, page:%d from flash.\n";
                  LogEntry(FuncName,format,block,page);
                  status=RFAIL;
               }

               /* the first page contains the filename and file length */
               if (!lpage && !page)
               {
                  /* create a buffer to store the filename */
                  char fname[FILENAME_MAX+1];
                  
                  /* copy the filename to the local buffer */
                  fstrncpy(fname,(far char *)buf,FILENAME_MAX); fname[FILENAME_MAX]=0;
                  
                  /* validate the filename */
                  if (fnameok(fname)>0) 
                  {
                     /* extract the file length from the first page */
                     if ((fioblk->len=(*(far size_t *)(buf+FILENAME_MAX+1)))>BUFSIZE)
                     {
                        static cc format[]="Invalid file length: %u bytes.\n";
                        LogEntry(FuncName,format,fioblk->len);

                        /* break out of the loop */
                        fioblk->len=0; status=BADLEN; break;
                     }

                     /* copy the filename to the FioBlk */
                     else fncpy(fioblk->fname,fname,FILENAME_MAX);
                  }
                  
                  /* empty file folder */
                  else 
                  {
                     static cc format[]="File folder is empty: fid=%d.\n";
                     LogEntry(FuncName,format,fid);

                     /* break out of the loop */
                     fioblk->len=0; status=FEMPTY; break;
                  }
               }

               /* increment the logical-page count */
               else lpage++;
            }

            /* check loop termination criteria */
            if (lpage*PAGESIZE>=fioblk->len || lpage*PAGESIZE>=sizeof(fioblk->buf))  break;
         }
         
         /* make a logentry */
         else if (debuglevel>=4 || debugbits&FLASHIO_H)
         {
            static cc msg[]="Bad block: %u - skipping.\n";
            LogEntry(FuncName,msg,block);
         }
      }

      /* guard against truncated files */
      if (fioblk->len>lpage*PAGESIZE) {fioblk->len=lpage*PAGESIZE; status=FTRUNC;}
   }
   
   return status;
}

/*------------------------------------------------------------------------*/
/* function to format the RAM file system                                 */
/*------------------------------------------------------------------------*/
/**
   This function initializes the FioBlk objects in ram.  On success, this
   function returns a positive value.  On failure, this function returns
   zero.
*/
int fioInit(void)
{
   int i,n,status=OK;
   
   /* compute the length of the filename */
   const int len=sizeof(fioblk_[0].fname);
   
   /* pet the watchdog */
   WatchDog();

   /* loop through each of FioBlk */
   for (i=0; i<NFIOBLK; i++)
   {
      /* initialize the integer elements */
      fioblk_[i].fid=EOF; fioblk_[i].fpos=0; fioblk_[i].len=0;

      /* initialize the file name to zeros */
      for (n=0; n<len; n++) fioblk_[i].fname[n]=0;
   }
   
   return status;
}

/*------------------------------------------------------------------------*/
/* function to read the filename from the first page of the first block   */
/*------------------------------------------------------------------------*/
/**
   This function reads the filename from the first page of the first (good)
   block of a specified file folder in flash ram.

      \begin{verbatim}
      input:
         fid ... The file-folder identifier for the flash file system.  The
                   fid must be in the semiopen range [0-1024).
         bufsize...The size of the "fname" buffer.

      output:
         fname...The buffer into which the filename is written.
      \end{verbatim}

      This function returns a positive value if the read attempt was
      successful and the filename is valid.  On failure, this function
      returns zero.  A negative return value indicates that an exception was
      encountered.
*/
size_t fioLen(int fid)
{
   /* function name for log entries */
   static cc FuncName[] = "fioLen()";

   /* initialize the return value */
   size_t len=-1;
   
   /* pet the watchdog */
   WatchDog();

   /* validate the function arguments */
   if (fid<0 || fid>=MAXFILES)
   {
      static cc format[]="Invalid fid: %d\n";
      LogEntry(FuncName,format,fid);
   }

   else
   {
      int i; char fname[FILENAME_MAX+1];
      
      /* validate the bad-block list */
      if (Tc58v64BadBlockListInit()<=0)
      {
         static cc msg[]="Warning: Flash ram has not been initialized.\n";
         LogEntry(FuncName,msg);
      }

      /* loop through each block of the file to find the first good one */
      for (i=0; i<BLOCKSPERFILE; i++)
      {
         /* compute the flash block-id */
         unsigned int block = fid*BLOCKSPERFILE + i;

         /* check if the block is in the bad-block table */
         if (!Tc58v64IsBadBlock(block))
         {
            /* read the first page of the block */
            if (Tc58v64PageGet(block,0,pagebuf)<0)
            {
               static cc format[]="Attempt failed to read block:%u, page:0 from flash.\n";
               LogEntry(FuncName,format,block);
            }

            /* copy the filename to the local buffer */
            fstrncpy(fname,(far char *)pagebuf,FILENAME_MAX); fname[FILENAME_MAX]=0;

            /* validate the filename */
            if (fnameok(fname)>0) 
            {
               /* extract the file length from the first page */
               if ((len=(*(far size_t *)(pagebuf+FILENAME_MAX+1)))>BUFSIZE)
               {
                  static cc format[]="Invalid file length: %ld bytes.\n";
                  LogEntry(FuncName,format,len); len=-1;
               }
            }

            /* make a low-priority logentry */
            else if (debuglevel>=3 || debugbits&FLASHIO_H)
            {
               static cc format[]="File folder is empty: fid=%d.\n";
               LogEntry(FuncName,format,fid);

            }

            break;
         }
         
         /* make a logentry */
         else if (debuglevel>=4 || debugbits&FLASHIO_H)
         {
            static cc msg[]="Bad block: %u - skipping.\n";
            LogEntry(FuncName,msg,block);
         }
      }
   }
   
   return len;
}

/*------------------------------------------------------------------------*/
/* function to read the filename from the first page of the first block   */
/*------------------------------------------------------------------------*/
/**
   This function reads the filename from the first page of the first (good)
   block of a specified file folder in flash ram.

      \begin{verbatim}
      input:
         fid ... The file-folder identifier for the flash file system.  The
                   fid must be in the semiopen range [0-1024).
         bufsize...The size of the "fname" buffer.

      output:
         fname...The buffer into which the filename is written.

      This function returns a positive value if the read attempt was
      successful and the filename is valid.  On failure, this function
      returns zero.  A negative return value indicates that an exception was
      encountered.  Below is a list of possible return values:
         BADARG: One of the function's arguments was invalid.
         RFAIL:  Attempt to read one or more pages from flash failed.
         FAIL:   The filename read from flash was missing or invalid.
         OK:     The file was successfully read.
      \end{verbatim}
*/
int fioName(int fid,char *fname,size_t bufsize)
{
   /* function name for log entries */
   static cc FuncName[] = "fioName()";

   /* initialize the return value */
   int i,status=BADARG;
   
   /* pet the watchdog */
   WatchDog();

   /* validate the function arguments */
   if (!fname || bufsize<FILENAME_MAX || fid<0 || fid>=MAXFILES)
   {
      static cc msg[]="Invalid function parameter.\n";
      LogEntry(FuncName,msg);
   }

   else
   {
      /* reinitialize the return value */
      status=FAIL;
      
      /* validate the bad-block list */
      if (Tc58v64BadBlockListInit()<=0)
      {
         static cc msg[]="Warning: Flash ram has not been initialized.\n";
         LogEntry(FuncName,msg);
      }

      /* loop through each block of the file to find the first good one */
      for (i=0; i<BLOCKSPERFILE; i++)
      {
         /* compute the flash block-id */
         unsigned int block = fid*BLOCKSPERFILE + i;

         /* check if the block is in the bad-block table */
         if (!Tc58v64IsBadBlock(block))
         {
            /* read the first page of the block */
            if (Tc58v64PageGet(block,0,pagebuf)<0)
            {
               static cc format[]="Attempt failed to read block:%u, page:0 from flash.\n";
               LogEntry(FuncName,format,block);
               status=RFAIL;
            }
            else
            {
               /* copy the filename to the function parameter */
               fstrncpy(fname,(far char *)pagebuf,FILENAME_MAX); fname[FILENAME_MAX]=0;
      
               /* validate the filename */
               if (fnameok(fname)>0) status=OK; else {fname[0]=0; status=FAIL;}
            }
            
            /* exit the loop */
            break;
         }

         /* make a logentry */
         else if (debuglevel>=4 || debugbits&FLASHIO_H)
         {
            static cc msg[]="Bad block: %u - skipping.\n";
            LogEntry(FuncName,msg,block);
         }
      }
   }
   
   return status;
}

/*------------------------------------------------------------------------*/
/* function to write a file to the flash file system                      */
/*------------------------------------------------------------------------*/
/**
   This function writes a file stored in an FioBlk object to the flash file
   system.

      \begin{verbatim}
      input:
         fioblk....The FioBlk object from which the file is written.

      This function returns one of the following integer values:
         BADARG: One of the function's arguments was invalid.
         OK:     The file was successfully read.
         WFAIL:  Attempt to write one or more pages to flash failed.
         FTRUNC: The file read from flash was truncated.
      \end{verbatim}
*/
int fioPut(const FioBlk *fioblk)
{
   /* function name for log entries */
   static cc FuncName[] = "fioPut()";

   /* initialize the return value */
   int status=BADARG;
   
   /* pet the watchdog */
   WatchDog();
 
   /* validate the function arguments */
   if (!fioblk)
   {
      static cc msg[]="Invalid FioBlk.\n";
      LogEntry(FuncName,msg);
   }

   /* validate the function arguments */
   else if (fioblk->fid<0 || fioblk->fid>=MAXFILES)
   {
      static cc format[]="Invalid fid: %d\n";
      LogEntry(FuncName,format,fioblk->fid);
   }

   /* validate the function arguments */
   else if (fioblk->len>BUFSIZE)
   {
      static cc format[]="Invalid file length: %u\n";
      LogEntry(FuncName,format,fioblk->len);
   }

   /* prepare the file folder for writing */
   else if (fioWipe(fioblk->fid)<=0)
   {
      static cc format[]="Failed attempt to erase file folder: fid=%u\n";
      LogEntry(FuncName,format,fioblk->fid);
      status=FAIL;
   }
   
   else
   {
      unsigned int i,j,lpage;

      /* reinitialize the return value */
      status=OK;

      /* validate the bad-block list */
      if (Tc58v64BadBlockListInit()<=0)
      {
         static cc msg[]="Warning: Flash ram has not been initialized.\n";
         LogEntry(FuncName,msg);
      }

      /* loop through each block of the file folder */
      for (lpage=0, i=0; i<BLOCKSPERFILE; i++)
      {
         /* compute the flash block-id */
         unsigned int page=0,block = fioblk->fid*BLOCKSPERFILE + i;
   
         /* pet the watchdog */
         WatchDog();

         /* check if the block is in the bad-block table */
         if (!Tc58v64IsBadBlock(block))
         {
            /* check if the file header needs to be written */
            if (!lpage)
            {
               /* compute a pointer into the page buffer */
               far unsigned int *len=((far unsigned char *)pagebuf)+FILENAME_MAX+1;

               /* write the file length into the page buffer */
               *len=fioblk->len;

               /* write the filename into the page buffer */
               for (j=0; j<FILENAME_MAX+1; j++) pagebuf[j]=fioblk->fname[j];

               /* write the file header to flash */
               if (Tc58v64PagePut(block,page,pagebuf)<=0)
               {
                  static cc format[]="Failed attempt to write header to block:%u, page:%u of flash.\n";
                  LogEntry(FuncName,format,block,page);
                  status=WFAIL; 
               }
               else if (debuglevel>=3 || debugbits&FLASHIO_H)
               {
                  static cc format[]="File[fid=%u] header written to block:%u, page:%u of flash.\n";
                  LogEntry(FuncName,format,fioblk->fid,block,page);
               }
               
               /* increment the block's page counter */
               page++;
            }

            /* loop over each page in the current block */
            for (; page<16 && lpage*PAGESIZE<fioblk->len; page++,lpage++)
            {
               /* compute the location of the page buffer in the FioBlk */
               far unsigned char *buf = fioblk->buf + lpage*PAGESIZE;
              
               if (Tc58v64PagePut(block,page,buf)<=0)
               {
                  static cc format[]="Failed attempt to write block:%d, page:%d to flash.\n";
                  LogEntry(FuncName,format,block,page);
                  status=WFAIL; 
               }
            }

            /* check if the whole file buffer has been written yet */
            if (lpage*PAGESIZE>=fioblk->len) break;
         }
      }

      /* verify that all of the buffer was written */
      if (lpage*PAGESIZE<fioblk->len) status=FTRUNC;
   }
   
   return status;
}

/*------------------------------------------------------------------------*/
/* function to rename a file in the flash file system                     */
/*------------------------------------------------------------------------*/
/**
   This function renames a file in the flash file system.  

      \begin{verbatim}
      input:
         fid......The file-folder identifier for the flash file system.  The
                  fid must be in the semiopen range [0-1024).
         fname....The new name to be assigned to the file in the flash file
                  system. 

      This function returns one of the following integer values:
         BADARG: One of the function's arguments was invalid.
         BUSY:   All FioBlk objects are already in use.
         FAIL:   General failure when attempting to rename file.
         RFAIL:  Attempt to read one or more pages from flash failed.
         WFAIL:  Attempt to write one or more pages to flash failed.
         OK:     The file was successfully read.
      \end{verbatim}
*/
int fioRename(int fid,const char *fname)
{
   /* function name for log entries */
   static cc FuncName[] = "fioRename()";

   /* initialize the return value */
   int fd,i,status=BADARG;
   
   /* pet the watchdog */
   WatchDog();

   /* validate the function arguments */
   if (!fname || !fname[0] || fid<0 || fid>=MAXFILES)
   {
      static cc msg[]="Invalid function parameter.\n";
      LogEntry(FuncName,msg);
   }

   /* validate the filename */
   else if (fnameok(fname)<=0)
   {
      static cc format[]="Invalid filename: [%s]\n";
      LogEntry(FuncName,format,fname); 
   }

   /* get an available FioBlk */
   else if ((fd=fdes())<0)
   {
      static cc msg[]="All FioBlk's are already in use.";
      LogEntry(FuncName,msg); status=BUSY;
   }
   
   else
   {
      /* reinitialize the return value */
      status=FAIL;
      
      /* validate the bad-block list */
      if (Tc58v64BadBlockListInit()<=0)
      {
         static cc msg[]="Warning: Flash ram has not been initialized.\n";
         LogEntry(FuncName,msg);
      }

      /* loop through each block of the file to find the first good one */
      for (i=0; i<BLOCKSPERFILE; i++)
      {
         /* compute the flash block-id */
         unsigned int page,block = fid*BLOCKSPERFILE + i;

         /* check if the block is in the bad-block table */
         if (!Tc58v64IsBadBlock(block))
         {
            /* read all 16 pages of the block */
            for (status=OK,page=0; page<16; page++)
            {
               /* compute a pointer to the page buffer */
               far unsigned char *buf = fioblk_[fd].buf + page*PAGESIZE;
               
               /* read the current page from the block */
               if (Tc58v64PageGet(block,page,buf)<0)
               {
                  static cc format[]="Failed attempt to read block:%u, page:%d from flash.\n";
                  LogEntry(FuncName,format,block,page); status=RFAIL;
               }
            }

            /* verify that all 16 pages were read */
            if (status>FAIL)
            {
               /* copy the filename to the function parameter */
               fncpy((far char *)fioblk_[fd].buf,fname,FILENAME_MAX);

               /* erase the block */
               Tc58v64Erase(block);

               /* write all 16 pages of the block */
               for (page=0; page<16; page++)
               {
                  /* compute a pointer to the page buffer */
                  far unsigned char *buf = fioblk_[fd].buf + page*PAGESIZE;

                  /* write the current page to flash */
                  if (Tc58v64PagePut(block,page,buf)<0)
                  {
                     static cc format[]="Failed attempt to write block:%u, page:%d from flash.\n";
                     LogEntry(FuncName,format,block,page); status=WFAIL;
                  }
               }
            }
            
            /* exit the loop */
            break;
         }

         /* make a logentry */
         else if (debuglevel>=4 || debugbits&FLASHIO_H)
         {
            static cc msg[]="Bad block: %u - skipping.\n";
            LogEntry(FuncName,msg,block);
         }
      }
   }
   
   return status;
}

/*------------------------------------------------------------------------*/
/* function to delete a file from the flash file system                   */
/*------------------------------------------------------------------------*/
/**
   This function deletes a file from the flash file system by using the
   fioWipe() function.  See fioWipe() for details.
*/
int fioUnlink(int fid)
{
   return fioWipe(fid);
}

/*------------------------------------------------------------------------*/
/* function to wipe a file clean of data                                  */
/*------------------------------------------------------------------------*/
/**
   This function wipes clean any data in a file.

      \begin{verbatim}
      input:

         fid...The file identifier for the file to be wiped clean.

      output:

         This function returns a positive value on success, zero on failure,
         or a negative number if an exception was encountered.  
      \end{verbatim}
*/
int fioWipe(int fid)
{
   /* function name for log entries */
   static cc FuncName[] = "fioWipe()";

   int i,status=BADARG;
   
   /* pet the watchdog */
   WatchDog();

   /* validate the function arguments */
   if (fid<0 || fid>=MAXFILES)
   {
      static cc format[]="Invalid fid: %d\n";
      LogEntry(FuncName,format,fid);
   }

   /* validate the function argument */
   else
   {
      /* loop through each block in the current file */
      for (status=OK,i=0; i<BLOCKSPERFILE; i++)
      {
         /* compute the block-id */
         int block = fid*BLOCKSPERFILE + i;

         /* erase the block */
         if (Tc58v64Erase(block)<=0) status=FAIL;
      }
   }

   return status;
}



