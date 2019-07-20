#ifndef HYDROOBS_OOP
#define HYDROOBS_OOP

#include <defs.oop>
#include <lsprf.oop>

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * $Id: HydroObs.cpp,v 1.1 2006/11/11 18:29:34 swift Exp $
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * RCS Log:
 *
 * $Log: HydroObs.cpp,v $
 * Revision 1.1  2006/11/11 18:29:34  swift
 * Added energy budget model to the project.
 *
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

struct HydroObs {float p,t,s,rho;};

/*========================================================================*/
/*                                                                        */
/*========================================================================*/
class Hydrography
{
   // data members
   protected:

      // define an array of hydrographic observation
      LSplineProfile t,s;
      
   // function members
   public:

      // virtual destructor
      virtual ~Hydrography(void) {}
      
      // function to return the number of points in the profile
      unsigned NPnt(void) {return t.NPnt();}
      
      // function to return the (i)th pressure in the profile
      double P(unsigned int i) {return t.x[i];}
      
      // function to return the maximum pressure in the profile
      double Pmax(void) const {return t.xmax;}

      // function to return the minimum pressure in the profile
      double Pmin(void) const {return t.xmin;}

      virtual void Properties(ostream &dest)=0;
      
      // function to compute the insitu density at a given pressure
      double Rho(float p);

      // function to compute the salinity at a given pressure
      double S(float p);

      // function to compute the temperature at a given pressure
      double T(float p);
};

/*========================================================================*/
/*                                                                        */
/*========================================================================*/
class ArabianSeaHydrography: public Hydrography
{
   // function members
   public:

      // initialization constructor
      ArabianSeaHydrography(void);

      // function to print properties of the profile
      void Properties(ostream &dest=cout);
};

/*========================================================================*/
/*                                                                        */
/*========================================================================*/
class BayOfBengalHydrography: public Hydrography
{
   // function members
   public:

      // initialization constructor
      BayOfBengalHydrography(void);

      // function to print properties of the profile
      void Properties(ostream &dest=cout);
};

/*========================================================================*/
/*                                                                        */
/*========================================================================*/
class BlackSeaHydrography: public Hydrography
{
   // function members
   public:

      // initialization constructor
      BlackSeaHydrography(void);

      // function to print properties of the profile
      void Properties(ostream &dest=cout);
};

/*========================================================================*/
/*                                                                        */
/*========================================================================*/
class EqPacificHydrography: public Hydrography
{
   // function members
   public:

      // initialization constructor
      EqPacificHydrography(void);

      // function to print properties of the profile
      void Properties(ostream &dest=cout);
};

/*========================================================================*/
/*                                                                        */
/*========================================================================*/
class GulfOfMexicoHydrography: public Hydrography
{
   // function members
   public:

      // initialization constructor
      GulfOfMexicoHydrography(void);

      // function to print properties of the profile
      void Properties(ostream &dest=cout);
};

/*========================================================================*/
/*                                                                        */
/*========================================================================*/
class HawaiiPacificHydrography: public Hydrography
{
   // function members
   public:

      // initialization constructor
      HawaiiPacificHydrography(void);

      // function to print properties of the profile
      void Properties(ostream &dest=cout);
};

/*========================================================================*/
/*                                                                        */
/*========================================================================*/
class JapanSeaHydrography: public Hydrography
{
   // function members
   public:

      // initialization constructor
      JapanSeaHydrography(void);

      // function to print properties of the profile
      void Properties(ostream &dest=cout);
};

/*========================================================================*/
/*                                                                        */
/*========================================================================*/
class KurishioHydrography: public Hydrography
{
   // function members
   public:

      // initialization constructor
      KurishioHydrography(void);

      // function to print properties of the profile
      void Properties(ostream &dest=cout);
};

/*========================================================================*/
/*                                                                        */
/*========================================================================*/
class LaboradorSeaHydrography: public Hydrography
{
   // function members
   public:

      // initialization constructor
      LaboradorSeaHydrography(void);

      // function to print properties of the profile
      void Properties(ostream &dest=cout);
};

/*========================================================================*/
/*                                                                        */
/*========================================================================*/
class SargassoHydrography: public Hydrography
{
   // function members
   public:

      // initialization constructor
      SargassoHydrography(void);

      // function to print properties of the profile
      void Properties(ostream &dest=cout);
};

/*========================================================================*/
/*                                                                        */
/*========================================================================*/
class SouthPacificNoppHydrography: public Hydrography
{
   // function members
   public:

      // initialization constructor
      SouthPacificNoppHydrography(void);

      // function to print properties of the profile
      void Properties(ostream &dest=cout);
};

/*========================================================================*/
/*                                                                        */
/*========================================================================*/
class SIndianOceanHydrography: public Hydrography
{
   // function members
   public:

      // initialization constructor
      SIndianOceanHydrography(void);

      // function to print properties of the profile
      void Properties(ostream &dest=cout);
};

/*========================================================================*/
/*                                                                        */
/*========================================================================*/
class SubtropicalNePacificHydrography: public Hydrography
{
   // function members
   public:

      // initialization constructor
      SubtropicalNePacificHydrography(void);

      // function to print properties of the profile
      void Properties(ostream &dest=cout);
};

/*========================================================================*/
/*                                                                        */
/*========================================================================*/
class TropicalAtlanticHydrography: public Hydrography
{
   // function members
   public:

      // initialization constructor
      TropicalAtlanticHydrography(void);

      // function to print properties of the profile
      void Properties(ostream &dest=cout);
};

#ifdef HYDROOBS_CPP

/*------------------------------------------------------------------------*/
/*                                                                        */
/*------------------------------------------------------------------------*/
void ArabianSeaHydrography::Properties(ostream &dest)
{
   fprintf(dest,"$ Hydrography:    %s\n"
             "$    %6s %7s %7s %10s\n"
             "$    %6s %7s %7s %10s\n",
             "Arabian Sea (21.50N 63.41E) Dec-21-1986",
             "pres","temp","sal","density",
             "dbar","C","PSU","g/ml"
             );

   for (unsigned int i=0; i<t.NPnt(); i++)
   {
      double rho = 1 + eqstate(s.y[i],t.y[i],t.x[i])/1000;

      fprintf(dest,"$    %6.1f %7.3f %7.3f %10.6f\n",
                t.x[i],t.y[i],s.y[i],rho);
   }
   fprintf(dest,"$\n");
}

/*------------------------------------------------------------------------*/
/*                                                                        */
/*------------------------------------------------------------------------*/
void BayOfBengalHydrography::Properties(ostream &dest)
{
   fprintf(dest,"$ Hydrography:    %s\n"
             "$    %6s %7s %7s %10s\n"
             "$    %6s %7s %7s %10s\n",
             "Arabian Sea (21.50N 63.41E) Dec-21-1986",
             "pres","temp","sal","density",
             "dbar","C","PSU","g/ml"
             );

   for (unsigned int i=0; i<t.NPnt(); i++)
   {
      double rho = 1 + eqstate(s.y[i],t.y[i],t.x[i])/1000;

      fprintf(dest,"$    %6.1f %7.3f %7.3f %10.6f\n",
                t.x[i],t.y[i],s.y[i],rho);
   }
   fprintf(dest,"$\n");
}

/*------------------------------------------------------------------------*/
/*                                                                        */
/*------------------------------------------------------------------------*/
void BlackSeaHydrography::Properties(ostream &dest)
{
   fprintf(dest,"$ Hydrography:    %s\n"
             "$    %6s %7s %7s %10s\n"
             "$    %6s %7s %7s %10s\n",
             "Black Sea (42.51N 30.74E) May-26-2001",
             "pres","temp","sal","density",
             "dbar","C","PSU","g/ml"
             );

   for (unsigned int i=0; i<t.NPnt(); i++)
   {
      double rho = 1 + eqstate(s.y[i],t.y[i],t.x[i])/1000;

      fprintf(dest,"$    %6.1f %7.3f %7.3f %10.6f\n",
                t.x[i],t.y[i],s.y[i],rho);
   }
   fprintf(dest,"$\n");
}

/*------------------------------------------------------------------------*/
/*                                                                        */
/*------------------------------------------------------------------------*/
void EqPacificHydrography::Properties(ostream &dest)
{
   fprintf(dest,"$ Hydrography:    %s\n"
             "$    %6s %7s %7s %10s\n"
             "$    %6s %7s %7s %10s\n",
             "Equatorial Pacific (5.00N 110.0WE) Jan-12-2004",
             "pres","temp","sal","density",
             "dbar","C","PSU","g/ml"
             );

   for (unsigned int i=0; i<t.NPnt(); i++)
   {
      double rho = 1 + eqstate(s.y[i],t.y[i],t.x[i])/1000;

      fprintf(dest,"$    %6.1f %7.3f %7.3f %10.6f\n",
                t.x[i],t.y[i],s.y[i],rho);
   }
   fprintf(dest,"$\n");
}

/*------------------------------------------------------------------------*/
/*                                                                        */
/*------------------------------------------------------------------------*/
void GulfOfMexicoHydrography::Properties(ostream &dest)
{
   fprintf(dest,"$ Hydrography:    %s\n"
             "$    %6s %7s %7s %10s\n"
             "$    %6s %7s %7s %10s\n",
             "Gulf of Mexico (24.93N, 84.58W) Nov-12-1983",
             "pres","temp","sal","density",
             "dbar","C","PSU","g/ml"
             );

   for (unsigned int i=0; i<t.NPnt(); i++)
   {
      double rho = 1 + eqstate(s.y[i],t.y[i],t.x[i])/1000;

      fprintf(dest,"$    %6.1f %7.3f %7.3f %10.6f\n",
                t.x[i],t.y[i],s.y[i],rho);
   }
   fprintf(dest,"$\n");
}

/*------------------------------------------------------------------------*/
/*                                                                        */
/*------------------------------------------------------------------------*/
void HawaiiPacificHydrography::Properties(ostream &dest)
{
   fprintf(dest,"$ Hydrography:    %s\n"
             "$    %6s %7s %7s %10s\n"
             "$    %6s %7s %7s %10s\n",
             "Hawaii-Pacific (21.85N, 155.03W) Sep-09-1973",
             "pres","temp","sal","density",
             "dbar","C","PSU","g/ml"
             );

   for (unsigned int i=0; i<t.NPnt(); i++)
   {
      double rho = 1 + eqstate(s.y[i],t.y[i],t.x[i])/1000;

      fprintf(dest,"$    %6.1f %7.3f %7.3f %10.6f\n",
                t.x[i],t.y[i],s.y[i],rho);
   }
   fprintf(dest,"$\n");
}

/*------------------------------------------------------------------------*/
/*                                                                        */
/*------------------------------------------------------------------------*/
void JapanSeaHydrography::Properties(ostream &dest)
{
   fprintf(dest,"$ Hydrography:    %s\n"
             "$    %6s %7s %7s %10s\n"
             "$    %6s %7s %7s %10s\n",
             "Japan Sea (39.33N, 133.90E) Sep-09-1999 JES float 195",
             "pres","temp","sal","density",
             "dbar","C","PSU","g/ml"
             );

   for (unsigned int i=0; i<t.NPnt(); i++)
   {
      double rho = 1 + eqstate(s.y[i],t.y[i],t.x[i])/1000;

      fprintf(dest,"$    %6.1f %7.3f %7.3f %10.6f\n",
                t.x[i],t.y[i],s.y[i],rho);
   }
   fprintf(dest,"$\n");
}

