#ifndef LITHIUMBATTERYPACK_OOP
#define LITHIUMBATTERYPACK_OOP

#include <fstream>
#include <defs.oop>

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * $Id: LithiumBatteryPack.cpp,v 1.1 2006/11/11 18:29:34 swift Exp $
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * RCS Log:
 *
 * $Log: LithiumBatteryPack.cpp,v $
 * Revision 1.1  2006/11/11 18:29:34  swift
 * Added energy budget model to the project.
 *
 *========================================================================*/
/*                                                                        */
/*========================================================================*/
class LithiumBatteryPack
{
   // data members
   private:

      double V;                       // battery potential (volts)      
      double Imax;                    // maximum current that can be drawn from the battery pack
      double E;                       // energy reserves (Joules)
      const double Eo;                // energy reserves of a fresh pack (Joules)
      unsigned int NPack;             // number of battery packs in parallel
      const double SelfDischargeRate; // Shelf losses (%/year)
      
   // function members
   public:

      // initializer constructor
      LithiumBatteryPack(double Energy, unsigned int npack, double MaxAmp=1.0, double Volts=15.8);
      
      // function to return the remaining energy reserve of the battery
      double EnergyConsumed(void) const {return Eo-E;}

      // function to return the remaining energy reserve of the battery
      double EnergyReserve(void) const {return E;}

      // function to drain a specified amount of energy from the battery back
      bool Drain(double Joules);
         
      // function to return a reference to the maximum current drain allowed
      double &MaxCurrent(void) {return Imax;}
      
      // function to list the battery properties
      void Properties(ostream &dest=cout) const;

      // function to return internal resistance of the battery pack
      double R(void) const {return 2.0/NPack;}
      
      // function to return the self-discharge over a specified time interval
      double SelfDischargeEnergyConsumed(double days) const;

      // function to return reference to the battery voltage
      double Volts(double Amps=0) const;
};

#ifdef  LITHIUMBATTERYPACK_CPP

/*------------------------------------------------------------------------*/
/* initializer constructor                                                */
/*------------------------------------------------------------------------*/
LithiumBatteryPack::LithiumBatteryPack(double Energy, unsigned int npack,
                                       double MaxAmp, double Volts):
      V(Volts), Imax(MaxAmp), E(Energy), Eo(Energy), NPack(npack),
      SelfDischargeRate(0.02)
{}

/*------------------------------------------------------------------------*/
/*                                                                        */
/*------------------------------------------------------------------------*/
bool LithiumBatteryPack::Drain(double Joules)
{
   bool status=false;
   
   // make sure the energy to be drained from battery pack is non-negative      
   if (Joules<0) message("warning in LithiumBatteryPack::Drain() ... "
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
void LithiumBatteryPack::Properties(ostream &dest) const
{
   fprintf(dest,"$ Battery model:              %s\n"
             "$    Maximum current:         %g Amp\n"
             "$    Initial energy reserves: %g kJoules\n"
             "$    Number of battery packs: %d\n"
             "$    Self-discharge rate:     %g%/year\n"
             "$\n","Lithium",Imax,Eo/1000,NPack,SelfDischargeRate*100);
}

/*------------------------------------------------------------------------*/
/* function to return the self-discharge over a specified time interval   */
/*------------------------------------------------------------------------*/
double LithiumBatteryPack::SelfDischargeEnergyConsumed(double days) const
{
   if (days<0)
   {
      swifterr("error in LithiumBatteryPack::SelfDischargeEnergyConsumed() ... "
               "self-discharge interval may not be negative.\n");
   }

   // compute the amount of energy lost due to self-discharge
   double dE = EnergyReserve()*SelfDischargeRate*days/365;

   return dE;
}

/*------------------------------------------------------------------------*/
/*                                                                        */
/*------------------------------------------------------------------------*/
double LithiumBatteryPack::Volts(double Amps) const
{
   double v;

   // make sure that the current drain is within allowed limits
   if (Amps<0 || Amps>Imax) swifterr("error in LithiumBatteryPack::Volts() ... "
                                     "current (%g Amps) exceeds maximum allowed (%g Amps).\n",
                                     Amps,Imax);

   // compute the voltage under the applied load
   v = V - Amps*R();

   return v;
}

#endif // LITHIUMBATTERYPACK_CPP
#endif // LITHIUMBATTERYPACK_OOP
