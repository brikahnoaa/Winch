#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
# $Id: Apf9.Readme,v 1.3 2008/07/14 17:09:24 swift Exp $
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
# RCS Log:
#
# $Log: Apf9.Readme,v $
# Revision 1.3  2008/07/14 17:09:24  swift
# Update the deployable firmware list.
#
# Revision 1.2  2007/10/05 22:34:32  swift
# Add documentation about mission parameters.
#
# Revision 1.1  2004/06/07 22:33:47  swift
# Initial revision
#
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

A fundamental architectural difference between the APF8 and the APF9 is that
the APF8 firmware runs exactly once for each mission whereas the APF9 runs
thousands of times each mission.  The APF8's processor state is maintained
during each sleep period and upon waking the program execution picks up
where it left off.  The APF8 has a 2 second heartbeat...it wakes up every 2
seconds, executes any pending tasks, and then goes back to sleep.  The
APF9's firmware runs once each time it wakes up.  Upon waking, the APF9
executes any pending tasks and then the processor is completely powered
down.  These differences are very fundamental to the behavior of each
controller.  

The schematics for the APF9 controller are available in the 'doc' directory
of this distribution.  Documentation of the firmware is provided in the
comment sections of the C source code in the 'src' directory.   Start with
'control.c'. 

-dds


Lineage of deployable firmware revisions.
-----------------------------------------
ApexSbeIridium-040405
ApexSbeIridium-062805
ApexSbeIridium-081705
ApexSbeIridium-100305
ApexSbeIridium-102505
ApexSbeIridium-032406
ApexSbeIridium-051906
Apf9iSbe41cp-012407  
Apf9iSbe41cp-050907
Apf9iSbe41cp-101207


List of Mission Parameters
--------------------------

APEX version 101207  sn 5315
User: f5315                                    
Pwd:  0xafb3
Pri:  ATDT59312                                  Mhp
Alt:  ATDT63256                                  Mha
INACTV ToD for down-time expiration. (Minutes)   Mtc
 00661 Down time. (Minutes)                      Mtd
 00600 Up time. (Minutes)                        Mtu
 00479 Ascent time-out. (Minutes)                Mta
 00240 Deep-profile descent time. (Minutes)      Mtj
 00240 Park descent time. (Minutes)              Mtk
 00360 Mission prelude. (Minutes)                Mtp
 00015 Telemetry retry interval. (Minutes)       Mhr
 00060 Host-connect time-out. (Seconds)          Mht
   985 Continuous profile activation. (Decibars) Mc
  1000 Park pressure. (Decibars)                 Mk
  2000 Deep-profile pressure. (Decibars)         Mj
   066 Park piston position. (Counts)            Mbp
   016 Deep-profile piston position. (Counts)    Mbj
   010 Ascent buoyancy nudge. (Counts)           Mbn
   022 Initial buoyancy nudge. (Counts)          Mbi
   001 Park-n-profile cycle length.              Mn
   124 Maximum air bladder pressure. (Counts)    Mfb
   110 OK vacuum threshold. (Counts)             Mfv
   227 Piston full extension. (Counts)           Mff
   016 Piston storage position. (Counts)         Mfs
     2 Logging verbosity. [0-5]                  D
  0002 DebugBits.                                D


Main Menu
---------

Menu selections are not case sensitive.
?  Print this help menu.
C  Calibrate: battery volts, current, & vacuum.
D  Set logging verbosity. [0-5]
E  Execute (activate) mission.
F  Float vitals agent.
Fb Maximum air-bladder pressure. [1-254] (counts)
Ff Piston full extension. [1-254] (counts)
Fn Display float serial number.
Fs Storage piston position. [1-254] (counts)
Fv OK vacuum threshold. [1-254] (counts)
I  Diagnostics agent.
I? Diagnostics menu.
K  Kill (deactivate) mission.
L  List mission parameters.
M  Mission programming agent.
M? Mission programming menu.
P  Display the pressure table.
Q  Exit command mode.
S  Sensor module agent.
Sc Display the SBE41 calibration coefficients.
Sm Measure power consumption by SBE41.
Sn Display SBE41 serial number.
Sp Get SBE41 pressure.
Ss Get SBE41 P T & S.
T  Get/Set RTC time. (format 'mm/dd/yyyy:hh:mm:ss')