/*------------------------------------------------------------------------*/
/*                                                                        */
/*------------------------------------------------------------------------*/
void KurishioHydrography::Properties(ostream &dest)
{
   fprintf(dest,"$ Hydrography:    %s\n"
             "$    %6s %7s %7s %10s\n"
             "$    %6s %7s %7s %10s\n",
             "Kurishio (33.83N, 141.20W) Feb-14-1988",
             "pres","temp","sal","density",
             "dbar","C","PSU","g/ml"
             );

   for (unsigned int i=0; i<t.NPnt(); i++)
   {
      double rho = 1 + eqstate(s.y[i],t.y[i],t.x[i])/1000;

      fprintf(dest,"$    %6.1f %7.3f %7.3f %10.6f\n",
                t.x[i],t.y[i],s.y[i],rho);
   }
   fprintf(dest,"$\n");
}

/*------------------------------------------------------------------------*/
/*                                                                        */
/*------------------------------------------------------------------------*/
void LaboradorSeaHydrography::Properties(ostream &dest)
{
   fprintf(dest,"$ Hydrography:    %s\n"
             "$    %6s %7s %7s %10s\n"
             "$    %6s %7s %7s %10s\n",
             "LaboradorSea (57.50N, 56.0W) Jul-30-2003",
             "pres","temp","sal","density",
             "dbar","C","PSU","g/ml"
             );

   for (unsigned int i=0; i<t.NPnt(); i++)
   {
      double rho = 1 + eqstate(s.y[i],t.y[i],t.x[i])/1000;

      fprintf(dest,"$    %6.1f %7.3f %7.3f %10.6f\n",
                t.x[i],t.y[i],s.y[i],rho);
   }
   fprintf(dest,"$\n");
}

/*------------------------------------------------------------------------*/
/*                                                                        */
/*------------------------------------------------------------------------*/
void SargassoHydrography::Properties(ostream &dest)
{
   fprintf(dest,"$ Hydrography:    %s\n"
             "$    %6s %7s %7s %10s\n"
             "$    %6s %7s %7s %10s\n",
             "Sargasso Region (32.56N, 64.23W) Sep-08-2000 NOPP float 422",
             "pres","temp","sal","density",
             "dbar","C","PSU","g/ml"
             );

   for (unsigned int i=0; i<t.NPnt(); i++)
   {
      double rho = 1 + eqstate(s.y[i],t.y[i],t.x[i])/1000;

      fprintf(dest,"$    %6.1f %7.3f %7.3f %10.6f\n",
                t.x[i],t.y[i],s.y[i],rho);
   }
   fprintf(dest,"$\n");
}

/*------------------------------------------------------------------------*/
/*                                                                        */
/*------------------------------------------------------------------------*/
void SIndianOceanHydrography::Properties(ostream &dest)
{
   fprintf(dest,"$ Hydrography:    %s\n"
             "$    %6s %7s %7s %10s\n"
             "$    %6s %7s %7s %10s\n",
             "South Indian Ocean (27.0S 60.2E) Nov-04-2006 (5076.003.edf)",
             "pres","temp","sal","density",
             "dbar","C","PSU","g/ml"
             );

   for (unsigned int i=0; i<t.NPnt(); i++)
   {
      double rho = 1 + eqstate(s.y[i],t.y[i],t.x[i])/1000;

      fprintf(dest,"$    %6.1f %7.3f %7.3f %10.6f\n",
                t.x[i],t.y[i],s.y[i],rho);
   }
   fprintf(dest,"$\n");
}

/*------------------------------------------------------------------------*/
/*                                                                        */
/*------------------------------------------------------------------------*/
void SouthPacificNoppHydrography::Properties(ostream &dest)
{
   fprintf(dest,"$ Hydrography:    %s\n"
             "$    %6s %7s %7s %10s\n"
             "$    %6s %7s %7s %10s\n",
             "South Pacific NOPP Region (12.10S, 129.76W) Nov-14-1999 NOPP float 421",
             "pres","temp","sal","density",
             "dbar","C","PSU","g/ml"
             );

   for (unsigned int i=0; i<t.NPnt(); i++)
   {
      double rho = 1 + eqstate(s.y[i],t.y[i],t.x[i])/1000;

      fprintf(dest,"$    %6.1f %7.3f %7.3f %10.6f\n",
                t.x[i],t.y[i],s.y[i],rho);
   }
   fprintf(dest,"$\n");
}

/*------------------------------------------------------------------------*/
/*                                                                        */
/*------------------------------------------------------------------------*/
void SubtropicalNePacificHydrography::Properties(ostream &dest)
{
   fprintf(dest,"$ Hydrography:    %s\n"
             "$    %6s %7s %7s %10s\n"
             "$    %6s %7s %7s %10s\n",
             "Subtropical NE Pacific (36.60N, 123.10W)",
             "pres","temp","sal","density",
             "dbar","C","PSU","g/ml"
             );

   for (unsigned int i=0; i<t.NPnt(); i++)
   {
      double rho = 1 + eqstate(s.y[i],t.y[i],t.x[i])/1000;

      fprintf(dest,"$    %6.1f %7.3f %7.3f %10.6f\n",
                t.x[i],t.y[i],s.y[i],rho);
   }
   fprintf(dest,"$\n");
}

/*------------------------------------------------------------------------*/
/*                                                                        */
/*------------------------------------------------------------------------*/
void TropicalAtlanticHydrography::Properties(ostream &dest)
{
   fprintf(dest,"$ Hydrography:    %s\n"
             "$    %6s %7s %7s %10s\n"
             "$    %6s %7s %7s %10s\n",
             "Tropical Atlantic (0.23S, 2.33E)",
             "pres","temp","sal","density",
             "dbar","C","PSU","g/ml"
             );

   for (unsigned int i=0; i<t.NPnt(); i++)
   {
      double rho = 1 + eqstate(s.y[i],t.y[i],t.x[i])/1000;

      fprintf(dest,"$    %6.1f %7.3f %7.3f %10.6f\n",
                t.x[i],t.y[i],s.y[i],rho);
   }
   fprintf(dest,"$\n");
}

/*------------------------------------------------------------------------*/
/*                                                                        */
/*------------------------------------------------------------------------*/
ArabianSeaHydrography::ArabianSeaHydrography(void)
{
   HydroObs pts[]=
   {
      //    lat     lon       date   time 
      //  21.50   63.41 12/21/1986 19.400 
      //     p       t       s   //  theta  sigma
      {    0.0, 25.010, 36.400}, // 25.005 24.400
      {   23.1, 25.010, 36.400}, // 25.005 24.400
      {   73.2, 24.990, 36.390}, // 24.974 24.402
      {   99.3, 22.220, 36.160}, // 22.200 25.045
      {  148.5, 19.330, 35.900}, // 19.303 25.632
      {  198.7, 17.790, 35.920}, // 17.756 26.038
      {  275.2, 17.160, 36.320}, // 17.114 26.502
      {  401.0, 14.250, 36.000}, // 14.190 26.920
      {  476.5, 13.470, 35.890}, // 13.402 27.001
      {  602.5, 12.020, 35.690}, // 11.940 27.137
      {  803.2, 10.520, 35.560}, // 10.420 27.316
      { 1006.1,  9.000, 35.420}, //  8.885 27.466
      { 1208.3,  7.530, 35.260}, //  7.404 27.567
   };
   unsigned int n = sizeof(pts)/sizeof(HydroObs);
   
   // make sure there are at least 3 points in the profile
   if (n<3)
   {
      swifterr("error in ArabianSeaHydrography::ArabianSeaHydrography() ... "
               "Hydrographic profile must contain at least 3 points\n");
   }
   
   // make sure the profile starts at the surface
   if (pts[0].p!=0)
   {
      swifterr("error in ArabianSeaHydrography::ArabianSeaHydrography() ... "
               "First observation in profile must be at surface.\n");
   }
   
   // make sure the pressure increases monotonically
   for (unsigned int i=1; i<n; i++)
   {
      if (pts[i].p<=pts[i-1].p)
      {
         swifterr("error in ArabianSeaHydrography::ArabianSeaHydrography() ... "
                  "Hydrographic profile not monotonically increasing in pressure.\n");
      }
   }

   // load the profiles into the linear spline objects
   for (unsigned int i=0; i<n; i++)
   {
      t.Append(pts[i].p,pts[i].t); s.Append(pts[i].p,pts[i].s);
   }

   // seek extremes in the profiles
   t.SeekExtremes(); s.SeekExtremes();
   
   // make sure the density increases monotonically
   for (unsigned int i=1; i<n; i++)
   {
      if (Rho(pts[i].p)<=Rho(pts[i-1].p))
      {
         swifterr("error in ArabianSeaHydrography::ArabianSeaHydrography() ... "
                  "Hydrographic profile not monotonically increasing in density.\n");
      }
   }
}

/*------------------------------------------------------------------------*/
/*                                                                        */
/*------------------------------------------------------------------------*/
BayOfBengalHydrography::BayOfBengalHydrography(void)
{
   HydroObs pts[]=
   {
      //    lat     lon       date   time
      //  14.74   81.82 11/09/2002  3.711
      //   p        t        s   //   theta    sigma
      {   0.0,  27.973,  27.520}, //  27.970  16.7930
      {  14.1,  27.973,  27.520}, //  27.970  16.7930      
      {  19.8,  28.240,  28.522}, //  28.235  17.4587
      {  29.8,  28.445,  31.879}, //  28.438  19.9097
      {  39.8,  28.543,  33.459}, //  28.534  21.0641
      {  49.6,  28.583,  33.927}, //  28.571  21.4031
      {  59.8,  27.894,  34.717}, //  27.880  22.2232
      {  69.6,  25.772,  34.899}, //  25.757  23.0347
      {  79.4,  24.037,  34.946}, //  24.020  23.5962
      {  89.6,  23.404,  34.909}, //  23.385  23.7546
      {  99.7,  22.649,  34.946}, //  22.629  24.0006
      { 109.6,  21.134,  34.889}, //  21.113  24.3800
      { 119.6,  20.604,  34.895}, //  20.581  24.5284
      { 139.5,  19.286,  34.932}, //  19.261  24.9035
      { 159.6,  18.121,  34.982}, //  18.093  25.2363
      { 179.6,  16.601,  34.915}, //  16.572  25.5509
      { 199.5,  15.094,  34.956}, //  15.064  25.9248
      { 224.5,  13.797,  34.994}, //  13.765  26.2324
      { 249.2,  12.827,  35.015}, //  12.793  26.4466
      { 274.0,  12.490,  35.019}, //  12.453  26.5167
      { 298.8,  12.031,  35.031}, //  11.992  26.6153
      { 324.5,  11.667,  35.031}, //  11.625  26.6848
      { 349.6,  11.350,  35.031}, //  11.306  26.7442
      { 374.1,  11.088,  35.033}, //  11.041  26.7942
      { 399.2,  10.814,  35.032}, //  10.765  26.8434
      { 449.3,  10.441,  35.030}, //  10.387  26.9088
      { 499.0,  10.031,  35.034}, //   9.972  26.9837
      { 549.3,   9.777,  35.029}, //   9.713  27.0238
      { 599.3,   9.480,  35.017}, //   9.411  27.0648
      { 645.0,   9.216,  35.018}, //   9.143  27.1095
      { 784.3,   8.860,  35.185},
      { 986.1,   8.080,  35.222},
      {1248.8,   6.000,  35.005},
      {1547.2,   4.480,  34.901},
      {1843.1,   3.420,  34.847},
      {2135.3,   2.440,  34.790},
      {2426.9,   2.140,  34.765},
      {2724.0,   1.920,  34.760},
      {3029.6,   1.760,  34.761},
      {3182.6,   1.700,  34.746}
   };
   unsigned int n = sizeof(pts)/sizeof(HydroObs);
   
   // make sure there are at least 3 points in the profile
   if (n<3)
   {
      swifterr("error in BayOfBengalHydrography::BayOfBengalHydrography() ... "
               "Hydrographic profile must contain at least 3 points\n");
   }
   
   // make sure the profile starts at the surface
   if (pts[0].p!=0)
   {
      swifterr("error in BayOfBengalHydrography::BayOfBengalHydrography() ... "
               "First observation in profile must be at surface.\n");
   }
   
   // make sure the pressure increases monotonically
   for (unsigned int i=1; i<n; i++)
   {
      if (pts[i].p<=pts[i-1].p)
      {
         swifterr("error in BayOfBengalHydrography::BayOfBengalHydrography() ... "
                  "Hydrographic profile not monotonically increasing in pressure.\n");
      }
   }

   // load the profiles into the linear spline objects
   for (unsigned int i=0; i<n; i++)
   {
      t.Append(pts[i].p,pts[i].t); s.Append(pts[i].p,pts[i].s);
   }

   // seek extremes in the profiles
   t.SeekExtremes(); s.SeekExtremes();
   
   // make sure the density increases monotonically
   for (unsigned int i=1; i<n; i++)
   {
      if (Rho(pts[i].p)<=Rho(pts[i-1].p))
      {
         swifterr("error in BayOfBengalHydrography::BayOfBengalHydrography() ... "
                  "Hydrographic profile not monotonically increasing in density.\n");
      }
   }
}

