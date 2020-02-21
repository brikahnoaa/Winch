#ifndef REGCOMP_H
#define REGCOMP_H

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * $Id: regcomp.c,v 1.2 2005/02/22 22:01:25 swift Exp $
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
 * Revision 1.2  2005/02/22 22:01:25  swift
 * Eliminated GNU regex implementation.
 *
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
#define RegcompChangeLog "$RCSfile: regcomp.c,v $  $Revision: 1.2 $   $Date: 2005/02/22 22:01:25 $"

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

#ifndef REGCCLASS_H
#define REGCCLASS_H

/* character-class table */
static struct cclass {
   char *name;
   char *chars;
   char *multis;
} cclasses[] = {
   {"alnum", "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789", ""},
   {"alpha", "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz", ""},
   {"blank", " \t",      ""},
   {"cntrl", "\007\b\t\n\v\f\r\1\2\3\4\5\6\16\17\20\21\22\23\24\25\26\27\30\31\32\33\34\35\36\37\177", ""},
   {"digit", "0123456789",  ""},
   {"graph", "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789!\"#$%&'()*+,-./:;<=>?@[\\]^_`{|}~", ""},
   {"lower", "abcdefghijklmnopqrstuvwxyz", ""},
   {"print", "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789!\"#$%&'()*+,-./:;<=>?@[\\]^_`{|}~ ", ""},
   {"punct", "!\"#$%&'()*+,-./:;<=>?@[\\]^_`{|}~", ""},
   {"space", "\t\n\v\f\r ", ""},
   {"upper", "ABCDEFGHIJKLMNOPQRSTUVWXYZ", ""},
   {"xdigit",   "0123456789ABCDEFabcdef", ""},
   {NULL, 0, ""}
};

#endif /* REGCCLASS_H */

#ifndef REGCNAME_H
#define REGCNAME_H

/* character-name table */
static struct cname {
   char *name;
   char icode;
} cnames[] = {
   {"NUL",   '\0'},
   {"SOH",   '\001'},
   {"STX",   '\002'},
   {"ETX",   '\003'},
   {"EOT",   '\004'},
   {"ENQ",   '\005'},
   {"ACK",   '\006'},
   {"BEL",   '\007'},
   {"alert", '\007'},
   {"BS",    '\010'},
   {"backspace",   '\b'},
   {"HT",    '\011'},
   {"tab",      '\t'},
   {"LF",    '\012'},
   {"newline",  '\n'},
   {"VT",    '\013'},
   {"vertical-tab",   '\v'},
   {"FF",    '\014'},
   {"form-feed",   '\f'},
   {"CR",    '\015'},
   {"carriage-return",   '\r'},
   {"SO", '\016'},
   {"SI", '\017'},
   {"DLE",   '\020'},
   {"DC1",   '\021'},
   {"DC2",   '\022'},
   {"DC3",   '\023'},
   {"DC4",   '\024'},
   {"NAK",   '\025'},
   {"SYN",   '\026'},
   {"ETB",   '\027'},
   {"CAN",   '\030'},
   {"EM", '\031'},
   {"SUB",   '\032'},
   {"ESC",   '\033'},
   {"IS4",   '\034'},
   {"FS", '\034'},
   {"IS3",   '\035'},
   {"GS", '\035'},
   {"IS2",   '\036'},
   {"RS", '\036'},
   {"IS1",   '\037'},
   {"US", '\037'},
   {"space",    ' '},
   {"exclamation-mark",  '!'},
   {"quotation-mark", '"'},
   {"number-sign",    '#'},
   {"dollar-sign",    '$'},
   {"percent-sign",      '%'},
   {"ampersand",      '&'},
   {"apostrophe",     '\''},
   {"left-parenthesis",  '('},
   {"right-parenthesis", ')'},
   {"asterisk", '*'},
   {"plus-sign",   '+'},
   {"comma", ','},
   {"hyphen",   '-'},
   {"hyphen-minus",   '-'},
   {"period",   '.'},
   {"full-stop",   '.'},
   {"slash", '/'},
   {"solidus",  '/'},
   {"zero",     '0'},
   {"one",      '1'},
   {"two",      '2'},
   {"three", '3'},
   {"four",     '4'},
   {"five",     '5'},
   {"six",      '6'},
   {"seven", '7'},
   {"eight", '8'},
   {"nine",     '9'},
   {"colon", ':'},
   {"semicolon",   ';'},
   {"less-than-sign", '<'},
   {"equals-sign",    '='},
   {"greater-than-sign", '>'},
   {"question-mark",  '?'},
   {"commercial-at",  '@'},
   {"left-square-bracket",  '['},
   {"backslash",      '\\'},
   {"reverse-solidus",   '\\'},
   {"right-square-bracket", ']'},
   {"circumflex",     '^'},
   {"circumflex-accent", '^'},
   {"underscore",     '_'},
   {"low-line",    '_'},
   {"grave-accent",      '`'},
   {"left-brace",     '{'},
   {"left-curly-bracket",   '{'},
   {"vertical-line",  '|'},
   {"right-brace",    '}'},
   {"right-curly-bracket",  '}'},
   {"tilde",    '~'},
   {"DEL",   '\177'},
   {NULL, 0}
};

#endif /* REGCNAME_H */

static char nuls[10];      /* place to point scanner in event of error */

/*
 * macros for use with parse structure
 * BEWARE:  these know that the parse structure is named `p' !!!
 */
#define  PEEK()           (*(p->next))
#define  PEEK2()          (*(p->next+1))
#define  MORE()           (p->next < p->end)
#define  MORE2()          (p->next+1 < p->end)
#define  SEE(c)           (MORE() && PEEK() == (c))
#define  SEETWO(a, b)     (MORE() && MORE2() && PEEK() == (a) && PEEK2() == (b))
#define  EAT(c)           ((SEE(c)) ? (NEXT(), 1) : 0)
#define  EATTWO(a, b)     ((SEETWO(a, b)) ? (NEXT2(), 1) : 0)
#define  NEXT()           (p->next++)
#define  NEXT2()          (p->next += 2)
#define  NEXTn(n)         (p->next += (n))
#define  GETNEXT()        (*p->next++)
#define  SETERROR(e)      seterr(p, (e))
#define  REQUIRE(co, e)   ((co) || SETERROR(e))
#define  MUSTSEE(c, e)    (REQUIRE(MORE() && PEEK() == (c), e))
#define  MUSTEAT(c, e)    (REQUIRE(MORE() && GETNEXT() == (c), e))
#define  MUSTNOTSEE(c, e) (REQUIRE(!MORE() || PEEK() != (c), e))
#define  EMIT(op, sopnd)  doemit(p, (sop)(op), (size_t)(sopnd))
#define  INSERT(op, pos)  doinsert(p, (sop)(op), HERE()-(pos)+1, pos)
#define  AHEAD(pos)       dofwd(p, pos, HERE()-(pos))
#define  ASTERN(sop, pos) EMIT(sop, HERE()-pos)
#define  HERE()           (p->slen)
#define  THERE()          (p->slen - 1)
#define  THERETHERE()     (p->slen - 2)
#define  DROP(n)          (p->slen -= (n))

