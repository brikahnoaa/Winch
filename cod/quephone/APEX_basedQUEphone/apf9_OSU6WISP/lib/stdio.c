#ifndef STDIO_H
#define STDIO_H (0x1000U)

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * $Id: stdio.c,v 1.7.2.1 2008/09/11 19:45:22 dbliudnikas Exp $
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
 * $Log: stdio.c,v $
 * Revision 1.7.2.1  2008/09/11 19:45:22  dbliudnikas
 * Add routines without CR/LF translation for Seascan TD support
 *
 * Revision 1.7  2007/04/24 01:43:29  swift
 * Added acknowledgement and funding attribution.
 *
 * Revision 1.6  2006/12/26 20:58:38  swift
 * Fixed an error in a comment.
 *
 * Revision 1.5  2006/10/13 18:55:29  swift
 * Added fstreamok() validation to most functions that use FILE objects.
 * Modified fstreamcrc() to speed up the calculation and cut overhead.
 *
 * Revision 1.4  2006/10/11 21:05:43  swift
 * Implemented the new flashio file system.
 *
 * Revision 1.3  2006/04/21 13:45:40  swift
 * Changed copyright attribute.
 *
 * Revision 1.2  2004/12/29 23:06:50  swift
 * Modified LogEntry() to use strings stored in the CODE segment.  This saves
 * lots of space in the DATA segment and significantly speeds code start-up.
 *
 * Revision 1.1  2004/07/14 22:52:56  swift
 * A new implmentation of the stdio library.
 *
 * Revision 1.3  2003/11/20 18:59:35  swift
 * Added GNU Lesser General Public License to source file.
 *
 * Revision 1.2  2003/06/07 20:44:15  swift
 * Fixed a bug in fprintf() - the output was always written to stdout
 * regardless of what the stream pointer in the function argument was.
 * Also implemented printf() by calling vfprintf() with stdout as the
 * stream pointer.
 *
 * Revision 1.1  2003/05/31 22:11:25  swift
 * Initial revision
 * \end{verbatim}
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
#define FileIoChangeLog "$RCSfile: stdio.c,v $ $Revision: 1.7.2.1 $ $Date: 2008/09/11 19:45:22 $"

#include <stddef.h>
#include <stdarg.h>

/* file management structure - internal use only, use const-correct FILE instead */
typedef persistent struct 
{
   char          fdes;
   unsigned char mode;
   unsigned char err;
   unsigned int  crc;
} File;

/* read-only version of file management structure */
typedef const File FILE;

/* typedef for file-position type */
typedef long int fpos_t;

/* function prototypes for stdio library */
int    clearerr(FILE *stream);
int    fclose(FILE *stream);
int    feof(FILE *stream);
int    ferror(FILE *stream);
int    fgetc(FILE *stream);
char  *fgets(char *s, int size, FILE *stream);
int    fgetpos(FILE *stream, fpos_t *pos);
int    fileno(FILE *stream);
int    fflush(FILE *stream);
FILE  *fopen(const char *fname, const char *mode);
int    fprintf(FILE *stream, const char *format, ...);
int    fputc(int c,FILE *stream);
int    fputs(const char *buf, FILE *stream);
size_t fread(void *ptr, size_t size, size_t nmemb, FILE *stream);
int    fseek(FILE *stream, long offset, int whence);
int    fsetpos(FILE *stream, fpos_t *pos);
long   ftell(FILE *stream);
size_t fwrite(const void *buf, size_t size, size_t nmemb, FILE *stream);
void   perror(const char *s);
int    printf(const char *format, ...);
int    puts(const char *s);
int    rename(const char *oldpath, const char *newpath);
int    remove(const char *pathname);
void   rewind(FILE *stream);
int    snprintf(char *str, size_t size, const char *format, ...);
int    sprintf(char *str, const char *format, ...);
int    vfprintf(FILE *fp, const char *format, va_list ap);

int    fputc_CRLF(int c,FILE *stream); /* fputc WITHOUT CR/LF translation */

#define getc(stream)      fgetc(stream)
#define getchar()         fgetc(stdin)
#define putc(byte,stream) fputc(byte,stream)
#define putchar(byte)     fputc(byte,stdout)
#define putchar_CRLF(byte) fputc_CRLF(byte,stdout) /* putchar W/O CR/LF translation */

/* nonstandard functions to augment the stdio library */
int   fcloseall(void);
int   fdes(void);
int   fdump(FILE *stream);
int   fformat(void);
int   fnameok(const char *fname);
int   fstreamok(FILE *stream);

/* external declarations of stdio streams */
extern FILE *stdin, *stdout, *stderr;

/* stdio resident macros */
#define FOPEN_MAX     (8)
#define FILENAME_MAX  (63)
#define EOF           (-1)
#define SEEK_SET      (0)
#define SEEK_CUR      (1)
#define SEEK_END      (2)
 
/* nonstandard macros */
#define NSTD     (3)
#define NFIO     (FOPEN_MAX-NSTD)
#define KB       (1L<<10)
#define MB       (1L<<20)

#endif /* STDIO_H */

#include <crc16bit.h>
#include <fstring.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <assert.h>
#include <flashio.h>

#ifdef _XA_
   #include <apf9.h>
#else
   #define WatchDog()
   #define StackOk() 1
#endif /* _XA_ */

#define STDIN   (NFIO+0)
#define STDOUT  (NFIO+1)
#define STDERR  (NFIO+2)

#define IOOK    (0x00)
#define IOR     (0x01)
#define IOW     (0x02)
#define IOEOF   (0x40)
#define IOERR   (0x80)

/* define the array of _iobuf objects */
static File fio[NFIO];

/* define stdio streams */
static File stdio[NSTD]=
{
   { STDIN, IOR, 0,  (STDIN<<8)|IOR},
   {STDOUT, IOW, 0, (STDOUT<<8)|IOW},
   {STDERR, IOW, 0, (STDERR<<8)|IOW}
};

/* initialize stdio streams */
FILE *stdin=&stdio[0], *stdout=&stdio[1], *stderr=&stdio[2];

/* prototypes for functions with static linkage */
static unsigned int fstreamcrc(FILE *stream);
static void         fvalid(void);
static int          fiobusy(const char *fname);
static void         fioinit(File *fio);
 