/*------------------------------------------------------------------------*/
/*                                                                        */
/*------------------------------------------------------------------------*/
BlackSeaHydrography::BlackSeaHydrography(void)
{
   HydroObs pts[]=
   {
      //   lat     lon       date   time zmax    n
      // 42.51   30.74 05/26/2001 10.982 2097 2096
      //     p        t        s
      {    0.0, 15.5340, 18.0722},
      {    5.0, 15.4726, 18.0719},
      {   10.0, 15.2263, 18.0724},
      {   15.0, 14.8097, 18.0751},
      {   20.0, 13.0517, 18.0763},
      {   25.0,  9.9502, 18.0831},
      {   30.0,  9.1794, 18.1036},
      {   40.0,  8.5238, 18.1158},
      {   50.0,  8.0191, 18.2878},
      {   60.0,  7.7286, 19.0419},
      {   70.0,  7.8680, 19.5805},
      {   80.0,  7.9812, 19.9843},
      {   90.0,  8.1186, 20.3591},
      {  100.0,  8.2126, 20.5830},
      {  110.0,  8.3072, 20.7834},
      {  120.0,  8.3723, 20.9102},
      {  130.0,  8.4283, 21.0314},
      {  140.0,  8.4895, 21.1355},
      {  150.0,  8.5413, 21.2216},
      {  160.0,  8.5817, 21.2925},
      {  170.0,  8.6149, 21.3577},
      {  180.0,  8.6510, 21.4195},
      {  190.0,  8.6764, 21.4735},
      {  200.0,  8.7104, 21.5157},
      {  250.0,  8.7768, 21.6945},
      {  300.0,  8.8189, 21.8097},
      {  350.0,  8.8435, 21.8941},
      {  400.0,  8.8600, 21.9605},
      {  450.0,  8.8724, 22.0176},
      {  500.0,  8.8803, 22.0645},
      {  600.0,  8.8926, 22.1401},
      {  700.0,  8.9046, 22.1964},
      {  800.0,  8.9190, 22.2354},
      {  900.0,  8.9358, 22.2669},
      { 1000.0,  8.9535, 22.2897},
      { 1100.0,  8.9698, 22.3039},
      { 1200.0,  8.9855, 22.3138},
      { 1300.0,  9.0013, 22.3209},
      { 1400.0,  9.0160, 22.3256},
      { 1500.0,  9.0307, 22.3293},
      { 1550.0,  9.0378, 22.3306},
      { 1600.0,  9.0451, 22.3322},
      { 1700.0,  9.0594, 22.3341},
      { 1800.0,  9.0783, 22.3378},
      { 1900.0,  9.0898, 22.3380},
      { 2000.0,  9.1015, 22.3380},
      { 2097.0,  9.1130, 22.3380},
   };
   unsigned int n = sizeof(pts)/sizeof(HydroObs);
   
   // make sure there are at least 3 points in the profile
   if (n<3)
   {
      swifterr("error in BlackSeaHydrography::BlackSeaHydrography() ... "
               "Hydrographic profile must contain at least 3 points\n");
   }
   
   // make sure the profile starts at the surface
   if (pts[0].p!=0)
   {
      swifterr("error in BlackSeaHydrography::BlackSeaHydrography() ... "
               "First observation in profile must be at surface.\n");
   }
   
   // make sure the pressure increases monotonically
   for (unsigned int i=1; i<n; i++)
   {
      if (pts[i].p<=pts[i-1].p)
      {
         swifterr("error in BlackSeaHydrography::BlackSeaHydrography() ... "
                  "Hydrographic profile not monotonically increasing in pressure.\n");
      }
   }

   // load the profiles into the linear spline objects
   for (unsigned int i=0; i<n; i++)
   {
      t.Append(pts[i].p,pts[i].t); s.Append(pts[i].p,pts[i].s);
   }

   // seek extremes in the profiles
   t.SeekExtremes(); s.SeekExtremes();
   
   // make sure the density increases monotonically
   for (unsigned int i=1; i<n; i++)
   {
      if (Rho(pts[i].p)<=Rho(pts[i-1].p))
      {
         swifterr("error in BlackSeaHydrography::BlackSeaHydrography() ... "
                  "Hydrographic profile not monotonically increasing in density.\n");
      }
   }
}

/*------------------------------------------------------------------------*/
/*                                                                        */
/*------------------------------------------------------------------------*/
EqPacificHydrography::EqPacificHydrography(void)
{
   HydroObs pts[]=
   {
      //    lat     lon       date   time  zmax    stnid      n
      //   5.00  242.99 01/121008 0003.001     52
      //      p        t        s       theta    sigma
      {     0.0,  26.392,  34.347}, // 26.391  22.4207
      {     6.6,  26.392,  34.347}, // 26.391  22.4207
      {    10.5,  26.354,  34.351}, // 26.352  22.4359
      {    20.3,  26.265,  34.392}, // 26.260  22.4954
      {    30.5,  26.114,  34.475}, // 26.107  22.6059
      {    39.7,  26.065,  34.551}, // 26.056  22.6791
      {    50.1,  25.998,  34.602}, // 25.987  22.7391
      {    60.2,  25.974,  34.624}, // 25.961  22.7639
      {    70.1,  25.505,  34.685}, // 25.490  22.9555
      {    80.5,  24.468,  34.758}, // 24.451  23.3257
      {    90.5,  21.933,  34.788}, // 21.915  24.0819
      {   100.7,  17.853,  34.608}, // 17.836  25.0131
      {   110.5,  16.100,  34.640}, // 16.083  25.4527
      {   120.2,  14.349,  34.714}, // 14.331  25.8967
      {   130.4,  13.128,  34.761}, // 13.110  26.1862
      {   140.1,  12.317,  34.717}, // 12.299  26.3125
      {   150.0,  11.992,  34.735}, // 11.973  26.3890
      {   159.6,  11.779,  34.753}, // 11.759  26.4436
      {   169.5,  11.728,  34.767}, // 11.706  26.4643
      {   180.5,  11.597,  34.766}, // 11.574  26.4883
      {   190.6,  11.444,  34.762}, // 11.420  26.5138
      {   199.9,  11.249,  34.750}, // 11.224  26.5405
      {   210.3,  10.991,  34.728}, // 10.965  26.5705
      {   219.9,  10.992,  34.760}, // 10.965  26.5954
      {   230.1,  10.823,  34.746}, // 10.795  26.6151
      {   240.5,  10.684,  34.737}, // 10.655  26.6330
      {   250.0,  10.539,  34.729}, // 10.509  26.6525
      {   259.8,  10.468,  34.724}, // 10.437  26.6613
      {   270.1,  10.333,  34.717}, // 10.301  26.6795
      {   280.5,  10.257,  34.712}, // 10.224  26.6890
      {   290.4,  10.125,  34.706}, // 10.091  26.7072
      {   300.1,  10.025,  34.701}, //  9.990  26.7206
      {   310.0,   9.879,  34.692}, //  9.843  26.7385
      {   320.5,   9.849,  34.691}, //  9.812  26.7429
      {   330.0,   9.802,  34.688}, //  9.764  26.7487
      {   339.8,   9.661,  34.681}, //  9.622  26.7670
      {   350.3,   9.513,  34.675}, //  9.473  26.7870
      {   360.0,   9.414,  34.670}, //  9.374  26.7995
      {   380.2,   9.156,  34.660}, //  9.114  26.8339
      {   399.7,   8.948,  34.652}, //  8.904  26.8612
      {   450.2,   8.497,  34.634}, //  8.449  26.9184
      {   500.0,   7.876,  34.611}, //  7.825  26.9946
      {   550.0,   7.435,  34.594}, //  7.381  27.0457
      {   600.2,   6.766,  34.573}, //  6.710  27.1224
      {   650.0,   6.443,  34.565}, //  6.383  27.1595
      {   700.0,   5.959,  34.555}, //  5.897  27.2142
      {   750.4,   5.542,  34.553}, //  5.478  27.2645
      {   800.4,   5.228,  34.555}, //  5.161  27.3038
      {   850.3,   5.038,  34.557}, //  4.968  27.3279
      {   899.2,   4.859,  34.559}, //  4.786  27.3503
      {   949.8,   4.659,  34.564}, //  4.583  27.3770
      {   997.7,   4.495,  34.569}, //  4.416  27.3993
      {  1008.4,   4.434,  34.567}, //  4.354  27.4043
      {  1111.0,   4.016,  34.505}, //  3.931  27.400
      {  1212.1,   3.737,  34.527}, //  3.646  27.446
      {  1313.3,   3.444,  34.543}, //  3.347  27.488
      {  1415.5,   3.243,  34.555}, //  3.139  27.517
      {  1515.7,   3.035,  34.564}, //  2.925  27.544
      {  1617.1,   2.834,  34.576}, //  2.718  27.572
      {  1719.4,   2.664,  34.585}, //  2.542  27.595
      {  1819.8,   2.518,  34.595}, //  2.389  27.616
      {  1922.3,   2.371,  34.604}, //  2.235  27.635
      {  2023.8,   2.219,  34.613}, //  2.077  27.656
      {  2227.0,   2.028,  34.626}, //  1.871  27.682
      {  2434.5,   1.895,  34.639}, //  1.722  27.704
      {  2636.0,   1.737,  34.648}, //  1.549  27.724
      {  2837.7,   1.650,  34.657}, //  1.445  27.739
      {  3041.7,   1.583,  34.662}, //  1.360  27.749
   };
   unsigned int n = sizeof(pts)/sizeof(HydroObs);
   
   // make sure there are at least 3 points in the profile
   if (n<3)
   {
      swifterr("error in EqPacificHydrography::EqPacificHydrography() ... "
               "Hydrographic profile must contain at least 3 points\n");
   }
   
   // make sure the profile starts at the surface
   if (pts[0].p!=0)
   {
      swifterr("error in EqPacificHydrography::EqPacificHydrography() ... "
               "First observation in profile must be at surface.\n");
   }
   
   // make sure the pressure increases monotonically
   for (unsigned int i=1; i<n; i++)
   {
      if (pts[i].p<=pts[i-1].p)
      {
         swifterr("error in EqPacificHydrography::EqPacificHydrography() ... "
                  "Hydrographic profile not monotonically increasing in pressure.\n");
      }
   }

   // load the profiles into the linear spline objects
   for (unsigned int i=0; i<n; i++)
   {
      t.Append(pts[i].p,pts[i].t); s.Append(pts[i].p,pts[i].s);
   }

   // seek extremes in the profiles
   t.SeekExtremes(); s.SeekExtremes();
   
   // make sure the density increases monotonically
   for (unsigned int i=1; i<n; i++)
   {
      if (Rho(pts[i].p)<=Rho(pts[i-1].p))
      {
         swifterr("error in EqPacificHydrography::EqPacificHydrography() ... "
                  "Hydrographic profile not monotonically increasing in density.\n");
      }
   }
}

