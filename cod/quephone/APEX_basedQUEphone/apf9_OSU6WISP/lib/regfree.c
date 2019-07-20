/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * $Id: regfree.c,v 1.1 2002/10/08 23:54:30 swift Exp $
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/** 
   This C source code implements the regcomp() component of a POSIX regular
   expression parser library.  Henry Spencer was the author of the C source
   code library on which this is based.  Most of the source remains
   unmodified from Spencer's version.  The following copyright notice was
   included with Henry's distribution:
 
      \begin{verbatim}
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
      \end{verbatim}
      
   No modifications have been made to the functionality of regcomp().  The
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
   
          void regfree(regex_t *preg);
   
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
   
          Regfree frees any dynamically-allocated storage associated
          with the compiled RE pointed to by  preg.   The  remaining
          regex_t is no longer a valid compiled RE and the effect of
          supplying it to regexec or regerror is undefined.
   
          None of these functions references global variables except
          for  tables of constants; all are safe for use from multi-
          ple threads if the arguments are safe.
   
   SEE ALSO
   
          POSIX 1003.2, sections 2.8 (Regular  Expression  Notation)
          and B.5 (C Binding for Regular Expression Matching).
   
   HISTORY
          Written by Henry Spencer, henry@zoo.toronto.edu.
          
   \end{verbatim}
   
 * RCS log of revisions to the C source code.
 *
 * \begin{verbatim}
 * $Log: regfree.c,v $
 * Revision 1.1  2002/10/08 23:54:30  swift
 * Initial revision
 *
 * \end{verbatim}
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
#define RegfreeChangeLog "$RCSfile: regfree.c,v $  $Revision: 1.1 $   $Date: 2002/10/08 23:54:30 $"

#include <stdio.h>
#include <stdlib.h>
#include "regex.h"
#include "regutils.h"
#include "regex2.h"

/*
 - regfree - free everything
 = extern void regfree(regex_t *);
 */
void regfree(regex_t *preg)
{
	register struct re_guts *g;

	if (preg->re_magic != MAGIC1)	/* oops */
		return;			/* nice to complain, but hard */

	g = preg->re_g;
	if (g == NULL || g->magic != MAGIC2)	/* oops again */
		return;
	preg->re_magic = 0;		/* mark it invalid */
	g->magic = 0;			/* mark it invalid */

	if (g->strip != NULL)
		free((char *)g->strip);
	if (g->sets != NULL)
		free((char *)g->sets);
	if (g->setbits != NULL)
		free((char *)g->setbits);
	if (g->must != NULL)
		free(g->must);
	free((char *)g);
}
