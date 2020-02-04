#ifndef REGERROR_H
#define REGERROR_H

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * $Id: regerror.c,v 1.2 2003/07/02 17:24:22 swift Exp $
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
 * Revision 1.2  2003/07/02 17:24:22  swift
 * The name for the integer object 'code' in the rerr structure was renamed to
 * be 'icode'.  This is a work-around for the fact that the HiTech compiler
 * used for APF9 embedded firmware has a 'code' keyword.
 *
 * Revision 1.1  2002/10/08 23:54:07  swift
 * Initial revision
 * \end{verbatim}
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
#define RegerrorChangeLog "$RCSfile: regerror.c,v $  $Revision: 1.2 $   $Date: 2003/07/02 17:24:22 $"

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