/*------------------------------------------------------------------------*/
/*                                                                        */
/*------------------------------------------------------------------------*/
GulfOfMexicoHydrography::GulfOfMexicoHydrography(void)
{
   HydroObs pts[]=
   {
      //    lat     lon       date   time  zbot  zmax sh co           stnid     n
      //  24.93  -84.58 11/12/1983 13.300 *****  1500  * 32 obs,3077,774761    13
      //    p      t      s      //  theta  sigma     O2     Si    NO3    PO4
      {    0.0, 25.850, 36.029}, 
      {    5.0, 25.850, 36.029}, // 25.849 23.859  4.380    1.1   0.20  0.100
      {   41.1, 25.830, 35.979}, // 25.821 23.830  3.970    4.1  12.00  0.720
      {   99.3, 18.919, 36.389}, // 18.901 26.110  3.660    2.5   6.50  0.410
      {  147.6, 16.869, 36.240}, // 16.845 26.505  2.890    2.3   5.10  0.380
      {  200.9, 14.200, 35.839}, // 14.170 26.800  2.610    7.2  19.30  1.150
      {  301.5, 11.180, 35.389}, // 11.142 27.053  2.450      *      *      *
      {  396.2,  9.560, 35.149}, //  9.515 27.151  2.420   15.8  28.70  1.830
      {  502.0,  8.330, 35.009}, //  8.277 27.239  2.360   14.1  24.50  1.680
      {  603.8,  7.230, 34.919}, //  7.171 27.331  2.560   21.3  31.80  2.070
      {  805.7,  5.640, 34.899}, //  5.570 27.527  3.090   25.1  29.80  2.000
      { 1007.7,  5.181, 34.919}, //  5.095 27.600  3.500   25.5  28.30  1.900
      { 1211.0,  4.711, 34.949}, //  4.610 27.680  3.880   25.9  26.60  1.780
      { 1513.7,  4.280, 34.970}, //  4.155 27.746  4.050   24.5  24.20  1.620
   };
   unsigned int n = sizeof(pts)/sizeof(HydroObs);
   
   // make sure there are at least 3 points in the profile
   if (n<3)
   {
      swifterr("error in GulfOfMexicoHydrography::GulfOfMexicoHydrography() ... "
               "Hydrographic profile must contain at least 3 points\n");
   }
   
   // make sure the profile starts at the surface
   if (pts[0].p!=0)
   {
      swifterr("error in GulfOfMexicoHydrography::GulfOfMexicoHydrography() ... "
               "First observation in profile must be at surface.\n");
   }
   
   // make sure the pressure increases monotonically
   for (unsigned int i=1; i<n; i++)
   {
      if (pts[i].p<=pts[i-1].p)
      {
         swifterr("error in GulfOfMexicoHydrography::GulfOfMexicoHydrography() ... "
                  "Hydrographic profile not monotonically increasing in pressure.\n");
      }
   }

   // load the profiles into the linear spline objects
   for (unsigned int i=0; i<n; i++)
   {
      t.Append(pts[i].p,pts[i].t); s.Append(pts[i].p,pts[i].s);
   }

   // seek extremes in the profiles
   t.SeekExtremes(); s.SeekExtremes();
   
   // make sure the density increases monotonically
   for (unsigned int i=1; i<n; i++)
   {
      if (Rho(pts[i].p)<=Rho(pts[i-1].p))
      {
         swifterr("error in GulfOfMexicoHydrography::GulfOfMexicoHydrography() ... "
                  "Hydrographic profile not monotonically increasing in density.\n");
      }
   }
}

/*------------------------------------------------------------------------*/
/*                                                                        */
/*------------------------------------------------------------------------*/
HawaiiPacificHydrography::HawaiiPacificHydrography(void)
{
   HydroObs pts[]=
   {
      //    lat     lon       date   time   zmax 
      //  21.85 -155.03 09/09/1973 15.200   3042
      //     p       t       s        theta  sigma
      {    0.0, 25.527, 35.133}, //  25.527 23.282
      {   10.0, 25.527, 35.135}, //  25.525 23.284
      {   20.0, 25.531, 35.146}, //  25.527 23.292
      {   30.0, 25.536, 35.148}, //  25.529 23.293
      {   50.1, 25.495, 35.285}, //  25.484 23.410
      {   76.1, 22.779, 35.226}, //  22.764 24.175
      {  101.2, 21.420, 35.233}, //  21.400 24.563
      {  126.3, 20.299, 35.195}, //  20.275 24.839
      {  151.4, 19.643, 35.192}, //  19.615 25.010
      {  176.5, 18.128, 34.946}, //  18.098 25.208
      {  201.6, 16.691, 34.771}, //  16.658 25.420
      {  226.7, 14.940, 34.510}, //  14.906 25.615
      {  251.9, 13.488, 34.361}, //  13.453 25.807
      {  303.1, 11.342, 34.209}, //  11.304 26.105
      {  353.4,  9.662, 34.169}, //   9.622 26.367
      {  404.8,  8.655, 34.159}, //   8.612 26.521
      {  454.1,  7.914, 34.195}, //   7.868 26.661
      {  504.5,  7.151, 34.205}, //   7.103 26.779
      {  554.8,  6.478, 34.253}, //   6.428 26.907
      {  605.2,  6.230, 34.313}, //   6.176 26.988
      {  655.6,  5.793, 34.330}, //   5.736 27.056
      {  706.1,  5.643, 34.368}, //   5.582 27.105
      {  757.5,  5.351, 34.400}, //   5.287 27.166
      {  806.9,  5.105, 34.423}, //   5.038 27.213
      {  858.4,  4.866, 34.445}, //   4.796 27.259
      {  908.9,  4.589, 34.453}, //   4.517 27.296
      {  959.4,  4.481, 34.472}, //   4.405 27.323
      { 1009.9,  4.295, 34.489}, //   4.216 27.357
      { 1111.0,  4.016, 34.505}, //   3.931 27.400
      { 1212.1,  3.737, 34.527}, //   3.646 27.446
      { 1313.3,  3.444, 34.543}, //   3.347 27.488
      { 1415.5,  3.243, 34.555}, //   3.139 27.517
      { 1515.7,  3.035, 34.564}, //   2.925 27.544
      { 1617.1,  2.834, 34.576}, //   2.718 27.572
      { 1719.4,  2.664, 34.585}, //   2.542 27.595
      { 1819.8,  2.518, 34.595}, //   2.389 27.616
      { 1922.3,  2.371, 34.604}, //   2.235 27.635
      { 2023.8,  2.219, 34.613}, //   2.077 27.656
      { 2227.0,  2.028, 34.626}, //   1.871 27.682
      { 2434.5,  1.895, 34.639}, //   1.722 27.704
      { 2636.0,  1.737, 34.648}, //   1.549 27.724
      { 2837.7,  1.650, 34.657}, //   1.445 27.739
      { 3041.7,  1.583, 34.662}, //   1.360 27.749
   };
   unsigned int n = sizeof(pts)/sizeof(HydroObs);
   
   // make sure there are at least 3 points in the profile
   if (n<3)
   {
      swifterr("error in HawaiiPacificHydrography::HawaiiPacificHydrography() ... "
               "Hydrographic profile must contain at least 3 points\n");
   }
   
   // make sure the profile starts at the surface
   if (pts[0].p!=0)
   {
      swifterr("error in HawaiiPacificHydrography::HawaiiPacificHydrography() ... "
               "First observation in profile must be at surface.\n");
   }
   
   // make sure the pressure increases monotonically
   for (unsigned int i=1; i<n; i++)
   {
      if (pts[i].p<=pts[i-1].p)
      {
         swifterr("error in HawaiiPacificHydrography::HawaiiPacificHydrography() ... "
                  "Hydrographic profile not monotonically increasing in pressure.\n");
      }
   }

   // load the profiles into the linear spline objects
   for (unsigned int i=0; i<n; i++)
   {
      t.Append(pts[i].p,pts[i].t); s.Append(pts[i].p,pts[i].s);
   }

   // seek extremes in the profiles
   t.SeekExtremes(); s.SeekExtremes();
   
   // make sure the density increases monotonically
   for (unsigned int i=1; i<n; i++)
   {
      if (Rho(pts[i].p)<=Rho(pts[i-1].p))
      {
         swifterr("error in HawaiiPacificHydrography::HawaiiPacificHydrography() ... "
                  "Hydrographic profile not monotonically increasing in density.\n");
      }
   }
}

