#ifndef APEX260SBE41CPAPF9I_OOP
#define APEX260SBE41CPAPF9I_OOP

#include <Apex260.oop> 
#include <Sbe41cp.oop>
#include <Apf9i.oop>
#include <Iridium.oop>
#include <map>
#include <vector>
#include <EnergyBudget.oop>
#include <SmplStat.oop>

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * $Id: Apex260Sbe41cpApf9i.cpp,v 1.5 2008/07/14 17:05:16 swift Exp $
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * RCS Log:
 *
 * $Log: Apex260Sbe41cpApf9i.cpp,v $
 * Revision 1.5  2008/07/14 17:05:16  swift
 * Implement compensator hyper-retraction feature.
 *
 * Revision 1.4  2006/11/22 04:05:54  swift
 * Added TelemetryPayloadReport().
 *
 * Revision 1.3  2006/11/19 17:11:46  swift
 * Change the name of the energy-budget model source code file to follow
 * current name conventions.
 *
 * Revision 1.2  2006/11/19 15:08:22  swift
 * Fixed missing parameter in model output.
 *
 * Revision 1.1  2006/11/18 17:42:37  swift
 * Energy model for iridium floats.
 *
 *========================================================================*/
/*                                                                        */
/*========================================================================*/
template <class BatteryPack>
class Apex260Sbe41cpIridium
{
   // data members
   private:

      // define the number STP sample
      const size_t bytes_per_sample;

      // define the down time
      unsigned int down_time;

      // define the ballast piston position
      unsigned int ballast_piston_position;

      // define the initial piston extension 
      unsigned int initial_piston_extension;

      // define the number of counts to full piston extension
      unsigned int piston_full_extension;

      // define the number of counts for compensator hyper-retraction
      unsigned int compensator_hyper_retraction;

      // define the pressure where CP mode is activated
      float CpActivationP;
      
      // define the sample interval for pressure (dbars)
      float p_interval;
      
      // define the target pressure
      float ptrg;

      // define the park pressure
      float pprk;

      // define the park-n-profile schedule
      unsigned int n;
      
      // define the vertical rate of ascent
      float dPdt;

      // define the p-sample period for autoballasting
      float autoballast_p_sample_period; // Hours

      // define the p-sample period during vertical ascent
      float ascent_p_sample_period; // Seconds

      // define the table to hold the sampled pressures
      vector<float> ptable;

      // maintain statistics of the number of kilobytes telemetered
      SampleStatistic XmitKbytes;

      // add the buoyancy engine
      Apex260<BatteryPack> apex260;

      // add the sensor package
      Sbe41cp<BatteryPack> sbe41cp;

      // add the controller
      Apf9i<BatteryPack> apf9i;

      // add the Iridium modem
      Iridium<BatteryPack> iridium;
      
   // function members
   public:

      // initialization constructor
      Apex260Sbe41cpIridium(unsigned int down_time,float ptrg,
                            float pprk,unsigned int n,
                            unsigned int CompHypRet,
                            float CpActivationP=1000);

      // function to execute a single profile
      bool ExecuteProfile(BatteryPack &bat, Hydrography &sea, EnergyBudgetMap &energy_budget);

      // function to write the mission parameters
      void Mission(ostream &dest=cout);

      // function to write a report of the telemetry payload
      void TelemetryPayloadReport(ostream &dest=cout);
}; 

/*------------------------------------------------------------------------*/
/*                                                                        */
/*------------------------------------------------------------------------*/
template <class BatteryPack>
void Apex260Sbe41cpIridium<BatteryPack>::Mission(ostream &dest)
{
   fprintf(dest,"$ Float Model:                                   %s\n" 
           "$    Down time:                                  %d hours\n"
           "$    Ballast piston position:                    %d\n"
           "$    Initial piston extension:                   %d\n"
           "$    Piston full extension:                      %d\n"
           "$    Compensator hyper-retraction:               %d\n"           
           "$    Target pressure:                            %g dbar\n"
           "$    Park pressure:                              %g dbar\n"
           "$    CP activation pressure:                     %g dbar\n"
           "$    Park-n-Profile cycle length:                %d\n"
           "$    Vertical rate of ascent:                    %g dbar/sec\n"
           "$    Pressure sample-rate during autoballast:    %g hr\n"
           "$    Pressure sample-rate during low-res ascent: %g sec\n"
           "$    Pressure sample interval in vertical:       %g dbar\n"
           "$    Table of sampled pressures (dbar):",
           "Apex260Sbe41cpIridium",down_time,
           ballast_piston_position,initial_piston_extension,
           piston_full_extension,compensator_hyper_retraction,ptrg,pprk,
           CpActivationP,n,dPdt,autoballast_p_sample_period,
           ascent_p_sample_period,p_interval);

   int i,n;
   
   for (n=0,i=ptable.size()-1; i>=0; i--,n++)
   {
      if (!(n%15)) fprintf(dest,"\n$      ");
      fprintf(dest," %4.0f",ptable[i]);
   }
   fprintf(dest,"\n$\n");

   apex260.Properties(dest);
   sbe41cp.Properties(dest);
   apf9i.Properties(dest);
   iridium.Properties(dest);
}

