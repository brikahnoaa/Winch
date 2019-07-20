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
