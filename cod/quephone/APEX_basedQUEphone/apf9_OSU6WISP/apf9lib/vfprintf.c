#define _HOSTED 1
#include	<stdio.h>
#include	<ctype.h>
#include	<stdlib.h>
#include	<string.h>
#include	<stdarg.h>
#include	<conio.h>
#include	<sys.h>
#include	<math.h>
#include	<float.h>
#include <trace.h>

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * $Id: vfprintf.c,v 1.20 2007/04/24 01:43:05 swift Exp $
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
 * $Log: vfprintf.c,v $
 * Revision 1.20  2007/04/24 01:43:05  swift
 * Added acknowledgement and funding attribution.
 *
 * Revision 1.19  2006/04/21 13:45:38  swift
 * Changed copyright attribute.
 *
 * Revision 1.18  2004/06/07 21:16:11  swift
 * Fixed a rounding bug and a bug that could cause the APF9 to hang for long
 * enough to activate the watchdog.
 *
 * Revision 1.17  2003/11/20 18:59:35  swift
 * Added GNU Lesser General Public License to source file.
 *
 * Revision 1.16  2003/06/11 18:44:49  swift
 * Changed the union _fdp from external linkage to static linkage to
 * prevent multiple definition errors during the link stage.  Also
 * added some trace() capability.
 *
 * Revision 1.15  2003/06/07 20:50:34  swift
 * Modifications to allow for floats and longs.
 *
 * Revision 1.14  2003/06/02 16:25:42  swift
 * Defined '__FLOAT' so that handling of floats, doubles, and longs are enabled
 * by default.
 *
 * Revision 1.13  2003/06/02 16:22:35  swift
 * Specified static linkage for fround() to avoid multiple definition.  The
 * function definition was changed from 'double fround(unsigned char prec)' to
 * 'static double fround(unsigned char prec)'.
 *
 * Revision 1.12  2003/05/31 22:53:05  swift
 * As received with the HiTech distribution.
 *
 * Revision 1.11  97/04/10  16:52:44  clyde
 * Fix rounding of doubles in %e format
 * 
 * Revision 1.10  97/04/10  09:56:52  clyde
 * Fix rounding error with %e format
 * 
 * Revision 1.9  97/04/09  23:46:01  clyde
 * Fix rounding problem with digits near the limit
 * 
 * Revision 1.8  97/04/09  22:53:37  clyde
 * Fix problem printing 9.99 in %.1e format
 * 
 * Revision 1.7  96/12/09  13:54:32  jeremy
 * Modified a call to "ifrexp" to call "frexp".
 * 
 * Revision 1.6  96/11/05  10:36:45  jeremy
 * When printing floats in 'e' format, now treats zero as a special number
 * and keeps the exponent as e00.
 * 
 * Revision 1.5  96/09/25  15:24:05  clyde
 * Fix overflow problem with floats
 * 
 * Revision 1.4  96/08/20  23:46:00  clyde
 * Added support for 4 digit exponents
 * 
 * Revision 1.3  96/05/25  08:33:21  clyde
 * Use long data for pointers in 8086 large model
 * 
 * Revision 1.2  95/10/18  15:37:22  clyde
 * 64 bit mods
* \end{verbatim}
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
#define VfprintfChangeLog "$RCSfile: vfprintf.c,v $  $Revision: 1.20 $   $Date: 2007/04/24 01:43:05 $"

/* enable handling of floats, doubles, and longs. -dds */
#define __FLOAT

#ifndef	_MPC_
static char	rcsid[] = "$Id: vfprintf.c,v 1.20 2007/04/24 01:43:05 swift Exp $";
#endif

#if	(sizeof(long) == sizeof(int) || defined(__FLOAT)) && !defined(__LONG)
#define	__LONG	1
#endif

#if	sizeof(double) == sizeof(long) && DBL_MAX_EXP == 128
#define	frexp(val, ptr) (void)(*(ptr) = ((*(unsigned long *)&val >> 23) & 255) - 126)
#endif

#if	defined(__FLOAT) && sizeof(double) == 8
#define	DOUBLE	1

extern double	_dto64i(double);
extern double	_64itod(double);
extern double	_div64(double, double);
extern double	_divto64i(double, double);
extern unsigned	_div64ir(double, double);
extern unsigned	_mod64i10(double);
#else
#define	DOUBLE	0
#endif

#ifdef	__LONG
#define	value	long
#define	NDIG	12		/* max number of digits to be printed */
#else
#define	value	int
#define	NDIG	6		/* max number of digits to be printed */
#endif