/* low-level IO functions for console IO */
extern void putch(char byte);
extern int  getch(void);

/*------------------------------------------------------------------------*/
/* function to test the EOF indicator of a stream                         */
/*------------------------------------------------------------------------*/
/**
   This function tests the EOF indicator for the stream pointed to by
   stream, returning non-zero if it is set.  The error indicator can only be
   reset by the clearerr function.  If the function argument is NULL then
   errno is set to ENULLARG and the function will return zero.
*/
int clearerr(FILE *stream)
{
   int status=0;

   /* pet the watchdog */
   WatchDog();

   /* validate the function argument */
   if (!stream) errno=ENULLARG;

   /* validate the stream */
   else if (fstreamok(stream)<=0) {errno=EBADF; fioinit((File *)stream);} 
   
   /* make sure the stream is associated with a non-stdio stream */
   else if (stream->fdes>=0 && stream->fdes<NFIO)
   {
      /* clear the IOERR and IOEOF bits */
      ((File *)stream)->err&=(~(IOERR|IOEOF));

      /* indicate success */
      status=1;
   }
      
   return status;
}

/*------------------------------------------------------------------------*/
/*                                                                        */
/*------------------------------------------------------------------------*/
int fcloseall(void)
{
   int i,status=1;

   for (i=0; i<NFIO; i++)
   {
      if (fstreamok(&fio[i])<=0) fioinit(&fio[i]);
      else if (fclose(&fio[i])) status=0;
   }

   return status;
}

/*------------------------------------------------------------------------*/
/* function to close a FILE stream                                        */
/*------------------------------------------------------------------------*/
/**
   This function dissociates the named stream from its underlying file or
   set of functions.  If the stream was being used for output, any buffered
   data is written first, using fflush().
      \begin{verbatim}
      input:
         stream...The FILE stream to close.

      output:
         Upon successful completion 0 is returned.  Otherwise, EOF is
         returned and the global variable errno is set to indicate the
         error:
            ENULLARG: A null function argument.
            EFULL: The file buffer was already full when a write attempt occurred.
            EPERM: The stdio streams can not be closed.
      \end{verbatim}
*/
int fclose(FILE *stream)
{
   /* initialize the function's return value */
   int err=EOF;

   /* pet the watchdog */
   WatchDog();

   /* clear the IOEOF bit */
   if (stream) ((File *)stream)->err&=(~IOEOF);
   
   /* validate the function parameter */
   if (!stream) errno=ENULLARG;
      
   /* validate the stream */
   else if (fstreamok(stream)<=0) {errno=EBADF; fioinit((File *)stream);} 

   /* refuse to close stdio streams */
   else if (stream->fdes==STDIN || stream->fdes==STDOUT || stream->fdes==STDERR) errno=EPERM;
   
   /* write the file to flash */
   else if ((stream->mode&IOW) && !fflush(stream)) err=0;

   /* no action required for read-only streams */
   else if (stream->mode&IOR) err=0;
   
   /* reinitialize the stream */
   if (!err) {fioinit((File *)stream);}
   
   return err;  
}

/*------------------------------------------------------------------------*/
/*                                                                        */
/*------------------------------------------------------------------------*/
/**
   This function returns the next available file descriptor.
*/
int fdes(void)
{
   int i;
   
   /* search for the next available FILE object */
   for (i=0; i<NFIO; i++) {if (fio[i].fdes<0) break;}

   /* check for too many files open */
   if (i>=NFIO) i=EOF;

   return i;
}

/*------------------------------------------------------------------------*/
/* diagnostic function to dump a FioBlk object to stderr                  */
/*------------------------------------------------------------------------*/
/**
   This function dumps the contents of a FILE stream object to stderr.  It
   was intended mostly for diagnostic purposes.  If the function argument is
   NULL then errno is set to ENULLARG and the function will return EOF.
   Otherwise, the return value and the value of errno will be as determined
   by fstreamok().
*/
int fdump(FILE *stream) 
{
   int status=EOF;
   
   /* pet the watchdog */
   WatchDog();

   /* validate the function arguments */
   if (!stream) errno=ENULLARG;

   else
   {
      /* reinitialize the function's return value */
      status=fstreamok(stream);

      /* dump the members of the FILE structure */
      fprintf(stderr,"%s: fdes[%d] mode[0x%04x] err[0x%02x] crc[0x%04x] [%s]\n",
              (stream->fdes==STDIN || stream->fdes==STDOUT || stream->fdes==STDERR)?"STDIO":"FILE",
              stream->fdes,stream->mode,stream->err,stream->crc,((status>0)?"OK":"Invalid"));

      /* validate the stream */
      if (status>0 && stream->fdes>=0 && stream->fdes<NFIO)
      {
         size_t i;
         char fname[FILENAME_MAX+1];
         
         /* create a local pointer to the IO control block */
         FioBlk *fioblk=&fioblk_[stream->fdes];

         /* dump the length, file position, and file name */
         fprintf(stderr,"FioBlk[%d]: fid[%d] fpos[%u] len[%u] fname[%s]\n",
                 stream->fdes,fioblk->fid,fioblk->fpos,fioblk->len,
                 fstrncpy(fname,fioblk->fname,FILENAME_MAX));

         /* dump the buffer */
         for (i=0; i<fioblk->len; i++) putchar(fioblk->buf[i]);

         /* terminate the dump */
         if (fioblk->buf[fioblk->len-1]!='\n') putchar('\n');
      }
   }
   
   return status;
}

/*------------------------------------------------------------------------*/
/* function to test the EOF indicator of a stream                         */
/*------------------------------------------------------------------------*/
/**
   This function tests the EOF indicator for the stream pointed to by
   stream, returning non-zero if it is set.  The EOF indicator can only be
   reset by the clearerr function.  If the function argument is NULL then
   errno is set to ENULLARG and the function will return zero.
*/
int feof(FILE *stream)
{
   int status=0;

   /* pet the watchdog */
   WatchDog();

   /* validate the function argument */
   if (!stream) errno=ENULLARG;
      
   /* validate the stream */
   else if (fstreamok(stream)<=0) {errno=EBADF; fioinit((File *)stream);} 

   /* make sure the stream is associated with a non-stdio stream */
   else if (stream->fdes>=0 && stream->fdes<NFIO)
   {
      /* test the IOEOF bit */
      if (stream->err&IOEOF) status=1;
   }
   
   return status;
}