/*------------------------------------------------------------------------*/
/*                                                                        */
/*------------------------------------------------------------------------*/
JapanSeaHydrography::JapanSeaHydrography(void)
{
   HydroObs pts[]=
   {
      //    lat     lon       date   time  zmax   stnid  n
      //  39.33  133.90 09/09/1999 16.956  1007 195.026 53
      //     p       t        s        theta    sigma
      {    0.0, 24.164, 33.2488}, //  24.163  22.2700
      {    9.7, 23.933, 33.3437}, //  23.931  22.4099
      {   19.8, 22.168, 33.6167}, //  22.164  23.1228
      {   29.6, 17.843, 34.4372}, //  17.838  24.8818
      {   39.5, 15.902, 34.3900}, //  15.896  25.3029
      {   49.7, 14.503, 34.3972}, //  14.496  25.6170
      {   59.4, 12.733, 34.3107}, //  12.725  25.9140
      {   69.6, 11.215, 34.2526}, //  11.206  26.1566
      {   79.3, 10.157, 34.1898}, //  10.148  26.2945
      {   89.2,  8.984, 34.1405}, //   8.975  26.4495
      {   99.3,  8.026, 34.1176}, //   8.016  26.5787
      {  109.5,  7.209, 34.0945}, //   7.199  26.6782
      {  119.6,  6.160, 34.0764}, //   6.150  26.8040
      {  129.5,  5.270, 34.0635}, //   5.260  26.9028
      {  139.4,  4.472, 34.0555}, //   4.462  26.9863
      {  149.5,  4.219, 34.0482}, //   4.208  27.0075
      {  159.5,  3.680, 34.0515}, //   3.669  27.0649
      {  169.1,  3.396, 34.0563}, //   3.385  27.0962
      {  179.5,  3.137, 34.0545}, //   3.126  27.1189
      {  189.0,  2.734, 34.0554}, //   2.723  27.1556
      {  199.1,  2.231, 34.0519}, //   2.220  27.1947
      {  209.5,  1.937, 34.0511}, //   1.926  27.2171
      {  219.5,  1.769, 34.0573}, //   1.758  27.2348
      {  229.6,  1.604, 34.0579}, //   1.593  27.2474
      {  239.5,  1.522, 34.0601}, //   1.510  27.2551
      {  249.4,  1.419, 34.0691}, //   1.407  27.2696
      {  259.1,  1.291, 34.0700}, //   1.279  27.2792
      {  269.0,  1.188, 34.0688}, //   1.175  27.2852
      {  279.3,  1.124, 34.0687}, //   1.111  27.2893
      {  289.5,  1.062, 34.0704}, //   1.049  27.2948
      {  299.4,  1.030, 34.0705}, //   1.016  27.2970
      {  309.6,  0.985, 34.0721}, //   0.971  27.3012
      {  319.1,  0.963, 34.0719}, //   0.948  27.3025
      {  329.3,  0.930, 34.0721}, //   0.915  27.3048
      {  339.3,  0.896, 34.0720}, //   0.881  27.3068
      {  349.4,  0.863, 34.0726}, //   0.847  27.3094
      {  359.5,  0.832, 34.0719}, //   0.816  27.3108
      {  379.5,  0.787, 34.0730}, //   0.770  27.3145
      {  399.5,  0.735, 34.0722}, //   0.717  27.3171
      {  449.4,  0.629, 34.0718}, //   0.609  27.3233
      {  499.3,  0.556, 34.0710}, //   0.534  27.3270
      {  549.1,  0.495, 34.0702}, //   0.471  27.3300
      {  599.3,  0.440, 34.0699}, //   0.414  27.3331
      {  649.5,  0.398, 34.0691}, //   0.370  27.3349
      {  699.2,  0.367, 34.0687}, //   0.336  27.3365
      {  749.3,  0.333, 34.0686}, //   0.300  27.3384
      {  799.4,  0.315, 34.0679}, //   0.279  27.3390
      {  849.3,  0.296, 34.0680}, //   0.258  27.3402
      {  899.3,  0.280, 34.0679}, //   0.239  27.3411
      {  949.3,  0.268, 34.0679}, //   0.225  27.3419
      {  978.9,  0.259, 34.0676}, //   0.214  27.3422
      {  999.2,  0.253, 34.0676}, //   0.207  27.3426
      { 1006.8,  0.252, 34.0681}, //   0.206  27.3431
   };
   unsigned int n = sizeof(pts)/sizeof(HydroObs);
   
   // make sure there are at least 3 points in the profile
   if (n<3)
   {
      swifterr("error in JapanSeaHydrography::JapanSeaHydrography() ... "
               "Hydrographic profile must contain at least 3 points\n");
   }
   
   // make sure the profile starts at the surface
   if (pts[0].p!=0)
   {
      swifterr("error in JapanSeaHydrography::JapanSeaHydrography() ... "
               "First observation in profile must be at surface.\n");
   }
   
   // make sure the pressure increases monotonically
   for (unsigned int i=1; i<n; i++)
   {
      if (pts[i].p<=pts[i-1].p)
      {
         swifterr("error in JapanSeaHydrography::JapanSeaHydrography() ... "
                  "Hydrographic profile not monotonically increasing in pressure.\n");
      }
   }

   // load the profiles into the linear spline objects
   for (unsigned int i=0; i<n; i++)
   {
      t.Append(pts[i].p,pts[i].t); s.Append(pts[i].p,pts[i].s);
   }

   // seek extremes in the profiles
   t.SeekExtremes(); s.SeekExtremes();
   
   // make sure the density increases monotonically
   for (unsigned int i=1; i<n; i++)
   {
      if (Rho(pts[i].p)<=Rho(pts[i-1].p))
      {
         swifterr("error in JapanSeaHydrography::JapanSeaHydrography() ... "
                  "Hydrographic profile not monotonically increasing in density.\n");
      }
   }
}

/*------------------------------------------------------------------------*/
/*                                                                        */
/*------------------------------------------------------------------------*/
KurishioHydrography::KurishioHydrography(void)
{
   HydroObs pts[]=
   {
      //    lat     lon       date   time  zbot  zmax sh co           stnid     n
      //  33.83  141.20 02/14/1988  5.200 *****  2532  * 49 obs,3900,802295    24
      //     p       t       s       theta  sigma     O2    NO3    PO4
      {    0.0, 18.760, 34.873}, // 18.760 24.988  5.290   1.00  0.150
      {   10.0, 18.760, 34.873}, // 18.738 24.992  5.260   1.00  0.140
      {   25.1, 18.760, 34.873}, // 18.756 24.987  5.260   1.00  0.140
      {   50.2, 18.740, 34.873}, // 18.731 24.994  5.260   1.00  0.140
      {   74.3, 18.730, 34.873}, // 18.717 24.997  5.240   1.00  0.130
      {   99.4, 18.650, 34.874}, // 18.633 25.019  5.240   1.20  0.150
      {  124.5, 18.490, 34.864}, // 18.468 25.053  5.150   1.60  0.170
      {  148.7, 18.430, 34.870}, // 18.404 25.073  5.240   1.40  0.170
      {  172.8, 18.350, 34.872}, // 18.320 25.096  5.170   1.40  0.170
      {  197.9, 18.350, 34.872}, // 18.316 25.095  5.200   1.50  0.190
      {  246.2, 18.150, 34.854}, // 18.107 25.135  5.020   2.30  0.300
      {  294.4, 17.060, 34.769}, // 17.011 25.335  4.610   5.60  0.470
      {  427.3, 14.860, 34.624}, // 14.795 25.727  4.390   9.80  0.750
      {  527.0, 12.430, 34.442}, // 12.359 26.087  4.280  14.20  1.080
      {  627.8,  9.910, 34.270}, //  9.837 26.410  4.010  20.10  1.510
      {  727.7,  7.550, 34.145}, //  7.477 26.679  3.320  26.80  1.990
      {  827.6,  6.080, 34.134}, //  6.006 26.868  2.690  31.60  2.380
      {  927.6,  4.960, 34.184}, //  4.884 27.042  1.950  36.70  2.740
      { 1028.7,  4.500, 34.304}, //  4.418 27.188  1.680  38.10  2.860
      { 1279.6,  3.360, 34.424}, //  3.267 27.401  1.590  39.70  2.980
      { 1550.2,  2.530, 34.510}, //  2.424 27.545  1.640  41.00  3.050
      { 1802.8,  2.240, 34.551}, //  2.116 27.603  1.770  40.90  3.040
      { 2055.8,  1.990, 34.586}, //  1.849 27.652  1.950  40.50  3.030
      { 2562.6,  1.690, 34.638}, //  1.510 27.719  2.640  38.60  2.890
   };
   unsigned int n = sizeof(pts)/sizeof(HydroObs);
   
   // make sure there are at least 3 points in the profile
   if (n<3)
   {
      swifterr("error in KurishioHydrography::KurishioHydrography() ... "
               "Hydrographic profile must contain at least 3 points\n");
   }
   
   // make sure the profile starts at the surface
   if (pts[0].p!=0)
   {
      swifterr("error in KurishioHydrography::KurishioHydrography() ... "
               "First observation in profile must be at surface.\n");
   }
   
   // make sure the pressure increases monotonically
   for (unsigned int i=1; i<n; i++)
   {
      if (pts[i].p<=pts[i-1].p)
      {
         swifterr("error in KurishioHydrography::KurishioHydrography() ... "
                  "Hydrographic profile not monotonically increasing in pressure.\n");
      }
   }

   // load the profiles into the linear spline objects
   for (unsigned int i=0; i<n; i++)
   {
      t.Append(pts[i].p,pts[i].t); s.Append(pts[i].p,pts[i].s);
   }

   // seek extremes in the profiles
   t.SeekExtremes(); s.SeekExtremes();
   
   // make sure the density increases monotonically
   for (unsigned int i=1; i<n; i++)
   {
      if (Rho(pts[i].p)<=Rho(pts[i-1].p))
      {
         swifterr("error in KurishioHydrography::KurishioHydrography() ... "
                  "Hydrographic profile not monotonically increasing in density.\n");
      }
   }
}