#if	DOUBLE
static union {
	unsigned long	ul[40];
	double		db[20];
}	_fdp =
{
#if	BIG_ENDIAN
	0x00000000,0x00000001,
	0x00000000,0x0000000A,
	0x00000000,0x00000064,
	0x00000000,0x000003E8,
	0x00000000,0x00002710,
	0x00000000,0x000186A0,
	0x00000000,0x000F4240,
	0x00000000,0x00989680,
	0x00000000,0x05F5E100,
	0x00000000,0x3B9ACA00,
	0x00000002,0x540BE400,
	0x00000017,0x4876E800,
	0x000000E8,0xD4A51000,
	0x00000918,0x4E72A000,
	0x00005AF3,0x107A4000,
	0x00038D7E,0xA4C68000,
	0x002386F2,0x6FC10000,
	0x01634578,0x5D8A0000,
	0x0DE0B6B3,0xA7640000,
	0x8AC72304,0x89E80000,
#else
	0x00000001,0x00000000,
	0x0000000A,0x00000000,
	0x00000064,0x00000000,
	0x000003E8,0x00000000,
	0x00002710,0x00000000,
	0x000186A0,0x00000000,
	0x000F4240,0x00000000,
	0x00989680,0x00000000,
	0x05F5E100,0x00000000,
	0x3B9ACA00,0x00000000,
	0x540BE400,0x00000002,
	0x4876E800,0x00000017,
	0xD4A51000,0x000000E8,
	0x4E72A000,0x00000918,
	0x107A4000,0x00005AF3,
	0xA4C68000,0x00038D7E,
	0x6FC10000,0x002386F2,
	0x5D8A0000,0x01634578,
	0xA7640000,0x0DE0B6B3,
	0x89E80000,0x8AC72304,
#endif
};

#define	fdpowers	_fdp.db

#else
#define	fdpowers	dpowers
#endif
#ifdef	_MPC_
code
#endif
const static unsigned value	dpowers[] =	{1, 10, 100, 1000, 10000,
#ifdef	__LONG
						 100000, 1000000, 10000000, 100000000,
						 1000000000
#endif
							 };
#ifdef	_MPC_
code
#endif
const static unsigned value	hexpowers[] =	{1, 0x10, 0x100, 0x1000,
#if	__LONG
						 0x10000, 0x100000, 0x1000000, 0x10000000
#endif
							};
#ifdef	_MPC_
code
#endif
const static unsigned value	octpowers[] =	{1, 010, 0100, 01000, 010000, 0100000,
#ifdef	__LONG
						01000000,
						 010000000, 0100000000, 01000000000, 010000000000,
						 0100000000000
#endif
							};

#ifdef	__FLOAT

#define	NDDIG	(sizeof fdpowers/sizeof fdpowers[0])

#if	DBL_MAX_10_EXP > 120
#define	expon	int
#else
#define	expon	signed char
#endif

extern const double	_powers_[], _npowers_[];
extern unsigned long	_div_to_l_(double, double);

/* this routine returns a value to round to the number of decimal
	places specified */

static double
fround(unsigned char prec)
{
	/* prec is guaranteed to be less than NDIG */

	if(prec > 10)
		return 0.5 * _npowers_[prec/10+9] * _npowers_[prec%10];
	return 0.5 * _npowers_[prec];
}

/* this routine returns a scaling factor equal to 1 to the decimal
   power supplied */

static double
scale(expon scl)
{

	if(scl < 0) {
		scl = -scl;
#if	DBL_MAX_10_EXP >= 400
		if(scl > 1000) {
			double	i;
			i = 1;
			do {
				i *= 1e-100;
			} while((scl -= 100) >= 100);
			return i*_npowers_[scl/10+9] * _npowers_[scl%10];
		}
#endif
#if	DBL_MAX_10_EXP >= 100
		if(scl > 100)
			return _npowers_[scl/100+18]*_npowers_[(scl%100)/10+9] * _npowers_[scl%10];
#endif
		if(scl > 10)
			return _npowers_[scl/10+9] * _npowers_[scl%10];
		return _npowers_[scl];
	}
#if	DBL_MAX_10_EXP >= 400
	if(scl > 1000) {
		double	i;
		i = 1;
		do {
			i *= 1e100;
		} while((scl -= 100) >= 100);
		return i*_npowers_[scl/10+9] * _npowers_[scl%10];
	}
#endif
#if	DBL_MAX_10_EXP >= 100
	if(scl > 100)
		return _powers_[scl/100+18]*_powers_[(scl%100)/10+9] * _powers_[scl%10];
#endif
	if(scl > 10)
		return _powers_[scl/10+9] * _powers_[scl%10];
	return _powers_[scl];
}


