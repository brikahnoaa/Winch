#ifndef CMDS_H
#define CMDS_H
//QUEphone version
//Changed i*u (User) command to take a gain (0-3), max detections per dive (1-999999)
//and DSP power on/off depth (0-9999). Pwd is now QUEphone ID, e.g., Q001. HM. OSU
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * $Id: cmds.c,v 1.46.2.1 2008/09/11 19:55:27 dbliudnikas Exp $
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Copyright University of Washington.   Written by Dana Swift.
 *
 * This software was developed at the University of Washington using funds
 * generously provided by the US Office of Naval Research, the National
 * Science Foundation, and NOAA.
 *  
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 2.1 of the License, or (at
 * your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser
 * General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
 */
/** RCS log of revisions to the C source code.
 *
 * \begin{verbatim}
 * $Log: cmds.c,v $
 * Revision 1.46.2.1  2008/09/11 19:55:27  dbliudnikas
 * Replace SBE41 with Seascan TD: Seascan agent, gateway, remove not applicable PressureCP
 *
 * User name i*u command is now to control gain, number of detection per profile to make 
 * float to ascent and the detection threshold to DSP board. 
 * A one-digit number followed by G sets the gain. For example i*uG2D001000P0080 set 
 * gain=2, max number of detection to make float to come up(Max_Detects=1000), and 
 * DSP power ON/OFF depth (DspPwrDpt=100 default). HM Feb 11, 2010.

 * Revision 1.46  2008/07/14 16:51:24  swift
 * Implement hyper-retraction mechanism for N2 compensator.  Add
 * command to query LBT for signal strength.  Add Sbe41cp gateway mode.
 *
 * Revision 1.45  2007/10/05 22:30:21  swift
 * Change limits of the telemetry retry interval and down time.
 *
 * Revision 1.44  2007/06/08 02:15:26  swift
 * Added a few comments.
 *
 * Revision 1.43  2007/05/09 19:13:42  swift
 * Added more commands related to the TimeOfDay feature.
 *
 * Revision 1.42  2007/05/08 18:10:16  swift
 * Added TimeOfDay feature to implement ability to schedule profiles to start
 * at a specified time of day.  Added attribution just below the copyright in
 * the main comment section.
 *
 * Revision 1.41  2007/05/07 21:06:15  swift
 * Added commands to set the maximum engineering log size and execute SelfTest().
 *
 * Revision 1.40  2007/03/26 20:52:06  swift
 * Implemented a command to retrieve the ICCID/MSISDN numbers from the SIM card.
 *
 * Revision 1.39  2007/01/24 17:15:51  swift
 * Fixed a minor misuse of fnameok().
 *
 * Revision 1.38  2007/01/22 18:09:02  swift
 * Fixed a minor typo in a format spec.
 *
 * Revision 1.37  2007/01/15 20:51:10  swift
 * Added a function to fill the flash with test files.
 *
 * Revision 1.36  2006/12/20 00:03:54  swift
 * Fixed a syntax error in the command that manually creates a log file.
 *
 * Revision 1.35  2006/12/19 23:42:22  swift
 * Add commands to manually create and close the logfile.
 *
 * Revision 1.34  2006/11/25 00:05:35  swift
 * Fixed a bug in the directory listing of flashio filesystem.
 *
 * Revision 1.33  2006/11/22 03:58:00  swift
 * Add a sanity check for the CP activation pressure to warn against spot
 * sampling in the main thermocline or above.
 *
 * Revision 1.32  2006/11/18 17:43:16  swift
 * Removed orphaned menu selection.
 *
 * Revision 1.31  2006/10/13 18:56:42  swift
 * Added a comment.
 *
 * Revision 1.30  2006/10/11 20:58:15  swift
 * Integrated the new flashio files system.
 *
 * Revision 1.29  2006/08/17 21:17:59  swift
 * Modifications to allow better logging control.
 *
 * Revision 1.28  2006/05/19 19:28:05  swift
 * Modifications to allow the bearer service type to be included in the AT dial string.
 *
 * Revision 1.27  2006/05/15 18:52:44  swift
 * Expanded valid range of park descent and profile descent periods from
 * 6 hours to 8 hours.
 *
 * Revision 1.26  2006/04/21 13:45:42  swift
 * Changed copyright attribute.
 *
 * Revision 1.25  2006/02/22 21:54:28  swift
 * Added biographical queries to the modem agent.
 *
 * Revision 1.24  2006/01/06 23:24:58  swift
 * Fixed an inconsistency in the mission nazi that wouldn't allow for longer
 * than 6-hour descents.
 *
 * Revision 1.23  2005/10/12 20:11:46  swift
 * Modifications to account for changes to SBE41CP API.
 *
 * Revision 1.22  2005/10/11 20:37:44  swift
 * Implemented pressure-activation of iridium floats.
 *
 * Revision 1.21  2005/09/02 22:21:06  swift
 * Added ApfId to the command line leader.
 *
 * Revision 1.20  2005/08/06 17:18:54  swift
 * Allow for telemetry-retry intervals in the range 5 minutes to 6 hours.
 *
 * Revision 1.19  2005/07/12 18:17:50  swift
 * Changed units of time in menus from hours to minutes.
 *
 * Revision 1.18  2005/06/23 20:35:33  swift
 * Added a sanity check for the maximum air-bladder check.
 *
 * Revision 1.17  2005/06/14 19:00:24  swift
 * Added an LBT agent to manually control and exercise the modem.
 *
 * Revision 1.16  2005/05/01 14:27:38  swift
 * Added menu options for displaying SBE41CP firmware and for initiating a
 * low-power TP sample.
 *
 * Revision 1.15  2005/04/04 22:35:33  swift
 * Change maximum telemetry-retry interval to 6 hours.
 *
 * Revision 1.14  2005/03/30 20:25:00  swift
 * Changed maximum telemetry-retry period.
 *
 * Revision 1.13  2005/02/22 20:59:27  swift
 * Added constraints to the mission nazi.  Added commands to control the
 * Garmin GPS15L.
 *
 * Revision 1.12  2005/01/06 00:52:52  swift
 * Added testing functionality in ManufacturerDiagnosticsAgent() for
 * board-level testing.
 *
 * Revision 1.11  2004/12/30 17:10:08  swift
 * Added tests of hardware control lines on the GPS port and RF switch port.
 *
 * Revision 1.10  2004/12/29 23:11:26  swift
 * Modified LogEntry() to use strings stored in the CODE segment.  This saves
 * lots of space in the DATA segment and significantly speeds code start-up.
 *
 * Revision 1.9  2004/04/23 23:45:42  swift
 * Added a function call to open the air valve when the mission is killed.
 *
 * Revision 1.8  2004/04/14 16:40:15  swift
 * Eliminated most of the sanity checks and added feature to print the
 * pressure table.
 *
 * Revision 1.7  2004/03/24 00:15:01  swift
 * Added provision for debuglevel to be a persistent variable.
 *
 * Revision 1.6  2004/02/05 23:50:40  swift
 * Added include directive for the Xicor eeprom API.
 *
 * Revision 1.5  2003/12/20 19:49:02  swift
 * Added a command to display the SBE41 calibration coefficients.
 *
 * Revision 1.4  2003/11/20 18:59:47  swift
 * Added GNU Lesser General Public License to source file.
 *
 * Revision 1.3  2003/11/12 22:40:17  swift
 * Increase time-out periods in response to user input.  Implemented special
 * case where the PnP cycle length of 254 disables the PnP feature.
 *
 * Revision 1.2  2003/09/11 17:10:18  swift
 * Modifications to the mission analyzer.  Added many features to the user
 * interface to allow float control and exercise.
 *
 * Revision 1.1  2003/08/22 14:54:17  swift
 * Initial revision
 * \end{verbatim}
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
#define cmdsChangeLog "$RCSfile: cmds.c,v $ $Revision: 1.46.2.1 $ $Date: 2008/09/11 19:55:27 $"

/* function prototypes for exported functions */
void CmdMode(void);
int  CmdModeRequest(void);

#endif /* CMDS_H */