/*------------------------------------------------------------------------*/
/*                                                                        */
/*------------------------------------------------------------------------*/
LaboradorSeaHydrography::LaboradorSeaHydrography(void)
{
   HydroObs pts[]=
   {
      //    lat     lon       date   time  zbot  zmax sh co           stnid      n
      //  57.50  304.05 07/30/2003  2.853     *  1999  *  *        0570.074     61
      //     p        t        s        theta    sigma
      {    0.0,  10.608,  34.190}, //  10.607  26.2151
      {    9.8,  10.608,  34.190}, //  10.607  26.2151
      {   19.6,   8.056,  34.162}, //   8.054  26.6079
      {   29.4,   6.179,  34.320}, //   6.176  26.9930
      {   39.4,   4.887,  34.449}, //   4.884  27.2518
      {   49.2,   4.437,  34.528}, //   4.433  27.3647
      {   59.3,   4.226,  34.600}, //   4.222  27.4447
      {   69.1,   4.149,  34.668}, //   4.144  27.5071
      {   79.1,   4.068,  34.710}, //   4.062  27.5491
      {   99.6,   4.108,  34.761}, //   4.101  27.5856
      {  119.5,   4.115,  34.789}, //   4.107  27.6073
      {  139.5,   4.236,  34.822}, //   4.226  27.6208
      {  159.2,   4.300,  34.840}, //   4.288  27.6284
      {  179.5,   4.209,  34.840}, //   4.196  27.6383
      {  199.5,   4.157,  34.840}, //   4.143  27.6440
      {  224.5,   3.980,  34.832}, //   3.964  27.6564
      {  249.0,   3.956,  34.840}, //   3.939  27.6654
      {  273.9,   3.911,  34.841}, //   3.892  27.6711
      {  299.1,   3.946,  34.851}, //   3.925  27.6756
      {  324.2,   3.896,  34.849}, //   3.873  27.6793
      {  349.5,   3.851,  34.847}, //   3.826  27.6826
      {  374.3,   3.827,  34.849}, //   3.801  27.6868
      {  399.0,   3.790,  34.848}, //   3.762  27.6899
      {  424.2,   3.768,  34.848}, //   3.738  27.6923
      {  449.5,   3.745,  34.849}, //   3.713  27.6956
      {  473.9,   3.735,  34.851}, //   3.702  27.6984
      {  499.5,   3.703,  34.850}, //   3.668  27.7010
      {  524.4,   3.676,  34.849}, //   3.639  27.7031
      {  549.5,   3.667,  34.851}, //   3.628  27.7058
      {  574.5,   3.657,  34.850}, //   3.617  27.7062
      {  599.4,   3.630,  34.849}, //   3.588  27.7083
      {  624.5,   3.647,  34.853}, //   3.603  27.7099
      {  649.5,   3.634,  34.854}, //   3.588  27.7122
      {  674.2,   3.628,  34.855}, //   3.580  27.7138
      {  699.2,   3.618,  34.856}, //   3.568  27.7158
      {  749.4,   3.576,  34.855}, //   3.523  27.7195
      {  799.3,   3.543,  34.855}, //   3.486  27.7231
      {  849.2,   3.511,  34.854}, //   3.450  27.7258
      {  899.3,   3.471,  34.852}, //   3.407  27.7285
      {  949.5,   3.439,  34.850}, //   3.371  27.7303
      {  999.1,   3.416,  34.850}, //   3.344  27.7329
      { 1049.2,   3.369,  34.845}, //   3.294  27.7338
      { 1099.3,   3.361,  34.848}, //   3.282  27.7374
      { 1149.5,   3.415,  34.860}, //   3.331  27.7422
      { 1199.6,   3.454,  34.870}, //   3.366  27.7468
      { 1249.0,   3.465,  34.877}, //   3.372  27.7517
      { 1299.1,   3.465,  34.881}, //   3.368  27.7553
      { 1349.1,   3.453,  34.885}, //   3.352  27.7601
      { 1399.6,   3.411,  34.884}, //   3.306  27.7637
      { 1449.5,   3.371,  34.884}, //   3.262  27.7679
      { 1498.3,   3.360,  34.884}, //   3.247  27.7694
      { 1499.2,   3.315,  34.880}, //   3.203  27.7704
      { 1549.1,   3.270,  34.879}, //   3.154  27.7742
      { 1599.4,   3.236,  34.877}, //   3.116  27.7762
      { 1649.4,   3.189,  34.874}, //   3.065  27.7786
      { 1699.2,   3.155,  34.872}, //   3.027  27.7805
      { 1749.4,   3.132,  34.874}, //   3.000  27.7846
      { 1799.4,   3.109,  34.876}, //   2.973  27.7887
      { 1849.3,   3.094,  34.879}, //   2.954  27.7929
      { 1899.2,   3.066,  34.880}, //   2.922  27.7966
      { 1949.0,   3.037,  34.882}, //   2.889  27.8012
      { 1999.4,   3.014,  34.884}, //   2.861  27.8053
   };
   unsigned int n = sizeof(pts)/sizeof(HydroObs);
   
   // make sure there are at least 3 points in the profile
   if (n<3)
   {
      swifterr("error in LaboradorSeaHydrography::LaboradorSeaHydrography() ... "
               "Hydrographic profile must contain at least 3 points\n");
   }
   
   // make sure the profile starts at the surface
   if (pts[0].p!=0)
   {
      swifterr("error in LaboradorSeaHydrography::LaboradorSeaHydrography() ... "
               "First observation in profile must be at surface.\n");
   }
   
   // make sure the pressure increases monotonically
   for (unsigned int i=1; i<n; i++)
   {
      if (pts[i].p<=pts[i-1].p)
      {
         swifterr("error in LaboradorSeaHydrography::LaboradorSeaHydrography() ... "
                  "Hydrographic profile not monotonically increasing in pressure.\n");
      }
   }

   // load the profiles into the linear spline objects
   for (unsigned int i=0; i<n; i++)
   {
      t.Append(pts[i].p,pts[i].t); s.Append(pts[i].p,pts[i].s);
   }

   // seek extremes in the profiles
   t.SeekExtremes(); s.SeekExtremes();
   
   // make sure the density increases monotonically
   for (unsigned int i=1; i<n; i++)
   {
      if (Rho(pts[i].p)<=Rho(pts[i-1].p))
      {
         swifterr("error in LaboradorSeaHydrography::LaboradorSeaHydrography() ... "
                  "Hydrographic profile not monotonically increasing in density.\n");
      }
   }
}

/*------------------------------------------------------------------------*/
/*                                                                        */
/*------------------------------------------------------------------------*/
SIndianOceanHydrography::SIndianOceanHydrography(void)
{
   HydroObs pts[]=
   {
      //    lat     lon       date    stnid
      // -27.04   60.16 11/04/2006 5076.003
      //     p       t       s   theta    sigma
      {    0.0, 22.902, 35.538},
      {    6.3, 22.902, 35.538}, //  22.901  24.3719
      {   10.1, 22.563, 35.542}, //  22.561  24.4724
      {   20.3, 21.496, 35.525}, //  21.492  24.7599
      {   29.9, 20.673, 35.548}, //  20.667  25.0028
      {   39.8, 20.374, 35.588}, //  20.367  25.1141
      {   50.4, 20.014, 35.580}, //  20.005  25.2043
      {   59.2, 19.815, 35.586}, //  19.804  25.2617
      {   70.4, 19.590, 35.578}, //  19.577  25.3151
      {   80.3, 19.351, 35.581}, //  19.336  25.3799
      {   90.0, 19.089, 35.598}, //  19.073  25.4608
      {  100.1, 18.797, 35.614}, //  18.779  25.5481
      {  109.9, 18.502, 35.636}, //  18.483  25.6399
      {  119.9, 18.244, 35.623}, //  18.223  25.6950
      {  130.0, 18.015, 35.622}, //  17.993  25.7515
      {  138.4, 17.810, 35.631}, //  17.786  25.8092
      {  150.2, 17.522, 35.621}, //  17.497  25.8724
      {  160.3, 17.052, 35.586}, //  17.025  25.9591
      {  170.4, 16.660, 35.559}, //  16.632  26.0316
      {  179.8, 16.337, 35.545}, //  16.308  26.0967
      {  189.4, 15.992, 35.539}, //  15.962  26.1720
      {  200.4, 15.690, 35.519}, //  15.659  26.2258
      {  210.2, 15.258, 35.484}, //  15.226  26.2960
      {  220.2, 14.996, 35.475}, //  14.963  26.3474
      {  230.3, 14.808, 35.459}, //  14.773  26.3765
      {  239.8, 14.679, 35.446}, //  14.643  26.3949
      {  250.2, 14.426, 35.421}, //  14.389  26.4305
      {  260.2, 14.292, 35.409}, //  14.254  26.4502
      {  270.3, 14.190, 35.399}, //  14.150  26.4644
      {  279.9, 13.992, 35.376}, //  13.951  26.4888
      {  289.8, 13.884, 35.364}, //  13.842  26.5024
      {  300.4, 13.783, 35.352}, //  13.740  26.5146
      {  309.8, 13.735, 35.345}, //  13.691  26.5194
      {  320.3, 13.666, 35.337}, //  13.620  26.5279
      {  330.0, 13.590, 35.328}, //  13.543  26.5369
      {  340.3, 13.516, 35.318}, //  13.468  26.5447
      {  349.7, 13.499, 35.315}, //  13.449  26.5461
      {  359.6, 13.364, 35.300}, //  13.313  26.5624
      {  380.2, 13.237, 35.282}, //  13.184  26.5749
      {  399.5, 13.030, 35.251}, //  12.974  26.5932
      {  449.6, 12.555, 35.177}, //  12.494  26.6313
      {  500.3, 12.101, 35.104}, //  12.035  26.6639
      {  549.8, 11.665, 35.036}, //  11.594  26.6946
      {  600.0, 11.146, 34.957}, //  11.070  26.7298
      {  649.8, 10.756, 34.898}, //  10.676  26.7549
      {  700.3, 10.242, 34.827}, //  10.158  26.7902
      {  750.2,  9.478, 34.727}, //   9.392  26.8411
      {  800.5,  8.539, 34.627}, //   8.452  26.9125
      {  850.2,  7.811, 34.561}, //   7.723  26.9702
      {  900.6,  7.110, 34.506}, //   7.021  27.0269
      {  950.3,  6.412, 34.462}, //   6.323  27.0861
      { 1000.4,  5.763, 34.432}, //   5.674  27.1447
      { 1002.7,  5.654, 34.431}, //   5.566  27.1572
      { 1049.7,  5.102, 34.420}, //   5.014  27.2140
      { 1099.7,  4.731, 34.421}, //   4.641  27.2570
      { 1149.9,  4.416, 34.425}, //   4.324  27.2947
      { 1199.6,  4.092, 34.442}, //   3.999  27.3425
      { 1250.2,  3.869, 34.462}, //   3.773  27.3814
      { 1299.8,  3.650, 34.496}, //   3.552  27.4305
      { 1349.9,  3.496, 34.520}, //   3.395  27.4649
      { 1399.9,  3.397, 34.535}, //   3.293  27.4866
      { 1449.8,  3.275, 34.562}, //   3.168  27.5199
      { 1500.3,  3.193, 34.575}, //   3.083  27.5383
      { 1550.0,  3.086, 34.592}, //   2.973  27.5620
      { 1600.5,  2.977, 34.608}, //   2.861  27.5849
      { 1649.6,  2.872, 34.626}, //   2.753  27.6089
      { 1700.2,  2.803, 34.636}, //   2.680  27.6233
      { 1750.1,  2.697, 34.649}, //   2.571  27.6432
      { 1799.9,  2.604, 34.661}, //   2.475  27.6610
      { 1850.3,  2.519, 34.672}, //   2.387  27.6773
      { 1899.5,  2.490, 34.676}, //   2.354  27.6832
      { 1949.6,  2.387, 34.687}, //   2.249  27.7008
      { 1999.2,  2.350, 34.692}, //   2.208  27.7082
      { 2200.0,  2.350, 34.692}
   };
   unsigned int n = sizeof(pts)/sizeof(HydroObs);
   
   // make sure there are at least 3 points in the profile
   if (n<3)
   {
      swifterr("error in SIndianOceanHydrography::SIndianOceanHydrography() ... "
               "Hydrographic profile must contain at least 3 points\n");
   }
   
   // make sure the profile starts at the surface
   if (pts[0].p!=0)
   {
      swifterr("error in SIndianOceanHydrography::SIndianOceanHydrography() ... "
               "First observation in profile must be at surface.\n");
   }
   
   // make sure the pressure increases monotonically
   for (unsigned int i=1; i<n; i++)
   {
      if (pts[i].p<=pts[i-1].p)
      {
         swifterr("error in SIndianOceanHydrography::SIndianOceanHydrography() ... "
                  "Hydrographic profile not monotonically increasing in pressure.\n");
      }
   }

   // load the profiles into the linear spline objects
   for (unsigned int i=0; i<n; i++)
   {
      t.Append(pts[i].p,pts[i].t); s.Append(pts[i].p,pts[i].s);
   }

   // seek extremes in the profiles
   t.SeekExtremes(); s.SeekExtremes();
   
   // make sure the density increases monotonically
   for (unsigned int i=1; i<n; i++)
   {
      if (Rho(pts[i].p)<=Rho(pts[i-1].p))
      {
         swifterr("error in SIndianOceanHydrography::SIndianOceanHydrography() ... "
                  "Hydrographic profile not monotonically increasing in density.\n");
      }
   }
}

