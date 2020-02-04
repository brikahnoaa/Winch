/****************************************/
/*       member of utils library        */
/****************************************/
#include <defs.p>

#ifndef PRECISION 
   #define PRECISION float
   #define FINDBIN   findbin
#endif

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * $Id: findbin.c,v 1.1.1.1 2003/11/12 23:22:58 swift Exp $
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * RCS Log:
 *
 * $Log: findbin.c,v $
 * Revision 1.1.1.1  2003/11/12 23:22:58  swift
 * Apf9 firmware for a 260ml Apex with SBE41 CTD.
 *
 * Revision 1.5  2001/03/07 20:13:50  swift
 * Fixed a bug caused when long int's where changed to unsigned long int's.
 *
 * Revision 1.4  1999/01/20 16:04:25  swift
 * Changed unsigned long's to long's.
 *
 * Revision 1.3  1999/01/19 02:34:21  swift
 * Changed int's to unsigned long's to permit larger arrays.
 *
 * Revision 1.2  1997/01/04 22:57:44  swift
 * Added const qualifiers to function argument type list.
 *
 * Revision 1.1  1996/06/15 15:48:29  swift
 * Initial revision
 *
 *========================================================================*/
/* function to determine if a number is within a semiclosed range         */
/*========================================================================*/
/*
   This function finds the bin to which the target value (xtrg) belongs
   within the ordered array x[].  That is, it locates the index (ndx) such
   that the point (xtrg) is between x[ndx] and x[ndx+1].  For best results,
   the array x[] should be ordered and monotonic (either increasing or
   decreasing).  A linear search algorithm is used starting at a user
   supplied guess for the correct index.  

      input:
         x......An array containing the ordered table.
         npnt...The number of elements in the ordered table.
         xtrg...The target value to be bracketed.
         ndx....An initial guess for the bin (x[ndx],x[ndx+1]) containing the
                target value.
         
      output:
         ndx....The index of the interval containing the target value.  If
                the target value is out of the range of the table then (ndx)
                is set to the interval closest to the target.

         This function returns 1 if a bin was found or 0 if the target value
         was out of range of the table.

  written by Dana Swift, Dec 1995 
*/
int FINDBIN(const PRECISION *x,long npnt,const PRECISION xtrg,long *ndx)
{
   PRECISION dx,del;
   long k,ndx_;
   long step=1;

   /* make sure we have more than one point */
   if (npnt<2) {(*ndx) = 0; return (x[0]==xtrg);}

   /* make sure (*ndx) starts out within bounds and compute discrepancy between target and current abscissas */
   if ((*ndx)<0 || (*ndx)>=npnt-1) (*ndx)=0; ndx_=(*ndx); dx = fabs(xtrg-x[ndx_]);

   /* determine loop direction via eqn 3 on p 209 of 1992-93 SwiftWare notebook */
   if (x[(*ndx)]!=x[(*ndx)+1]) step = ((xtrg-x[(*ndx)])/(x[(*ndx)+1]-x[(*ndx)])<0) ? -1 : 1;

   /* locate bracketing abscissas */
   for (k=0; k<npnt; (*ndx)+=step, k++) 
   {
      /* wrap ndx to head of array if necessary */
      if ((*ndx)>npnt-2 && step>0) (*ndx) = 0;

      /* check if target abscissa is in the interval from x[ndx] to x[ndx+1] */
      if (inRange(x[(*ndx)],xtrg,x[(*ndx)+1])) return 1;

      /* keep track of nearest interval so far */
      if ((del=fabs(xtrg-x[(*ndx)])) < dx) {dx=del; ndx_=(*ndx);}

      /* wrap (*ndx) to tail of array if necessary */
      if ((*ndx)<=0 && step<0) (*ndx) = npnt-1;
   }

   /* check if last point matches target else set ndx to nearest interval */
   if (xtrg==x[npnt-1]) {(*ndx)=npnt-2; return 1;} else {(*ndx) = ndx_; return 0;}
}