/*------------------------------------------------------------------------*/
/*                                                                        */
/*------------------------------------------------------------------------*/
template <class BatteryPack>
Apex260Sbe41cpIridium<BatteryPack>::Apex260Sbe41cpIridium(unsigned int down_time,
                                                          float ptrg,float pprk,
                                                          unsigned int n,
                                                          unsigned int CompHypRet,
                                                          float CpActivationP):
   bytes_per_sample(14),                       // encoding factor
   ballast_piston_position(16),                // counts
   initial_piston_extension(25),               // counts
   piston_full_extension(227),                 // counts
   compensator_hyper_retraction(CompHypRet),   // counts 
   p_interval(2.0),                            // pressure sample interval (dbars)
   dPdt(0.10),                                 // vertical ascent rate (dbar/sec)
   autoballast_p_sample_period(1),             // hours
   ascent_p_sample_period(10)                  // seconds
{
   // the sample table has 50dbar bins between 2000 dbar and 400 dbar
   for (int p=2000; p>400; p-=50) ptable.push_back(p);

   // the sample table has 20dbar bins between 400 dbar and 350 dbar
   for (int p=400; p>350; p-=20) ptable.push_back(p);

   // the sample table has 10dbar bins between 350 dbar and the surface
   for (int p=350; p>0; p-=10) ptable.push_back(p);

   // the shallowest sample is at 6 dbar
   ptable.push_back(6);

   // set the pressure where CP mode is activated
   this->CpActivationP=CpActivationP;
   
   if (ptrg>0) this->ptrg=ptrg;
   else swifterr("error in Apex260Sbe41cpIridium::Apex260Sbe41cpIridium() ... "
                 "Target pressure must be positive\n");

   if (pprk>0 && pprk<=ptrg) this->pprk=pprk;
   else swifterr("error in Apex260Sbe41cpIridium::Apex260Sbe41cpIridium() ... "
                 "Park pressure must be positive and less than the target pressure.\n");

   if (n>0) this->n=n;
   else swifterr("error in Apex260Sbe41cpIridium::Apex260Sbe41cpIridium() ... "
                 "Park-n-Profile cycle length must be positive\n");
   
   if (down_time>=13) this->down_time=down_time;
   else swifterr("error in Apex260Sbe41cpIridium::Apex260Sbe41cpIridium() ... "
                 "Down time must not be less than 13 hours.\n");

   // seed the random number generator
   srandom(time(NULL));
}

