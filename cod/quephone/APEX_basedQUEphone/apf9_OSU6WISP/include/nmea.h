#ifndef NMEA_H
#define NMEA_H

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * $Id: nmea.c,v 2.10 2007/04/24 01:43:29 swift Exp $
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
 * $Log: nmea.c,v $
 * Revision 2.10  2007/04/24 01:43:29  swift
 * Added acknowledgement and funding attribution.
 *
 * Revision 2.9  2006/03/15 15:58:45  swift
 * Implemented prescreening of NMEA messages in GetGpsFix() to speed up
 * processing and decrease time to get fix.
 *
 * Revision 2.8  2006/01/06 23:11:46  swift
 * Significant improvements made to the function that synchronizes the APF clock
 * to GPS time.
 *
 * Revision 2.7  2005/05/13 15:26:47  swift
 * Modified acceptance criteria to allow for a '2' in field 6 of the GGA nmea
 * string (this value is undocumented in the garmin manual).
 *
 * Revision 2.6  2005/04/30 15:09:22  swift
 * Fixed a bug in the mechanism to prevent overflow of the GPS fifo - reinitialize
 * the reference time for periodic IO flushes.
 *
 * Revision 2.5  2005/04/29 20:45:41  swift
 * Implement mechanism to prevent clogging or overflow of GPS fifo (periodic flushes).
 *
 * Revision 2.4  2005/02/22 21:51:28  swift
 * Modified GetGpsFix() to return the time needed to obtain the fix.
 *
 * Revision 2.3  2005/01/15 00:48:53  swift
 * Modified NMEA API for use with XA compiler for the APF9.
 *
 * Revision 2.2  2002/10/08 23:51:52  swift
 * Combined header file into source file.  Added hack to work around a firmware bug
 * in the Garmin GPS25 where latency exists between the number of satellites used and
 * the fix determination.
 *
 * Revision 2.1  2002/07/31 14:27:47  swift
 * Revision for use with Garmin GPS25 model gps engine.
 *
 * Revision 1.2  2002/07/30 18:27:10  swift
 * Modifications to use POSIX regex library rather than the GNU regex library.
 *
 * Revision 1.1  2002/05/07 22:19:55  swift
 * Initial revision
 * \end{verbatim}
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
#define NmeaChangeLog "$RCSfile: nmea.c,v $  $Revision: 2.10 $   $Date: 2007/04/24 01:43:29 $"

#include <serial.h> 

/*========================================================================*/
/* structure to contain data from the NMEA 0183 messages                  */
/*========================================================================*/
/**
   This structure is a repository in which to store data from NMEA 0183
   messages received from a GPS receiver.  The following description of the
   NMEA format was taken from Appendix E of the Trimble Lassen LP GPS System
   Designer Reference Manual (Part No 39264-00, Firmware 7.82, August 1999).

   \begin{verbatim}
   E.2 NMEA 0183 Message Format 
      
      The NMEA 0183 protocol covers a broad array of navigation data. This 
      broad array of information is separated into discrete messages which 
      convey a specific set of information.  The entire protocol encompasses 
      over 50 messages, but only a sub-set of these messages apply to a GPS 
      receiver like the Lassen LP GPS. The NMEA message structure is 
      described below. 
      
          \$IDMSG,D1,D2,D3,D4,.......,Dn*CS[CR][LF]  
      
          "\$"    The "\$" signifies the start of a message. 
      
           ID    The talker identification is a two letter mnemonic which 
                 describes the source of the navigation information. The GP 
                 identification signifies a GPS source. 
      
          MSG    The message identification is a three letter mnemonic which
                 describes the message content and the number and order of
                 the data fields.
      
          ","    Commas serve as delimiters for the data fields. 
      
           Dn    Each message contains multiple data fields (Dn) which are 
                 delimited by commas. 
      
           "*"   The asterisk serves as a checksum delimiter. 
      
            CS   The checksum field contains two ASCII characters which
                 indicate the hexadecimal value of the checksum.
      
      [CR][LF]   The carriage return [CR] and line feed [LF] combination 
                 terminate the message. 
      
      NMEA 0183 messages vary in length, but each message is limited to 79 
      characters or less.  This length limitation excludes the "\$" and the 
      [CR][LF]. The data field block, including delimiters, is limited to 74
      characters or less. 
   \end{verbatim}

   Written by Dana Swift
*/
struct NmeaGpsFields
{
   /** Latitude [-90, 90]. */
   float lat;
      
   /** Longitude (-180, 180]. */
   float lon;      

   /** UTC time of day [0, 24) (hours). */
   float hour;
      
   /** UTC day (DDMMYY) of the GPS fix. */
   long ddmmyy;   
      
   /** UTC time (HHMMSS) of the GPS fix. */
   long hhmmss;   

   /** GPS status: 'A'=ValidPosition, 'V'=NavReceiverWarning */   
   char status;

   /** GPS quality indicator: 0=NoGps, 1=Gps, 2=DGps. */   
   int quality;
      
   /** Number of satellites in use. */   
   int nsat;
      
   /** day of month of GPS fix [1, 31]. */
   int day;

   /** month of year of GPS fix [1, 12]. */   
   int mon;

   /** year of GPS fix */   
   int year;
};

/* declare an initialization NmeaGpsFields object */
extern const struct NmeaGpsFields zeros;

/* define the maximum allowed string length for a NMEA string */
#define MaxNmeaStrLen 82
 
/* function prototypes for public functions to read NMEA 0183 messages and data */
time_t        GetGpsFix(const struct SerialPort *port, struct NmeaGpsFields *gpsfix, time_t sec);
time_t        GetGpsTime(const struct SerialPort *port,  time_t sec);
void          init_nmea_struct(struct NmeaGpsFields *gpsfix);
unsigned char nmea_checksum(const char *msg,int start,int end);
int           nmea0183(const char *msg,struct NmeaGpsFields *gpsfix);

#endif /* NMEA_H */