#ifndef NDEBUG
static int never = 0;      /* for use in asserts; shuts lint up */
#else
#define  never 0     /* some <assert.h>s have bugs too */
#endif

/*
 - regcomp - interface for parser and compilation
 = extern int regcomp(regex_t *, const char *, int);
 = #define  REG_BASIC   0000
 = #define  REG_EXTENDED   0001
 = #define  REG_ICASE   0002
 = #define  REG_NOSUB   0004
 = #define  REG_NEWLINE 0010
 = #define  REG_NOSPEC  0020
 = #define  REG_PEND 0040
 = #define  REG_DUMP 0200
 */
int            /* 0 success, otherwise REG_something */
regcomp(preg, pattern, cflags)
regex_t *preg;
const char *pattern;
int cflags;
{
   struct parse pa;
   register struct re_guts *g;
   register struct parse *p = &pa;
   register int i;
   register size_t len;
#ifdef REDEBUG
#  define   GOODFLAGS(f)   (f)
#else
#  define   GOODFLAGS(f)   ((f)&~REG_DUMP)
#endif

   /* pet the watchdog */
   WatchDog();
   
   /* stack-check assertion */
   assert(StackOk());

   cflags = GOODFLAGS(cflags);
   if ((cflags&REG_EXTENDED) && (cflags&REG_NOSPEC))
      return(REG_INVARG);

   if (cflags&REG_PEND) {
      if (preg->re_endp < pattern)
         return(REG_INVARG);
      len = preg->re_endp - pattern;
   } else
      len = strlen((char *)pattern);
   
   /* do the mallocs early so failure handling is easy */
   g = (struct re_guts *)malloc(sizeof(struct re_guts) + (NC-1)*sizeof(cat_t));
   if (g == NULL) return(REG_ESPACE);
   
   p->ssize = len/(size_t)2*(size_t)3 + (size_t)1; /* ugh */
   p->strip = (sop *)malloc(p->ssize * sizeof(sop));
   p->slen = 0;
   if (p->strip == NULL) {
      free((char *)g);
      return(REG_ESPACE);
   }

   /* set things up */
   p->g = g;
   p->next = (char *)pattern; /* convenience; we do not modify it */
   p->end = p->next + len;
   p->error = 0;
   p->ncsalloc = 0;
   for (i = 0; i < NPAREN; i++) {
      p->pbegin[i] = 0;
      p->pend[i] = 0;
   }
   g->csetsize = NC;
   g->sets = NULL;
   g->setbits = NULL;
   g->ncsets = 0;
   g->cflags = cflags;
   g->iflags = 0;
   g->nbol = 0;
   g->neol = 0;
   g->must = NULL;
   g->mlen = 0;
   g->nsub = 0;
   g->ncategories = 1;  /* category 0 is "everything else" */
   g->categories = &g->catspace[-(CHAR_MIN)];
   (void) memset((char *)g->catspace, 0, NC*sizeof(cat_t));
   g->backrefs = 0;

   /* do it */
   EMIT(OEND, 0);
   g->firststate = THERE();
   if (cflags&REG_EXTENDED)
      p_ere(p, OUT);
   else if (cflags&REG_NOSPEC)
      p_str(p);
   else
      p_bre(p, OUT, OUT);
   EMIT(OEND, 0);
   g->laststate = THERE();

   /* tidy up loose ends and fill things in */
   categorize(p, g);
   stripsnug(p, g);
   findmust(p, g);
   g->nplus = pluscount(p, g);
   g->magic = MAGIC2;
   preg->re_nsub = g->nsub;
   preg->re_g = g;
   preg->re_magic = MAGIC1;
#ifndef REDEBUG
   /* not debugging, so can't rely on the assert() in regexec() */
   if (g->iflags&BAD)
      SETERROR(REG_ASSERT);
#endif

   /* win or lose, we're done */
   if (p->error != 0)   /* lose */
      regfree(preg);
   return(p->error);
}

/*
 - p_ere - ERE parser top level, concatenation and alternation
 == static void p_ere(register struct parse *p, int stop);
 */
static void
p_ere(p, stop)
register struct parse *p;
int stop;         /* character this ERE should end at */
{
   register char c;
   register sopno prevback=0;
   register sopno prevfwd=0;
   register sopno conc;
   register int first = 1;    /* is this the first alternative? */

   /* pet the watchdog */
   WatchDog();
   
   /* stack-check assertion */
   assert(StackOk());

   for (;;) {
      /* do a bunch of concatenated expressions */
      conc = HERE();
      while (MORE() && (c = PEEK()) != '|' && c != stop)
         p_ere_exp(p);
      REQUIRE(HERE() != conc, REG_EMPTY); /* require nonempty */

      if (!EAT('|'))
         break;      /* NOTE BREAK OUT */

      if (first) {
         INSERT(OCH_, conc);  /* offset is wrong */
         prevfwd = conc;
         prevback = conc;
         first = 0;
      }
      ASTERN(OOR1, prevback);
      prevback = THERE();
      AHEAD(prevfwd);         /* fix previous offset */
      prevfwd = HERE();
      EMIT(OOR2, 0);       /* offset is very wrong */
   }

   if (!first) {     /* tail-end fixups */
      AHEAD(prevfwd);
      ASTERN(O_CH, prevback);
   }

   assert(!MORE() || SEE(stop));
}

/*
 - p_ere_exp - parse one subERE, an atom possibly followed by a repetition op
 == static void p_ere_exp(register struct parse *p);
 */