/*------------------------------------------------------------------------*/
/* function to test the error indicator of a stream                       */
/*------------------------------------------------------------------------*/
/**
   This function tests the error indicator for the stream pointed to by
   stream, returning non-zero if it is set.  The error indicator can only be
   reset by the clearerr function.  If the function argument is NULL then
   errno is set to ENULLARG and the function will return zero.
*/
int ferror(FILE *stream)
{
   /* initialize the return value */
   int status=0;
   
   /* pet the watchdog */
   WatchDog();
     
   /* validate the function argument */
   if (!stream) errno=ENULLARG;
 
   /* validate the stream */
   else if (fstreamok(stream)<=0) {errno=EBADF; fioinit((File *)stream);} 

   /* make sure the stream is associated with a non-stdio stream */
   if (stream->fdes>=0 && stream->fdes<NFIO)
   {
      /* test the IOERR bit */
      if (stream->err&IOERR) status=1;
   }

   return status;
}

/*------------------------------------------------------------------------*/
/* function to format and reinitialize the file system                    */
/*------------------------------------------------------------------------*/
/**
   This function formats and reinitializes the entire file system.
*/
int fformat(void)
{
   int i,status=0;

   /* reinitialize the File objects */
   for (i=0; i<NFIO; i++) fioinit(&fio[i]);

   /* reinitialize the FioBlk objects */
   status=fioInit();
   
   return status;
}

/*------------------------------------------------------------------------*/
/* function to get the next byte from a FILE stream                       */
/*------------------------------------------------------------------------*/
/**
   This function reads the next byte from a FILE stream and returns it as an
   unsigned char cast to an int.  Upon end of file or error, this function
   returns EOF and errno is set accordingly:
      ENULLARG: A null function argument.
      EBADF: The FILE object 'stream' is invalid or has been corrupted.
      EINVAL: The stream is invalid. 
*/
int fgetc(FILE *stream)
{
   int c=EOF;

   /* pet the watchdog */
   WatchDog();

   /* validate the function argument */
   if (!stream) errno=ENULLARG;
 
   /* validate the stream */
   else if (fstreamok(stream)<=0) {errno=EBADF; fioinit((File *)stream);} 

   /* check for input on stdio streams */
   else if (stream->fdes==STDIN)
   {
      /* read a character from stdin */
      if ((c=getch())==EOF) errno=EBADF;
   }
   
   /* validate the stream */
   else if (stream->fdes<0 || stream->fdes>=NFIO) errno=EINVAL;
   
   /* check if the stream is read-enabled */
   else if (!(stream->mode&IOR)) errno=EMODE;
   
   /* check for EOF */
   else if (fioblk_[stream->fdes].fpos>=fioblk_[stream->fdes].len) 
   {
      /* set the stream's EOF bit */
      ((File *)stream)->err |= IOEOF;
   }

   else
   {
      /* create a local pointer to the FioBlk object */
      FioBlk *fioblk = &fioblk_[stream->fdes];

      /* extract the next byte from the FioBlk buffer */
      c=fioblk->buf[fioblk->fpos++];
   }
   
   return c;
}

/*------------------------------------------------------------------------*/
/* function to return the file position indicator for a FILE stream       */
/*------------------------------------------------------------------------*/
/**
   This function gets the file position indicator for a FILE stream.  Refer
   to ftell() for details.
*/
int fgetpos(FILE *stream, fpos_t *pos) 
{
   int err = -1;

   /* pet the watchdog */
   WatchDog();

   /* validate the function arguments */
   if (!stream || !pos) errno=ENULLARG;
 
   /* validate the stream */
   else if (fstreamok(stream)<=0) {errno=EBADF; fioinit((File *)stream);} 

   /* set the file position */
   else {*pos = ftell(stream); err=0;}

   return err;
}

/*------------------------------------------------------------------------*/
/* function to read a string from a FILE stream                           */
/*------------------------------------------------------------------------*/
/**
   This function reads in at most one less than size characters from stream
   and stores them into the buffer pointed to by buf.  Reading stops after
   an EOF or a newline.  If a newline is read, it is stored into the buffer.
   A NULL is stored after the last character in the buffer.  
*/
char *fgets(char *buf, int size, FILE *stream)
{
   int n=0,c;

   /* pet the watchdog */
   WatchDog();

   /* validate the function arguments */
   if (!stream || !buf) errno=ENULLARG;

   /* validate the buffer size */
   else if (size<=0) errno=EINVAL;
    
   /* validate the stream */
   else if (fstreamok(stream)<=0) {errno=EBADF; fioinit((File *)stream);} 

   else
   {
      /* read at most (size-1) bytes */
      for (n=0; n<(size-1);)
      {
         /* exit on EOF */
         if ((c=fgetc(stream))==EOF) break;

         /* store the character in the buffer */
         buf[n++]=c;

         /* exit on newline */ 
         if (c=='\n') break; 
      }

      /* terminate the string */
      buf[n]=0;
   }

   /* return the buffer if at least one byte was read */
   return (n>0) ? buf : NULL;
}

/*------------------------------------------------------------------------*/
/* function to return the file descriptor of a FILE stream                */
/*------------------------------------------------------------------------*/
/**
   This function returns the file descriptor of a FILE stream.  If the
   stream is invalid this function returns -1 and sets errno to indicate the
   nature of the error.  ENULLARG indicates that the function argument was
   NULL.  EBADF indicates that the FILE object 'stream' is invalid or has
   been corrupted.
*/
int fileno(FILE *stream)
{
   int fdes=-1;

   /* pet the watchdog */
   WatchDog();

   /* validate the function argument */
   if (!stream) errno=ENULLARG;
 
   /* validate the stream */
   else if (fstreamok(stream)<=0) {errno=EBADF; fioinit((File *)stream);} 

   /* extract the file descriptor */
   else fdes=stream->fdes;

   return fdes;
}