/*------------------------------------------------------------------------*/
/*                                                                        */
/*------------------------------------------------------------------------*/
SouthPacificNoppHydrography::SouthPacificNoppHydrography(void)
{
   HydroObs pts[]=
   {
      //     lat     lon       date   time  zmax   stnid  n
      //  -12.10  230.24 11/14/2000  1.593  2002 421.001 72
      //     p        t        s        theta    sigma
      {    0.0,  26.483,  35.688}, //  26.482  23.4029
      {    9.5,  26.493,  35.690}, //  26.491  23.4017
      {   19.1,  26.431,  35.689}, //  26.427  23.4212
      {   29.2,  26.366,  35.689}, //  26.359  23.4424
      {   39.3,  26.354,  35.691}, //  26.345  23.4484
      {   49.5,  26.348,  35.690}, //  26.337  23.4503
      {   59.1,  25.689,  35.769}, //  25.676  23.7165
      {   69.3,  25.507,  35.809}, //  25.492  23.8037
      {   79.1,  25.154,  35.869}, //  25.137  23.9582
      {   89.1,  25.074,  35.999}, //  25.055  24.0816
      {   99.1,  24.932,  36.093}, //  24.910  24.1967
      {  119.7,  24.277,  36.283}, //  24.251  24.5397
      {  139.0,  22.670,  36.199}, //  22.642  24.9482
      {  159.1,  21.862,  36.099}, //  21.830  25.1023
      {  179.2,  21.323,  36.006}, //  21.288  25.1823
      {  199.3,  20.402,  35.824}, //  20.364  25.2947
      {  259.2,  15.191,  34.961}, //  15.151  25.9093
      {  299.3,  11.959,  34.643}, //  11.920  26.3276
      {  349.4,  10.261,  34.666}, //  10.220  26.6538
      {  398.9,   9.256,  34.646}, //   9.212  26.8071
      {  449.5,   8.324,  34.611}, //   8.277  26.9268
      {  499.3,   7.675,  34.586}, //   7.625  27.0042
      {  549.1,   7.257,  34.567}, //   7.204  27.0495
      {  599.2,   6.861,  34.555}, //   6.804  27.0953
      {  649.3,   6.485,  34.540}, //   6.425  27.1343
      {  699.1,   6.041,  34.524}, //   5.979  27.1794
      {  749.0,   5.720,  34.517}, //   5.655  27.2143
      {  799.2,   5.477,  34.514}, //   5.409  27.2419
      {  849.5,   5.192,  34.511}, //   5.121  27.2737
      {  899.1,   4.923,  34.514}, //   4.849  27.3074
      {  949.1,   4.672,  34.519}, //   4.596  27.3398
      {  999.5,   4.460,  34.525}, //   4.381  27.3681
      { 1099.5,   4.090,  34.537}, //   4.005  27.4174
      { 1199.3,   3.798,  34.551}, //   3.707  27.4589
      { 1299.3,   3.502,  34.565}, //   3.406  27.4998
      { 1399.1,   3.258,  34.576}, //   3.156  27.5323
      { 1499.3,   3.005,  34.589}, //   2.897  27.5664
      { 1599.0,   2.787,  34.599}, //   2.673  27.5943
      { 1699.5,   2.668,  34.606}, //   2.547  27.6109
      { 1799.6,   2.485,  34.616}, //   2.358  27.6349
      { 1899.6,   2.337,  34.626}, //   2.204  27.6557
      { 1999.4,   2.197,  34.638}, //   2.057  27.6771
      { 2001.7,   2.194,  34.639}, //   2.054  27.6782
      { 2500.0,   2.194,  34.639}
   };
   unsigned int n = sizeof(pts)/sizeof(HydroObs);
   
   // make sure there are at least 3 points in the profile
   if (n<3)
   {
      swifterr("error in SouthPacificNoppHydrography::SouthPacificNoppHydrography() ... "
               "Hydrographic profile must contain at least 3 points\n");
   }
   
   // make sure the profile starts at the surface
   if (pts[0].p!=0)
   {
      swifterr("error in SouthPacificNoppHydrography::SouthPacificNoppHydrography() ... "
               "First observation in profile must be at surface.\n");
   }
   
   // make sure the pressure increases monotonically
   for (unsigned int i=1; i<n; i++)
   {
      if (pts[i].p<=pts[i-1].p)
      {
         swifterr("error in SouthPacificNoppHydrography::SouthPacificNoppHydrography() ... "
                  "Hydrographic profile not monotonically increasing in pressure.\n");
      }
   }

   // load the profiles into the linear spline objects
   for (unsigned int i=0; i<n; i++)
   {
      t.Append(pts[i].p,pts[i].t); s.Append(pts[i].p,pts[i].s);
   }

   // seek extremes in the profiles
   t.SeekExtremes(); s.SeekExtremes();
   
   // make sure the density increases monotonically
   for (unsigned int i=1; i<n; i++)
   {
      if (Rho(pts[i].p)<=Rho(pts[i-1].p))
      {
         swifterr("error in SouthPacificNoppHydrography::SouthPacificNoppHydrography() ... "
                  "Hydrographic profile not monotonically increasing in density.\n");
      }
   }
}

/*------------------------------------------------------------------------*/
/*                                                                        */
/*------------------------------------------------------------------------*/
SargassoHydrography::SargassoHydrography(void)
{
   HydroObs pts[]=
   {
      //    lat     lon       date   time  zmax   stnid   n
      //  32.56  295.77 09/08/2000 20.298  2005 422.001  72
      //     p       t       s        theta    sigma
      {    0.0, 27.077, 36.351}, //  27.076  23.7135
      {    9.8, 27.076, 36.352}, //  27.074  23.7149
      {   19.4, 27.010, 36.355}, //  27.006  23.7391
      {   29.7, 25.042, 36.537}, //  25.035  24.4943
      {   39.8, 23.518, 36.637}, //  23.510  25.0283
      {   49.5, 21.962, 36.685}, //  21.952  25.5138
      {   59.4, 21.218, 36.693}, //  21.206  25.7280
      {   69.3, 20.792, 36.672}, //  20.779  25.8293
      {   79.4, 20.426, 36.658}, //  20.411  25.9183
      {   89.5, 19.738, 36.604}, //  19.721  26.0609
      {   99.4, 19.433, 36.602}, //  19.415  26.1398
      {  119.6, 19.264, 36.603}, //  19.242  26.1855
      {  139.1, 18.943, 36.597}, //  18.918  26.2647
      {  159.3, 18.804, 36.596}, //  18.775  26.3004
      {  179.5, 18.712, 36.586}, //  18.680  26.3172
      {  199.4, 18.655, 36.579}, //  18.619  26.3272
      {  249.3, 18.508, 36.565}, //  18.464  26.3560
      {  299.1, 18.319, 36.545}, //  18.266  26.3905
      {  349.3, 18.035, 36.506}, //  17.974  26.4336
      {  399.3, 17.579, 36.434}, //  17.510  26.4927
      {  449.5, 16.959, 36.325}, //  16.884  26.5606
      {  499.5, 16.106, 36.174}, //  16.025  26.6462
      {  549.4, 15.151, 36.017}, //  15.066  26.7427
      {  599.2, 13.839, 35.806}, //  13.751  26.8634
      {  649.4, 12.387, 35.592}, //  12.299  26.9918
      {  699.3, 11.653, 35.488}, //  11.561  27.0523
      {  749.2, 10.235, 35.305}, //  10.144  27.1658
      {  798.9,  9.095, 35.202}, //   9.005  27.2760
      {  849.2,  8.353, 35.156}, //   8.261  27.3570
      {  899.5,  7.405, 35.103}, //   7.314  27.4559
      {  949.4,  6.821, 35.100}, //   6.728  27.5354
      {  999.2,  6.171, 35.068}, //   6.078  27.5966
      { 1099.1,  5.535, 35.070}, //   5.437  27.6787
      { 1199.5,  5.118, 35.060}, //   5.014  27.7214
      { 1299.0,  4.832, 35.046}, //   4.722  27.7440
      { 1399.5,  4.536, 35.023}, //   4.419  27.7596
      { 1499.3,  4.323, 35.010}, //   4.199  27.7732
      { 1599.0,  4.099, 34.988}, //   3.968  27.7801
      { 1699.3,  3.958, 34.981}, //   3.820  27.7899
      { 1799.1,  3.838, 34.974}, //   3.692  27.7974
      { 1899.1,  3.738, 34.969}, //   3.584  27.8043
      { 2004.6,  3.641, 34.968}, //   3.478  27.8139
      { 2500.0,  3.641, 34.968}
   };
   unsigned int n = sizeof(pts)/sizeof(HydroObs);
   
   // make sure there are at least 3 points in the profile
   if (n<3)
   {
      swifterr("error in SargassoHydrography::SargassoHydrography() ... "
               "Hydrographic profile must contain at least 3 points\n");
   }
   
   // make sure the profile starts at the surface
   if (pts[0].p!=0)
   {
      swifterr("error in SargassoHydrography::SargassoHydrography() ... "
               "First observation in profile must be at surface.\n");
   }
   
   // make sure the pressure increases monotonically
   for (unsigned int i=1; i<n; i++)
   {
      if (pts[i].p<=pts[i-1].p)
      {
         swifterr("error in SargassoHydrography::SargassoHydrography() ... "
                  "Hydrographic profile not monotonically increasing in pressure.\n");
      }
   }

   // load the profiles into the linear spline objects
   for (unsigned int i=0; i<n; i++)
   {
      t.Append(pts[i].p,pts[i].t); s.Append(pts[i].p,pts[i].s);
   }

   // seek extremes in the profiles
   t.SeekExtremes(); s.SeekExtremes();
   
   // make sure the density increases monotonically
   for (unsigned int i=1; i<n; i++)
   {
      if (Rho(pts[i].p)<=Rho(pts[i-1].p))
      {
         swifterr("error in SargassoHydrography::SargassoHydrography() ... "
                  "Hydrographic profile not monotonically increasing in density.\n");
      }
   }
}