static void
p_ere_exp(p)
register struct parse *p;
{
   register char c;
   register sopno pos;
   register int count;
   register int count2;
   register sopno subno;
   int wascaret = 0;

   /* pet the watchdog */
   WatchDog();
   
   /* stack-check assertion */
   assert(StackOk());

   assert(MORE());      /* caller should have ensured this */
   c = GETNEXT();

   pos = HERE();
   switch (c) {
   case '(':
      REQUIRE(MORE(), REG_EPAREN);
      p->g->nsub++;
      subno = p->g->nsub;
      if (subno < NPAREN)
         p->pbegin[subno] = HERE();
      EMIT(OLPAREN, subno);
      if (!SEE(')'))
         p_ere(p, ')');
      if (subno < NPAREN) {
         p->pend[subno] = HERE();
         assert(p->pend[subno] != 0);
      }
      EMIT(ORPAREN, subno);
      MUSTEAT(')', REG_EPAREN);
      break;
#ifndef POSIX_MISTAKE
   case ')':      /* happens only if no current unmatched ( */
      /*
       * You may ask, why the ifndef?  Because I didn't notice
       * this until slightly too late for 1003.2, and none of the
       * other 1003.2 regular-expression reviewers noticed it at
       * all.  So an unmatched ) is legal POSIX, at least until
       * we can get it fixed.
       */
      SETERROR(REG_EPAREN);
      break;
#endif
   case '^':
      EMIT(OBOL, 0);
      p->g->iflags |= USEBOL;
      p->g->nbol++;
      wascaret = 1;
      break;
   case '$':
      EMIT(OEOL, 0);
      p->g->iflags |= USEEOL;
      p->g->neol++;
      break;
   case '|':
      SETERROR(REG_EMPTY);
      break;
   case '*':
   case '+':
   case '?':
      SETERROR(REG_BADRPT);
      break;
   case '.':
      if (p->g->cflags&REG_NEWLINE)
         nonnewline(p);
      else
         EMIT(OANY, 0);
      break;
   case '[':
      p_bracket(p);
      break;
   case '\\':
      REQUIRE(MORE(), REG_EESCAPE);
      c = GETNEXT();
      ordinary(p, c);
      break;
   case '{':      /* okay as ordinary except if digit follows */
      REQUIRE(!MORE() || !isdigit(PEEK()), REG_BADRPT);
      /* FALLTHROUGH */
   default:
      ordinary(p, c);
      break;
   }

   if (!MORE())
      return;
   c = PEEK();
   /* we call { a repetition if followed by a digit */
   if (!( c == '*' || c == '+' || c == '?' ||
            (c == '{' && MORE2() && isdigit(PEEK2())) ))
      return;     /* no repetition, we're done */
   NEXT();

   REQUIRE(!wascaret, REG_BADRPT);
   switch (c) {
   case '*':   /* implemented as +? */
      /* this case does not require the (y|) trick, noKLUDGE */
      INSERT(OPLUS_, pos);
      ASTERN(O_PLUS, pos);
      INSERT(OQUEST_, pos);
      ASTERN(O_QUEST, pos);
      break;
   case '+':
      INSERT(OPLUS_, pos);
      ASTERN(O_PLUS, pos);
      break;
   case '?':
      /* KLUDGE: emit y? as (y|) until subtle bug gets fixed */
      INSERT(OCH_, pos);      /* offset slightly wrong */
      ASTERN(OOR1, pos);      /* this one's right */
      AHEAD(pos);       /* fix the OCH_ */
      EMIT(OOR2, 0);       /* offset very wrong... */
      AHEAD(THERE());         /* ...so fix it */
      ASTERN(O_CH, THERETHERE());
      break;
   case '{':
      count = p_count(p);
      if (EAT(',')) {
         if (isdigit(PEEK())) {
            count2 = p_count(p);
            REQUIRE(count <= count2, REG_BADBR);
         } else      /* single number with comma */
            count2 = INFINITY;
      } else      /* just a single number */
         count2 = count;
      repeat(p, pos, count, count2);
      if (!EAT('}')) {  /* error heuristics */
         while (MORE() && PEEK() != '}')
            NEXT();
         REQUIRE(MORE(), REG_EBRACE);
         SETERROR(REG_BADBR);
      }
      break;
   }

   if (!MORE())
      return;
   c = PEEK();
   if (!( c == '*' || c == '+' || c == '?' ||
            (c == '{' && MORE2() && isdigit(PEEK2())) ) )
      return;
   SETERROR(REG_BADRPT);
}

/*
 - p_str - string (no metacharacters) "parser"
 == static void p_str(register struct parse *p);
 */
static void
p_str(p)
register struct parse *p;
{
   REQUIRE(MORE(), REG_EMPTY);
   while (MORE())
      ordinary(p, GETNEXT());
}

/*
 - p_bre - BRE parser top level, anchoring and concatenation
 == static void p_bre(register struct parse *p, register int end1, \
 ==   register int end2);
 * Giving end1 as OUT essentially eliminates the end1/end2 check.
 *
 * This implementation is a bit of a kludge, in that a trailing $ is first
 * taken as an ordinary character and then revised to be an anchor.  The
 * only undesirable side effect is that '$' gets included as a character
 * category in such cases.  This is fairly harmless; not worth fixing.
 * The amount of lookahead needed to avoid this kludge is excessive.
 */
static void
p_bre(p, end1, end2)
register struct parse *p;
register int end1;      /* first terminating character */
register int end2;      /* second terminating character */
{
   register sopno start = HERE();
   register int first = 1;       /* first subexpression? */
   register int wasdollar = 0;

   if (EAT('^')) {
      EMIT(OBOL, 0);
      p->g->iflags |= USEBOL;
      p->g->nbol++;
   }
   while (MORE() && !SEETWO(end1, end2)) {
      wasdollar = p_simp_re(p, first);
      first = 0;
   }
   if (wasdollar) {  /* oops, that was a trailing anchor */
      DROP(1);
      EMIT(OEOL, 0);
      p->g->iflags |= USEEOL;
      p->g->neol++;
   }

   REQUIRE(HERE() != start, REG_EMPTY);   /* require nonempty */
}

/*
 - p_simp_re - parse a simple RE, an atom possibly followed by a repetition
 == static int p_simp_re(register struct parse *p, int starordinary);
 */