/*------------------------------------------------------------------------*/
/* function to write buffered data to the flash file system               */
/*------------------------------------------------------------------------*/
/**
  This function forces a write of all buffered data for the specified FILE
  stream.  The open status of the stream is unaffected.  If the stream
  argument is NULL then all open output streams are flushed.  Upon
  successful completion 0 is returned.  Otherwise, EOF is returned and the
  global variable errno is set to indicate the error.  EBADF indicates that
  'stream' is not an open/valid stream or is not open for writing.  EINVAL
  indicates failure of low-level flash IO.
*/
int fflush(FILE *stream)
{
   /* initialize the function's return value */
   int i,status=EOF;

   /* pet the watchdog */
   WatchDog();

   /* clear the IOEOF bit */
   if (stream) ((File *)stream)->err&=(~IOEOF);

   /* check to see if all streams should be flushed */
   if (!stream) 
   {
      /* loop through each FILE object */
      for (status=0,i=0; i<NFIO; i++)
      {
         /* associate the stream to the current FILE object & clear the IOEOF bit */
         stream=&fio[i]; ((File *)stream)->err&=(~IOEOF);

         /* validate the stream and check its writeability */
         if (fstreamok(stream)>=0 && (stream->mode&IOW))
         {
            /* write the file to flash */
            fioPut(&fioblk_[stream->fdes]);
         }
      }
   }
 
   /* validate the stream */
   else if (fstreamok(stream)<=0) {errno=EBADF; fioinit((File *)stream);} 

   /* make sure the IO stream is writeable */
   else if (!(stream->mode&IOW)) errno=EBADF;

   /* nothing to do for stdio streams */
   else if (stream->fdes==STDIN || stream->fdes==STDOUT || stream->fdes==STDERR) status=0;

   /* write the stream buffers to flash */
   else if (fioPut(&fioblk_[stream->fdes])>=0) status=0;

   /* indicate general write failure */
   else errno=EINVAL;
   
   return status;
}

/*------------------------------------------------------------------------*/
/* function to determine if a given file is already open                  */
/*------------------------------------------------------------------------*/
/**
   This function determines if a file with a specified name is already
   open.  This function returns a non-negative value (ie., the file
   descriptor) if an open file is found.  Otherwise, this function returns
   the value -1.  If the function argument is NULL then errno is set to
   ENULLARG and the function returns the value -1.
*/
static int fiobusy(const char *fname)
{
   int i,fdes=EOF;

   /* pet the watchdog */
   WatchDog();

   /* validate the function argument */
   if (!fname) errno=ENULLARG;

   else for (i=0; i<NFIO; i++)
   {
      if (fio[i].fdes==EOF || fio[i].mode==IOOK) continue;
      if (!fstrncmp(fname,fioblk_[i].fname,FILENAME_MAX)) fdes=fio[i].fdes;
   }

   return fdes;
}

/*------------------------------------------------------------------------*/
/* function to initialize a FILE object                                   */
/*------------------------------------------------------------------------*/
/**
   This function initializes a FILE object to be in a valid but inactive
   state.  If the function argument is NULL then errno is set to ENULLARG.
*/
static void fioinit(File *fio)
{
   /* pet the watchdog */
   WatchDog();

   /* validate the function argument */
   if (!fio) errno=ENULLARG;

   /* initialize the FILE structure elements */
   else {fio->fdes=EOF; fio->mode=IOOK; fio->err=0; fio->crc=fstreamcrc(fio);}
}

/*------------------------------------------------------------------------*/
/* function to open a stream and associate it with a filename             */
/*------------------------------------------------------------------------*/
/**
   The fopen function opens the file whose name is the string pointed to by
   path and associates a stream with it.

      \begin{verbatim}
      input:
         mode....The argument mode points to one of the following strings:

                 r  Open text file for reading.  The stream is positioned at
                    the beginning of the file.

                 r+ Open for reading and writing.  The stream is positioned
                    at the beginning of the file.

                 w  Truncate file to zero length or create text file for
                    writing.  The stream is positioned at the beginning of
                    the file.

                 w+ Open for reading and writing.  The file is created if it
                    does not exist, otherwise it is truncated.  The stream
                    is positioned at the beginning of the file.

                 a  Open for appending (writing at end of file).  The file is
                    created if it does not exist.  The stream is positioned
                    at the end of the file.

                 a+ Open for reading and appending (writing at end of file).
                    The file is created if it does not exist.  The stream is
                    positioned at the end of the file.

         The mode string can also include the letter 'b' as a last character
         in any of the strings described above.  This is strictly for
         compatibility with ANSI X3.159-1989 (ANSI C) and has no effect; the
         'b' is ignored on all POSIX conforming systems, including Linux.
         (Other systems may treat text files and binary files differently,
         and adding the 'b' may be a good idea if you do I/O to a binary
         file and expect that your program may be ported to non-Unix
         environments.)

      output:
         FILE....Upon successful completion fopen() returns a FILE pointer.
                 Otherwise, NULL is returned and the global variable errno
                 is set to indicate the error:
                    ENULLARG: A null function argument.
                    EFNAME: Invalid file name.
                    EMFILE: Too many files open - no available descriptors.
                    EBUSY: A file with the same name is already open.
                    ENIMP: Mode not implemented.
                    EFULL: The flash file system was already full when a open attempt occurred.
                    EMODE: Invalid mode specified.
      \end{verbatim}
*/
FILE *fopen(const char *fname, const char *mode)
{
   /* initialize the return value */
   File *stream=NULL;
   
   /* pet the watchdog */
   WatchDog();

   /* validate the file objects */
   fvalid();

   /* validate the function arguments */
   if (!fname || !mode) errno=ENULLARG;

   /* validate the file name */
   else if (!fnameok(fname)) errno=EFNAME;

   else
   {
      int fd,fid;

      /* check for too many files open */
      if ((fd=fdes())<0) errno=EMFILE;
      
      /* make sure a file with the same name is not already open */
      else if (fiobusy(fname)>=0) errno=EBUSY;
      
      /* open a file in append mode */
      else if (mode[0]=='a') 
      {
         /* check if the file exists in flash */
         if ((fid=fioFind(fname))>=0)
         {
            /* create a local pointer to the file IO block */
            FioBlk *fioblk = &fioblk_[fd];

            /* read the file from flash and initialize the file IO block members*/
            fioGet(fid,fioblk); fioblk->fpos=0;

            /* initialize the return value */
            stream = &fio[fd];

            /* initialize the file IO stream members */
            stream->fdes=fd; stream->err=0;
            stream->mode=(mode[1]=='+') ? (IOR|IOW) : IOW;
            stream->crc=fstreamcrc(stream);
         }

         /* file doesn't exist in flash - create a new file */
         else if ((fid=fidnext())>=0)
         {
            /* create a local pointer to the file IO block */
            FioBlk *fioblk = &fioblk_[fd];

            /* initialize the file IO block members */
            fioblk->fid=fid; fioblk->len=0; fioblk->fpos=0; fioblk->buf[0]=0;
            fncpy(fioblk->fname,fname,FILENAME_MAX);

            /* initialize the return value */
            stream = &fio[fd];

            /* initialize the file IO stream members */
            stream->fdes=fd; stream->err=0;
            stream->mode=(mode[1]=='+') ? (IOR|IOW) : IOW;
            stream->crc=fstreamcrc(stream);
         }
      
         /* set the errno to indicate that the flash is full */
         else {errno=EFULL; fioinit(stream); stream=NULL;}
      }
      
      /* open a file in read mode */
      else if (mode[0]=='r')
      {
         /* compute the file id for flash storage */
         if ((fid=fioFind(fname))>=0)
         {
            /* create a local pointer to the file IO block */
            FioBlk *fioblk = &fioblk_[fd];

            /* read the file from flash and initialize the file IO block members*/
            fioGet(fid,fioblk); fioblk->fpos=0;

            /* initialize the return value */
            stream = &fio[fd];

            /* initialize the file IO stream members */
            stream->fdes=fd; stream->err=0;
            stream->mode=(mode[1]=='+') ? (IOR|IOW) : IOR;
            stream->crc=fstreamcrc(stream);
         }

         /* indicate that the file doesn't exist */
         else errno=NOPATH;
      }

      /* open a file in write mode */
      else if (mode[0]=='w')
      {
         /* compute the file id for flash storage */
         if ((fid=fioFind(fname))>=0 || (fid=fidnext())>=0)
         {
            /* create a local pointer to the file IO block */
            FioBlk *fioblk = &fioblk_[fd];
            
            /* initialize the file IO block members */
            fioblk->fid=fid; fioblk->len=0; fioblk->fpos=0; fioblk->buf[0]=0;
            fncpy(fioblk->fname,fname,FILENAME_MAX);

            /* initialize the return value */
            stream = &fio[fd];

            /* initialize the file IO stream members */
            stream->fdes=fd; stream->err=0;
            stream->mode=(mode[1]=='+') ? (IOR|IOW) : IOW;
            stream->crc=fstreamcrc(stream);
         }

         /* set the errno to indicate that the flash is full */
         else {errno=EFULL;}
      }
      
      /* illegal in unimplemented mode */
      else errno=EMODE;
   }

   return stream;
}

