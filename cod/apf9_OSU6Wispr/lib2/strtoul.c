#ifndef STRTOUL_H
#define STRTOUL_H

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * $Id: strtoul.c,v 1.5 2007/04/24 01:43:29 swift Exp $
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
 * $Log: strtoul.c,v $
 * Revision 1.5  2007/04/24 01:43:29  swift
 * Added acknowledgement and funding attribution.
 *
 * Revision 1.4  2006/04/21 13:45:40  swift
 * Changed copyright attribute.
 *
 * Revision 1.3  2003/11/20 18:59:42  swift
 * Added GNU Lesser General Public License to source file.
 *
 * Revision 1.2  2003/07/16 23:20:24  swift
 * Fixed a couple of bugs in error detection.
 *
 * Revision 1.1  2003/07/15 22:59:17  swift
 * Initial revision
 * \end{verbatim}
*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
#define strtoulChangeLog "$RCSfile: strtoul.c,v $ $Revision: 1.5 $ $Date: 2007/04/24 01:43:29 $"

/* function prototypes */
int atoi(const char *s);
long int atol(const char *s);
long int strtol(const char *str, char **endptr, int base);
unsigned long int strtoul(const char *str, char **endptr, int base);

#endif /* STRTOUL_H */

#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <stddef.h>
#include <string.h>

/* define the largest base that this function can decode */
#define BASE_MAX 36

/* define the digits */
static const char digits[]={"0123456789abcdefghijklmnopqrstuvwxyz"};

/* for each base, define critical number of digits were 32-bit integer overflows */
static const char ndigs[BASE_MAX+1] = 
{
   0, 0, 33, 21, 17, 14, 13, 12, 11, 11, 10, 10, 9, 9, 9, 9, 9,
   8, 8, 8, 8, 8, 8, 8, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7
};

/* function prototypes for local functions */
static unsigned long int Strtoul(const char *s, char **endptr, int base);

/*------------------------------------------------------------------------*/
/* function to convert a string to an integer                             */
/*------------------------------------------------------------------------*/
/**
   This function converts the initial portion of a string to an integer.

      \begin{verbatim}
      input:
         s....The string to be converted.

      output:   
         This function returns the value of the string up to the first
         nonconvertable character.  
      \end{verbatim}
*/
int atoi(const char *s)
{
   return ((int)Strtoul(s,NULL,10));
}

/*------------------------------------------------------------------------*/
/* function to convert a string to a long integer                         */
/*------------------------------------------------------------------------*/
/**
   This function converts the initial portion of a string to a long integer.

      \begin{verbatim}
      input:
         s....The string to be converted.

      output:   
         This function returns the value of the string up to the first
         nonconvertable character.  
      \end{verbatim}
*/
long int atol(const char *s)
{
   return ((long int)Strtoul(s,NULL,10));
}

/*------------------------------------------------------------------------*/
/* function to convert a string to a long integer                         */
/*------------------------------------------------------------------------*/
/**
   The strtol() function converts the string in nptr to a long integer value
   according to the given base, which must be between 2 and 36 inclusive, or
   be the special value 0.

   The string must begin with an arbitrary amount of white space (as
   determined by isspace()) followed by a single optional `+' or `-' sign.
   If base is zero or 16, the string may then include a `0x' prefix, and the
   number will be read in base 16; otherwise, a zero base is taken as 10
   (decimal) unless the next character is `0', in which case it is taken
   as 8 (octal).

   The remainder of the string is converted to a long int value in the
   obvious manner, stopping at the first character which is not a valid
   digit in the given base.  (In bases above 10, the letter `A' in either
   upper or lower case represents 10, `B' represents 11, and so forth,
   with `Z' representing 35.)

   If endptr is not NULL, strtol() stores the address of the first invalid
   character in *endptr.  If there were no digits at all, strtol() stores
   the original value of nptr in *endptr (and returns 0).  In particular, if
   *nptr is not `\0' but **endptr is `\0' on return, the entire string is
   valid.

      \begin{verbatim}
      input:
         str.....The string to convert.
         base....The base to use in the conversion.

      output:
         The strtol() function returns the result of the conversion, unless
         the value would underflow or overflow.  If an underflow occurs,
         strtol() returns LONG_MIN.  If an overflow occurs, strtol() returns
         LONG_MAX.  In both cases, errno is set to ERANGE.
      \end{verbatim}
*/
long int strtol(const char *str, char **endptr, int base)
{ 
   const char *sc;

   /* convert the string to an unsigned long */
   unsigned long x = Strtoul(str,endptr,base);

   /* ignore leading whitespace */
   for (sc=str; isspace(*sc); ++sc) {}

   /* check for underflow */
   if ((*sc)=='-' && x>0 && x<=LONG_MAX) {errno=ERANGE; return LONG_MIN;}

   /* check for overflow */
   else if ((*sc)!='-' && x>LONG_MAX) {errno=ERANGE; return LONG_MAX;}

   return x;
}