static int        /* was the simple RE an unbackslashed $? */
p_simp_re(p, starordinary)
register struct parse *p;
int starordinary;    /* is a leading * an ordinary character? */
{
   register int c;
   register int count;
   register int count2;
   register sopno pos;
   register int i;
   register sopno subno;
#  define   BACKSL   (1<<CHAR_BIT)

   /* pet the watchdog */
   WatchDog();
   
   /* stack-check assertion */
   assert(StackOk());

   pos = HERE();     /* repetion op, if any, covers from here */

   assert(MORE());      /* caller should have ensured this */
   c = GETNEXT();
   if (c == '\\') {
      REQUIRE(MORE(), REG_EESCAPE);
      c = BACKSL | (unsigned char)GETNEXT();
   }
   switch (c) {
   case '.':
      if (p->g->cflags&REG_NEWLINE)
         nonnewline(p);
      else
         EMIT(OANY, 0);
      break;
   case '[':
      p_bracket(p);
      break;
   case BACKSL|'{':
      SETERROR(REG_BADRPT);
      break;
   case BACKSL|'(':
      p->g->nsub++;
      subno = p->g->nsub;
      if (subno < NPAREN)
         p->pbegin[subno] = HERE();
      EMIT(OLPAREN, subno);
      /* the MORE here is an error heuristic */
      if (MORE() && !SEETWO('\\', ')'))
         p_bre(p, '\\', ')');
      if (subno < NPAREN) {
         p->pend[subno] = HERE();
         assert(p->pend[subno] != 0);
      }
      EMIT(ORPAREN, subno);
      REQUIRE(EATTWO('\\', ')'), REG_EPAREN);
      break;
   case BACKSL|')':  /* should not get here -- must be user */
   case BACKSL|'}':
      SETERROR(REG_EPAREN);
      break;
   case BACKSL|'1':
   case BACKSL|'2':
   case BACKSL|'3':
   case BACKSL|'4':
   case BACKSL|'5':
   case BACKSL|'6':
   case BACKSL|'7':
   case BACKSL|'8':
   case BACKSL|'9':
      i = (c&~BACKSL) - '0';
      assert(i < NPAREN);
      if (p->pend[i] != 0) {
         assert(i <= p->g->nsub);
         EMIT(OBACK_, i);
         assert(p->pbegin[i] != 0);
         assert(OP(p->strip[p->pbegin[i]]) == OLPAREN);
         assert(OP(p->strip[p->pend[i]]) == ORPAREN);
         (void) dupl(p, p->pbegin[i]+1, p->pend[i]);
         EMIT(O_BACK, i);
      } else
         SETERROR(REG_ESUBREG);
      p->g->backrefs = 1;
      break;
   case '*':
      REQUIRE(starordinary, REG_BADRPT);
      /* FALLTHROUGH */
   default:
      ordinary(p, (char)c);   /* takes off BACKSL, if any */
      break;
   }

   if (EAT('*')) {      /* implemented as +? */
      /* this case does not require the (y|) trick, noKLUDGE */
      INSERT(OPLUS_, pos);
      ASTERN(O_PLUS, pos);
      INSERT(OQUEST_, pos);
      ASTERN(O_QUEST, pos);
   } else if (EATTWO('\\', '{')) {
      count = p_count(p);
      if (EAT(',')) {
         if (MORE() && isdigit(PEEK())) {
            count2 = p_count(p);
            REQUIRE(count <= count2, REG_BADBR);
         } else      /* single number with comma */
            count2 = INFINITY;
      } else      /* just a single number */
         count2 = count;
      repeat(p, pos, count, count2);
      if (!EATTWO('\\', '}')) {  /* error heuristics */
         while (MORE() && !SEETWO('\\', '}'))
            NEXT();
         REQUIRE(MORE(), REG_EBRACE);
         SETERROR(REG_BADBR);
      }
   } else if (c == (unsigned char)'$') /* $ (but not \$) ends it */
      return(1);

   return(0);
}

/*
 - p_count - parse a repetition count
 == static int p_count(register struct parse *p);
 */
static int        /* the value */
p_count(p)
register struct parse *p;
{
   register int count = 0;
   register int ndigits = 0;

   while (MORE() && isdigit(PEEK()) && count <= DUPMAX) {
      count = count*10 + (GETNEXT() - '0');
      ndigits++;
   }

   REQUIRE(ndigits > 0 && count <= DUPMAX, REG_BADBR);
   return(count);
}

/*
 - p_bracket - parse a bracketed character list
 == static void p_bracket(register struct parse *p);
 *
 * Note a significant property of this code:  if the allocset() did SETERROR,
 * no set operations are done.
 */
static void
p_bracket(p)
register struct parse *p;
{
   register cset *cs = allocset(p);
   register int invert = 0;

   /* pet the watchdog */
   WatchDog();
   
   /* stack-check assertion */
   assert(StackOk());

   /* Dept of Truly Sickening Special-Case Kludges */
   if (p->next + 5 < p->end && strncmp(p->next, "[:<:]]", 6) == 0) {
      EMIT(OBOW, 0);
      NEXTn(6);
      return;
   }
   if (p->next + 5 < p->end && strncmp(p->next, "[:>:]]", 6) == 0) {
      EMIT(OEOW, 0);
      NEXTn(6);
      return;
   }

   if (EAT('^'))
      invert++;   /* make note to invert set at end */
   if (EAT(']'))
      CHadd(cs, ']');
   else if (EAT('-'))
      CHadd(cs, '-');
   while (MORE() && PEEK() != ']' && !SEETWO('-', ']'))
      p_b_term(p, cs);
   if (EAT('-'))
      CHadd(cs, '-');
   MUSTEAT(']', REG_EBRACK);

   if (p->error != 0)   /* don't mess things up further */
      return;

   if (p->g->cflags&REG_ICASE) {
      register int i;
      register int ci;

      for (i = p->g->csetsize - 1; i >= 0; i--)
         if (CHIN(cs, i) && isalpha(i)) {
            ci = othercase(i);
            if (ci != i)
               CHadd(cs, ci);
         }
      if (cs->multis != NULL)
         mccase(p, cs);
   }
   if (invert) {
      register int i;

      for (i = p->g->csetsize - 1; i >= 0; i--)
         if (CHIN(cs, i))
            CHsub(cs, i);
         else
            CHadd(cs, i);
      if (p->g->cflags&REG_NEWLINE)
         CHsub(cs, '\n');
      if (cs->multis != NULL)
         mcinvert(p, cs);
   }

   assert(cs->multis == NULL);      /* xxx */

   if (nch(p, cs) == 1) {     /* optimize singleton sets */
      ordinary(p, firstch(p, cs));
      freeset(p, cs);
   } else
      EMIT(OANYOF, freezeset(p, cs));
}