/*------------------------------------------------------------------------*/
/* function to determine if a specified filename is valid                 */
/*------------------------------------------------------------------------*/
/**
   This function determines if a specified filename is valid.  Filenames are
   case sensitive, must start with a letter, and be composed from the the
   set of characters inside the brackets: [A-Za-z0-9.:;-_].  The length of
   the filename may range from 1 to FILENAME_MAX.

   This function returns a positive value if the filename is valid and zero
   if the filename is invalid.  This function returns a negative value if an
   exception is detected and errno is set accordingly.  ENULLARG indicates
   that the function argument was NULL.
*/
int fnameok(const char *fname)
{
   int len,status=EOF;

   /* pet the watchdog */
   WatchDog();

   /* validate the function argument */
   if (!fname) errno=ENULLARG;

   /* valid names must be at least one byte long */
   else if (!fname[0]) status=0;
   
   else
   {
      /* reiniatialize the return value */
      status=0;
      
      /* validate the first character */
      if (toupper(*fname)>='A' && toupper(*fname)<='Z') status=1;
      
      /* make sure the first character is a letter of the alphabet */
      else if (toupper(*fname)>='0' && toupper(*fname)<='9') status=1;

      /* loop through and validate each character of the file name */
      for (len=0; (*fname) && status; fname++,len++) 
      {
         /* reject any characters not in the set: [A-Za-z0-9.:;-_] */
         if ((*fname)<'-' || (*fname)>'z') status=0;
         else if (strchr("/\\[]<>{}^=?`@",*fname)) status=0;
      }

      /* check to see if the filename is too long */
      if (len>FILENAME_MAX) status=0;
   }
           
   return status;
}

/*------------------------------------------------------------------------*/
/* function to print to a FILE stream                                     */
/*------------------------------------------------------------------------*/
/**
   This function prints data to a FILE stream according to a specified
   format.  Refer to any C reference for further documentation.
*/
int fprintf(FILE *stream, const char *format, ...)
{
   int n=0;
   va_list arg_pointer;
   va_start(arg_pointer,format);
   n=vfprintf(stream, format, arg_pointer);
   va_end(arg_pointer);
   return n;
}

/*------------------------------------------------------------------------*/
/* write a character to a stream                                          */
/*------------------------------------------------------------------------*/
/**
   This function writes a byte to a FILE stream and returns a non-negative
   number on success, or EOF on error.

      \begin{verbatim}
      intput:
         c........The character (cast to an unsigned char) to write.
         stream...The FILE stream to which the byte will be written.

      output:

         This function returns a non-negative number on success, or EOR on
         error.  If an error occurs, errno is set to indicate the fault.
            ENULLARG: A null function argument.
            EFULL: The file buffer was already full when a write attempt occurred.
            EPERM: The FILE stream was not write enabled.
            EBADF: The file descriptor is invalid.
      \end{verbatim}
*/
int fputc(int c, FILE *stream)
{
   /* initialize the function's return value */
   int status=EOF;

   /* pet the watchdog */
   WatchDog();

   /* validate the function parameter */
   if (!stream) errno=ENULLARG;

   /* check for output to stdio streams */
   else if (stream->fdes==STDOUT || stream->fdes==STDERR) 
   {
      /* write the character stdout and initialize return value */
      putch(c); status=c; 
   }
       
   /* validate the stream */
   else if (fstreamok(stream)<=0) {errno=EBADF; fioinit((File *)stream);} 

   /* validate the file descriptor */
   else if (stream->fdes<0 || stream->fdes>=NFIO) errno=EBADF;
   
   /* check if the stream is write-enabled */
   else if (stream->mode&IOW)
   {
      /* create a local pointer to the file control block */
      FioBlk *fioblk = &fioblk_[stream->fdes];

      /* guard against buffer overflow */
      if (fioblk->len<BUFSIZE)
      {
         /* write the character to the file buffer */
         fioblk->buf[fioblk->len++]=(unsigned char)c;

         /* initialize the return value */
         status=(unsigned char)c;
      }

      /* indicate that the file is full */
      else {errno=EFULL;  ((File *)stream)->err|=IOEOF;}
   }

   /* indicate that the stream is not write-enabled */
   else errno=EPERM;
   
   return status;
}

