#ifndef SBE41CP_H
#define SBE41CP_H (0x0008U)

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * $Id: sbe41cp.c,v 1.29 2007/04/24 01:43:29 swift Exp $
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/* Copyright University of Washington.   Written by Dana Swift.
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
 */
/** RCS log of revisions to the C source code.
 *
 * \begin{verbatim}
 * $Log: sbe41cp.c,v $
 * Revision 1.29  2007/04/24 01:43:29  swift
 * Added acknowledgement and funding attribution.
 *
 * Revision 1.28  2007/01/24 17:14:47  swift
 * Implement new formulation of pump-time computations for the Sbe43f.
 *
 * Revision 1.27  2006/11/07 15:18:41  swift
 * Modifications to the way that SeaBird handles calculation of pump-periods
 * for the IDO.
 *
 * Revision 1.26  2006/10/13 18:55:55  swift
 * Modifications to the way that SeaBird handles calculation of pump-periods
 * for the IDO.
 *
 * Revision 1.25  2006/10/12 00:19:03  swift
 * Eliminated unused timeout variable.
 *
 * Revision 1.24  2006/10/11 21:05:12  swift
 * Implemented the ability to configure the SBE41CP to output PTS data during
 * CP mode.
 *
 * Revision 1.23  2006/08/17 21:17:56  swift
 * Modifications to allow better logging control.
 *
 * Revision 1.22  2006/05/15 18:51:03  swift
 * Changed Sbe41cpUploadCP() to accomodate change in resolution of encoded
 * values of bin-averaged CP data.
 *
 * Revision 1.21  2006/04/21 13:45:40  swift
 * Changed copyright attribute.
 *
 * Revision 1.20  2006/02/22 21:39:38  swift
 * Changed the descretization parameterization to eliminate empty deep bins.
 *
 * Revision 1.19  2006/02/03 00:23:27  swift
 * Fixed a buffer-overrun bug in use of pgets().
 *
 * Revision 1.18  2005/10/25 17:02:16  swift
 * Modifications to accomodate the SBE41CP-IDO.
 *
 * Revision 1.17  2005/10/12 20:10:04  swift
 * Refactored SBE41CP configuration to account for presence of IDO.
 *
 * Revision 1.16  2005/08/11 20:14:42  swift
 * Strengthened the pendantic regexs for S,T,P,O.
 *
 * Revision 1.15  2005/08/11 16:31:45  swift
 * Modifications to fix a timing-related bug when requesting a P-only
 * measurement while the ctd is in CP mode.
 *
 * Revision 1.14  2005/07/07 15:07:36  swift
 * Fixed a bug in chat() caused by 1-second time descretization.
 *
 * Revision 1.13  2005/06/14 19:05:59  swift
 * Minor modifications to logging verbosity.
 *
 * Revision 1.12  2005/04/30 22:52:18  swift
 * Implemented an API to the low-power PT sampling feature of the SBE41CP.
 * Added function to query the SBE41CP for its firmware revision.
 *
 * Revision 1.11  2005/01/06 00:46:22  swift
 * Modification to reflect changes to LogEntry() and improved robustness.
 *
 * Revision 1.10  2004/03/20 01:30:37  swift
 * Modifications made to reflect the changes requested for the SBE41CP firmware.
 *
 * Revision 1.9  2004/03/17 01:13:21  swift
 * Tighter implementation of the master-slave model.
 *
 * Revision 1.8  2003/12/20 19:11:26  swift
 * Fixed a bug that ignored negative signs in STP measurements and caused the
 * parse results to be always non-negative.  Elevated Sbe41cpStatus() to be a
 * nonstatitically linked support function.  Added Sbe43fPumpTime() function.
 *
 * Revision 1.7  2003/12/19 23:03:14  swift
 * Fairly major rework of several functions.  The implementation of
 * Sbe41cpStatus() was changed from using regex's to using string matching
 * techniques to match parameter tokens.  Replaced "\r\n" pairs with "\r" to
 * fix parsing problems.
 *
 * Revision 1.6  2003/12/18 22:53:05  swift
 * Unstable major revision prior to reimplementing Sbe41cpStatus().
 *
 * Revision 1.5  2003/10/21 17:59:43  swift
 * Changed TimeOut period of Sbe41cpGetPtso() from 30 seconds to 60 seconds to
 * accomodate SeaBird's change in protocol for the SBE43F oxygen sensor.
 *
 * Revision 1.4  2003/07/16 23:52:23  swift
 * Implement the master/slave model in a more robust way especially when in
 * communications mode.  Also, IEEE definitions of NaN were exploited in order
 * to write and use functions that set and detect NaN and Inf.
 *
 * Revision 1.3  2003/07/05 21:11:13  swift
 * Eliminated 'Sbe41cpLogCal()' because it would require too large of a FIFO
 * buffer for the serial port.  The console serial port of the APF9 is limited
 * to 4800 baud while the CTD serial port operates at 9600 baud.  Since no
 * hardware or software handshaking is implemented then the FIFO has to buffer
 * the differing rates.
 *
 * In 'Sbe41cpInitiateSample()', some of the code was refactored away from being
 * hardware independent toward being hardware dependent.  This was a concession
 * to reliability and robustness that was caused by the slow processing speed
 * of the APF9.  Its prior factorization suffered from too many function calls
 * to low-level hardware dependent primitives.  These calls threw off timing
 * enough to reduce the reliability communications and sequencing of SBE41CP
 * control.
 *
 * Revision 1.2  2003/07/03 22:47:46  swift
 * Major revisions.  This revision is not yet stable.
 *
 * Revision 1.1  2003/06/29 01:36:06  swift
 * Initial revision
 * \end{verbatim}
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
#define sbe41ChangeLog "$RCSfile: sbe41cp.c,v $ $Revision: 1.29 $ $Date: 2007/04/24 01:43:29 $"

#include <serial.h>
#include <stdio.h>

/* function prototypes */
int    Sbe41cpBinAverage(void);
int    Sbe41cpConfig(float PCutOff);
int    Sbe41cpEnablePts(void);
int    Sbe41cpEnterCmdMode(void);
int    Sbe41cpExitCmdMode(void);
int    Sbe41cpFwRev(char *buf,unsigned int bufsize);
int    Sbe41cpGetP(float *p);
int    Sbe41cpGetPt(float *p, float *t);
int    Sbe41cpGetPts(float *p, float *t, float *s);
int    Sbe41cpGetPtso(float *p, float *t, float *s, float *o);
int    Sbe41cpLogCal(void);
int    Sbe41cpSerialNumber(void);
int    Sbe41cpStartCP(time_t FlushSec);
int    Sbe41cpStatus(float *pcutoff, unsigned *serno,
                     unsigned *nsample, unsigned *nbin, time_t *tswait, 
                     float *topint, float *topsize, float *topmax,
                     float *midint, float *midsize, float *midmax,
                     float *botint, float *botsize);
int    Sbe41cpStopCP(void);
int    Sbe41cpTsWait(time_t sec);
int    Sbe41cpUploadCP(FILE *dest);
time_t Sbe43fPumpTime(float p, float t, float Tau1P);
float  Sbe43Tau(void); 

/* define the return states of the SBE41CP API */
extern const char Sbe41cpTooFew;          /* Too few CP samples */
extern const char Sbe41cpChatFail;        /* Failed chat attempt. */
extern const char Sbe41cpNoResponse;      /* No response received from SBE41CP. */
extern const char Sbe41cpRegExceptn;      /* Response received, regexec() exception */
extern const char Sbe41cpRegexFail;       /* response received, regex no-match */
extern const char Sbe41cpNullArg;         /* Null function argument. */
extern const char Sbe41cpFail;            /* General failure */
extern const char Sbe41cpOk;              /* response received, regex match */
extern const char Sbe41cpPedanticFail;    /* response received, pedantic regex no-match */
extern const char Sbe41cpPedanticExceptn; /* response received, pedantic regex exception */

#endif /* SBE41CP_H */
