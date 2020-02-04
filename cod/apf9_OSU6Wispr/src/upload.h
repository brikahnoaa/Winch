#ifndef UPLOAD_H
#define UPLOAD_H (0x0100U)
//*********Upload.c v.1.8 09/22/2014  Haru Matsumoto *********************
//Transmit multiple files in one connection.  Receive multiple commands at
//the end of first successful file upload.  Requires "done" reply to the land
//at the end each command.  After the first file, normal file uploads.
//*********Upload.c v.1.7b 03/01/2010 Haru Matsumoto**********************
//Transmit one file per one connection.  Commands are received at the end of
//successful upload.
//If the file size exceed MaxTXSize, it splits the file into multiple files.
//Files are named *.loA, *.loB, and *.loC in descending order with the first 
//part into *.loA, second part into *.loB & etc.  *.loA is the smallest and
//*.loC the largest. Each file is sent separately one by one from a smallest
//file first, largest file last.  File can be splitted up to five smaller files.
//3/01/10  
//
//Bug on the file_blknum inconsistency fixed.//2/28/10
//
//Original Xmodem scheme was removed and replaced by NOAA's Iridium protocol.
//Password and user name feature was replaced by gain, max event detection
//and DSP power ON/OFF depth.  QUEH is project name is hardwired in the program 
//now.  For secure communication, NOAA's protocol uses the several unique layers
//of data structures including platform ID, project ID with CRC, data block 
//layer. If Tx is successful, "done" signal is sent at the end of TX.  If not 
//successful, the bad block resend request is sent from the PMEL server at the 
//end of transmission. If there is a new command, at the end of successful files
//upload, a command file from the PMEL is sent.
//Each file is sent at separate phone connection.  Typically it requires minimum of
//2 phone connections when the float surfaces to send *.msg file and *.log file.
//Log file is larger, and it is larger than MaxTXSize (set to 20kyte now), the
//file is split into multiple files to ensure a successful transmission of file.
//
//It sends all the APEX files in the file system as well as an acoustic detection
//file *.dtg, if it is available.  For now *.dtg file creation is disabled. Numbers
//of event detections between the float status checks is sent.  During the descent
//Park mode, at every 30 minutes and during the profile at specified depths.
//3/02/2010
//
//Haru Matsumoto, NOAA, Newport, OR
//BLOCK_SIZE = up to 5000 bytes, typical 2000 
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * $Id: upload.c,v 1.7 2008/07/14 16:58:35 swift Exp $
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
 * Revised heavily by Logan Williams and Haru Matsumoto, 10/20/2009
 * Incoorporated NOAA's buoy Rudics com protocol
 *
 * \begin{verbatim}
 * $Log: upload.c,v $
 * Revision 1.7  2008/07/14 16:58:35  swift
 * Check for zero-length files and delete them.
 *
 * Revision 1.6  2007/05/08 18:10:39  swift
 * Added attribution just below the copyright in the main comment section.
 *
 * Revision 1.5  2006/10/11 20:52:05  swift
 * Modifications for compatibility with new flashio file system.
 *
 * Revision 1.4  2006/08/17 21:17:59  swift
 * Modifications to allow better logging control.
 *
 * Revision 1.3  2006/04/21 13:45:42  swift
 * Changed copyright attribute.
 *
 * Revision 1.2  2005/02/22 21:37:24  swift
 * Eliminate writes to the profile file during telemetry retries.
 *
 * Revision 1.1  2004/12/29 23:11:27  swift
 * Modified LogEntry() to use strings stored in the CODE segment.  This saves
 * lots of space in the DATA segment and significantly speeds code start-up.
 *
 * \end{verbatim}
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
#define uploadChangeLog "$RCSfile: upload.c,v $ $Revision: 1.7 $ $Date: 2008/07/14 16:58:35 $"

#include <serial.h>

/* function prototypes */
int UpLoad(const struct SerialPort *modem, unsigned int MAX_RECONNECTS);
//int UpLoadFile(const struct SerialPort *modem, const char *localpath, const long filesize, unsigned char resend_bitmap[], int last_file, int fid);
int UpLoadFile(const struct SerialPort *modem, const char *localpath, const long filesize, unsigned char resend_bitmap[], int fid);
int ChkResendReq(const struct SerialPort *modem, unsigned char resend_bitmap[], unsigned char resend_bytes[], unsigned char resend_crc[], int wait);
void RemoveFile(int min_fid, int max_fid, int LastTXstatus);
#endif /* UPLOAD_H */
