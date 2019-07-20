#include <stdio.h>
#include <string.h>

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * $Id: machar.c,v 1.1.1.1 2003/11/12 23:22:58 swift Exp $
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * RCS Log:
 *
 * $Log: machar.c,v $
 * Revision 1.1.1.1  2003/11/12 23:22:58  swift
 * Apf9 firmware for a 260ml Apex with SBE41 CTD.
 *
 * Revision 1.3  2001/06/03 07:22:15  navo
 * Added prototypes to satisfy compiler.
 *
 * Revision 1.2  1999/01/16 19:42:02  swift
 * Changed return type of `main()' from void to int.
 *
 * Revision 1.1  1996/07/05 01:54:07  swift
 * Initial revision
 *
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

/* declare function prototypes */
void report(char precision_spec,int ibeta,int it,int irnd,int ngrd,int machep,int negep,int iexp,int minexp,int maxexp,double eps,double epsneg,double xmin,double xmax);
void machar(int *ibeta, int *it, int *irnd, int *ngrd,int *machep,
            int *negep, int *iexp, int *minexp, int *maxexp, float *eps,
            float *epsneg, float *xmin, float *xmax);
void dmachar(int *ibeta,int *it,int *irnd,int *ngrd,int *machep,
             int *negep,int *iexp,int *minexp,int *maxexp,double *eps,
             double *epsneg,double *xmin,double *xmax);

/*========================================================================*/
/* program to ferret out machine parameters                               */
/*========================================================================*/
int main(void)
{
   int ibeta,it,irnd,ngrd,machep,negep,iexp,minexp,maxexp;

   { /* ferret out single precision parameters */
      float eps,epsneg,xmin,xmax;

      /* call C-recipes routine for floats */
      machar(&ibeta,&it,&irnd,&ngrd,&machep,&negep,&iexp,&minexp,&maxexp,&eps,&epsneg,&xmin,&xmax);

      /* write the report to stdout */
      report('f',ibeta,it,irnd,ngrd,machep,negep,iexp,minexp,maxexp,eps,epsneg,xmin,xmax);
   }

   { /* ferret out double precision parameters */
      double eps,epsneg,xmin,xmax;

      /* call C-recipes routine for floats */
      dmachar(&ibeta,&it,&irnd,&ngrd,&machep,&negep,&iexp,&minexp,&maxexp,&eps,&epsneg,&xmin,&xmax);

      /* write the report to stdout */
      report('d',ibeta,it,irnd,ngrd,machep,negep,iexp,minexp,maxexp,eps,epsneg,xmin,xmax);
   }

   return 0;
}

/*========================================================================*/
/* procedure to report machine parameters                                 */
/*========================================================================*/
void report
   (
      char precision_spec,
      int ibeta,
      int it,
      int irnd,
      int ngrd,
      int machep,
      int negep,
      int iexp,
      int minexp,
      int maxexp,
      double eps,
      double epsneg,
      double xmin,
      double xmax
   )
{
   char spec[50]="single"; if (precision_spec=='d') strcpy(spec,"double");

   printf("\n/* %s precision machine parameters as ferretted by C-recipes routine */\n",spec);
   printf("#define %cIBETA  (%10d)   /* %s */\n",precision_spec,ibeta,"radix used to represent numbers.");
   printf("#define %cIT     (%10d)   /* %s */\n",precision_spec,it,"number of (base-IBETA) digits in floating point mantissa.");
   printf("#define %cMACHEP (%10d)   /* %s */\n",precision_spec,machep,"exponent of most negative power of IBETA that, added to 1.0, gives something different from 1.0.");
   printf("#define %cEPS    (%10.3e)   /* %s */\n",precision_spec,eps,"the number IBETA**MACHEP...referred to as \"floating point precision\".");
   printf("#define %cNEGEP  (%10d)   /* %s */\n",precision_spec,negep,"exponent of the smallest power of IBETA that, subtracted from 1.0, gives something different from 1.0.");
   printf("#define %cEPSNEG (%10.3e)   /* %s */\n",precision_spec,epsneg,"the value IBETA**NEGEP...another way of defining floating  point precision.");
   printf("#define %cIEXP   (%10d)   /* %s */\n",precision_spec,iexp,"number of bits in the exponent.");
   printf("#define %cMINEXP (%10d)   /* %s */\n",precision_spec,minexp,"smallest value of IBETA consistent with there being no leading zeros in mantissa.");
   printf("#define %cXMIN   (%10.3e)   /* %s */\n",precision_spec,xmin,"the number IBETA**MINEXP...generally the smallest (in magnitude) useable floating number.");
   printf("#define %cMAXEXP (%10d)   /* %s */\n",precision_spec,maxexp,"the smallest (positive) value of IBETA that causes overflow.");
   printf("#define %cXMAX   (%10.3e)   /* %s */\n",precision_spec,xmax,"the value (1-EPSNEG)*IBETA**MAXEXP...generally the largest (in magnitude) useable floating point value.");
   printf("#define %cIRND   (%10d)   /* %s */\n",precision_spec,irnd,"diagnostic info on rounding characteristics...2,5 means IEEE compliant; 1,4 means non-IEEE rounding; 0,3 means truncation (undesirable).");
   printf("#define %cNGRD   (%10d)   /* %s */\n",precision_spec,ngrd,"number of \"guard digits\" used when truncating the product of two mantissas to fit representation.");
}
