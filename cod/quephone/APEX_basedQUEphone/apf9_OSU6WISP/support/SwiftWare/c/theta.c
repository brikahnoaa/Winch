/****************************************/
/*       member of hydro library        */
/****************************************/
#include "defs.p"

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * $Id: theta.c,v 1.1 2006/11/18 17:20:24 swift Exp $
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * RCS Log:
 *
 * $Log: theta.c,v $
 * Revision 1.1  2006/11/18 17:20:24  swift
 * Added new functionality to library.
 *
 * Revision 1.1  1996/06/15 05:42:04  swift
 * Initial revision
 *
 *========================================================================*/
/*  function to compute potential temperature of water parcel             */
/*========================================================================*/
/*
   This function uses Bryden's formula for computing the potential
   temperature of a parcel of sea water with state (s,t,p) if it is
   moved adiabatically to another pressure level (refpres).  The
   potential temperature is defined as the final temperature (theta [C])
   the parcel with salinity (s [PSU]) and insitu temperature (t [C])
   will have if it is transported adiabatically from pressure (p [Bars])
   to pressure (refpres [dBars]).  Reference: New Polynomials for
   Thermal Expansion, Adiabatic Temperature Gradient and Potential
   Temperature of Sea Water by Harry L Bryden.  Deep Sea Research,
   1973, Vol 20, pp 401-408.

   Input:
      sal     = (float) salinity [PSU] of water parcel.
      temp    = (float) insitu temperature [C] of parcel.
      pres    = (float) insitu pressure [dBar] of parcel.
      refpres = (float) reference pressure level [dBar] for
                        which potential temperature is to be
                        computed.
   Output:
      theta   = (double) potential temperature [C] at refpres.

   Written by Dana Swift (March 1988)
*/
double potential_temp(float sal,float temp,float pres,float refpres)
{
   int iteration,maxiterations=25;
   double s,t,p,ref,theta;
   double b1,b2,b3,b4,epsilon=5*fEPS,previous_t;

   double   a1  =  3.6504e-5,   a2  =  8.3198e-6,   a3  = -5.4065e-8,
      a4  =  4.0274e-10,  a5  =  1.7439e-6,   a6  = -2.9778e-8,
      a7  = -4.1057e-11,  a8  =  8.9309e-9,   a9  = -3.1628e-10,
      a10 =  2.1987e-12,  a11 = -1.6056e-13,  a12 =  5.0484e-15;

   /* transfer single precision parms to double precision parms */
   s = (sal - 35.0);
   t = temp;
   p = pres;
   ref = refpres;

   /* compute potential temperature for surface refence level */
   if (pres<0 || pres>10000 || temp<-2 || temp>30 || sal<30 || sal>40)
   {
      fprintf(swiftware_stderr,"theta() range error: (30,-2,0) < (s,t,p)=(%6.3f,%6.3f,%4d) < (40,30,10000)\n",sal,temp,(int)pres);
   }

   theta = t - p*(a1 + t*(a2 + t*(a3 +t*a4)) +
                      (s*(a5 + t*a6 + p*a7)) +
                      p*(a8 + t*(a9 + t*a10) + p*(a11 + t*a12)));

   if (ref==0.0) return(theta);

   /* compute theta for nonzero reference pressure */
   b1 = theta + ref*(a1 + ref*(a8 + ref*a11)) + s*(ref*(a5 + ref*a7));
   b2 = ref*(a2 + ref*(a9 + ref*a12)) + s*(ref*a6);
   b3 = ref*(a3 + ref*a10);
   b4 = ref*a4;

   for (iteration=1; iteration<maxiterations; iteration++) {
      previous_t = t;
      t = b1 + t*(b2 + t*(b3 + t*b4));
      if (fabs(previous_t - t) < epsilon) break;
   };

   if (iteration>=maxiterations) {
      /* too many iterations - error in routine */
      printf("Error in potential temperature routine: non-convergent sol'n");
      abort();
   }

   return(t);

}