Float Biography Agent
---------------------

Menu of float parameters.
?  Print this menu.
Fb Maximum air-bladder pressure. [1-254] (counts)
Ff Piston full extension. [1-254] (counts)
Fn Display float serial number.
Fs Storage piston position. [1-254] (counts)
Fv OK vacuum threshold. [1-254] (counts)



Diagnostics Agent
-----------------

Menu of diagnostics.
?  Print this menu.
a  Run air pump for 6 seconds.
b  Move piston to the piston storage position.
c  Close air valve.
d  Display piston position
e  Extend the piston 4 counts.
g  Goto a specified position. [1-254] (counts)
o  Open air valve.
r  Retract the piston 4 counts.
t  Argos PTT test.
1  Run air pump for 6 seconds (deprecated).
2  Argos PTT test (deprecated).
5  Retract the piston 4 counts (deprecated).
6  Extend the piston 4 counts (deprecated).
7  Display piston position (deprecated).
8  Open air valve (deprecated).
9  Close air valve (deprecated).


Mission Programming Agent
-------------------------

Menu selections are not case sensitive.
?  Print this menu.
A  Enter ARGOS ID number in HEX.
B  Buoyancy control agent.
Bi Ascent initiation buoyancy nudge. [25-254] (piston counts)
Bj Deep-profile piston position. [1-254] (counts)
Bn Ascent maintenance buoyancy nudge. [5-254] (piston counts)
Bp Park piston position [1-254] (counts)
L  List mission parameters.
N  Park and profile cycle length. [1-254]
J  Deep-profile pressure. (0-2000] (decibars)
K  Park pressure. (0-2000] (decibars)
Q  Quit the mission programming agent.
R  Repetition period for Argos transmissions [30-120] (sec).
T  Mission timing agent.
Ta Ascent time-out period. [2-10 hours] (Hours)
Td Down time (0-336 hours] (Hours).
Tj Deep-profile descent time. [0-6 hours] (Hours).
Tk Park descent time. (0-6 hours] (Hours).
Tp Mission prelude. (0-6 hours] (Hours).
Tu Up time (0-24 hours] (Hours).
Z  Analyze the current mission programming.



SBE41 Sensor Module Agent
-------------------------

Menu of SBE41 functions.
?  Print this menu.
Sc Display the SBE41 calibration coefficients.
Sm Measure power consumption by SBE41.
Sn Display SBE41 serial number.
Sp Get SBE41 pressure.
Ss Get SBE41 P T & S.

> i * 
Menu of manufacturer's diagnostics. (commands changed of added 
 by HM are %)

?  Print this menu.
a  Fill flash with test files.
c  Perform RTC ram test.
d  Load mission configuration with defaults.
e  EEPROM test (destructive to stored data).
f  Set float id. [0-9999]
l  List mission parameters.
m  Mission programming agent.
p  Specify password for remote host. (e.g., i*pQ001)%
r  Perform APF9 far RAM test.
s  Display sequence points and timing info.
t  Set the mission timer/alarm.
u  Set Qphone's acoustic sys parameters (e.g, i*uG2D999999P0400)%
    gain=2, # of max det/dive =999999, DSP on/off depth =0400m
v  Set default user name and password.
o  DSP power ON %
x  Send signal to stop DSP log/det program %
k  Kill DSP power in 15 sec %
1  1/2Hz air-solenoid test. (disconnect solenoid)
2  1/2Hz buoyancy-pump test. (disconnect pump)
3  Sweep of AD channels.
4  400Hz timer test on TP1. (5s duration)
5  Com1 serial port tests.
6  Com2 serial port tests.
7  Iridium serial port tests.
8  GPS serial port tests.
9  RF switch port tests.

> 
Iridium 2-way communication functions added or changed
User()- modified to allow gain change, number of detections per dive or 
        reactivate the mission after recovery mode was activated.
usage -
User(G2D020000P0300) - change the gain to 2, # of det to 20000 and DSP
				ON/OFF depth to 300m
User (E)       - reactivate the mission again