/*------------------------------------------------------------------------*/
/* write a character to a stream WITHOUT CR/LF translation                */
/*------------------------------------------------------------------------*/
/**
   This function writes a byte to a FILE stream and returns a non-negative
   number on success, or EOF on error.  The putch_CRLF is used
   instead of putch to remove unwanted CR/LF translation so serial display
   output reflects only actual characters transmitted.

      \begin{verbatim}
      intput:
         c........The character (cast to an unsigned char) to write.
         stream...The FILE stream to which the byte will be written.

      output:

         This function returns a non-negative number on success, or EOR on
         error.  If an error occurs, errno is set to indicate the fault.
            ENULLARG: A null function argument.
            EFULL: The file buffer was already full when a write attempt occurred.
            EPERM: The FILE stream was not write enabled.
            EBADF: The file descriptor is invalid.
      \end{verbatim}
*/
int fputc_CRLF(int c, FILE *stream)
{
   /* initialize the function's return value */
   int status=EOF;

   /* pet the watchdog */
   WatchDog();

   /* validate the function parameter */
   if (!stream) errno=ENULLARG;

   /* check for output to stdio streams */
   else if (stream->fdes==STDOUT || stream->fdes==STDERR) 
   {
      /* write the character stdout W/O CR/LF translation and initialize return value */
      putch_CRLF(c); status=c; 
   }
       
   /* validate the stream */
   else if (fstreamok(stream)<=0) {errno=EBADF; fioinit((File *)stream);} 

   /* validate the file descriptor */
   else if (stream->fdes<0 || stream->fdes>=NFIO) errno=EBADF;
   
   /* check if the stream is write-enabled */
   else if (stream->mode&IOW)
   {
      /* create a local pointer to the file control block */
      FioBlk *fioblk = &fioblk_[stream->fdes];

      /* guard against buffer overflow */
      if (fioblk->len<BUFSIZE)
      {
         /* write the character to the file buffer */
         fioblk->buf[fioblk->len++]=(unsigned char)c;

         /* initialize the return value */
         status=(unsigned char)c;
      }

      /* indicate that the file is full */
      else {errno=EFULL;  ((File *)stream)->err|=IOEOF;}
   }

   /* indicate that the stream is not write-enabled */
   else errno=EPERM;
   
   return status;
}

/*------------------------------------------------------------------------*/
/* function to write a string to a FILE stream                            */
/*------------------------------------------------------------------------*/
/**
   This function writes the string buf to a FILE stream (without its
   trailing NULL) and returns the number of bytes written.  If an exception
   occurs then this function returns EOF and errno is set accordingly.
*/
int fputs(const char *buf, FILE *stream)
{
   int n=EOF;

   /* pet the watchdog */
   WatchDog();

   /* validate function arguments */
   if (!stream || !buf) errno=ENULLARG;
 
   /* validate the stream */
   else if (fstreamok(stream)<=0) {errno=EBADF; fioinit((File *)stream);} 

   /* write the buffer to the FILE stream */
   else for (n=0; *buf; n++,buf++)
   {
      /* write the next character to the stream */
      if (fputc(*buf,stream)==EOF) {n=EOF; break;}
   }

   /* return the number of bytes written */
   return n;
}

/*------------------------------------------------------------------------*/
/* function to reads objects of arbitrary size from a FILE stream         */
/*------------------------------------------------------------------------*/
/**
   This function is used to read objects of arbitrary size from a FILE
   stream.
      \begin{verbatim}
      input:
         buf......A pointer to the first byte of the collection of objects
                  to be read from the FILE stream.  The objects must be
                  contiguous in memory.
         size.....The size (ie., number of bytes) of each object.
         nmemb....The number of objects (each with 'size' bytes) to read.
         stream...The FILE stream to which the objects should be read.

      output:
         This function returns the number of objects (ie., not the number of
         bytes) successfully read from the FILE stream.  If an error occurs,
         or the end-of-file is reached, the return value is a short item
         count (or zero).  If an error occurs, errno will be set with one of
         the values indicated in fgetc().
      \end{verbatim}
*/
size_t fread(void *buf, size_t size, size_t nmemb, FILE *stream)
{
   int c;
   size_t i,n=0;

   /* assign a pointer to the buffer */
   unsigned char *p=(unsigned char *)buf;
   
   /* pet the watchdog */
   WatchDog();

   /* validate the function arguments */
   if (!buf || !stream) errno=ENULLARG;

   /* check for degenerate case */
   else if (size<=0 || nmemb<=0) errno=EINVAL;
 
   /* validate the stream */
   else if (fstreamok(stream)<=0) {errno=EBADF; fioinit((File *)stream);} 

   /* loop through each object */
   else for (n=0; n<nmemb; n++) 
   {
      /* loop through the bytes in the current object */
      for (i=0; i<size; i++,p++) 
      {
         /* read a byte from the stream */
         if ((c=fgetc(stream))==EOF) goto error;
         else (*p) = (unsigned char)c;
      }
   }
   
   error: /* collection point when errors are detected in fgetc() */

   return n;
}