/*
 - p_b_term - parse one term of a bracketed character list
 == static void p_b_term(register struct parse *p, register cset *cs);
 */
static void
p_b_term(p, cs)
register struct parse *p;
register cset *cs;
{
   register char c;
   register char start, finish;
   register int i;

   /* classify what we've got */
   switch ((MORE()) ? PEEK() : '\0') {
   case '[':
      c = (MORE2()) ? PEEK2() : '\0';
      break;
   case '-':
      SETERROR(REG_ERANGE);
      return;        /* NOTE RETURN */
   default:
      c = '\0';
      break;
   }

   switch (c) {
   case ':':      /* character class */
      NEXT2();
      REQUIRE(MORE(), REG_EBRACK);
      c = PEEK();
      REQUIRE(c != '-' && c != ']', REG_ECTYPE);
      p_b_cclass(p, cs);
      REQUIRE(MORE(), REG_EBRACK);
      REQUIRE(EATTWO(':', ']'), REG_ECTYPE);
      break;
   case '=':      /* equivalence class */
      NEXT2();
      REQUIRE(MORE(), REG_EBRACK);
      c = PEEK();
      REQUIRE(c != '-' && c != ']', REG_ECOLLATE);
      p_b_eclass(p, cs);
      REQUIRE(MORE(), REG_EBRACK);
      REQUIRE(EATTWO('=', ']'), REG_ECOLLATE);
      break;
   default:    /* symbol, ordinary character, or range */
/* xxx revision needed for multichar stuff */
      start = p_b_symbol(p);
      if (SEE('-') && MORE2() && PEEK2() != ']') {
         /* range */
         NEXT();
         if (EAT('-'))
            finish = '-';
         else
            finish = p_b_symbol(p);
      } else
         finish = start;
/* xxx what about signed chars here... */
      REQUIRE(start <= finish, REG_ERANGE);
      for (i = start; i <= finish; i++)
         CHadd(cs, i);
      break;
   }
}

/*
 - p_b_cclass - parse a character-class name and deal with it
 == static void p_b_cclass(register struct parse *p, register cset *cs);
 */
static void
p_b_cclass(p, cs)
register struct parse *p;
register cset *cs;
{
   register char *sp = p->next;
   register struct cclass *cp;
   register size_t len;
   register char *u;
   register char c;

   while (MORE() && isalpha(PEEK()))
      NEXT();
   len = p->next - sp;
   for (cp = cclasses; cp->name != NULL; cp++)
      if (strncmp(cp->name, sp, len) == 0 && cp->name[len] == '\0')
         break;
   if (cp->name == NULL) {
      /* oops, didn't find it */
      SETERROR(REG_ECTYPE);
      return;
   }

   u = cp->chars;
   while ((c = *u++) != '\0')
      CHadd(cs, c);
   for (u = cp->multis; *u != '\0'; u += strlen(u) + 1)
      MCadd(p, cs, u);
}

/*
 - p_b_eclass - parse an equivalence-class name and deal with it
 == static void p_b_eclass(register struct parse *p, register cset *cs);
 *
 * This implementation is incomplete. xxx
 */
static void
p_b_eclass(p, cs)
register struct parse *p;
register cset *cs;
{
   register char c;

   c = p_b_coll_elem(p, '=');
   CHadd(cs, c);
}

/*
 - p_b_symbol - parse a character or [..]ed multicharacter collating symbol
 == static char p_b_symbol(register struct parse *p);
 */
static char       /* value of symbol */
p_b_symbol(p)
register struct parse *p;
{
   register char value;

   REQUIRE(MORE(), REG_EBRACK);
   if (!EATTWO('[', '.'))
      return(GETNEXT());

   /* collating symbol */
   value = p_b_coll_elem(p, '.');
   REQUIRE(EATTWO('.', ']'), REG_ECOLLATE);
   return(value);
}

/*
 - p_b_coll_elem - parse a collating-element name and look it up
 == static char p_b_coll_elem(register struct parse *p, int endc);
 */
static char       /* value of collating element */
p_b_coll_elem(p, endc)
register struct parse *p;
int endc;         /* name ended by endc,']' */
{
   register char *sp = p->next;
   register struct cname *cp;
   register int len;

   while (MORE() && !SEETWO(endc, ']'))
      NEXT();
   if (!MORE()) {
      SETERROR(REG_EBRACK);
      return(0);
   }
   len = p->next - sp;
   for (cp = cnames; cp->name != NULL; cp++)
      if (strncmp(cp->name, sp, len) == 0 && cp->name[len] == '\0')
         return(cp->icode);   /* known name */
   if (len == 1)
      return(*sp);   /* single character */
   SETERROR(REG_ECOLLATE);       /* neither */
   return(0);
}

/*
 - othercase - return the case counterpart of an alphabetic
 == static char othercase(int ch);
 */
static char       /* if no counterpart, return ch */
othercase(ch)
int ch;
{
   assert(isalpha(ch));
   if (isupper(ch))
      return(tolower(ch));
   else if (islower(ch))
      return(toupper(ch));
   else        /* peculiar, but could happen */
      return(ch);
}

/*
 - bothcases - emit a dualcase version of a two-case character
 == static void bothcases(register struct parse *p, int ch);
 *
 * Boy, is this implementation ever a kludge...
 */
static void
bothcases(p, ch)
register struct parse *p;
int ch;
{
   register char *oldnext = p->next;
   register char *oldend = p->end;
   char bracket[3];

   assert(othercase(ch) != ch);  /* p_bracket() would recurse */
   p->next = bracket;
   p->end = bracket+2;
   bracket[0] = ch;
   bracket[1] = ']';
   bracket[2] = '\0';
   p_bracket(p);
   assert(p->next == bracket+2);
   p->next = oldnext;
   p->end = oldend;
}

/*
 - ordinary - emit an ordinary character
 == static void ordinary(register struct parse *p, register int ch);
 */
