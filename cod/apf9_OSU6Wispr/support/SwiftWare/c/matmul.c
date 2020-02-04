/****************************************/
/*       member of math library         */
/****************************************/
#include <defs.p>

#ifndef PRECISION
   #define PRECISION float
   #define MATMUL matmul_
#endif

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * $Id: matmul.c,v 1.1 2006/11/18 17:20:24 swift Exp $
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * RCS Log:
 *
 * $Log: matmul.c,v $
 * Revision 1.1  2006/11/18 17:20:24  swift
 * Added new functionality to library.
 *
 * Revision 1.2  1997/03/07 02:42:49  swift
 * Modifications to permit cross-compilation on different machines
 *
 * Revision 1.1  1996/06/15 16:31:22  swift
 * Initial revision
 *
 *========================================================================*/
/* Procedure to multiply two matricies                                    */
/*========================================================================*/
/*
   Procedure to multiply two matricies                             
                                                                     
   This procedure performs matrix multiplication.  The operation   
   performed is lft[lmr][lmc] *  rt[lmc][rmc] = prd[lmr][rmc].      
                                                                     
   input:
      lft.........Multiplier matrix of dimension lmr x lmc.    
      rt..........Multiplicand matrix of dimension lmc x rmc.  
      lmr.........Row dimension of lft matrix.                     
      lmc.........Column dimension of lft matrix.                  
      l_col_dim...Column dimension of left array in calling procedure.
      r_col_dim...Column dimension of rt array in calling procedure.
      p_col_dim...Column dimension of prd array in calling procedure.

   output:
      prd.........Product matrix of dimension lmr x rmc.
   
   written by Dana Swift (Jan 1988).                               
*/                                                                     
void MATMUL
(
   PRECISION    lft[],
   PRECISION    rt[],
   PRECISION    prd[],
   int         lmr,
   int         lmc,
   int         rmc,
   int         l_col_dim,
   int         r_col_dim,
   int         p_col_dim
)
{
   PRECISION sum;
   int i,j,k;

   for (i=0; i<lmr; i++)
   {
      for (j=0; j<rmc; j++)
      {
         sum=0.0;

         for (k=0; k<lmc; k++)
         {
            sum += (lft[n_(i,k,l_col_dim)]*rt [n_(k,j,r_col_dim)]);
         }

         prd[n_(i,j,p_col_dim)] = sum;
      }
   }
}
