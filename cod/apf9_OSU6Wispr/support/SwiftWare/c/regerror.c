#ifndef REGERROR_H
#define REGERROR_H

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * $Id: regerror.c,v 1.2 2005/02/22 22:01:25 swift Exp $
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/** 
   This C source code implements the regerror() component of a POSIX regular
   expression parser library.  Henry Spencer was the author of the C source
   code library on which this is based.  Most of the source remains
   unmodified from Henry's version.  The following copyright notice was
   included with Henry's distribution:
   
      Copyright 1992, 1993, 1994, 1997 Henry Spencer.  All rights reserved.
      This software is not subject to any license of the American Telephone
      and Telegraph Company or of the Regents of the University of California.
      
      Permission is granted to anyone to use this software for any purpose on
      any computer system, and to alter it and redistribute it, subject
      to the following restrictions:
      
      1. The author is not responsible for the consequences of use of this
         software, no matter how awful, even if they arise from flaws in it.
      
      2. The origin of this software must not be misrepresented, either by
         explicit claim or by omission.  Since few users ever read sources,
         credits must appear in the documentation.
      
      3. Altered versions must be plainly marked as such, and must not be
         misrepresented as being the original software.  Since few users
         ever read sources, credits must appear in the documentation.
      
      4. This notice may not be removed or altered.

   No modifications have been made to the functionality of regerror().  The
   nature of the modifications are mostly repackaging of the source code.
   The following documetation was cut from the man (3) page that accompanied
   Spencer's distribution. 

   \begin{verbatim}
   REGEX(3)                                                 REGEX(3)
   
   NAME
          regcomp,  regexec,  regerror, regfree - regular-expression
          library
   
   SYNOPSIS
          #include <regex.h>
   
       size_t regerror(int errcode, const regex_t *preg,
                       char *errbuf, size_t errbuf_size);
   
   DESCRIPTION
          These routines implement POSIX 1003.2 regular  expressions
          (``RE''s);  see  regex(7).  Regcomp compiles an RE written
          as a string into an internal form,  regexec  matches  that
          internal  form  against  a  string  and  reports  results,
          regerror transforms error codes from  either  into  human-
          readable messages, and regfree frees any dynamically-allo-
          cated storage used by the internal form of an RE.
   
          The header <regex.h> declares two structure types, regex_t
          and regmatch_t, the former for compiled internal forms and
          the latter for match reporting.  It also declares the four
          functions, a type regoff_t, and a number of constants with
          names starting with ``REG_''.
   
          Regerror maps a non-zero errcode from  either  regcomp  or
          regexec  to  a human-readable, printable message.  If preg
          is non-NULL, the error code should have arisen from use of
          the regex_t pointed to by preg, and if the error code came
          from regcomp, it should have been the result from the most
          recent  regcomp using that regex_t.  (Regerror may be able
          to supply a more detailed message using  information  from
          the  regex_t.)  Regerror places the NUL-terminated message
          into the buffer pointed to by errbuf, limiting the  length
          (including  the NUL) to at most errbuf_size bytes.  If the
          whole message won't fit, as much of it as will fit  before
          the  terminating  NUL  is  supplied.   In  any  case,  the
          returned value is the size of buffer needed  to  hold  the
          whole message (including terminating NUL).  If errbuf_size
          is 0, errbuf is ignored but the return value is still cor-
          rect.
   
          If  the  errcode  given  to  regerror  is  first ORed with
          REG_ITOA, the ``message'' that results  is  the  printable
          name  of the error code, e.g. ``REG_NOMATCH'', rather than
          an explanation thereof.  If errcode is REG_ATOI, then preg
          shall  be non-NULL and the re_endp member of the structure
          it points to must point to the printable name of an  error
          code;  in  this  case, the result in errbuf is the decimal
          digits of the numeric value of the error code  (0  if  the
          name  is  not  recognized).   REG_ITOA  and  REG_ATOI  are
          intended  primarily  as  debugging  facilities;  they  are
          extensions,  compatible  with  but  not specified by POSIX
          1003.2, and  should  be  used  with  caution  in  software
          intended  to be portable to other systems.  Be warned also
          that they are considered experimental and changes are pos-
          sible.
   
   SEE ALSO
   
          POSIX 1003.2, sections 2.8 (Regular  Expression  Notation)
          and B.5 (C Binding for Regular Expression Matching).
   
   HISTORY
          Written by Henry Spencer, henry@zoo.toronto.edu.
   
   \end{verbatim}

 * RCS log of revisions to the C source code.
 *
 * \begin{verbatim}
 * $Log: regerror.c,v $
 * Revision 1.2  2005/02/22 22:01:25  swift
 * Eliminated GNU regex implementation.
 *
 * Revision 1.2  2003/07/02 17:24:22  swift
 * The name for the integer object 'code' in the rerr structure was renamed to
 * be 'icode'.  This is a work-around for the fact that the HiTech compiler
 * used for APF9 embedded firmware has a 'code' keyword.
 *
 * Revision 1.1  2002/10/08 23:54:07  swift
 * Initial revision
 * \end{verbatim}
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
#define RegerrorChangeLog "$RCSfile: regerror.c,v $  $Revision: 1.2 $   $Date: 2005/02/22 22:01:25 $"

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>
#include <stdlib.h>
#include "regex.h"
#include "regutils.h"

#ifdef __cplusplus
extern "C" {
#endif

/* === regerror.c === */
static char *regatoi(const regex_t *preg, char *localbuf);