#endif	/* __FLOAT */


#define	OPTSIGN	0x00
#define	SPCSIGN	0x01
#define	MANSIGN	0x02
#define	NEGSIGN	0x03
#define	FILL	0x04
#define	LEFT	0x08
#define	LONG	0x10
#define	UPCASE	0x20
#define	TEN	0x00
#define	EIGHT	0x40
#define	SIXTEEN	0x80
#define	UNSIGN	0xC0
#define	BASEM	0xC0
#define	EFMT	0x100
#define	GFMT	0x200
#define	FFMT	0x400
#define	ALTERN	0x800
#define	DEFPREC	0x1000

#ifdef	_HOSTED
#define	pputc(c)	(putc(c, fp) != EOF && ++ccnt)
int
vfprintf(FILE * fp, register const  char * f, register va_list ap)
{
	char		cbuf[2];
#else	/* _HOSTED */
#define	pputc(c)	if(pb->ptr) (*pb->ptr++ = c),++ccnt; else ((pb->func(c)),++ccnt)
int
#ifdef	_MPC_
_doprnt(struct __prbuf * pb, register code  char * f, register va_list ap)
#else
_doprnt(struct __prbuf * pb, register const  char * f, register va_list ap)
#endif
{
#endif	/* _HOSTED */
	int		prec;
	char		c;
	int		width;
	unsigned	flag;
	int		ccnt = 0;
#ifdef	__FLOAT
	double		fval;
	int		exp;
#if	DOUBLE
	double		ival;
#endif
	union {
		unsigned value	_val;
		struct {
#if	i8086 && SMALL_DATA
		    far char *	_cp;
#elif	_MPC_
		    code char *	_cp;
#else
		    char *	_cp;
#endif
		    unsigned	_len;
		}		_str;
		double		_integ;
	}		_val;
#else
	union {
		unsigned value	_val;
		struct {
#if	i8086 && SMALL_DATA
		    far char *	_cp;
#elif	_MPC_
		    code char *	_cp;
#else
		    char *	_cp;
#endif
		    unsigned	_len;
		}		_str;
	}		_val;
#endif
#define	val	_val._val
#define	cp	_val._str._cp
#define	len	_val._str._len
#define	integ	_val._integ

   trace("vfprintf()","Enter.\n");
	
	flag = 0;
	while(c = *f++) {
		if(c != '%') {
			pputc(c);
			continue;
		}
		width = 0;
		flag = 0;
		for(;;) {

			switch(*f) {

			case '-':
				flag |= LEFT;
				f++;
				continue;

			case ' ':
				flag |= SPCSIGN;
				f++;
				continue;

			case '+':
				flag |= MANSIGN;
				f++;
				continue;

			case '#':
				flag |= ALTERN;
				f++;
				continue;

			case '0':
				flag |= FILL;
				f++;
				continue;
			}
			break;
		}
		if(flag & MANSIGN)
			flag &= ~SPCSIGN;
		if(flag & LEFT)
			flag &= ~FILL;
		if(isdigit((unsigned)*f)) {
			width = 0;
			do
				width = width*10 + *f++ - '0';
			while(isdigit((unsigned)*f));
		} else if(*f == '*') {
			width = va_arg(ap, int);
			f++;
		}
		if(*f == '.')
			if(*++f == '*') {
				prec = va_arg(ap, int);
				f++;
			} else {
				prec = 0;
				while(isdigit((unsigned)*f))
					prec = prec*10 + *f++ - '0';
			}
		else {
			prec = 0;
#ifdef	__FLOAT
			flag |= DEFPREC;
#endif
		}
#ifdef	__LONG
loop:
#endif
		switch(c = *f++) {

		case 0: {trace("vfprintf()","Exit.\n"); return ccnt;}

		case 'l':
#ifdef	__LONG
			flag |= LONG;
			goto loop;
#else
			cp = "(non-long printf)";
			goto strings;
#endif

#ifndef	__FLOAT
		case 'E':
		case 'f':
		case 'e':
		case 'g':
			cp = "(non-float printf)";
			goto strings;
#else
		case 'f':
			flag |= FFMT;
			break;

		case 'E':
			flag |= UPCASE;
		case 'e':
			flag |= EFMT;
			break;

		case 'g':
			flag |= GFMT;
			break;
#endif
		case 'o':
			flag |= EIGHT;
			break;

		case 'd':
		case 'i':
			break;

		case 'p':
#if	i8086 && LARGE_DATA
			flag |= LONG;
#endif
		case 'X':
			flag |= UPCASE;
		case 'x':
			flag |= SIXTEEN;
			break;

		case 's':
#if	i8086 && SMALL_DATA
			if(flag & LONG)
				cp = va_arg(ap, far char *);
			else
#endif
#ifdef	_MPC_
				cp = va_arg(ap, code char *);
#else
				cp = va_arg(ap, char *);
#endif
#if	!defined(__FLOAT)
strings:
#endif
			if(!cp)
				cp = "(null)";
			len = 0;
			while(cp[len])
				len++;
dostring:
			if(prec && prec < len)
				len = prec;
			if(width > len)
				width -= len;
			else
				width = 0;
			if(!(flag & LEFT))
				while(width--)
					pputc(' ');
			while(len--)
				pputc(*cp++);
			if(flag & LEFT)
				while(width--)
					pputc(' ');
			continue;
		case 'c':
#if	_HOSTED
			val = va_arg(ap, int);
			c = val >> 8;
			if(flag & LONG && c && (unsigned char)c != 0xFF) {
				cbuf[0] = c;
				cbuf[1] = val;
				len = 2;
			} else {
				cbuf[0] = val;
				len = 1;
			}
			cp = cbuf;
			goto dostring;
#else
			c = va_arg(ap, int);
#endif
		default:
			cp = &c;
			len = 1;
			goto dostring;

		case 'u':
			flag |= UNSIGN;
			break;

		}
#ifdef	__FLOAT
		if(flag & (EFMT|GFMT|FFMT)) {
			if(flag & DEFPREC)
				prec = 6;
			fval = va_arg(ap, double);
			if(fval < 0.0) {
				fval = -fval;
				flag |= NEGSIGN;
			}
			exp = 0;		/* If the number is zero, the exponent is zero. */
			if( fval!=0) {		/* If the number is non-zero, find the exponent. */
				frexp(fval, &exp);		/* get binary exponent */
				exp--;				/* adjust 0.5 -> 1.0 */
				exp *= 3;
				exp /= 10;			/* estimate decimal exponent */
				if(exp < 0)
					exp--;
				integ = fval * scale(-exp);
				if(integ < 1.0)
					exp--;
				else if(integ >= 10.0)
					exp++;
			}
			if(exp <= 0)
				c = 1;
			else
				c = exp;
			if(flag & EFMT || flag & GFMT && (exp < -4 || exp >= (int)prec)) {	/* use e format */
				if(prec && flag & GFMT)
					prec--;		/* g format precision includes integer digit */
				if((unsigned)prec > NDDIG - 2)
					c = NDDIG - 2;
				else
					c = prec;
				if( fval!=0) {	/* Normalise only if the number is non-zero. */
					fval *= scale(c-exp);
#if	DOUBLE
					ival = _dto64i(fval);
					if(fval - _64itod(ival) >= 0.5) {
						fval += 0.5;
						ival = _dto64i(fval);
					}
					if(ival >= fdpowers[c+1]) {
						fval *= 1e-1;
						exp++;
					} else if(ival < fdpowers[c]) {
						fval *= 10.0;
						exp--;
					}
#else	
					if((unsigned long)fval >= fdpowers[c+1]) {
						fval *= 1e-1;
						exp++;
					} else if((unsigned long)fval < fdpowers[c]) {
						fval *= 10.0;
						exp--;
					}
               if(fval - (float)(unsigned long)fval >= 0.5) fval += 0.5;
#endif
				}
				if(flag & GFMT && !(flag & ALTERN)) {		/* g format, precision means something different */
					if(prec > (int)(NDDIG))
						prec = NDDIG;
#if	DOUBLE
					ival = _dto64i(fval);
					while(ival != 0.0 && _mod64i10(ival) == 0) {
						prec--;
						ival = _div64(ival, fdpowers[1]);
					}
#else
					val = (unsigned long)fval;
					while(val && val % 10 == 0) {
						prec--;
						val /= 10;
					}
#endif
					if(prec < c) {
						fval *= scale(prec-c);
						c = prec;
					}

				}
				width -=  prec + 5;
				if(prec || flag & ALTERN)
					width--;
				if(flag & (MANSIGN|SPCSIGN))
					width--;
#if	DBL_MAX_10_EXP >= 100
#if	DBL_MAX_10_EXP >= 1000
				if(exp >= 1000 || exp <= -1000)	/* 3 digit exponent */
					width--;
#endif
				if(exp >= 100 || exp <= -100)	/* 3 digit exponent */
					width--;
#endif
				if(flag & FILL) {
					if(flag & MANSIGN)
						pputc(flag & SPCSIGN ? '-' : '+');
					else if(flag & SPCSIGN)
						pputc(' ');
					while(width > 0) {
						pputc('0');
						width--;
					}
				} else {
					if(!(flag & LEFT))
						while(width > 0) {
							pputc(' ');
							width--;
						}
					if(flag & MANSIGN)
						pputc(flag & SPCSIGN ? '-' : '+');
					else if(flag & SPCSIGN)
						pputc(' ');
				}
#if	DOUBLE
				ival = _dto64i(fval);
				pputc(_div64ir(ival, fdpowers[c]) + '0');
#else
				val = (unsigned long)fval;
				pputc(val/fdpowers[c] + '0');
#endif
				if(prec || flag & ALTERN) {
					pputc('.');
					prec -= c;
					while(c) {
#if	DOUBLE
						pputc('0' + _mod64i10(_div64(ival, fdpowers[--c])));
#else
						pputc('0' + (val/fdpowers[--c]) % 10);
#endif
					}
					while(prec) {
						pputc('0');
						prec--;
					}
				}
				if(flag & UPCASE)
					pputc('E');
				else
					pputc('e');
				if(exp < 0) {
					exp = -exp;
					pputc('-');
				} else
					pputc('+');
#if	DBL_MAX_10_EXP >= 100
#if	DBL_MAX_10_EXP >= 1000
				if(exp >= 1000) {
					pputc(exp / 1000 + '0');
					exp %= 1000;
				}
#endif
				if(exp >= 100) {
					pputc(exp / 100 + '0');
					exp %= 100;
				}
#endif
				pputc(exp / 10 + '0');
				pputc(exp % 10 + '0');
				if((flag & LEFT) && width)
					do
						pputc(' ');
					while(--width);
				continue;
			}
			/* here for f format */

			if(flag & GFMT) {
				if(exp < 0) 
					prec -= exp-1;
#if	DOUBLE
				ival = _dto64i(fval);
				for(c = 1 ; c != NDDIG ; c++)
					if(ival < fdpowers[c])
						break;
				prec -= c;
				ival = _dto64i((fval - _64itod(ival)) * scale(prec)+0.5);
				while(prec && _mod64i10(ival) == 0) {
					prec--;
					ival = _div64(ival, fdpowers[1]);
				}
			}
			if(prec <= (int)NDDIG)
				fval += fround(prec);
			if(exp > (int)(NDDIG)-1) {
				exp -= NDDIG-1;
				ival = _divto64i(fval, scale(exp));
				fval = 0.0;
			} else {
				ival = _dto64i(fval);
				fval -= _64itod(ival);
				exp = 0;
			}
			for(c = 1 ; c != NDDIG ; c++)
				if(ival < fdpowers[c])
					break;
#else
				val = (unsigned long)fval;
				for(c = 1 ; c != NDDIG ; c++)
					if(val < fdpowers[c])
						break;
				prec -= c; if (prec<0) prec=0;
				val = (unsigned long)((fval-(double)val) * scale(prec)+0.5);
				while(prec && val % 10 == 0) {
					val /= 10;
					prec--;
				}
			}
			if(prec <= NDIG)
				fval += fround(prec);
			if(exp > (int)(NDDIG)-2) {
				exp -= NDDIG-2;
				val = _div_to_l_(fval, scale(exp));
				fval = 0.0;
			} else {
				val = (unsigned long)fval;
				fval -= (double)val;
				exp = 0;
			}
			for(c = 1 ; c != NDDIG ; c++)
				if(val < fdpowers[c])
					break;
#endif
			width -= prec + c + exp;
			if(flag & ALTERN || prec)
				width--;
			if(flag & (MANSIGN|SPCSIGN))
				width--;
			if(flag & FILL) {
				if(flag & MANSIGN)
					pputc(flag & SPCSIGN ? '-' : '+');
				else if(flag & SPCSIGN)
					pputc(' ');
				while(width > 0) {
					pputc('0');
					width--;
				}
			} else {
				if(!(flag & LEFT))
					while(width > 0) {
						pputc(' ');
						width--;
					}
				if(flag & MANSIGN)
					pputc(flag & SPCSIGN ? '-' : '+');
				else if(flag & SPCSIGN)
					pputc(' ');
			}
			while(c--)
#if	DOUBLE
				pputc('0' + _mod64i10(_div64(ival, fdpowers[c])));
#else
				pputc('0' + (val/fdpowers[c]) % 10);
#endif
			while(exp > 0) {
				pputc('0');
				exp--;
			}
			if(prec > (int)(NDDIG))
				c = NDDIG;
			else
				c = prec;
			prec -= c;
			if(c || flag & ALTERN)
				pputc('.');
#if	DOUBLE
			ival = _dto64i(fval * scale(c));
			while(c)
				pputc('0' + _mod64i10(_div64(ival, fdpowers[--c])));
#else
			val = (long)(fval * scale(c));
			while(c)
				pputc('0' + (val/fdpowers[--c]) % 10);
#endif
			while(prec) {
				pputc('0');
				prec--;
			}
			if((flag & LEFT) && width)
				do
					pputc(' ');
				while(--width);
			continue;
		}
#endif	/* __FLOAT */
		if((flag & BASEM) == TEN) {
#ifdef	__LONG
			if(flag & LONG)
				val = va_arg(ap, long);
			else
#endif
				val = (value)va_arg(ap, int);
			if((value)val < 0) {
				flag |= NEGSIGN;
				val = -val;
			}
		} else {
#ifdef	__LONG
			if(flag & LONG)
				val = va_arg(ap, unsigned long);
			else
#endif
				val = va_arg(ap, unsigned);
		}
		if(prec == 0 && val == 0)
			prec++;
		switch((unsigned char)(flag & BASEM)) {

		case TEN:
		case UNSIGN:
			for(c = 1 ; c != sizeof dpowers/sizeof dpowers[0] ; c++)
				if(val < dpowers[c])
					break;
			break;
		case SIXTEEN:
			for(c = 1 ; c != sizeof hexpowers/sizeof hexpowers[0] ; c++)
				if(val < hexpowers[c])
					break;
			break;

		case EIGHT:
			for(c = 1 ; c != sizeof octpowers/sizeof octpowers[0] ; c++)
				if(val < octpowers[c])
					break;
			break;
		}
		if(c < prec)
			c = prec;
		else if(prec < c)
			prec = c;
		if(width && flag & NEGSIGN)
			width--;
		if(width > prec)
			width -= prec;
		else
			width = 0;
		if((flag & (FILL|BASEM|ALTERN)) == (EIGHT|ALTERN)) {
			if(width)
				width--;
		} else if((flag & (BASEM|ALTERN)) == (SIXTEEN|ALTERN)) {
			if(width > 2)
				width -= 2;
			else
				width = 0;
		}
		if(flag & FILL) {
			if(flag & MANSIGN)
				pputc(flag & SPCSIGN ? '-' : '+');
			else if(flag & SPCSIGN)
				pputc(' ');
			else if((flag & (BASEM|ALTERN)) == (SIXTEEN|ALTERN)) {
				pputc('0');
				pputc(flag & UPCASE ? 'X' : 'x');
			}
			if(width)
				do
					pputc('0');
				while(--width);
		} else {
			if(width && !(flag & LEFT))
				do
					pputc(' ');
				while(--width);
			if(flag & MANSIGN)
				pputc(flag & SPCSIGN ? '-' : '+');
			else if(flag & SPCSIGN)
				pputc(' ');
			if((flag & (BASEM|ALTERN)) == (EIGHT|ALTERN))
				pputc('0');
			else if((flag & (BASEM|ALTERN)) == (SIXTEEN|ALTERN)) {
				pputc('0');
				pputc(flag & UPCASE ? 'X' : 'x');
			}
		}
		while(prec > c)
			pputc('0');
		while(prec--) {
			switch((unsigned char)(flag & BASEM)) {

			case TEN:
			case UNSIGN:
				c = (val / dpowers[prec]) % 10 + '0';
				break;

			case SIXTEEN:
				c = (flag & UPCASE ? "0123456789ABCDEF" : "0123456789abcdef")[(val / hexpowers[prec]) & 0xF];
				break;

			case EIGHT:
				c = ((val / octpowers[prec]) & 07) + '0';
				break;
			}
			pputc(c);
		}
		if((flag & LEFT) && width)
			do
				pputc(' ');
			while(--width);
	}
 
   trace("vfprintf()","Exit.\n");
	return ccnt;
}