/*------------------------------------------------------------------------*/
/* function to seek a specified position in a stream                      */
/*------------------------------------------------------------------------*/
/**
   The fseek function sets the file position indicator for the stream
   pointed to by stream.  The new position, measured in bytes, is obtained
   by adding offset bytes to the position specified by whence.  If whence is
   set to SEEK_SET, SEEK_CUR, or SEEK_END, the offset is relative to the
   start of the file, the current position indicator, or end-of-file,
   respectively.

   Upon successful, this function returns zero.  Otherwise, -1 is returned
   and the global variable errno is set to indicate the error:
      ENULLARG: The function argument was NULL.
      EBADF: The FILE object 'stream' is invalid or has been corrupted.
      EINVAL: Invalid 'whence' specifier.
      ERANGE: Attempt to seek beyond one end of the file or the other.
*/
int fseek(FILE *stream, long offset, int whence)
{
   /* initialize the return value */
   int err=-1;

   /* pet the watchdog */
   WatchDog();

   /* validate the function argument */
   if (!stream) errno=ENULLARG;
 
   /* validate the stream */
   else if (fstreamok(stream)<=0) {errno=EBADF; fioinit((File *)stream);} 

   /* make sure the stream is seekable */
   else if (stream->fdes<0 || stream->fdes>=NFIO) errno=EBADF;
   
   else
   {
      long where;
      
      /* set a local pointer to the FioBlk object */
      FioBlk *fioblk=&fioblk_[stream->fdes];

      /* reinitialize the return value to indicate success */
      err=0;

      /* compute the new file position */
      switch (whence)
      {
         /* compute the offset relative to the beginning of the file */
         case SEEK_SET: {where=offset; break;}
            
         /* compute the offset relative to the current file position */
         case SEEK_CUR: {where=offset + fioblk->fpos; break;}
            
         /* compute the offset relative to the end of the file */
         case SEEK_END: {where=offset + fioblk->len; break;}

         /* invalid 'whence' specifier */   
         default:       {err=-1; errno=EINVAL;}
      }

      /* avoid reposition the file pointer if errors were detected */
      if (!err)
      {
         /* reposition the file pointer */
         if (where>=0 && where<=fioblk->len)
         {
            /* reposition the file pointer */
            fioblk->fpos=where;

            /* clear the IOEOF bits */
            ((File *)stream)->err&=(~IOEOF);
         }

         /* attempt to seek beyond either end of the file */
         else {err=-1; errno=ERANGE;}
      }
   }
   
   return err;
}

/*------------------------------------------------------------------------*/
/* function to set the file position of a stream                          */
/*------------------------------------------------------------------------*/
/**
   This function sets the file pointer to a specified file position.  Refer
   to fseek() for details.
*/
int fsetpos(FILE *stream, fpos_t *pos) 
{
   int err = -1;

   /* pet the watchdog */
   WatchDog();

   /* validate the function arguments */
   if (!stream || !pos) errno=ENULLARG;

   /* set the file position */
   else err=fseek(stream,*pos,SEEK_SET);

   return err;
}

/*------------------------------------------------------------------------*/
/* function to compute the FILE object's CRC                              */
/*------------------------------------------------------------------------*/
/**
   This function computes a FILE object's CRC element.  This CRC is used to
   determine that a stream is in a valid state.  If the function argument is
   NULL then this function returns zero (which is never a valid CRC).
*/
static unsigned int fstreamcrc(FILE *stream)
{
   unsigned int crc = 0;

   /* pet the watchdog */
   WatchDog();

   if (stream)
   {
      /* verify that the crc contains at least 2 bytes */
      assert(sizeof(crc)>=2);
      
      /* compute CRC of the stream object */
      crc = (((unsigned int)stream->fdes)<<8) | stream->mode;

      /* remap 0x0000 to 0x0001 */
      if (!crc) crc=0x0001;
   }
   
   return crc;
}

/*------------------------------------------------------------------------*/
/* function to determine if a FILE stream is valid                        */
/*------------------------------------------------------------------------*/
/**
   This function determines if a FILE stream is valid.  A stream is valid if
   its file descriptor is in the semiopen range [0,FOPEN_MAX] and has a
   valid CRC.  This function returns a positive number if the stream is
   valid and zero if the stream is invalid.  If the function argument is
   NULL then this function returns -1 and errno is set to ENULLARG.
*/
int fstreamok(FILE *stream)
{
   int status=1;

   /* pet the watchdog */
   WatchDog();

   /* validate the function argument */
   if (!stream) {errno=ENULLARG; status=-1;}

   /* apply weaker validation conditions for the stdio streams */
   else if (stream->fdes==STDIN || stream->fdes==STDOUT || stream->fdes==STDERR) status=1;
   
   /* validate the file descriptor */
   else if (stream->fdes<0 || stream->fdes>=FOPEN_MAX) status=0;

   /* compare the CRC of the stream object */
   else status=(fstreamcrc(stream)==stream->crc && stream->crc)?1:0;
   
   return status;
}

/*------------------------------------------------------------------------*/
/* function to return the file position indicator for a FILE stream       */
/*------------------------------------------------------------------------*/
/**
   This function obtains the current value of the file position indicator
   for a specified FILE stream.  On success, this function returns a
   non-negative file position indicator.  If an exception is detected then
   this function returns -1 and errno is set accordingly:
      ENULLARG: A null function argument.
      EBADF: The FILE object 'stream' is invalid or has been corrupted.
*/
long int ftell(FILE *stream)
{
   /* initialize the return value */
   long int fpos=-1;

   /* pet the watchdog */
   WatchDog();

   /* validate the function argument */
   if (!stream) errno=ENULLARG;
 
   /* validate the stream */
   else if (fstreamok(stream)<=0) {errno=EBADF; fioinit((File *)stream);} 

   /* make sure the stream has an associated position */
   else if (stream->fdes<0 || stream->fdes>=NFIO) errno=EBADF;

   /* validate the stream */
   else if (fstreamok(stream)<=0) errno=EBADF;

   /* extract the file position from the stream's FioBlk object */
   else fpos=fioblk_[stream->fdes].fpos;
   
   return fpos; 
}

