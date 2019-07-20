/****************************************/
/*       member of hydro library        */
/****************************************/
#include "defs.p"

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * $Id: eqstate.c,v 1.1 2006/11/18 17:20:24 swift Exp $
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * RCS Log:
 *
 * $Log: eqstate.c,v $
 * Revision 1.1  2006/11/18 17:20:24  swift
 * Added new functionality to library.
 *
 * Revision 1.1  1996/06/15 05:35:23  swift
 * Initial revision
 *
 *========================================================================*/
/*   function to compute density of seawater with hydro values s,t,p      */
/*========================================================================*/
/*
   This function computes sigma(s,t,p){=rho(s,t,p)-1000.0 [MKS units]}.
   It uses the international equation of state for seawater given by
   UNESCO (1981).  Reference: Atmosphere-Ocean Interactions by Gill,
   Appendix 3 (Properties of Seawater), 1982, pp 599-601.

   Input:
      sal  = (float) salinity [PSU] of seawater parcel.
      temp = (float) temperature [C] of seawater parcel
                     at pressure (pres [dBar]).
      pres = (float) pressure [dBar] of seawater parcel.

   Output:
      eqstate = sigma(s,t,p) [g/(cu cm)]; density of seawater.

   Written by Dana Swift (March 1988)
*/
double eqstate
   (
      float sal,
      float temp,
      float pres
   )
{
   double t, s, p, sig;
   double rho_0t0, rho_st0, rho_stp, sbm_0t0, sbm_st0, sbm_stp;

   double   a1 = 999.842594,   a2 = 6.793952e-2,   a3 = -9.095290e-3,
      a4 = 1.001685e-4,  a5 = -1.120083e-6,  a6 = 6.536332e-9;

   double   b1 = 0.824493,     b2 = -4.0899e-3,    b3 = 7.6438e-5,
      b4 = -8.2467e-7,   b5 = 5.3875e-9,     b6 = -5.72466e-3,
      b7 = 1.0227e-4,    b8 = -1.6546e-6,    b9 = 4.8314e-4;

   double   c1 = 19652.21,     c2 = 148.4206,      c3 = -2.327105,
      c4 = 1.360477e-2,  c5 = -5.155288e-5;

   double   d1 = 54.6746,      d2 = -0.603459,     d3 = 1.09987e-2,
      d4 = -6.1670e-5,   d5 = 7.944e-2,      d6 = 1.6483e-2,
      d7 = -5.3009e-4;

   double   e1 = 3.239908,     e2 = 1.43713e-3,    e3 = 1.16092e-4,
      e4 = -5.77905e-7,  e5 = 2.2838e-3,     e6 = -1.0981e-5,
      e7 = -1.6078e-6,   e8 = 1.91075e-4,    e9 = 8.50935e-5,
      e10= -6.12293e-6,  e11= 5.2787e-8,     e12= -9.9348e-7,
      e13= 2.0816e-8,    e14= 9.1697e-10;

   /* transfer single precision parameters to double precision */
   t = (double) temp;
   s = (double) sal;
   p = (double) pres/10.0;  /* convert from dBars to Bars */

   /* compute secant bulk modulus of pure water at one atmosphere */
   sbm_0t0 = c1 + t*(c2 +t*(c3 + t*(c4 + t*c5)));

   /* compute secant bulk modulus of sea water at one atmosphere */
   sbm_st0 = sbm_0t0 + s*(d1 + t*(d2 + t*(d3 + t*d4))) +
            pow(s,1.5)*(d5 + t*(d6 + t*d7));

   /* compute secant bulk modulus of sea water at given pressure */
   sbm_stp = sbm_st0 + (p*(e1 + t*(e2 + t*(e3 + t*e4)))) +
         (p*s*(e5 + t*(e6 + t*e7))) + (p*pow(s,1.5)*e8) +
         (p*p*(e9 + t*(e10 + t*e11))) +
         (p*p*s*(e12 + t*(e13 + t*e14)));

   /* compute density of pure water at one atmosphere */
   rho_0t0 = a1 + t*(a2 + t*(a3 + t*(a4 + t*(a5 + t*a6))));

   /* compute density of sea water at one atmosphere */
   rho_st0 = rho_0t0 + s*(b1 + t*(b2 + t*(b3 + t*(b4 + t*b5)))) +
       pow(s,1.5)*(b6 + t*(b7 + t*b8)) + pow(s,2.0)*b9;

   /* compute density of sea water at given pressure */
   rho_stp = rho_st0/(1.0 - (p/sbm_stp));

   /* convert from density to sigma units */
   sig = rho_stp - 1000.0;

   return(sig);
}