static void
ordinary(p, ch)
register struct parse *p;
register int ch;
{
   register cat_t *cap = p->g->categories;

   if ((p->g->cflags&REG_ICASE) && isalpha(ch) && othercase(ch) != ch)
      bothcases(p, ch);
   else {
      EMIT(OCHAR, (unsigned char)ch);
      if (cap[ch] == 0)
         cap[ch] = p->g->ncategories++;
   }
}

/*
 - nonnewline - emit REG_NEWLINE version of OANY
 == static void nonnewline(register struct parse *p);
 *
 * Boy, is this implementation ever a kludge...
 */
static void
nonnewline(p)
register struct parse *p;
{
   register char *oldnext = p->next;
   register char *oldend = p->end;
   char bracket[4];

   p->next = bracket;
   p->end = bracket+3;
   bracket[0] = '^';
   bracket[1] = '\n';
   bracket[2] = ']';
   bracket[3] = '\0';
   p_bracket(p);
   assert(p->next == bracket+3);
   p->next = oldnext;
   p->end = oldend;
}

/*
 - repeat - generate code for a bounded repetition, recursively if needed
 == static void repeat(register struct parse *p, sopno start, int from, int to);
 */
static void
repeat(p, start, from, to)
register struct parse *p;
sopno start;         /* operand from here to end of strip */
int from;         /* repeated from this number */
int to;           /* to this number of times (maybe INFINITY) */
{
   register sopno finish = HERE();
#  define   N  2
#  define   INF   3
#  define   REP(f, t)   ((f)*8 + (t))
#  define   MAP(n)   (((n) <= 1) ? (n) : ((n) == INFINITY) ? INF : N)
   register sopno copy;
   long int mapfrom,mapto;
      
   if (p->error != 0)   /* head off possible runaway recursion */
      return;

   assert(from <= to);

   mapfrom = MAP(from); mapto = MAP(to);
   switch (REP(mapfrom, mapto)) {
   case REP(0, 0):         /* must be user doing this */
      DROP(finish-start);  /* drop the operand */
      break;
   case REP(0, 1):         /* as x{1,1}? */
   case REP(0, N):         /* as x{1,n}? */
   case REP(0, INF):    /* as x{1,}? */
      /* KLUDGE: emit y? as (y|) until subtle bug gets fixed */
      INSERT(OCH_, start);    /* offset is wrong... */
      repeat(p, start+1, 1, to);
      ASTERN(OOR1, start);
      AHEAD(start);        /* ... fix it */
      EMIT(OOR2, 0);
      AHEAD(THERE());
      ASTERN(O_CH, THERETHERE());
      break;
   case REP(1, 1):         /* trivial case */
      /* done */
      break;
   case REP(1, N):         /* as x?x{1,n-1} */
      /* KLUDGE: emit y? as (y|) until subtle bug gets fixed */
      INSERT(OCH_, start);
      ASTERN(OOR1, start);
      AHEAD(start);
      EMIT(OOR2, 0);       /* offset very wrong... */
      AHEAD(THERE());         /* ...so fix it */
      ASTERN(O_CH, THERETHERE());
      copy = dupl(p, start+1, finish+1);
      assert(copy == finish+4);
      repeat(p, copy, 1, to-1);
      break;
   case REP(1, INF):    /* as x+ */
      INSERT(OPLUS_, start);
      ASTERN(O_PLUS, start);
      break;
   case REP(N, N):         /* as xx{m-1,n-1} */
      copy = dupl(p, start, finish);
      repeat(p, copy, from-1, to-1);
      break;
   case REP(N, INF):    /* as xx{n-1,INF} */
      copy = dupl(p, start, finish);
      repeat(p, copy, from-1, to);
      break;
   default:       /* "can't happen" */
      SETERROR(REG_ASSERT);   /* just in case */
      break;
   }
}

/*
 - seterr - set an error condition
 == static int seterr(register struct parse *p, int e);
 */
static int seterr(register struct parse *p, int e)
{
   if (p->error == 0)   /* keep earliest error condition */
      p->error = e;
   p->next = nuls;      /* try to bring things to a halt */
   p->end = nuls;
   return(0);     /* make the return value well-defined */
}

/*
 - allocset - allocate a set of characters for []
 == static cset *allocset(register struct parse *p);
 */
static cset *
allocset(p)
register struct parse *p;
{
   register int no = p->g->ncsets++;
   register size_t nc;
   register size_t nbytes;
   register cset *cs;
   register size_t css = (size_t)p->g->csetsize;
   register int i;

   if (no >= p->ncsalloc) {   /* need another column of space */
      p->ncsalloc += CHAR_BIT;
      nc = p->ncsalloc;
      assert(nc % CHAR_BIT == 0);
      nbytes = nc / CHAR_BIT * css;
      if (p->g->sets == NULL)
         p->g->sets = (cset *)malloc(nc * sizeof(cset));
      else
         p->g->sets = (cset *)realloc((char *)p->g->sets,
                     nc * sizeof(cset));
      if (p->g->setbits == NULL)
         p->g->setbits = (uch *)malloc(nbytes);
      else {
         p->g->setbits = (uch *)realloc((char *)p->g->setbits,
                        nbytes);
         /* xxx this isn't right if setbits is now NULL */
         for (i = 0; i < no; i++)
            p->g->sets[i].ptr = p->g->setbits + css*(i/CHAR_BIT);
      }
      if (p->g->sets != NULL && p->g->setbits != NULL)
         (void) memset((char *)p->g->setbits + (nbytes - css),
                        0, css);
      else {
         no = 0;
         SETERROR(REG_ESPACE);
         /* caller's responsibility not to do set ops */
      }
   }

   assert(p->g->sets != NULL);   /* xxx */
   cs = &p->g->sets[no];
   cs->ptr = p->g->setbits + css*((no)/CHAR_BIT);
   cs->mask = 1 << ((no) % CHAR_BIT);
   cs->hash = 0;
   cs->smultis = 0;
   cs->multis = NULL;

   return(cs);
}

/*
 - freeset - free a now-unused set
 == static void freeset(register struct parse *p, register cset *cs);
 */
static void
freeset(p, cs)
register struct parse *p;
register cset *cs;
{
   register int i;
   register cset *top = &p->g->sets[p->g->ncsets];
   register size_t css = (size_t)p->g->csetsize;

   for (i = 0; i < css; i++)
      CHsub(cs, i);
   if (cs == top-1)  /* recover only the easy case */
      p->g->ncsets--;
}