/*------------------------------------------------------------------------*/
/* function to initialize FILE objects                                    */
/*------------------------------------------------------------------------*/
/**
   This function makes sure that all of the FILE objects are initialized and
   valid. 
*/
static void fvalid(void)
{
   int i;

   /* pet the watchdog */
   WatchDog();

   /* validate/initialize the file IO control structures */
   for (i=0; i<NFIO; i++) {if (fstreamok(&fio[i])<=0) fioinit(&fio[i]);}

   /* validate the stdio streams */
   stdio[0].fdes=STDIN;  stdio[0].mode=IOR; stdio[0].err=0; stdio[0].crc=fstreamcrc(&stdio[0]);
   stdio[1].fdes=STDOUT; stdio[1].mode=IOW; stdio[1].err=0; stdio[1].crc=fstreamcrc(&stdio[1]);
   stdio[2].fdes=STDERR; stdio[2].mode=IOW; stdio[2].err=0; stdio[2].crc=fstreamcrc(&stdio[2]);
}

/*------------------------------------------------------------------------*/
/* function to write objects of arbitrary size to a FILE stream           */
/*------------------------------------------------------------------------*/
/**
   This function is used to write objects of arbitrary size to a FILE
   stream.
      \begin{verbatim}
      input:
         buf......A pointer to the first byte of the collection of objects
                  to be written to the FILE stream.  The objects must be
                  contiguous in memory.
         size.....The size (ie., number of bytes) of each object.
         nmemb....The number of objects (each with 'size' bytes) to write.
         stream...The FILE stream to which the objects should be written.

      output:

         This function returns the number of objects (ie., not the number of
         bytes) successfully written to the FILE stream.  If an error
         occurs, or the end-of-file is reached, the return value is a short
         item count (or zero).  If an error occurs, errno will be set with
         one of the values indicated in fputc().
      \end{verbatim}
*/
size_t fwrite(const void *buf, size_t size, size_t nmemb, FILE *stream)
{
   size_t i,n=0;

   /* pet the watchdog */
   WatchDog();

   /* validate the function arguments */
   if (!buf || !stream) errno=ENULLARG;

   /* check for degenerate case */
   else if (size<=0 || nmemb<=0) errno=EINVAL;
 
   /* validate the stream */
   else if (fstreamok(stream)<=0) {errno=EBADF; fioinit((File *)stream);} 

   else
   {
      /* initialize a pointer to the buffer */
      unsigned char *c = (unsigned char *)buf;

      /* loop through each object */
      for (n=0; n<nmemb; n++) 
      {
         /* loop through the bytes in the current object */
         for (i=0; i<size; i++,c++) 
         {
            /* write the current byte to the stream */
            if (fputc((*c),stream)==EOF) goto error;
         }
      }
   }
   
   error: /* collection point when errors are detected in fputc() */

   return n;
}

/*------------------------------------------------------------------------*/
/* function to write error code to stdout                                 */
/*------------------------------------------------------------------------*/
void perror(const char *s)
{
   if (s && *s) printf("%s: ",s);
   fprintf(stderr,"Stdio Error[%02d]\n",errno);
}

/*------------------------------------------------------------------------*/
/* function to print to stdout                                            */
/*------------------------------------------------------------------------*/
/**
   This function prints data to the stdout stream according to a specified
   format.  Refer to any C reference for further documentation.
*/
int printf(const char *format, ...)
{
   int n=0;
   va_list arg_pointer;
   va_start(arg_pointer,format);
   n=vfprintf(stdout, format, arg_pointer);
   va_end(arg_pointer);
   return n;
}

/*------------------------------------------------------------------------*/
/* function to write a string to stdout                                   */
/*------------------------------------------------------------------------*/
/**
   This function writes a string to stdout and returns the number of
   characters written.  See fputs() for details.
*/
int puts(const char *s)
{
   int n = fputs(s, stdout);

   return n;
}

/*------------------------------------------------------------------------*/
/* function to remove a file from the filesystem                          */
/*------------------------------------------------------------------------*/
/**
   This function removes a file from the filesystem.  The attempt will fail
   if the file is open at the time this function is called.
      \begin{verbatim}
      input:
         fname....The name of the file to remove from the filesystem.

      output:
         On success, this function will return a non-negative value equal
         to the file's fid.  Otherwise, EOF is returned and errno is set to
         indicate the cause of failure:
            ENULLARG: A null function argument.
            EINVAL: The filename is invalid.
            EBUSY:  The file can't be removed because it is open.
            ENOENT: The file does not exist.
      \end{verbatim}
*/
int remove(const char *fname)
{
   int fid=EOF;

   /* pet the watchdog */
   WatchDog();

   /* validate the function argument */
   if (!fname) errno=ENULLARG;

   /* guard against a null file name */
   else if (!(*fname) || fnameok(fname)<=0) errno=EINVAL;
   
   /* check if the file is open */
   else if (fiobusy(fname)>=0) errno=EBUSY;
      
   /* check if the file is already in flash storage */
   else if ((fid=fioFind(fname))>=0) fid=fioUnlink(fid);

   /* indicate that no such file exists */
   else errno=ENOENT;
   
   return fid;
}

/*------------------------------------------------------------------------*/
/* function to rename a file                                              */
/*------------------------------------------------------------------------*/
/**
   This function renames an existing file to a new name.  If successful,
   this function returns the non-negative file id (fid) of the file.  A
   negative value is returned on failure.
*/
int rename(const char *oldfname, const char *newfname)
{
   int fid=EOF;

   /* pet the watchdog */
   WatchDog();

   /* validate the function arguments */
   if (!oldfname || !newfname) errno=ENULLARG;

   /* validate the old and new filenames */
   else if (fnameok(oldfname)<=0 || fnameok(newfname)<=0) errno=EINVAL;

   /* make sure that neither filename is currently in use */
   else if (fiobusy(oldfname)>=0 || fiobusy(newfname)>=0) errno=EBUSY;

   /* make sure the new filename isn't already assigned to a file */
   else if (fioFind(newfname)>=0) errno=EACCESS;

   /* make sure the old file exists */
   else if ((fid=fioFind(oldfname))<0) errno=NOPATH;

   /* rename the file */
   else if (fioRename(fid,newfname)<=0) {errno=ENOEXEC; fid=EOF;}
   
   return fid;
}

/*------------------------------------------------------------------------*/
/* function to set the file position to the beginning                     */
/*------------------------------------------------------------------------*/
/**
   The function repositions the file position to the beginning of the file.  
*/
void rewind(FILE *stream) 
{
   fseek(stream,0L,SEEK_SET);
}