/*------------------------------------------------------------------------*/
/* function to execute a single profile                                   */
/*------------------------------------------------------------------------*/
template <class BatteryPack>
bool Apex260Sbe41cpIridium<BatteryPack>::ExecuteProfile(BatteryPack &bat,Hydrography &sea,
                                                   EnergyBudgetMap &energy_budget)
{ 
   // initialize the budget break-down
   static const string engine="Apex(260ml)";
   static const string sensor="Sbe41cp";    
   static const string controller="Apf9i"; 
   static const string telemetry="Iridium/GPS"; 
   static const string selfdischarge="Self-Discharge";
   static const string total="Total"; 
   static unsigned int prf=0;
   float p,dE,dV;
   unsigned int cnt,fcnt;
   bool status=true;
   float E_engine=0,E_sensor=NaN,E_controller=NaN,E_telemetry=NaN,E_selfdischarge=NaN;
   int m;

   // initialize the number of bytes to telemeter
   size_t eng_data=17000+(size_t)(7000*(((float)random())/RAND_MAX)), telemetry_bytes=eng_data;
   
   // make sure the the energy budget map is empty
   if (!energy_budget.empty()) energy_budget.erase(energy_budget.begin(),energy_budget.end());
   
   /********************* Buoyancy Engine *******************************************/

   // compute the buoyancy needed to reach the park pressure
   dV=apex260.dV(ptrg,pprk,sea);

   // compute the piston extension when the float reaches the park pressure
   cnt = ballast_piston_position + (int)(dV/apex260.dVdC());

   // check if a deep profile is indicated
   if (!(prf%n) || prf==1)
   {
      // set the pressure to the deep target
      p=ptrg;
   
      // compute the amount of energy needed to retract the piston under zero pressure
      dE=apex260.EnergyCost(bat,0,cnt-ballast_piston_position);

      // make sure the battery can supply the energy for the initial piston extension
      if (!isnan(dE) && bat.Drain(dE)) {E_engine+=dE;} else {status=false; goto profile_failed;}
   }

   // set the pressure to the park level
   else p=pprk;
   
   // compute the amount of energy needed to extend the piston under full pressure
   dE=apex260.EnergyCost(bat,p,initial_piston_extension);

   // make sure the battery can supply the energy for the initial piston extension
   if (!isnan(dE) && bat.Drain(dE)) {E_engine+=dE;} else {status=false; goto profile_failed;}
      
   // compute the energy needed to elevate the float to the surface
   dE=apex260.EnergyCost(sea,bat,p,0.0); 

   // make sure the battery can supply the energy to elevate the float to the surface
   if (!isnan(dE) && bat.Drain(dE)) {E_engine+=dE;} else {status=false; goto profile_failed;}

   // compute the buoyancy needed to reach the surface
   dV=apex260.dV(ptrg,0,sea);

   // compute the piston extension when the float reaches the surface
   cnt = ballast_piston_position + initial_piston_extension + (int)(dV/apex260.dVdC());

   // compute the final piston extension on the surface
   fcnt=cnt+initial_piston_extension;

   // check if the float can generate enough buoyancy to reach the surface
   if (cnt>piston_full_extension)
   {
      // warn the user that the float can't reach the surface
      message("warning in Apex260Sbe41cpIridium::ExecuteProfile() ... "
              "Float has insufficient buoyancy to reach surface.\n");
   }

   // check if the float can extend piston to provide additional surface buoyancy
   else if (fcnt>piston_full_extension) 
   {
      // warn the user that the float is marginally operable
      message("warning in Apex260Sbe41cpIridium::ExecuteProfile() ... "
              "Piston at maximum extension.\n");

      // reset the final extension to the full extension
      fcnt=piston_full_extension;
   }
         
   // compute the amount of energy needed to extend the piston under zero pressure
   dE=apex260.EnergyCost(bat,0,fcnt-cnt);
   
   // make sure the battery can supply the energy for the final piston extension
   if (!isnan(dE) && bat.Drain(dE)) {E_engine+=dE;} else {status=false; goto profile_failed;}

   // compute the buoyancy needed to reach the park pressure
   dV=apex260.dV(ptrg,pprk,sea);

   // compute the piston extension when the float reaches the park pressure
   cnt = ballast_piston_position + (int)(dV/apex260.dVdC());

   // check for N2 compensator hyper-retraction
   if (compensator_hyper_retraction)
   {
      if (cnt<(compensator_hyper_retraction+9)) cnt=9;
      else {cnt = (cnt-compensator_hyper_retraction);}
   }
   
   // retract the piston back to the park position
   dE=apex260.EnergyCost(bat,0,fcnt-cnt);
   
   // compute the piston extension when the float reaches the park pressure
   fcnt = ballast_piston_position + (int)(dV/apex260.dVdC());
   
   // undo the N2 compensator hyper-retraction (extension happens at park pressure)
   if (compensator_hyper_retraction) dE+=apex260.EnergyCost(bat,pprk,fcnt-cnt);

   // make sure the battery can supply the energy for the final piston extension
   if (!isnan(dE) && bat.Drain(dE)) {E_engine+=dE;} else {status=false; goto profile_failed;}
   
   /********************* Sensor Modual *********************************************/
   
   // count the number of STP spot-samples taken
   unsigned int i,nsamples;

   // initialize the energy consumed by sensor module and number of spot samples 
   dE=0; nsamples=2;

   // compute the number of P-only samples taken during the down time
   m=(int)(down_time/autoballast_p_sample_period);

   // add P-only samples during the go-deep phase of a deep profile (samples every 5 min)
   if (!(prf%n)||(prf==1)) m+=(int)((ptrg-pprk)/dPdt/300);

   // validate the activation pressure 
   if (CpActivationP<0) CpActivationP=0; else if (CpActivationP>p) CpActivationP=p;
   
   // check if low-res spot-samples will be collected
   if (p>CpActivationP)
   {
      // count the number of spot samples to be collected 
      for (i=0; i<ptable.size(); i++)
      {
         if (ptable[i]<=p && ptable[i]>CpActivationP) {nsamples++;}
      }
       
      // compute the number of P-only samples taken during the low-res ascent
      m+=(int)(((p-CpActivationP)/dPdt)/ascent_p_sample_period);

      // add bytes to the telemetry payload
      telemetry_bytes+=nsamples*30;
   }

   // test if high-resolution samples will be collected
   if (CpActivationP>0)
   {
      // compute the energy spent in CP mode
      dE+=sbe41cp.CtdEnergyCost((time_t)(CpActivationP/dPdt));

      // add bytes to the telemetry payload
      telemetry_bytes+=(size_t)(bytes_per_sample*CpActivationP/p_interval);
   }
   
   // compute the energy consumed for P-only samples
   dE += m*sbe41cp.PEnergyCost(bat);
   
   // compute the energy consumed for PT samples at the park level
   dE += down_time*sbe41cp.PtEnergyCost(bat);

   // add telemetry for park-level PT samples
   telemetry_bytes+=(size_t)(down_time*70);

   // compute the energy consumed for PTS spot samples
   dE += nsamples*sbe41cp.CtdEnergyCost(bat);

   // make sure that the battery can supply the energy
   if (!isnan(dE) && bat.Drain(dE)) {E_sensor=dE;} else {status=false; goto profile_failed;}
   
   /********************* Controller ************************************************/
    
   // compute the metabolic drain by the controller
   dE = apf9i.MetabolicEnergyCost(bat,down_time);
 
   // compute the number of P-only samples taken during the down time
   m=(int)(down_time/autoballast_p_sample_period);

   // add P-only samples during the go-deep phase of a deep profile (samples every 5 min)
   if (!(prf%n)||(prf==1)) m+=(int)((ptrg-pprk)/dPdt/300);

   // add P-only samples taken during the profile
   m+=(int)((p/dPdt)/ascent_p_sample_period);

   // add energy consumed by the Apf9i for boot-up
   dE += m*apf9i.BootUp();

   // add energy consumed by the Apf9i for P-only samples
   dE += m*apf9i.PEnergyCost();
   
   // add energy consumed by Apf9i for PT samples
   dE += down_time*apf9i.PtEnergyCost();
   
   // add energy consumed by Apf9i for PTS spot-samples
   dE += nsamples*apf9i.PtsEnergyCost();

   // add energy consumed by Apf9i for telemetry  
   dE += bat.Volts()*apf9i.WakeCurrent()*(iridium.LoginTime()+iridium.BytesPerSec());
      
   // make sure that the battery can supply the energy
   if (bat.Drain(dE)) {E_controller=dE;} else {status=false; goto profile_failed;}
  
   /********************* Telemetry *************************************************/

   // compute the energy consumed for data telemetry
   dE = iridium.EnergyCost(telemetry_bytes);

   // make sure that the battery can supply the energy
   if (!isnan(dE) && bat.Drain(dE)) {E_telemetry=dE;} else {status=false; goto profile_failed;}
  
   /********************* Battery Self-Discharge ************************************/

   // compute the self-discharge of the battery
   dE = bat.SelfDischargeEnergyConsumed(down_time/24.0);
 
   // make sure that the battery can supply the energy
   if (!isnan(dE) && bat.Drain(dE)) {E_selfdischarge=dE;} else {status=false; goto profile_failed;}

   /*********************************************************************************/

   // record the energy consumed by each major subsystem
   energy_budget[engine]=E_engine;
   energy_budget[sensor]=E_sensor;
   energy_budget[controller]=E_controller;
   energy_budget[selfdischarge]=E_selfdischarge;
   energy_budget[telemetry]=E_telemetry;
   energy_budget[total]=E_engine+E_sensor+E_controller+E_telemetry+E_selfdischarge;
 
   // maintain statistics of the number of kilobytes telemetered
   XmitKbytes += (float)telemetry_bytes/1024.0;

   // increment the profile counter
   prf++;
   
   // this is a collection point when the profile fails for lack of energy
   profile_failed: 
   
   return status;
}