#ifdef __cplusplus
}
#endif

#endif /* REGERROR_H */

/*
 = #define  REG_OKAY  0
 = #define  REG_NOMATCH  1
 = #define  REG_BADPAT   2
 = #define  REG_ECOLLATE    3
 = #define  REG_ECTYPE   4
 = #define  REG_EESCAPE  5
 = #define  REG_ESUBREG  6
 = #define  REG_EBRACK   7
 = #define  REG_EPAREN   8
 = #define  REG_EBRACE   9
 = #define  REG_BADBR   10
 = #define  REG_ERANGE  11
 = #define  REG_ESPACE  12
 = #define  REG_BADRPT  13
 = #define  REG_EMPTY   14
 = #define  REG_ASSERT  15
 = #define  REG_INVARG  16
 = #define  REG_ATOI 255   // convert name to number (!)
 = #define  REG_ITOA 0x100 // convert number to name (!)
 */
static struct rerr {
   int icode;
   char *name;
   char *explain;
} rerrs[] = {
   {REG_OKAY,   "REG_OKAY", "no errors detected"},
   {REG_NOMATCH,   "REG_NOMATCH", "regexec() failed to match"},
   {REG_BADPAT, "REG_BADPAT",  "invalid regular expression"},
   {REG_ECOLLATE,  "REG_ECOLLATE",   "invalid collating element"},
   {REG_ECTYPE, "REG_ECTYPE",  "invalid character class"},
   {REG_EESCAPE,   "REG_EESCAPE", "trailing backslash (\\)"},
   {REG_ESUBREG,   "REG_ESUBREG", "invalid backreference number"},
   {REG_EBRACK, "REG_EBRACK",  "brackets ([ ]) not balanced"},
   {REG_EPAREN, "REG_EPAREN",  "parentheses not balanced"},
   {REG_EBRACE, "REG_EBRACE",  "braces not balanced"},
   {REG_BADBR,  "REG_BADBR",   "invalid repetition count(s)"},
   {REG_ERANGE, "REG_ERANGE",  "invalid character range"},
   {REG_ESPACE, "REG_ESPACE",  "out of memory"},
   {REG_BADRPT, "REG_BADRPT",  "repetition-operator operand invalid"},
   {REG_EMPTY,  "REG_EMPTY",   "empty (sub)expression"},
   {REG_ASSERT, "REG_ASSERT",  "\"can't happen\" -- you found a bug"},
   {REG_INVARG, "REG_INVARG",  "invalid argument to regex routine"},
   {-1,      "",      "*** unknown regexp error code ***"},
};

/*
 - regerror - the interface to error numbers
 = extern size_t regerror(int, const regex_t *, char *, size_t);
 */
/* ARGSUSED */
size_t regerror(int errcode, const regex_t *preg, char *errbuf, size_t errbuf_size)
{
   register struct rerr *r;
   register size_t len;
   register int target = errcode & ~REG_ITOA;
   register char *s;
   char convbuf[50];

   if (errcode == REG_ATOI)
      s = regatoi(preg, convbuf);
   else
   {
      for (r = rerrs; r->icode >= 0; r++)
      {
         if (r->icode == target) break;
      }
      
      if (errcode&REG_ITOA)
      {
         if (r->icode >= 0) (void) strcpy(convbuf, r->name);
         else sprintf(convbuf, "REG_0x%x", target);
         
         assert(strlen(convbuf) < sizeof(convbuf));
         s = convbuf;
      }
      else {s = r->explain;}
   }

   len = strlen(s) + 1;
   if (errbuf_size > 0) {
      if (errbuf_size > len)
         (void) strcpy(errbuf, s);
      else {
         (void) strncpy(errbuf, s, errbuf_size-1);
         errbuf[errbuf_size-1] = '\0';
      }
   }

   return(len);
}

/*
 - regatoi - internal routine to implement REG_ATOI
 == static char *regatoi(const regex_t *preg, char *localbuf);
 */
static char *
regatoi(preg, localbuf)
const regex_t *preg;
char *localbuf;
{
   register struct rerr *r;

   for (r = rerrs; r->icode >= 0; r++)
      if (strcmp(r->name, preg->re_endp) == 0)
         break;
   if (r->icode < 0)
      return("0");

   sprintf(localbuf, "%d", r->icode);
   return(localbuf);
}