/*
 - freezeset - final processing on a set of characters
 == static int freezeset(register struct parse *p, register cset *cs);
 *
 * The main task here is merging identical sets.  This is usually a waste
 * of time (although the hash code minimizes the overhead), but can win
 * big if REG_ICASE is being used.  REG_ICASE, by the way, is why the hash
 * is done using addition rather than xor -- all ASCII [aA] sets xor to
 * the same value!
 */
static int        /* set number */
freezeset(p, cs)
register struct parse *p;
register cset *cs;
{
   register uch h = cs->hash;
   register int i;
   register cset *top = &p->g->sets[p->g->ncsets];
   register cset *cs2;
   register size_t css = (size_t)p->g->csetsize;

   /* look for an earlier one which is the same */
   for (cs2 = &p->g->sets[0]; cs2 < top; cs2++)
      if (cs2->hash == h && cs2 != cs) {
         /* maybe */
         for (i = 0; i < css; i++)
            if (!!CHIN(cs2, i) != !!CHIN(cs, i))
               break;      /* no */
         if (i == css)
            break;         /* yes */
      }

   if (cs2 < top) {  /* found one */
      freeset(p, cs);
      cs = cs2;
   }

   return((int)(cs - p->g->sets));
}

/*
 - firstch - return first character in a set (which must have at least one)
 == static int firstch(register struct parse *p, register cset *cs);
 */
static int        /* character; there is no "none" value */
firstch(p, cs)
register struct parse *p;
register cset *cs;
{
   register int i;
   register size_t css = (size_t)p->g->csetsize;

   for (i = 0; i < css; i++)
      if (CHIN(cs, i))
         return((char)i);
   assert(never);
   return(0);     /* arbitrary */
}

/*
 - nch - number of characters in a set
 == static int nch(register struct parse *p, register cset *cs);
 */
static int
nch(p, cs)
register struct parse *p;
register cset *cs;
{
   register int i;
   register size_t css = (size_t)p->g->csetsize;
   register int n = 0;

   for (i = 0; i < css; i++)
      if (CHIN(cs, i))
         n++;
   return(n);
}

/*
 - mcadd - add a collating element to a cset
 == static void mcadd(register struct parse *p, register cset *cs, \
 ==   register char *cp);
 */
static void
mcadd(p, cs, cp)
register struct parse *p;
register cset *cs;
register char *cp;
{
   register size_t oldend = cs->smultis;

   cs->smultis += strlen(cp) + 1;
   if (cs->multis == NULL)
      cs->multis = malloc(cs->smultis);
   else
      cs->multis = realloc(cs->multis, cs->smultis);
   if (cs->multis == NULL) {
      SETERROR(REG_ESPACE);
      return;
   }

   (void) strcpy(cs->multis + oldend - 1, cp);
   cs->multis[cs->smultis - 1] = '\0';
}

/*
 - mcinvert - invert the list of collating elements in a cset
 == static void mcinvert(register struct parse *p, register cset *cs);
 *
 * This would have to know the set of possibilities.  Implementation
 * is deferred.
 */
static void mcinvert(register struct parse *p, register cset *cs)
{
   assert(cs->multis == NULL);   
   if (p || cs){}
}

/*
 - mccase - add case counterparts of the list of collating elements in a cset
 == static void mccase(register struct parse *p, register cset *cs);
 *
 * This would have to know the set of possibilities.  Implementation
 * is deferred.
 */
static void mccase(register struct parse *p, register cset *cs)
{
   assert(cs->multis == NULL);
   if (p || cs){}
}

/*
 - isinsets - is this character in any sets?
 == static int isinsets(register struct re_guts *g, int c);
 */
static int        /* predicate */
isinsets(g, c)
register struct re_guts *g;
int c;
{
   register uch *col;
   register int i;
   register int ncols = (g->ncsets+(CHAR_BIT-1)) / CHAR_BIT;
   register unsigned uc = (unsigned char)c;

   for (i = 0, col = g->setbits; i < ncols; i++, col += g->csetsize)
      if (col[uc] != 0)
         return(1);
   return(0);
}

/*
 - samesets - are these two characters in exactly the same sets?
 == static int samesets(register struct re_guts *g, int c1, int c2);
 */
static int        /* predicate */
samesets(g, c1, c2)
register struct re_guts *g;
int c1;
int c2;
{
   register uch *col;
   register int i;
   register int ncols = (g->ncsets+(CHAR_BIT-1)) / CHAR_BIT;
   register unsigned uc1 = (unsigned char)c1;
   register unsigned uc2 = (unsigned char)c2;

   for (i = 0, col = g->setbits; i < ncols; i++, col += g->csetsize)
      if (col[uc1] != col[uc2])
         return(0);
   return(1);
}

/*
 - categorize - sort out character categories
 == static void categorize(struct parse *p, register struct re_guts *g);
 */
static void
categorize(p, g)
struct parse *p;
register struct re_guts *g;
{
   register cat_t *cats = g->categories;
   register int c;
   register int c2;
   register cat_t cat;

   /* avoid making error situations worse */
   if (p->error != 0)
      return;

   for (c = CHAR_MIN; c <= CHAR_MAX; c++)
      if (cats[c] == 0 && isinsets(g, c)) {
         cat = g->ncategories++;
         cats[c] = cat;
         for (c2 = c+1; c2 <= CHAR_MAX; c2++)
            if (cats[c2] == 0 && samesets(g, c, c2))
               cats[c2] = cat;
      }
}

/*
 - dupl - emit a duplicate of a bunch of sops
 == static sopno dupl(register struct parse *p, sopno start, sopno finish);
 */
static sopno         /* start of duplicate */
dupl(p, start, finish)
register struct parse *p;
sopno start;         /* from here */
sopno finish;        /* to this less one */
{
   register sopno ret = HERE();
   register sopno len = finish - start;

   assert(finish >= start);
   if (len == 0)
      return(ret);
   enlarge(p, p->ssize + len);   /* this many unexpected additions */
   assert(p->ssize >= p->slen + len);
   (void) memcpy((char *)(p->strip + p->slen),
      (char *)(p->strip + start), (size_t)len*sizeof(sop));
   p->slen += len;
   return(ret);
}

