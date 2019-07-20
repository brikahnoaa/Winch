#ifndef REGCOMP_H
#define REGCOMP_H

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * $Id: regcomp.c,v 1.3 2003/07/07 14:45:15 swift Exp $
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
      
   No significant modifications have been made to the functionality of
   regcomp().  The nature of the modifications are mostly repackaging of the
   source code.  However, the type of two variables (re_guts.magic,
   re_magic) were changed from 'int' to 'long int'.  The reason was that the
   values of the defined quantities MAGIC1 and MAGIC2 each exceed the
   capacity of a signed 16-bit integer.  The following documentation was cut
   from the man (3) page that accompanied Spencer's distribution.

   \begin{verbatim}
   REGEX(3)                                                 REGEX(3)
   
   NAME
          regcomp,  regexec,  regerror, regfree - regular-expression
          library
   
   SYNOPSIS
          #include <regex.h>
   
          int regcomp(regex_t *preg, const char *pattern, int cflags);
   
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
   
          Regcomp compiles the regular expression contained  in  the
          pattern string, subject to the flags in cflags, and places
          the results in the regex_t structure pointed to  by  preg.
          Cflags  is the bitwise OR of zero or more of the following
          flags:
   
          REG_EXTENDED  Compile modern  (``extended'')  REs,  rather
                        than  the  obsolete (``basic'') REs that are
                        the default.
   
          REG_BASIC     This is a synonym for 0, provided as a coun-
                        terpart to REG_EXTENDED to improve readabil-
                        ity.  This is an extension, compatible  with
                        but  not  specified  by  POSIX  1003.2,  and
                        should be  used  with  caution  in  software
                        intended to be portable to other systems.
   
          REG_NOSPEC    Compile  with  recognition  of  all  special
                        characters turned off.  All  characters  are
                        thus considered ordinary, so the ``RE'' is a
                        literal string.  This is an extension,  com-
                        patible  with  but  not  specified  by POSIX
                        1003.2, and should be used with  caution  in
                        software  intended  to  be portable to other
                        systems.  REG_EXTENDED  and  REG_NOSPEC  may
                        not be used in the same call to regcomp.
   
          REG_ICASE     Compile    for    matching    that   ignores
                        upper/lower    case    distinctions.     See
                        regex(7).
   
          REG_NOSUB     Compile  for  matching that need only report
                        success or failure, not what was matched.
   
          REG_NEWLINE   Compile for newline-sensitive matching.   By
                        default,  newline  is  a completely ordinary
                        character with no special meaning in  either
                        REs   or  strings.   With  this  flag,  `[^'
                        bracket expressions and `.' never match new-
                        line,  a  `^' anchor matches the null string
                        after any newline in the string in  addition
                        to  its  normal function, and the `$' anchor
                        matches the null string before  any  newline
                        in  the  string  in  addition  to its normal
                        function.
   
          REG_PEND      The regular  expression  ends,  not  at  the
                        first  NUL,  but  just  before the character
                        pointed to by  the  re_endp  member  of  the
                        structure  pointed  to by preg.  The re_endp
                        member is of type const char *.   This  flag
                        permits  inclusion  of  NULs in the RE; they
                        are considered ordinary characters.  This is
                        an extension, compatible with but not speci-
                        fied by POSIX 1003.2,  and  should  be  used
                        with  caution  in  software  intended  to be
                        portable to other systems.
   
          When successful, regcomp returns 0 and fills in the struc-
          ture  pointed  to  by  preg.  One member of that structure
          (other than  re_endp)  is  publicized:  re_nsub,  of  type
          size_t,  contains  the  number of parenthesized subexpres-
          sions within the RE (except that the value of this  member
          is  undefined if the REG_NOSUB flag was used).  If regcomp
          fails, it returns a non-zero error code; see  DIAGNOSTICS.
   
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
          grep(1), regex(7)
   
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
   
          Regcomp implements bounded repetitions by macro expansion,
          which is costly in time and space if counts are  large  or
          bounded   repetitions   are  nested.   An  RE  like,  say,
          `((((a{1,100}){1,100}){1,100}){1,100}){1,100}' will (even-
          tually) run almost any existing machine out of swap space.
   
          There are suspected  problems  with  response  to  obscure
          error  conditions.   Notably,  certain  kinds  of internal
          overflow, produced only by truly enormous REs or by multi-
          ply  nested  bounded repetitions, are probably not handled
          well.
   
          Due to a mistake in 1003.2, things like  `a)b'  are  legal
          REs  because  `)' is a special character only in the pres-
          ence of a previous unmatched `('.   This  can't  be  fixed
          until the spec is fixed.
   
          The  standard's  definition  of  back references is vague.
          For example, does `a\(\(b\)*\2\)*d' match `abbbd'?   Until
          the  standard  is clarified, behavior in such cases should
          not be relied on.
   
          The implementation of word-boundary matching is a bit of a
          kludge, and bugs may lurk in combinations of word-boundary
          matching and anchoring.
   \end{verbatim}
   
 * RCS log of revisions to the C source code.
 *
 * \begin{verbatim}
 * $Log: regcomp.c,v $
 * Revision 1.3  2003/07/07 14:45:15  swift
 * Added watchdog resets and stack-checks for the APF9 firmware work.
 *
 * Revision 1.2  2003/07/02 17:17:58  swift
 * The type of two variables (re_guts.magic, re_magic) were changed from 'int'
 * to 'long int' because the values of the defined quantities MAGIC1 and MAGIC2
 * each exceed the capacity of a signed 16-bit integer.  The source code for
 * three functions (mcin, mcsub,mcfind) were eliminated because they were
 * defined with static linkage and they were not used anywere in this
 * compilation unit.
 *
 * Revision 1.1  2002/10/08 23:53:12  swift
 * Initial revision
 * \end{verbatim}
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
#define RegcompChangeLog "$RCSfile: regcomp.c,v $  $Revision: 1.3 $   $Date: 2003/07/07 14:45:15 $"

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>
#include <stdlib.h>
#include "regex.h"
#include "regutils.h"
#include "regex2.h"

#ifdef _XA_
   #include <apf9.h>
#else /* _XA_ */
   #define WatchDog()
   #define StackOk() 1
#endif /* _XA_ */

/* define POSIX_MISTAKE for strictly POSIX (unintended) behavior */
#define POSIX_MISTAKE 1

/*
 * parse structure, passed up and down to avoid global variables and
 * other clumsinesses
 */
struct parse {
   char *next;           /* next character in RE */
   char *end;            /* end of string (-> NUL normally) */
   int error;            /* has an error been seen? */
   sop *strip;           /* malloced strip */
   sopno ssize;          /* malloced strip size (allocated) */
   sopno slen;           /* malloced strip length (used) */
   int ncsalloc;         /* number of csets allocated */
   struct re_guts *g;
#  define   NPAREN   10  /* we need to remember () 1-9 for back refs */
   sopno pbegin[NPAREN]; /* -> ( ([0] unused) */
   sopno pend[NPAREN];   /* -> ) ([0] unused) */
};

#ifdef __cplusplus
extern "C" {
#endif

static void p_ere(register struct parse *p, int stop);
static void p_ere_exp(register struct parse *p);
static void p_str(register struct parse *p);
static void p_bre(register struct parse *p, register int end1, register int end2);
static int p_simp_re(register struct parse *p, int starordinary);
static int p_count(register struct parse *p);
static void p_bracket(register struct parse *p);
static void p_b_term(register struct parse *p, register cset *cs);
static void p_b_cclass(register struct parse *p, register cset *cs);
static void p_b_eclass(register struct parse *p, register cset *cs);
static char p_b_symbol(register struct parse *p);
static char p_b_coll_elem(register struct parse *p, int endc);
static char othercase(int ch);
static void bothcases(register struct parse *p, int ch);
static void ordinary(register struct parse *p, register int ch);
static void nonnewline(register struct parse *p);
static void repeat(register struct parse *p, sopno start, int from, int to);
static int seterr(register struct parse *p, int e);
static cset *allocset(register struct parse *p);
static void freeset(register struct parse *p, register cset *cs);
static int freezeset(register struct parse *p, register cset *cs);
static int firstch(register struct parse *p, register cset *cs);
static int nch(register struct parse *p, register cset *cs);
static void mcadd(register struct parse *p, register cset *cs, register char *cp);
static void mcinvert(register struct parse *p, register cset *cs);
static void mccase(register struct parse *p, register cset *cs);
static int isinsets(register struct re_guts *g, int c);
static int samesets(register struct re_guts *g, int c1, int c2);
static void categorize(struct parse *p, register struct re_guts *g);
static sopno dupl(register struct parse *p, sopno start, sopno finish);
static void doemit(register struct parse *p, sop op, size_t opnd);
static void doinsert(register struct parse *p, sop op, size_t opnd, sopno pos);
static void dofwd(register struct parse *p, sopno pos, sop value);
static void enlarge(register struct parse *p, sopno size);
static void stripsnug(register struct parse *p, register struct re_guts *g);
static void findmust(register struct parse *p, register struct re_guts *g);
static sopno pluscount(register struct parse *p, register struct re_guts *g);

#ifdef __cplusplus
}
#endif

#endif /* REGCOMP_H */
