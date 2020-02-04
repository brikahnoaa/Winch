
/* single precision machine parameters as ferretted by C-recipes routine */
#define fIBETA  (         2)   /* radix used to represent numbers. */
#define fIT     (        24)   /* number of (base-IBETA) digits in floating point mantissa. */
#define fMACHEP (       -23)   /* exponent of most negative power of IBETA that, added to 1.0, gives something different from 1.0. */
#define fEPS    ( 1.192e-07)   /* the number IBETA**MACHEP...referred to as "floating point precision". */
#define fNEGEP  (       -24)   /* exponent of the smallest power of IBETA that, subtracted from 1.0, gives something different from 1.0. */
#define fEPSNEG ( 5.960e-08)   /* the value IBETA**NEGEP...another way of defining floating  point precision. */
#define fIEXP   (         8)   /* number of bits in the exponent. */
#define fMINEXP (      -126)   /* smallest value of IBETA consistent with there being no leading zeros in mantissa. */
#define fXMIN   ( 1.175e-38)   /* the number IBETA**MINEXP...generally the smallest (in magnitude) useable floating number. */
#define fMAXEXP (       128)   /* the smallest (positive) value of IBETA that causes overflow. */
#define fXMAX   ( 3.403e+38)   /* the value (1-EPSNEG)*IBETA**MAXEXP...generally the largest (in magnitude) useable floating point value. */
#define fIRND   (         5)   /* diagnostic info on rounding characteristics...2,5 means IEEE compliant; 1,4 means non-IEEE rounding; 0,3 means truncation (undesirable). */
#define fNGRD   (         0)   /* number of "guard digits" used when truncating the product of two mantissas to fit representation. */

/* double precision machine parameters as ferretted by C-recipes routine */
#define dIBETA  (         2)   /* radix used to represent numbers. */
#define dIT     (        53)   /* number of (base-IBETA) digits in floating point mantissa. */
#define dMACHEP (       -52)   /* exponent of most negative power of IBETA that, added to 1.0, gives something different from 1.0. */
#define dEPS    ( 2.220e-16)   /* the number IBETA**MACHEP...referred to as "floating point precision". */
#define dNEGEP  (       -53)   /* exponent of the smallest power of IBETA that, subtracted from 1.0, gives something different from 1.0. */
#define dEPSNEG ( 1.110e-16)   /* the value IBETA**NEGEP...another way of defining floating  point precision. */
#define dIEXP   (        11)   /* number of bits in the exponent. */
#define dMINEXP (     -1022)   /* smallest value of IBETA consistent with there being no leading zeros in mantissa. */
#define dXMIN   (2.225e-308)   /* the number IBETA**MINEXP...generally the smallest (in magnitude) useable floating number. */
#define dMAXEXP (      1024)   /* the smallest (positive) value of IBETA that causes overflow. */
#define dXMAX   (1.798e+308)   /* the value (1-EPSNEG)*IBETA**MAXEXP...generally the largest (in magnitude) useable floating point value. */
#define dIRND   (         5)   /* diagnostic info on rounding characteristics...2,5 means IEEE compliant; 1,4 means non-IEEE rounding; 0,3 means truncation (undesirable). */
#define dNGRD   (         0)   /* number of "guard digits" used when truncating the product of two mantissas to fit representation. */