/*------------------------------------------------------------------------*/
/*                                                                        */
/*------------------------------------------------------------------------*/
SubtropicalNePacificHydrography::SubtropicalNePacificHydrography(void)
{
   HydroObs pts[]=
   {
      //    lat     lon       date   time  zmax   stnid 
      //  36.60  236.89 07/20/2001 10.387  1999 544.009 
      //     p       t        s   //  theta    sigma
      {    0.0, 14.299,  33.611}, // 14.298  25.0522
      {    9.6, 14.295,  33.610}, // 14.294  25.0523
      {   19.5, 13.841,  33.609}, // 13.838  25.1464
      {   29.6, 12.132,  33.568}, // 12.128  25.4533
      {   39.2, 10.028,  33.493}, // 10.024  25.7717
      {   49.6,  9.967,  33.592}, //  9.961  25.8595
      {   59.5,  9.740,  33.732}, //  9.733  26.0068
      {   69.6,  9.558,  33.762}, //  9.550  26.0604
      {   79.4,  9.329,  33.796}, //  9.320  26.1245
      {   89.2,  9.108,  33.867}, //  9.098  26.2156
      {   99.4,  9.076,  33.926}, //  9.065  26.2671
      {  109.2,  8.996,  33.958}, //  8.984  26.3050
      {  119.5,  8.912,  33.977}, //  8.899  26.3333
      {  129.2,  8.811,  34.002}, //  8.797  26.3689
      {  139.3,  8.645,  34.029}, //  8.630  26.4160
      {  149.0,  8.519,  34.039}, //  8.504  26.4434
      {  159.3,  8.387,  34.057}, //  8.371  26.4779
      {  169.5,  8.303,  34.069}, //  8.286  26.5002
      {  179.6,  8.237,  34.078}, //  8.219  26.5173
      {  189.4,  8.069,  34.082}, //  8.050  26.5457
      {  199.4,  7.961,  34.084}, //  7.941  26.5634
      {  209.2,  7.787,  34.091}, //  7.766  26.5945
      {  219.3,  7.713,  34.098}, //  7.692  26.6109
      {  229.2,  7.621,  34.119}, //  7.599  26.6408
      {  239.3,  7.537,  34.127}, //  7.514  26.6593
      {  249.1,  7.509,  34.132}, //  7.485  26.6673
      {  259.1,  7.455,  34.133}, //  7.430  26.6759
      {  269.0,  7.417,  34.135}, //  7.391  26.6830
      {  279.5,  7.333,  34.145}, //  7.306  26.7029
      {  289.4,  7.269,  34.152}, //  7.242  26.7175
      {  299.2,  7.211,  34.153}, //  7.183  26.7265
      {  309.0,  7.138,  34.150}, //  7.109  26.7344
      {  318.9,  6.887,  34.128}, //  6.858  26.7515
      {  329.2,  6.860,  34.137}, //  6.830  26.7624
      {  339.2,  6.738,  34.127}, //  6.707  26.7710
      {  349.4,  6.580,  34.136}, //  6.549  26.7992
      {  359.6,  6.340,  34.125}, //  6.308  26.8220
      {  379.1,  6.056,  34.135}, //  6.023  26.8664
      {  399.3,  5.970,  34.156}, //  5.936  26.8940
      {  449.2,  5.718,  34.198}, //  5.680  26.9589
      {  499.5,  5.570,  34.227}, //  5.528  27.0003
      {  549.5,  5.385,  34.258}, //  5.340  27.0474
      {  599.2,  5.213,  34.281}, //  5.164  27.0863
      {  649.2,  5.087,  34.309}, //  5.034  27.1236
      {  699.4,  4.880,  34.348}, //  4.824  27.1785
      {  749.4,  4.651,  34.375}, //  4.592  27.2259
      {  799.6,  4.473,  34.394}, //  4.411  27.2607
      {  849.4,  4.339,  34.411}, //  4.274  27.2890
      {  899.2,  4.157,  34.431}, //  4.089  27.3244
      {  949.5,  4.030,  34.446}, //  3.958  27.3499
      {  999.4,  3.934,  34.454}, //  3.859  27.3664
      { 1007.2,  3.881,  34.462}, //  3.806  27.3782
      { 1049.3,  3.793,  34.473}, //  3.715  27.3961
      { 1099.5,  3.634,  34.483}, //  3.553  27.4201
      { 1149.3,  3.490,  34.494}, //  3.406  27.4431
      { 1199.3,  3.392,  34.500}, //  3.305  27.4576
      { 1249.4,  3.304,  34.506}, //  3.214  27.4710
      { 1299.2,  3.186,  34.515}, //  3.093  27.4895
      { 1349.1,  3.037,  34.525}, //  2.941  27.5113
      { 1399.5,  2.956,  34.534}, //  2.857  27.5261
      { 1449.2,  2.861,  34.537}, //  2.759  27.5372
      { 1499.2,  2.773,  34.544}, //  2.668  27.5508
      { 1549.0,  2.711,  34.548}, //  2.602  27.5597
      { 1599.4,  2.626,  34.555}, //  2.514  27.5729
      { 1649.4,  2.545,  34.561}, //  2.430  27.5849
      { 1699.3,  2.482,  34.566}, //  2.364  27.5944
      { 1749.1,  2.393,  34.574}, //  2.272  27.6085
      { 1799.1,  2.329,  34.579}, //  2.205  27.6180
      { 1849.4,  2.264,  34.584}, //  2.136  27.6275
      { 1899.2,  2.181,  34.589}, //  2.050  27.6384
      { 1949.5,  2.127,  34.595}, //  1.993  27.6478
      { 2001.0,  2.077,  34.602}, //  1.939  27.6576
      { 2500.0,  2.077,  34.602}
   };
   unsigned int n = sizeof(pts)/sizeof(HydroObs);
   
   // make sure there are at least 3 points in the profile
   if (n<3)
   {
      swifterr("error in SubtropicalNePacificHydrography::SubtropicalNePacificHydrography() ... "
               "Hydrographic profile must contain at least 3 points\n");
   }
   
   // make sure the profile starts at the surface
   if (pts[0].p!=0)
   {
      swifterr("error in SubtropicalNePacificHydrography::SubtropicalNePacificHydrography() ... "
               "First observation in profile must be at surface.\n");
   }
   
   // make sure the pressure increases monotonically
   for (unsigned int i=1; i<n; i++)
   {
      if (pts[i].p<=pts[i-1].p)
      {
         swifterr("error in SubtropicalNePacificHydrography::SubtropicalNePacificHydrography() ... "
                  "Hydrographic profile not monotonically increasing in pressure.\n");
      }
   }

   // load the profiles into the linear spline objects
   for (unsigned int i=0; i<n; i++)
   {
      t.Append(pts[i].p,pts[i].t); s.Append(pts[i].p,pts[i].s);
   }

   // seek extremes in the profiles
   t.SeekExtremes(); s.SeekExtremes();
   
   // make sure the density increases monotonically
   for (unsigned int i=1; i<n; i++)
   {
      if (Rho(pts[i].p)<=Rho(pts[i-1].p))
      {
         swifterr("error in SubtropicalNePacificHydrography::SubtropicalNePacificHydrography() ... "
                  "Hydrographic profile not monotonically increasing in density.\n");
      }
   }
}

/*------------------------------------------------------------------------*/
/*                                                                        */
/*------------------------------------------------------------------------*/
TropicalAtlanticHydrography::TropicalAtlanticHydrography(void)
{
   HydroObs pts[]=
   {
      //     lat     lon     date   time  zbot  zmax sh co           stnid     n
      //   -0.23    2.33 11/09/58  7.400 *****  4415  * 31  obs,831,299241    24
      //     p       t       s       theta  sigma     O2
      {    0.0, 26.530, 35.066}, // 26.530 22.919  4.380
      {   45.1, 23.060, 35.741}, // 23.051 24.483  4.270
      {   95.2, 17.210, 35.809}, // 17.194 26.090  3.120
      {  190.7, 13.950, 35.420}, // 13.922 26.529  3.210
      {  286.2, 11.510, 35.105}, // 11.474 26.771  1.530
      {  381.7,  8.330, 34.757}, //  8.290 27.039  1.940
      {  477.3,  7.320, 34.653}, //  7.274 27.107  2.450
      {  573.0,  6.230, 34.560}, //  6.179 27.182  2.820
      {  668.7,  5.390, 34.512}, //  5.334 27.249  2.980
      {  769.6,  4.660, 34.520}, //  4.599 27.340  3.200
      {  961.3,  4.330, 34.624}, //  4.255 27.460  3.530
      { 1158.2,  4.360, 34.746}, //  4.267 27.556  3.770
      { 1360.4,  4.200, 34.925}, //  4.090 27.717  4.670
      { 1557.8,  4.010, 34.962}, //  3.884 27.768  5.160
      { 1806.0,  3.760, 34.965}, //  3.615 27.798  5.310
      { 1998.7,  3.440, 34.959}, //  3.281 27.826  5.460
      { 2196.6,  3.250, 34.949}, //  3.075 27.838  5.350
      { 2491.4,  2.950, 34.939}, //  2.752 27.859  5.330
      { 2791.6,  2.790, 34.922}, //  2.565 27.862  5.390
      { 3087.1,  2.670, 34.915}, //  2.417 27.869  5.230
      { 3383.0,  2.540, 34.908}, //  2.259 27.877  5.320
      { 3986.3,  2.380, 34.894}, //  2.037 27.884  5.310
      { 4283.4,  2.340, 34.887}, //  1.963 27.884  5.330
      { 4483.4,  2.290, 34.883}, //  1.891 27.887  5.260
   };
   unsigned int n = sizeof(pts)/sizeof(HydroObs);
   
   // make sure there are at least 3 points in the profile
   if (n<3)
   {
      swifterr("error in TropicalAtlanticHydrography::TropicalAtlanticHydrography() ... "
               "Hydrographic profile must contain at least 3 points\n");
   }
   
   // make sure the profile starts at the surface
   if (pts[0].p!=0)
   {
      swifterr("error in TropicalAtlanticHydrography::TropicalAtlanticHydrography() ... "
               "First observation in profile must be at surface.\n");
   }
   
   // make sure the pressure increases monotonically
   for (unsigned int i=1; i<n; i++)
   {
      if (pts[i].p<=pts[i-1].p)
      {
         swifterr("error in TropicalAtlanticHydrography::TropicalAtlanticHydrography() ... "
                  "Hydrographic profile not monotonically increasing in pressure.\n");
      }
   }

   // load the profiles into the linear spline objects
   for (unsigned int i=0; i<n; i++)
   {
      t.Append(pts[i].p,pts[i].t); s.Append(pts[i].p,pts[i].s);
   }

   // seek extremes in the profiles
   t.SeekExtremes(); s.SeekExtremes();
   
   // make sure the density increases monotonically
   for (unsigned int i=1; i<n; i++)
   {
      if (Rho(pts[i].p)<=Rho(pts[i-1].p))
      {
         swifterr("error in TropicalAtlanticHydrography::TropicalAtlanticHydrography() ... "
                  "Hydrographic profile not monotonically increasing in density.\n");
      }
   }
}

/*------------------------------------------------------------------------*/
/* function to compute the insitu density at a given pressure             */
/*------------------------------------------------------------------------*/
double Hydrography::Rho(float p)
{
   double Rho = 1 + eqstate(S(p),T(p),p)/1000;

   return Rho;
}

/*------------------------------------------------------------------------*/
/* function to compute the salinity at a given pressure                   */
/*------------------------------------------------------------------------*/
double Hydrography::S(float p) 
{
   double S=NaN;
   
   if (inCRange(s.xmin,p,s.xmax)) {S=s(p);}
   else swifterr("error in Hydrography::S() ... "
                 "pressure (%g) out of range: [%g, %g]\n",p,s.xmin,s.xmax);

   return S;
}

/*------------------------------------------------------------------------*/
/* function to compute the temperature at a given pressure                */
/*------------------------------------------------------------------------*/
double Hydrography::T(float p) 
{
   double T=NaN;
   
   if (inCRange(t.xmin,p,t.xmax)) {T=t(p);}
   else swifterr("error in Hydrography::T() ... "
                 "pressure (%g) out of range: [%g, %g]\n",p,t.xmin,t.xmax);

   return T;
}

#endif // HYDROOBS_CPP
#endif // HYDROOBS_OOP