/*------------------------------------------------------------------------*/
/*                                                                        */
/*------------------------------------------------------------------------*/
template <class BatteryPack>
void Apex260Sbe41cpIridium<BatteryPack>::TelemetryPayloadReport(ostream &dest)
{
   fprintf(dest,"$ Telemetry payload:\n"
           "$    Number of profiles: %d\n"
           "$    Total: %0.1f kbytes\n"
           "$    Mean: %0.1f kbytes/profile\n"
           "$    Standard Deviation: %0.1f kbytes/profile\n"
           "$    Minimum: %0.1f kbytes/profile\n"
           "$    Maximum: %0.1f kbytes/profile\n"
           "$\n",XmitKbytes.samples(),XmitKbytes.mean()*XmitKbytes.samples(),
           XmitKbytes.mean(),XmitKbytes.stdDev(),XmitKbytes.min(),
           XmitKbytes.max());
}

#endif // APEX260SBE41CPAPF9I_OOP

#include <GetOpt.oop>
#include <LithiumBatteryPack.oop>
#include <AlkalineBatteryPack.oop>

/*========================================================================*/
/*                                                                        */
/*========================================================================*/
int main(int argc,char *argv[])
{
   enum {down_,Eo_,of_,a_,m_,n_,chr_,cp_,pprk_,ptrg_,v_,NCMD};
   MetaCmd cmd[NCMD]=
   {
      {"down=","   down-time (hours). [232 hr]"},
      {"Eo=","     initial battery reserves (kJoules). [3900 kJoules]"},
      {"of=","     pathname to output file. [stdout]"},
      {"-a","      use alkaline energy source (default is lithium)."},
      {"m=","      number of battery packs in parallel. [3]"},
      {"n=","      length of Park-n-Profile cycle. [1]"},
      {"chr=","    compensator hyper-retraction (counts). [0]"},
      {"cp=","     activation pressure for CP mode (dbar). [1000dbars]"},
      {"pprk=","   park pressure (dbar). [1000 dbar]"},
      {"ptrg=","   target pressure (dbar). [2000 dbar]"},
      {"-v","      verbose mode (default is terse summary)."},
   };
   
   // check for usage query
   check_cmd_line(1,make_usage(cmd,NCMD));

   // link metacommands to command line arguments
   link_meta_cmds(cmd,NCMD,argc,argv);
   
   double pprk = (cmd[pprk_].arg) ? atof(cmd[pprk_].arg) : 1000;
   double ptrg = (cmd[ptrg_].arg) ? atof(cmd[ptrg_].arg) : 2000;
   double cp = (cmd[cp_].arg) ? atof(cmd[cp_].arg) : 1000;
   double Eo   = (cmd[Eo_].arg)   ? atof(cmd[Eo_].arg)*1000   : 3.9e6;
   unsigned int down = (cmd[down_].arg) ? atoi(cmd[down_].arg) : 232;
   unsigned int m = (cmd[m_].arg) ? atoi(cmd[m_].arg) : 3;
   unsigned int n = (cmd[n_].arg) ? atoi(cmd[n_].arg) : 1;
   unsigned int chr = (cmd[chr_].arg) ? atoi(cmd[chr_].arg) : 0;
   bool verbose = (cmd[v_].arg) ? true : false;
   ostream dest(cout.rdbuf());
   filebuf obuf;

   if (cmd[of_].arg)
   {
      // open the designated file
      open_stream(cmd[of_].arg,ios::out,obuf); dest.rdbuf(&obuf);

      fprintf(dest,"$ Cmd Line:");
      for (int i=0; i<argc; i++) {fprintf(dest," %s",argv[i]);}
      fprintf(dest,"\n$\n");
   }

   SIndianOceanHydrography sea;
   
   if (cmd[a_].arg)
   {
      AlkalineBatteryPack alkaline(Eo,m);
      Apex260Sbe41cpIridium<AlkalineBatteryPack> apex(down,ptrg,pprk,n,chr,cp);
      EnergyBudget(apex,alkaline,sea,dest,verbose);
   }
   else
   {
      LithiumBatteryPack lithium(Eo,m);
      Apex260Sbe41cpIridium<LithiumBatteryPack> apex(down,ptrg,pprk,n,chr,cp);
      EnergyBudget(apex,lithium,sea,dest,verbose);
   }
}
