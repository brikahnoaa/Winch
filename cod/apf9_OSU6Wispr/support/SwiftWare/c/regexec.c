/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * $Id: regexec.c,v 1.2 2005/02/22 22:01:25 swift Exp $
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

       int regexec(const regex_t *preg, const char *string,
                   size_t nmatch, regmatch_t pmatch[], int eflags);
   
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
   
          Regexec matches the compiled RE pointed to by preg against
          the string, subject to the flags in  eflags,  and  reports
          results using nmatch, pmatch, and the returned value.  The
          RE must have been compiled by  a  previous  invocation  of
          regcomp.   The  compiled form is not altered during execu-
          tion of regexec, so a  single  compiled  RE  can  be  used
          simultaneously by multiple threads.
   
          By default, the NUL-terminated string pointed to by string
          is considered to be the text of an entire line,  with  the
          NUL  indicating  the end of the line.  (That is, any other
          end-of-line marker is considered to have been removed  and
          replaced  by the NUL.)  The eflags argument is the bitwise
          OR of zero or more of the following flags:
   
          REG_NOTBOL    The first character of the string is not the
                        beginning  of  a  line,  so  the  `^' anchor
                        should not match before it.  This  does  not
                        affect   the   behavior  of  newlines  under
                        REG_NEWLINE.
   
          REG_NOTEOL    The NUL terminating the string does not  end
                        a  line,  so the `$' anchor should not match
                        before it.  This does not affect the  behav-
                        ior of newlines under REG_NEWLINE.
   
          REG_STARTEND  The   string   is  considered  to  start  at
                        string + pmatch[0].rm_so and to have a  ter-
                        minating    NUL    located    at    string +
                        pmatch[0].rm_eo (there need not actually  be
                        a  NUL  at that location), regardless of the
                        value of nmatch.  See below for the  defini-
                        tion  of  pmatch  and  nmatch.   This  is an
                        extension, compatible with but not specified
                        by  POSIX  1003.2,  and  should be used with
                        caution in software intended to be  portable
                        to  other  systems.   Note  that  a non-zero
                        rm_so does not imply  REG_NOTBOL;  REG_STAR-
                        TEND   affects  only  the  location  of  the
                        string, not how it is matched.
   
          See regex(7) for a discussion of what is matched in situa-
          tions  where an RE or a portion thereof could match any of
          several substrings of string.
   
          Normally, regexec returns 0 for success and  the  non-zero
          code  REG_NOMATCH for failure.  Other non-zero error codes
          may be returned in exceptional  situations;  see  DIAGNOS-
          TICS.
   
          If  REG_NOSUB  was specified in the compilation of the RE,
          or if nmatch is 0, regexec  ignores  the  pmatch  argument
          (but  see  below for the case where REG_STARTEND is speci-
          fied).  Otherwise, pmatch points to  an  array  of  nmatch
          structures  of  type  regmatch_t.  Such a structure has at
          least the members rm_so and rm_eo, both of  type  regoff_t
          (a  signed  arithmetic  type at least as large as an off_t
          and a ssize_t), containing respectively the offset of  the
          first character of a substring and the offset of the first
          character after the end of  the  substring.   Offsets  are
          measured  from  the beginning of the string argument given
          to regexec.  An empty substring is denoted by  equal  off-
          sets,  both  indicating  the character following the empty
          substring.
   
          The 0th member of the pmatch array is filled in  to  indi-
          cate  what  substring  of string was matched by the entire
          RE.  Remaining members report what substring  was  matched
          by  parenthesized  subexpressions  within the RE; member i
          reports  subexpression  i,  with  subexpressions   counted
          (starting  at 1) by the order of their opening parentheses
          in the RE, left to right.  Unused entries in  the  array--
          corresponding  either  to subexpressions that did not par-
          ticipate in the match at all, or to subexpressions that do
          not  exist  in  the  RE (that is, i > preg->re_nsub)--have
          both rm_so and rm_eo set to -1.  If a  subexpression  par-
          ticipated  in  the  match several times, the reported sub-
          string is the last one it matched.  (Note, as  an  example
          in particular, that when the RE `(b*)+' matches `bbb', the
          parenthesized subexpression matches  the  three  `b's  and
          then  an  infinite  number  of empty strings following the
          last `b', so the reported substring is  one  of  the  emp-
          ties.)
   
          If  REG_STARTEND  is  specified,  pmatch  must point to at
          least one regmatch_t (even if nmatch is 0 or REG_NOSUB was
          specified),  to  hold  the input offsets for REG_STARTEND.
          Use for output is still entirely controlled by nmatch;  if
          nmatch  is  0  or  REG_NOSUB  was  specified, the value of
          pmatch[0] will not be changed by a successful regexec.
   
          None of these functions references global variables except
          for  tables of constants; all are safe for use from multi-
          ple threads if the arguments are safe.
   
   IMPLEMENTATION CHOICES
          There are a number of decisions that 1003.2 leaves  up  to
          the implementor, either by explicitly saying ``undefined''
          or by virtue of them being forbidden by  the  RE  grammar.
          This implementation treats them as follows.
   
          See  regex(7)  for a discussion of the definition of case-
          independent matching.
   
          There is no particular limit on the length of REs,  except
          insofar  as  memory  is limited.  Memory usage is approxi-
          mately linear in RE size, and largely  insensitive  to  RE
          complexity,  except for bounded repetitions.  See BUGS for
          one short RE using them that will run  almost  any  system
          out of memory.
   
          A  backslashed character other than one specifically given
          a magic meaning by 1003.2 (such magic meanings occur  only
          in obsolete [``basic''] REs) is taken as an ordinary char-
          acter.
   
          Any unmatched [ is a REG_EBRACK error.
   
          Equivalence classes cannot begin or end bracket-expression
          ranges.  The endpoint of one range cannot begin another.
   
          RE_DUP_MAX, the limit on repetition counts in bounded rep-
          etitions, is 255.
   
          A repetition operator (?, *, +, or bounds)  cannot  follow
          another repetition operator.  A repetition operator cannot
          begin an expression or subexpression or follow `^' or `|'.
   
          `|'  cannot  appear  first or last in a (sub)expression or
          after another `|', i.e. an operand of  `|'  cannot  be  an
          empty  subexpression.   An  empty parenthesized subexpres-
          sion, `()', is legal and matches an empty (sub)string.  An
          empty string is not a legal RE.
   
          A  `{'  followed by a digit is considered the beginning of
          bounds for a bounded repetition, which  must  then  follow
          the  syntax  for bounds.  A `{' not followed by a digit is
          considered an ordinary character.
   
          `^' and `$' beginning and ending subexpressions  in  obso-
          lete (``basic'') REs are anchors, not ordinary characters.
   
   SEE ALSO
   
          POSIX 1003.2, sections 2.8 (Regular  Expression  Notation)
          and B.5 (C Binding for Regular Expression Matching).
   
   DIAGNOSTICS
          Non-zero  error codes from regcomp and regexec include the
          following:
   
          REG_NOMATCH    regexec() failed to match
          REG_BADPAT     invalid regular expression
          REG_ECOLLATE   invalid collating element
          REG_ECTYPE     invalid character class
          REG_EESCAPE    \ applied to unescapable character
          REG_ESUBREG    invalid backreference number
          REG_EBRACK     brackets [ ] not balanced
          REG_EPAREN     parentheses ( ) not balanced
          REG_EBRACE     braces { } not balanced
          REG_BADBR      invalid repetition count(s) in { }
          REG_ERANGE     invalid character range in [ ]
          REG_ESPACE     ran out of memory
          REG_BADRPT     ?, *, or + operand invalid
          REG_EMPTY      empty (sub)expression
          REG_ASSERT     ``can't happen''--you found a bug
          REG_INVARG     invalid argument, e.g. negative-length string
   
   HISTORY
          Written by Henry Spencer, henry@zoo.toronto.edu.
   
   BUGS
          This is an  alpha  release  with  known  defects.   Please
          report problems.
   
          There  is one known functionality bug.  The implementation
          of  internationalization  is  incomplete:  the  locale  is
          always  assumed  to be the default one of 1003.2, and only
          the collating elements etc. of that locale are  available.
   
          The  back-reference code is subtle and doubts linger about
          its correctness in complex cases.
   
          Regexec performance is poor.  This will improve with later
          releases.    Nmatch   exceeding  0  is  expensive;  nmatch
          exceeding 1 is worse.  Regexec is largely  insensitive  to
          RE  complexity  except  that back references are massively
          expensive.  RE length does matter; in particular, there is
          a  strong speed bonus for keeping RE length under about 30
          characters, with most special characters counting  roughly
          double.
   \end{verbatim}

 * RCS log of revisions to the C source code.
 *
 * \begin{verbatim}
 * $Log: regexec.c,v $
 * Revision 1.2  2005/02/22 22:01:25  swift
 * Eliminated GNU regex implementation.
 *
 * Revision 1.2  2003/07/02 17:27:20  swift
 * The name of the file 'regengine.c' was shortened to 'regengin.c' for
 * conformance with the ANSI standard and because the HiTech compiler used to
 * write embedded firmware has a limit of 8 characters for filenames (excluding
 * extension) in include statements.
 *
 * Revision 1.1  2002/10/08 23:54:23  swift
 * Initial revision
 * \end{verbatim}
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
#define RegexecChangeLog "$RCSfile: regexec.c,v $  $Revision: 1.2 $   $Date: 2005/02/22 22:01:25 $"

/*
 * the outer shell of regexec()
 *
 * This file includes regengine.c *twice*, after muchos fiddling with the
 * macros that code uses.  This lets the same code operate on two different
 * representations for state sets.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <ctype.h>
#include "regex.h"
#include "regutils.h"
#include "regex2.h"

#ifdef REDEBUG
static int nope = 0;		/* for use in asserts; shuts lint up */
#endif /* REDEBUG */

/* macros for manipulating states, small version */
#define	states	unsigned
#define	states1	unsigned	/* for later use in regexec() decision */
#define	CLEAR(v)	((v) = 0)
#define	SET0(v, n)	((v) &= ~((unsigned)1 << (n)))
#define	SET1(v, n)	((v) |= (unsigned)1 << (n))
#define	ISSET(v, n)	((v) & ((unsigned)1 << (n)))
#define	ASSIGN(d, s)	((d) = (s))
#define	EQ(a, b)	((a) == (b))
#define	STATEVARS	int dummy	/* dummy version */
#define	STATESETUP(m, n)	/* nothing */
#define	STATETEARDOWN(m)	/* nothing */
#define	SETUP(v)	((v) = 0)
#define	onestate	unsigned
#define	INIT(o, n)	((o) = (unsigned)1 << (n))
#define	INC(o)	((o) <<= 1)
#define	ISSTATEIN(v, o)	((v) & (o))
/* some abbreviations; note that some of these know variable names! */
/* do "if I'm here, I can also be there" etc without branches */
#define	FWD(dst, src, n)	((dst) |= ((unsigned)(src)&(here)) << (n))
#define	BACK(dst, src, n)	((dst) |= ((unsigned)(src)&(here)) >> (n))
#define	ISSETBACK(v, n)	((v) & ((unsigned)here >> (n)))
/* function names */
#define SNAMES			/* regengine.c looks after details */

#include "regengin.c"

/* now undo things */
#undef	states
#undef	CLEAR
#undef	SET0
#undef	SET1
#undef	ISSET
#undef	ASSIGN
#undef	EQ
#undef	STATEVARS
#undef	STATESETUP
#undef	STATETEARDOWN
#undef	SETUP
#undef	onestate
#undef	INIT
#undef	INC
#undef	ISSTATEIN
#undef	FWD
#undef	BACK
#undef	ISSETBACK
#undef	SNAMES

/* macros for manipulating states, large version */
#define	states	char *
#define	CLEAR(v)	memset(v, 0, m->g->nstates)
#define	SET0(v, n)	((v)[n] = 0)
#define	SET1(v, n)	((v)[n] = 1)
#define	ISSET(v, n)	((v)[n])
#define	ASSIGN(d, s)	memcpy(d, s, m->g->nstates)
#define	EQ(a, b)	(memcmp(a, b, m->g->nstates) == 0)
#define	STATEVARS	int vn; char *space
#define	STATESETUP(m, nv)	{ (m)->space = malloc((nv)*(m)->g->nstates); \
				if ((m)->space == NULL) return(REG_ESPACE); \
				(m)->vn = 0; }
#define	STATETEARDOWN(m)	{ free((m)->space); }
#define	SETUP(v)	((v) = &m->space[m->vn++ * m->g->nstates])
#define	onestate	int
#define	INIT(o, n)	((o) = (n))
#define	INC(o)	((o)++)
#define	ISSTATEIN(v, o)	((v)[o])
/* some abbreviations; note that some of these know variable names! */
/* do "if I'm here, I can also be there" etc without branches */
#define	FWD(dst, src, n)	((dst)[here+(n)] |= (src)[here])
#define	BACK(dst, src, n)	((dst)[here-(n)] |= (src)[here])
#define	ISSETBACK(v, n)	((v)[here - (n)])
/* function names */
#define	LNAMES			/* flag */

#include "regengin.c"

/*
 - regexec - interface for matching
 = extern int regexec(const regex_t *, const char *, size_t, \
 =					regmatch_t [], int);
 = #define	REG_NOTBOL	00001
 = #define	REG_NOTEOL	00002
 = #define	REG_STARTEND	00004
 = #define	REG_TRACE	00400	// tracing of execution
 = #define	REG_LARGE	01000	// force large representation
 = #define	REG_BACKR	02000	// force use of backref code
 *
 * We put this here so we can exploit knowledge of the state representation
 * when choosing which matcher to call.  Also, by this point the matchers
 * have been prototyped.
 */
int				/* 0 success, REG_NOMATCH failure */
regexec(preg, string, nmatch, pmatch, eflags)
const regex_t *preg;
const char *string;
size_t nmatch;
regmatch_t pmatch[];
int eflags;
{
	register struct re_guts *g = preg->re_g;
#ifdef REDEBUG
#	define	GOODFLAGS(f)	(f)
#else
#	define	GOODFLAGS(f)	((f)&(REG_NOTBOL|REG_NOTEOL|REG_STARTEND))
#endif

	if (preg->re_magic != MAGIC1 || g->magic != MAGIC2)
		return(REG_BADPAT);
	assert(!(g->iflags&BAD));
	if (g->iflags&BAD)		/* backstop for no-debug case */
		return(REG_BADPAT);
	eflags = GOODFLAGS(eflags);

	if (g->nstates <= (sopno)(CHAR_BIT*sizeof(states1)) && !(eflags&REG_LARGE))
		return(smatcher(g, (char *)string, nmatch, pmatch, eflags));
	else
		return(lmatcher(g, (char *)string, nmatch, pmatch, eflags));
}