/*
 - doemit - emit a strip operator
 == static void doemit(register struct parse *p, sop op, size_t opnd);
 *
 * It might seem better to implement this as a macro with a function as
 * hard-case backup, but it's just too big and messy unless there are
 * some changes to the data structures.  Maybe later.
 */
static void
doemit(p, op, opnd)
register struct parse *p;
sop op;
size_t opnd;
{
   /* avoid making error situations worse */
   if (p->error != 0)
      return;

   /* deal with oversize operands ("can't happen", more or less) */
   assert(opnd < 1<<OPSHIFT);

   /* deal with undersized strip */
   if (p->slen >= p->ssize)
      enlarge(p, (p->ssize+1) / 2 * 3);   /* +50% */
   assert(p->slen < p->ssize);

   /* finally, it's all reduced to the easy case */
   p->strip[p->slen++] = SOP(op, opnd);
}

/*
 - doinsert - insert a sop into the strip
 == static void doinsert(register struct parse *p, sop op, size_t opnd, sopno pos);
 */
static void doinsert(register struct parse *p, sop op, size_t opnd, sopno pos)
{
   register sopno sn;
   register sop s;
   register int i;
   size_t n;
   
   /* avoid making error situations worse */
   if (p->error != 0)
      return;

   sn = HERE();
   EMIT(op, opnd);      /* do checks, ensure space */
   assert(HERE() == sn+1);
   s = p->strip[sn];

   /* adjust paren pointers */
   assert(pos > 0);
   for (i = 1; i < NPAREN; i++) {
      if (p->pbegin[i] >= pos) {
         p->pbegin[i]++;
      }
      if (p->pend[i] >= pos) {
         p->pend[i]++;
      }
   }

   n=(HERE()-pos-1);
   memmove((char *)&p->strip[pos+1], (char *)&p->strip[pos],n*sizeof(sop));
   p->strip[pos] = s;
}

/*
 - dofwd - complete a forward reference
 == static void dofwd(register struct parse *p, sopno pos, sop value);
 */
static void
dofwd(p, pos, value)
register struct parse *p;
register sopno pos;
sop value;
{
   /* avoid making error situations worse */
   if (p->error != 0)
      return;

   assert(value < 1<<OPSHIFT);
   p->strip[pos] = OP(p->strip[pos]) | value;
}

/*
 - enlarge - enlarge the strip
 == static void enlarge(register struct parse *p, sopno size);
 */
static void
enlarge(p, size)
register struct parse *p;
register sopno size;
{
   register sop *sp;

   if (p->ssize >= size)
      return;

   sp = (sop *)realloc(p->strip, size*sizeof(sop));
   if (sp == NULL) {
      SETERROR(REG_ESPACE);
      return;
   }
   p->strip = sp;
   p->ssize = size;
}

/*
 - stripsnug - compact the strip
 == static void stripsnug(register struct parse *p, register struct re_guts *g);
 */
static void
stripsnug(p, g)
register struct parse *p;
register struct re_guts *g;
{
   g->nstates = p->slen;
   g->strip = (sop *)realloc((char *)p->strip, p->slen * sizeof(sop));
   if (g->strip == NULL) {
      SETERROR(REG_ESPACE);
      g->strip = p->strip;
   }
}

/*
 - findmust - fill in must and mlen with longest mandatory literal string
 == static void findmust(register struct parse *p, register struct re_guts *g);
 *
 * This algorithm could do fancy things like analyzing the operands of |
 * for common subsequences.  Someday.  This code is simple and finds most
 * of the interesting cases.
 *
 * Note that must and mlen got initialized during setup.
 */
static void
findmust(p, g)
struct parse *p;
register struct re_guts *g;
{
   register sop *scan;
   sop *start;
   register sop *newstart=0;
   register sopno newlen;
   register sop s;
   register char *cp;
   register sopno i;

   /* avoid making error situations worse */
   if (p->error != 0)
      return;

   /* find the longest OCHAR sequence in strip */
   newlen = 0;
   scan = g->strip + 1;
   do {
      s = *scan++;
      switch (OP(s)) {
      case OCHAR:    /* sequence member */
         if (newlen == 0)     /* new sequence */
            newstart = scan - 1;
         newlen++;
         break;
      case OPLUS_:      /* things that don't break one */
      case OLPAREN:
      case ORPAREN:
         break;
      case OQUEST_:     /* things that must be skipped */
      case OCH_:
         scan--;
         do {
            scan += OPND(s);
            s = *scan;
            /* assert() interferes w debug printouts */
            if (OP(s) != O_QUEST && OP(s) != O_CH &&
                     OP(s) != OOR2) {
               g->iflags |= BAD;
               return;
            }
         } while (OP(s) != O_QUEST && OP(s) != O_CH);
         /* fallthrough */
      default:    /* things that break a sequence */
         if (newlen > g->mlen) {    /* ends one */
            start = newstart;
            g->mlen = newlen;
         }
         newlen = 0;
         break;
      }
   } while (OP(s) != OEND);

   if (g->mlen == 0)    /* there isn't one */
      return;

   /* turn it into a character string */
   g->must = malloc((size_t)g->mlen + 1);
   if (g->must == NULL) {     /* argh; just forget it */
      g->mlen = 0;
      return;
   }
   cp = g->must;
   scan = start;
   for (i = g->mlen; i > 0; i--) {
      while (OP(s = *scan++) != OCHAR)
         continue;
      assert(cp < g->must + g->mlen);
      *cp++ = (char)OPND(s);
   }
   assert(cp == g->must + g->mlen);
   *cp++ = '\0';     /* just on general principles */
}

/*
 - pluscount - count + nesting
 == static sopno pluscount(register struct parse *p, register struct re_guts *g);
 */
static sopno         /* nesting depth */
pluscount(p, g)
struct parse *p;
register struct re_guts *g;
{
   register sop *scan;
   register sop s;
   register sopno plusnest = 0;
   register sopno maxnest = 0;

   if (p->error != 0)
      return(0);  /* there may not be an OEND */

   scan = g->strip + 1;
   do {
      s = *scan++;
      switch (OP(s)) {
      case OPLUS_:
         plusnest++;
         break;
      case O_PLUS:
         if (plusnest > maxnest)
            maxnest = plusnest;
         plusnest--;
         break;
      }
   } while (OP(s) != OEND);
   if (plusnest != 0)
      g->iflags |= BAD;
   return(maxnest);
}