#ifndef ALKALINEBATTERYPACK_OOP
#define ALKALINEBATTERYPACK_OOP

#include <lsprf.oop>

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * $Id: AlkalineBatteryPack.cpp,v 1.1 2006/11/11 18:29:34 swift Exp $
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * RCS Log:
 *
 * $Log: AlkalineBatteryPack.cpp,v $
 * Revision 1.1  2006/11/11 18:29:34  swift
 * Added energy budget model to the project.
 *
 *========================================================================*/
/*                                                                        */
/*========================================================================*/
class AlkalineBatteryPack
{
   // data members
   private:

      double Imax;                    // maximum current that can be drawn from the battery pack
      double E;                       // energy reserves (Joules) 
      const double Eo;                // energy reserves of a fresh pack (Joules)
      const unsigned int NPack;       // number of battery packs in parallel
      const double SelfDischargeRate; // Shelf losses (%/year)
      LSplineProfile v;               // open-circuit voltage
      LSplineProfile r;               // internal resistance
      
   // function members
   public:

      // initializer constructor
      AlkalineBatteryPack(double Energy, unsigned int NPack, double MaxCurrent=1.0);

      // function to return the energy consumed for an open-curcuit potential of Vo
      double EnergyConsumed(double Vo) {return v.Invert(Vo)*Eo;}
         
      // function to return the remaining energy reserve of the battery
      double EnergyConsumed(void) const {return Eo-E;}
      
      // function to return the remaining energy reserve of the battery
      double EnergyReserve(void) const {return E;}

      // function to drain a specified amount of energy from the battery back
      bool Drain(double Joules);
         
      // function to return a reference to the maximum current drain allowed
      double &MaxCurrent(void) {return Imax;}

      // function to list the battery properties
      void Properties(ostream &dest=cout);
      
      // function to return internal resistance of the battery pack
      double R(void) {return r((Eo-E)/Eo)/NPack;}
      
      // function to return internal resistance of the battery pack
      double R(double E) {return r(E/Eo)/NPack;}

      // function to return the self-discharge over a specified time interval
      double SelfDischargeEnergyConsumed(double days) const;
      
      // function to return the open-circuit potential
      double Vo(void) {return v((Eo-E)/Eo);}

      // function to return the open-circuit potential
      double Vo(double E) {return v(E/Eo);}
      
      // function to return reference to the battery voltage
      double Volts(double Amps=0);
};

#ifdef  ALKALINEBATTERYPACK_CPP

/*------------------------------------------------------------------------*/
/* initializer constructor                                                */
/*------------------------------------------------------------------------*/
AlkalineBatteryPack::AlkalineBatteryPack(double Energy, unsigned int npack,
                                         double MaxCurrent):
      Imax(MaxCurrent), E(Energy), Eo(Energy), NPack(npack), SelfDischargeRate(0.05)
{
   //       Energy      R             Energy     Vo
   r.Append(   0.0,  7.00);  v.Append(   0.0, 15.70);  
   r.Append(  51.5,  7.66);  v.Append(  25.1, 15.07);
   r.Append( 101.8, 10.44);  v.Append(  50.2, 14.66);
   r.Append( 150.6, 12.25);  v.Append( 100.5, 14.16);
   r.Append( 200.9, 12.04);  v.Append( 151.9, 13.80);
   r.Append( 301.4, 12.48);  v.Append( 202.2, 13.64);
   r.Append( 401.7, 14.71);  v.Append( 300.2, 13.24);
   r.Append( 502.1, 16.54);  v.Append( 400.4, 12.94);
   r.Append( 552.4, 18.69);  v.Append( 500.9, 12.42);
   r.Append( 575.0, 20.43);  v.Append( 551.1, 12.08);
   r.Append( 600.1, 23.90);  v.Append( 601.3, 11.39);
   r.Append( 617.7, 31.00);  v.Append( 626.4, 10.84);
   r.Append( 640.2, 43.20);  v.Append( 651.5,  9.98);
   r.Append( 660.3, 57.25);  v.Append( 675.0,  9.49);
   r.Append( 675.0, 77.52);
   
   // search for the extremes of the profiles                              
   r.SeekExtremes();         v.SeekExtremes();

   // rescale the abscissa ('age' of the battery pack) to go from 0 to 1
   for (unsigned int i=0; i<v.NPnt(); i++) {v.x[i] /= v.xmax;}
   for (unsigned int i=0; i<r.NPnt(); i++)  {r.x[i] /= r.xmax;}
}

/*------------------------------------------------------------------------*/
/*                                                                        */
/*------------------------------------------------------------------------*/
bool AlkalineBatteryPack::Drain(double Joules)
{
   bool status=false;
   
   // make sure the energy to be drained from battery pack is non-negative      
   if (Joules<0) message("warning in AlkalineBatteryPack::Drain() ... "
                         "negative energy drain (%g) not allowed.\n",Joules);
   
   // check if the battery pack has sufficient energy stores
   else if (Joules>E) {E=0;}

   // drain the specified amount of energy from the battery packs
   else {E-=Joules; status=true;}

   return status;
}

/*------------------------------------------------------------------------*/
/* function to list the battery properties                                */
/*------------------------------------------------------------------------*/
void AlkalineBatteryPack::Properties(ostream &dest)
{
   fprintf(dest,"$ Battery model:              %s\n"
             "$    Maximum current:         %g Amp\n"
             "$    Initial energy reserves: %g kJoules\n"
             "$    Number of battery packs: %d\n"
             "$    Self-discharge rate:     %g%/year\n"
             "$    Open-circuit potential as a function of energy consumed (kJoules, Volts):",
             "Alkaline",Imax,Eo/1000,NPack,SelfDischargeRate*100);

   
   for (unsigned int i=0; i<v.NPnt(); i++)
   {
      if (!(i%5)) fprintf(dest,"\n$      ");
      fprintf(dest,"  (%4.0f, %4.1f)",v.x[i]*Eo/1000,v.y[i]);
   }
   fprintf(dest,"\n$    Effective internal resistance as a "
             "function of energy consumed (kJoules, Ohms):");
   
   for (unsigned int i=0; i<r.NPnt(); i++)
   {
      if (!(i%5)) fprintf(dest,"\n$      ");
      fprintf(dest,"  (%4.0f, %4.1f)",r.x[i]*Eo/1000,r.y[i]);
   }
   fprintf(dest,"\n$\n");
}

/*------------------------------------------------------------------------*/
/* function to return the self-discharge over a specified time interval   */
/*------------------------------------------------------------------------*/
double AlkalineBatteryPack::SelfDischargeEnergyConsumed(double days) const
{
   if (days<0)
   {
      swifterr("error in AlkalineBatteryPack::SelfDischargeEnergyConsumed() ... "
               "self-discharge interval may not be negative.\n");
   }

   // compute the amount of energy lost due to self-discharge
   double dE = EnergyReserve()*SelfDischargeRate*days/365;

   return dE;
}

/*------------------------------------------------------------------------*/
/*                                                                        */
/*------------------------------------------------------------------------*/
double AlkalineBatteryPack::Volts(double Amps)
{
   double V;

   // make sure that the current drain is within allowed limits
   if (Amps<0 || Amps>Imax) swifterr("error in AlkalineBatteryPack::Volts() ... "
                                     "current (%g Amps) exceeds maximum allowed (%g Amps).\n",
                                     Amps,Imax);

   // compute the voltage under the applied load
   V = v((Eo-E)/Eo) - R()*Amps;

   return V;
}

#endif // ALKALINEBATTERYPACK_CPP
#endif // ALKALINEBATTERYPACK_OOP