/*------------------------------------------------------------------------*/
/* function to convert a string to an unsigned long integer               */
/*------------------------------------------------------------------------*/
/**
   The strtoul() function converts the string in nptr to an unsigned long
   integer value according to the given base, which must be between 2 and 36
   inclusive, or be the special value 0.

   The string must begin with an arbitrary amount of white space (as
   determined by isspace(3)) followed by a single optional `+' or `-' sign.
   If base is zero or 16, the string may then include a `0x' prefix, and the
   number will be read in base 16; otherwise, a zero base is taken as 10
   (decimal) unless the next character is `0', in which case it is taken as
   8 (octal).

   The remainder of the string is converted to an unsigned long int value in
   the obvious manner, stopping at the first character which is not a valid
   digit in the given base.  (In bases above 10, the letter `A' in either
   upper or lower case represents 10, `B' represents 11, and so forth, with
   `Z' representing 35.)

   If endptr is not NULL, strtoul() stores the address of the first invalid
   character in *endptr.  If there were no digits at all, strtoul() stores
   the original value of nptr in *endptr.  (Thus, if *nptr is not `\0' but
   **endptr is `\0' on return, the entire string is valid.)

      \begin{verbatim}
      input:
         str.....The string to convert.
         base....The base to use in the conversion.

      output:

         The strtoul() function returns either the result of the conversion
         or, if there was a leading minus sign, the negation of the result
         of the conversion, unless the original (non-negated) value would
         overflow; in the latter case, strtoul() returns ULONG_MAX and sets
         the global variable errno to ERANGE.
      \end{verbatim}
*/
unsigned long int strtoul(const char *str, char **endptr, int base)
{
   return Strtoul(str,endptr,base);
}


/*------------------------------------------------------------------------*/
/* function to convert a string to an unsigned long integer               */
/*------------------------------------------------------------------------*/
/**
   The strtoul() function converts the string in nptr to an unsigned long
   integer value according to the given base, which must be between 2 and 36
   inclusive, or be the special value 0.

   The string must begin with an arbitrary amount of white space (as
   determined by isspace(3)) followed by a single optional `+' or `-' sign.
   If base is zero or 16, the string may then include a `0x' prefix, and the
   number will be read in base 16; otherwise, a zero base is taken as 10
   (decimal) unless the next character is `0', in which case it is taken as
   8 (octal).

   The remainder of the string is converted to an unsigned long int value in
   the obvious manner, stopping at the first character which is not a valid
   digit in the given base.  (In bases above 10, the letter `A' in either
   upper or lower case represents 10, `B' represents 11, and so forth, with
   `Z' representing 35.)

   If endptr is not NULL, strtoul() stores the address of the first invalid
   character in *endptr.  If there were no digits at all, strtoul() stores
   the original value of nptr in *endptr.  (Thus, if *nptr is not `\0' but
   **endptr is `\0' on return, the entire string is valid.)

      \begin{verbatim}
      input:
         str.....The string to convert.
         base....The base to use in the conversion.

      output:

         The strtoul() function returns either the result of the conversion
         or, if there was a leading minus sign, the negation of the result
         of the conversion, unless the original (non-negated) value would
         overflow; in the latter case, strtoul() returns ULONG_MAX and sets
         the global variable errno to ERANGE.
      \end{verbatim}
*/
static unsigned long int Strtoul(const char *s, char **endptr, int base)
{
   const char *sc, *sd;
   const char *s1, *s2;
   char z,sign;
   ptrdiff_t n;
   unsigned long x,y;

   /* ignore leading whitespace */
   for (sc=s; isspace(*sc); ++sc) {}

   /* assign the sign */
   sign = ((*sc)=='-' || (*sc)=='+') ? *sc++ : '+';

   /* check for illegal bases */
   if (base<0 || base==1 || base>BASE_MAX)
   {
      if (endptr) *endptr = (char *)s;
      return 0;
   }

   /* check for explicit specification of the base */
   else if (base)
   {
      /* strip any leading '0x' or '0X' if base 16 */
      if (base==16 && (*sc)=='0' && (sc[1]=='x' || sc[1]=='X')) sc += 2;
   }

   /* autodetection of base */
   else
   {
      /* autodetection of base: check of decimal */
      if ((*sc)!='0') base=10;

      /* autodetection of base: check for hex */
      else if (sc[1]=='x' || sc[1]=='X') {base=16; sc+=2;}

      /* autodetection of octal base */
      else base=8;
   }
      
   /* skip leading zeros */
   for (s1=sc; (*sc)=='0'; ++sc) {}

   /* loop through the characters of the string */
   for (x=0, y=0, z=0, s2=sc; (sd=memchr(digits,tolower(*sc),base))!=NULL; ++sc)
   {
      /* this will be used for overflow detection */
      y=x; z=(sd-digits);

      /* use the distributive law of addition to accumulate the value */
      x = x*base + z;
   }

   /* check string validity */
   if (s1==sc && endptr) {*endptr = (char *)s; return 0;}

   /* compute the number of digits in the result */
   n = sc - s2 - ndigs[base];

   /* check for overflow */
   if (n>0 || (x-z)>x || (x-z)/base!=y) {errno=ERANGE; x=ULONG_MAX;}

   /* assign the sign */
   if (sign=='-') x=-x;

   /* assign the end-pointer */
   if (endptr) *endptr = (char *)sc;

   return x;
}
