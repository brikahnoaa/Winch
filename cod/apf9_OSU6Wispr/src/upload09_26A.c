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

#include <math.h>
#include <logger.h>
#include <unistd.h>
#include <tx.h>
#include <chat.h>
#include <flashio.h>
#include <string.h>
#include <clogin.h>
#include <config.h>
#include <profile.h>
#include <expect.h>
#include <login.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>
#include <stdlib.h>
#include <conio.h>
#include <apf9.h>
#include <apf9icom.h>
#include <lbt9522.h>
#include <control.h>
#include <descent.h> //HM added to share accum_dtx
#include <modem.h>
#include "quecom.h" //HM

#ifdef _XA_
   #include <apf9.h>
   #include <apf9icom.h>
#else
   #define WatchDog()
   #define StackOk() 1
#endif /* _XA_ */

/* define the command to download via xmodem */
int 	  last_blk; 				//last block of the current file	
int		  persistent LastTXstatus;
int		  total_files_uploaded;
int 	  num_blocks;
int 	  sector;
int 	  block_num    = 1;
int 	  num_files    = 0;
const int REC_BUF_SIZE = 2000; 	  //HM 
const int MAX_UPLOAD_RETRIES  = 3;//HM
int 	  blk_end	   = 62;//largest blocks can be sent
const int BLOCK_SIZE   = 2000;    //up to 5000 NOAA, HM
long 	  MaxTXSize    = 20000L;  //File larger than this will be split into halves
								  //rename with the last char with A and B.
								  //Recommended value = MaxLogSize/2

/*------------------------------------------------------------------------*/
/* function to upload the contents of the file system                     */
/*------------------------------------------------------------------------*/
/**
   This function is designed to upload the contents of the file system
   (except the mission configuration file) to the remote host.  
*/
int UpLoad(const struct SerialPort *modem, unsigned int MAX_RECONNECTS)
{
   FILE *source=NULL;  	//commented out by HM Moved to the front as global     

   /* function name for log entries */
   static cc FuncName[] = "UpLoad()";

   /* initialize the function's return value */
   int 	fid;
   int	fileNo;
   int	first_file = 1;  //Define first file 

   /* create a local buffer to hold file system entries */
   char fname[FILENAME_MAX+1];
   char fname2[22];
   
   // char arrays for holding the resend reqeust data.  NOAA HM
   unsigned char resend_crc[2];
   unsigned char resend_bitmap[8];
   unsigned char resend_bytes[9];
   
   int flag = 0;
   int GetCmdretries = 0;
   int grace = 40;//grace period before giving up to check the reply from land
   
   time_t To,T;
 
   // char buffer for reading bytes from the modem
   unsigned char byte;
     
   // Initialize the resend bitmap
   resend_bitmap[0] = 0xff;
   resend_bitmap[1] = 0xff;
   resend_bitmap[2] = 0xff;
   resend_bitmap[3] = 0xff;   
   resend_bitmap[4] = 0xff;
   resend_bitmap[5] = 0xff;
   resend_bitmap[6] = 0xff;
   resend_bitmap[7] = 0xff;
   
   /* pet the watch dog */
   WatchDog(); 

   /* validate the port */
   if (!modem)
   {
      /* create the message */
      static cc msg[]="NULL serial port.\n";

      /* make the logentry */
      LogEntry(FuncName,msg);
   }
   
   /* A loop to log on to the remote host and upload files */
   else {
		//Prepare the log entries
		int upload_retry   = 0;
		int connect_retry  = 0;
		int max_fid;
		int min_fid;
		//int last_file;
		//int file_count;
		long max_file_size;
		long file_len;
		int start ;
		int bj, resend_blks;
		unsigned char temp3;
		long total_file_size;
		int splitTX ;
		long minFilesize;
		int minFilesizefid;
		int firstTX =1; //Rest 1-st TX status = 1
		//unsigned int filesize;
		unsigned char *buf;
		FILE *fp=NULL;
		int jj;
		int namelen;
		long accum;
		int fileleng[5], minfile;
		int  numfiles;
		char atrb='A';

		total_files_uploaded=0; //Reset the total file count	
		
		//All good transmissions come back here.  Transmit one file per loop.  	
		//This loop process eliminates the file transmitted after a good TX.
		//If the file size exceeds MaxTXSize, split into multiple files for reliable TX.
		
		GoodBeg: 
		
		WatchDog();
		max_fid   		= 0;
		splitTX   		= 0;  //if = 1, stay connected and re-log to send a new file
		num_files 		= 0;  //HM
		max_file_size	= 0L;
		minFilesize		= 62000L;//to find out which file is min
		last_blk  		= blk_end;
		// Initialize the resend bitmap
		for(jj=0;jj<8;jj++)resend_bitmap[jj]=0xff;
		
		//LogAdd("Last TX status = %d\n", LastTXstatus);//HM debug
		
		start			= 1;
		total_file_size	= 0L;
 		num_blocks 		= 0;  //HM
		
		//Check the file system
		//Determine the number of files and the number of blocks needed to transmit for one file
		//Find out the smallest file.  Files are sent smallest one first, largest last.
		for (fid=0;fid < MAXFILES; fid++) {
			if (fioName(fid,fname,FILENAME_MAX)>0) {
				if (strcmp(fname,config_path) != 0) {
					if (strcmp(fname, log_path) != 0) {
						file_len=fioLen(fid);
						if ((file_len != 0L)) {
							num_blocks += (int) ((fioLen(fid)-1L)/BLOCK_SIZE) + 1;
							num_files++;
							max_fid=fid;	 //find out the max fid number to save time in the next processes
							fileNo=max_fid;
							total_file_size += file_len;//total files size
							if(file_len<minFilesize){//find out min file fid
								minFilesizefid=fid; // this is the file to be sent
								minFilesize=file_len;
								}
							if(start==1) {//find out lowest fid num
								min_fid=fid;
								start=0;
								}
							if(file_len>max_file_size){//find out max file size
								max_file_size=file_len;
								}
						}//Erasing the 0-length file here HM
						else if (!fioLen(fid)){ /* check for zero-length files and delete them */
							/* create a log message */
							static cc format1[]="Refuse to trnsfr 0-len file. Erase : %s\n";
							LogEntry(FuncName,format1,fname);
							fioUnlink(fid); continue;
						}
					}
				}
			}
		}
		
		//debug 
		if(max_fid>=0){
		static cc format[]="Debug1 Total no of files to send %i\n";
		LogEntry(FuncName,format,num_files);
		//Debug 2 lines
		LogEntry(FuncName,format,max_fid);
		LogEntry(FuncName,format,fileNo);
		}
		
		//Splitting the file too large to transmit by one TX.  Erase the orignal.
		if(minFilesize > (long) MaxTXSize){
			fioName(minFilesizefid,fname,FILENAME_MAX);
			if((source=fopen(fname,"r"))!=0){
			//LogAdd("string len %d\n",strlen(fname));
				namelen=strlen(fname);
				fname[namelen]=0x00;
				WatchDog(); 
				numfiles=(int)((minFilesize-1L)/MaxTXSize+1L);
				for (jj=0;jj<namelen;jj++)fname2[jj]=fname[jj];
				fname2[namelen]=0x00;
				LogAdd("Splitting %s %d files\n",fname2, numfiles); //debug
				fileleng[numfiles-1]=(int)MaxTXSize;//last file size	
				accum=MaxTXSize;
				minfile=(int)MaxTXSize;
				for(jj=numfiles-2; jj>=1; jj--){
					fileleng[jj]=(int)MaxTXSize-(numfiles-jj-1);
					if(fileleng[jj]<minfile)minfile=fileleng[jj];
					accum+=(long)fileleng[jj];
					}
					fileleng[0]=minFilesize-accum;
					if(fileleng[0]>=minfile)fileleng[0]=minfile-1;
			
				for(jj=0;jj<numfiles;jj++){
					buf = (unsigned char *) malloc(sizeof(unsigned char)*fileleng[jj]);
					fread(buf,sizeof(unsigned char), fileleng[jj], source);
				   // namelen=strlen(fname);
					fname2[namelen-1]=atrb+jj;//change the name to ******.loA
					LogAdd(" %s %d\n",fname2, fileleng[jj]);//debug
					fp=fopen(fname2,"w");
					fwrite(buf, sizeof(unsigned char), fileleng[jj], fp);
					free(buf);
					fclose(fp);
				}
			
				fclose(source);
				LogAdd("Remove original %s\n",fname);
				remove(fname);
				
			} else {
				/* make a log entry that the file can't be opened for reading */

				/* create the message */
				static cc format2[]="Unable to open \"%s\" for reading.\n";

				/* make the logentry */
				LogEntry(FuncName,format2,fname);
			}
			goto GoodBeg;
		}
		
		//still more file to send
		if(num_files !=0){ 
			static cc format0[] = "%ldB %i files %i blks of max %iB Fid %d-%d\n";
			fid=minFilesizefid;
			file_len=minFilesize;
			//if(firstTX)LogAdd("MaxTXSize=%ld\n",MaxTXSize);	//HM debug	
			LogEntry(FuncName, format0, total_file_size, num_files, num_blocks, BLOCK_SIZE, min_fid, max_fid);
			//determine if sending files in multiple Txs
			fioName(fid,fname,FILENAME_MAX);
			//if(firstTX)LogAdd("Min file %s fid=%d num_blocks=%d\n",fname, minFilesizefid, num_blocks);//HM debug
			num_blocks = (int) ((file_len-1L)/BLOCK_SIZE) + 1;//recalculate the blk number
			if(num_files > 1){
				//if(firstTX)LogAdd("Multple TX sessions\n"); //HM debug
				//MAX_RECONNECTS++;//increase the max connection retry by one
				splitTX=1;
				}

			pflushio(modem);
			
			//Login, 
			//Check if send success or any new command
			do 
			{
			Rtry://previous TX was bad. Try again with the same file.
				block_num = 1;		 
				//last_file = 1;
				/* pet the watch dog */
				WatchDog(); 

				/* Connect, upload & check if the current fid contains a file */
				if (fioName(fid,fname,FILENAME_MAX)>0)
				{
					/* don't upload the mission configuration file */
					if (!strcmp(fname,config_path)) continue;
				   
					/* don't upload the active log file */
					if (!strcmp(fname,log_path)) continue;

					/* don't upload the active DTX file */
					//if (!strcmp(fname,DTX_path)) continue;//HM
					   
					/* make sure a login session exists before the xfer */
					if (CLogin(modem)>0)
					{							
						/* upload one file at a time to the remote host */
						
						//if(UpLoadFile(modem, fname, fioLen(fid), resend_bitmap, last_file, fid));
						if(UpLoadFile(modem, fname, fioLen(fid), resend_bitmap, fid));
						
						/* don't allow anything more to be written into the profile pathname */
						if (!strcmp(fname,prf_path)) prf_path[0]=0;
						
						if (modem->cd && !modem->cd()){//log session lost HM
							if(connect_retry <= MAX_RECONNECTS){
								LogAdd("Connection lost. Call again\n");
								pputs(modem, "+++", 20, "\n"); Wait(500);
								pputs(modem, "ATH0", 10, "\n"); Wait(100);//Hung up statement added by HM
								//hangup(modem);
								Wait(30000);//wait out long enough
								WatchDog();
								//Wait(30000);//wait out long enough
								WatchDog();
								last_blk  = blk_end;
								connect_retry++;
								goto Rtry; //try again
							}else {
								LogAdd("Call limit exceeded\n");
								LastTXstatus=0;
								goto Err;//call limit exceeded
							}
						}	
					} else{ //could not connect
						connect_retry++;
						LogAdd("Connect attmpt failed. Try again\n");
						pputs(modem, "+++", 20, "\n"); Wait(500);
						pputs(modem, "ATH0", 10, "\n"); Wait(100);//Hung up statement added by HM
						
						hangup(modem);//this may not need HM
						ModemDisable();
						Wait(30000);//wait long enough 
						WatchDog(); 
						Wait(30000);
						WatchDog();
						//Rev start here HM added to turn off modem
						ModemEnable(19200);
						if (IrModemRegister(modem)>0){
							
							//These 6 lines are original keep them here HM
							if (connect_retry <= MAX_RECONNECTS) {
								goto Rtry;
							}else {
								LogAdd("Call limit exceeded\n");					   
								goto Err; // calls limit exceeded
							}//Original ends here
							
						}//rev continues HM
						else
						{
							 static cc msg[]="Iridium modem registration failed.\n";
							 LogEntry(FuncName,msg); 
							 //These 5 lines were added Sept2014 HM
							 ModemDisable();
							 hangup(modem);
							 Wait(10000);
							 goto Err;
							 //
						}//Rev ends here HM
					}
				}
				
				//ResendReq:
				// A file uploaded. Now check if resend request from land
				resend_blks=0;		
				flag=ChkResendReq(modem, resend_bitmap, resend_bytes, resend_crc, grace);
				
				if (flag == 1) {//Good job! Good block TX
					//debug
					static cc format[]="Debug flag1 Total # of files to send %i\n";
					//static cc format2[]= "Upload OK. Remove file %s\n";
					LastTXstatus=1;
					firstTX=0;	//not 1-st TX file any more
					total_files_uploaded++;
					num_files--;
					//debug
					LogEntry(FuncName,format,num_files);
					//debug end
					RemoveFile(fid, fid, LastTXstatus); //remove one file
					first_file = 0; //Now first file is over
					//added three lines 
					//pputs(modem, "done", 20,""); //HM Sept2014 //All files sent. Check if any commands to read.				

					//Check if still more files to send
					//if(splitTX==1 || num_files >= 1)goto GoodBeg;
				}
				/* check the termination conditions */						
				else if (flag == 0) {    //timeout
					//header reading error or missed resend request
					static cc message[] = "\nTimeout occurred. Hung up and reconnect.\n";
					LogEntry(FuncName, message);
					upload_retry++;
					LastTXstatus=0;
					WatchDog();
					Wait(100);
					pputs(modem, "+++", 20, "\n"); Wait(500);
					pputs(modem, "ATH0", 10, "\n");Wait(100);//Hung up statement added by HM
					hangup(modem);//this may not need HM
   										
					if (upload_retry <= MAX_UPLOAD_RETRIES) { //repeat upload
						last_blk  = blk_end;					
						Wait(30000);//wait long enough 
						WatchDog(); 
						Wait(30000);
						WatchDog();
						//Try log again
						goto Rtry;
						}
						//Max try exceeded and file length is too long. Sorry but have to erase it.
					else if(file_len > MaxTXSize/2) {
						RemoveFile(fid, fid, LastTXstatus);
						goto Err;
						}
					else goto Err;//MAX_UPLOAD exceeded
					
				//Resend request
				}else if (flag == 2) { 
					static cc message1[] = "Resending %d blks, %d/%d of try\n";
					static cc message2[] = "Invalid resend rqst. Resend all blks.\n";
					int crctemp = Calc_Crc(resend_bytes, 9);

					upload_retry++;
					WatchDog();
					
					//find out the last bad block number bj HM
					for (bj=1;bj<=blk_end;bj++){
						temp3 = (char) ((int) (pow((double) 2, (double) ((bj-1) % 8))));
						if ((temp3 & resend_bitmap[(bj-1)/8]) == temp3) {
							last_blk = bj; // Check if the block is set in the bitmap
							resend_blks++;
						}
					}

					if (upload_retry <= MAX_UPLOAD_RETRIES) {		   
						if ((((unsigned char) ((crctemp >> 8) & 0x00FF)) == resend_crc[0]) && (((unsigned char) (crctemp & 0x00FF)) == resend_crc[1])) {
							//valid CRC	
							LogEntry(FuncName, message1, resend_blks, upload_retry, MAX_UPLOAD_RETRIES);
						} else { //invalid CRC
							LogEntry(FuncName, message2);				  
							for(jj=0;jj<8;jj++)resend_bitmap[jj]=0xff;
						}
					}else if (file_len > MaxTXSize/2) {//debug HM files are too big, don't send & erase
						LastTXstatus=0;
						LogAdd("Files are too big. Erase and hung up. %d/%d\n",upload_retry,MAX_UPLOAD_RETRIES);//debug HM
						RemoveFile(fid, fid, LastTXstatus);
						pputs(modem, "+++", 20, "\n"); Wait(500);
						pputs(modem, "ATH0", 10, "\n");Wait(100);//Hung up statement added by HM
						//hangup(modem);
						goto Err;
					}		
				// Prepare to download msg file from land
				} else if (flag ==3) { 
					// buffer to hold all received data
					unsigned char *rbuf = (unsigned char *) malloc(sizeof(unsigned char) * REC_BUF_SIZE);
					
					// keeps track of how many bytes have been recieved
					int leader = 0;
					
					// buffer to hold crc value
					unsigned char rcrc[2];
					// expected length of block
					int rilength = REC_BUF_SIZE;
					
					// calculated crc of block
					int ricrc;
						
					// the block number
					int rblknum;
						
					// the number of blocks
					int rnumblks;
						
					// iterator val
					int x = 0;
						
					// file pointer for the config path
					FILE * config;
					static cc msg[] = "Msg from land.\n";
					
					//came here after a successful file TX
					total_files_uploaded++;  //increment file numbers uploaded 
					num_files--;
					RemoveFile(fid, fid, 1); //remove one file	Good removal.			
					LogEntry(FuncName, msg);
					firstTX=0;			     //not 1-st TX anymore
					
					To=time(NULL);
					T=To;
					WatchDog();
					
					// Read the command message (command) from land
					// WARNING: fails for messages with >1 block
					do {
						if (modem->getb(&byte)>0) {
							//To = time(NULL);//HM
							/* write the current byte to the log stream */
							if (debuglevel>=2 || (debugbits&EXPECT_H)) {  //debuglevel>=4 HM Debug only  Sept2014
								if (byte=='\r') LogAdd("\\r");
								else if (byte=='\n') LogAdd("\\n");
								else if (isprint(byte)) LogAdd("%c",byte);
								else LogAdd("[0x%02x]",byte);
							}
							LogAdd("\n");//Debug Added for clarity Sept2014 
							if (byte == '@' && leader < 3){//3 @ signs
								leader++;
							} else if (leader == 3 ){ //crc
								rcrc[0] = byte;
								leader++;
							} else if (leader ==4) {  //crc
								rcrc[1] = byte;
								leader++;
							} else if (leader == 5) { //msg length msb
								rbuf[leader-5] = byte;
								leader++;
								rilength += (int) byte*256;
							} else if (leader == 6) { //msg length lsb
								rbuf[leader-5] = byte;
								leader++;
								rilength = rilength + ((int) byte) - REC_BUF_SIZE;
							} else if (leader == 7) { //blk num
								rblknum = (int) byte;
								rbuf[leader-5] = byte;
								leader++;
							} else if (leader == 8) {//num of blks
								rnumblks = (int) byte;
								rbuf[leader-5] = byte;
								leader++;
							} else if (leader >= 9) {//keep reading the rest
								rbuf[leader-5] = byte;
								leader++;
							}
						}
						/* get the current time */ //end of data
						else T=time(NULL);      
					} while ((T>=0 && To>=0 && difftime(T,To)<10) && (leader < (rilength+5))); /* check the termination conditions */
					//it works fine here

					ricrc = Calc_Crc(rbuf, rilength);
						
					// check the crc, send done to Rudics & copy msg to a file
					if ((((unsigned char) ((ricrc >> 8) & 0x00FF)) == rcrc[0]) && (((unsigned char) (ricrc & 0x00FF)) == rcrc[1])) {
						static cc msg[] = "CRC chk valid. Rcvd blk %i/%i, length %i, Files left =%i\n";
						
						//when transmitted, you really have to hang up HM 2010 Single file TX per connection scheme
						//pputs(modem, "done", 20, "ATH0\n");Wait(100);
						LogEntry(FuncName, msg, rbuf[3], rbuf[4], rilength, num_files);
						
						//Open the file and copy message into it
						if (!(config=fopen("rxconfig.tmp","w"))) {
							/* create the message */
							static cc format[]="Unable to open \"%s\" for writing.\n";
							/* make the logentry */
							LogEntry(FuncName,format,"rxconfig.tmp");
						}
										
						for (  x = 5; x < (rilength); x++) {
							fputc((int) rbuf[x], config); //writing to a file
						}
							
						//Cleanup
						fclose(config);
						free(rbuf);
						remove(config_path);
						rename("rxconfig.tmp", config_path);
							
						//Tell config that we have a new mission.cfg file
						vitals.status &= ~DownLoadCfg;
						
						//set the flag to exit recovery
						LastTXstatus = 1;
						GetCmdretries++;
											
						if(num_files >0 ) {
							static cc format[]="Debug3 'data' sent. Total # of files to send %i\n";
							LogEntry(FuncName,format,num_files);						
							pputs(modem, "data",20,""); //HM Sept2014 //More files to send
						}//Last data file.  Still need to check if any cmds or not
						//else if(first_file==1){
						else{
							static cc format[]="Debug5 'done' sent. No more file to send %i\n";
							LogEntry(FuncName,format,num_files);						
							pputs(modem,"done",20,"");
							//goto ResendReq;
						}
						//goto GoodBeg;  //Good blk TX. re-evaluate the entire file system
						
					} else {
						static cc msg[] = "CRC error\n";
						LogEntry(FuncName, msg);
						if (GetCmdretries > 5) { //GetCmdretries to get a new msg from the land is limited to 5
							LastTXstatus = 0;
							pputs(modem,"+++",20,"\n");Wait(500);
							pputs(modem,"ATH0",100,"\n");Wait(500);
							goto Err;   //This was how I found it. Honest.
						}
					}				            
				}
				 
				//Wait(100);//HM
				WatchDog();
			//}// while ((upload_retry <= MAX_UPLOAD_RETRIES) && (connect_retry<=MAX_RECONNECTS) && flag != 1 && flag !=3);
			//} while ((connect_retry<=MAX_RECONNECTS) && flag != 1 && flag !=3); 
			//} while ((connect_retry<=MAX_RECONNECTS) && flag != 1); //Repeat to check cmds from the land
			} while ((connect_retry<=MAX_RECONNECTS) && flag != 1 && flag !=3); //Repeat to check cmds from the land
		
			if(splitTX==1 && (flag==1 || flag==3)){//remove only the file successfully transmitted
				static cc format[]="File uploaded. Removing %s\n";      
				if(fioName(fid,fname,FILENAME_MAX)>0)remove(fname);//one file only
				LogEntry(FuncName,format,fname);
				goto GoodBeg;  //Good blk TX. re-evaluate the file system
			}
		}
		//debug
		LogAdd("Hung up. %d/%d\n",upload_retry, MAX_UPLOAD_RETRIES);//debug HM
		//debug end
		pputs(modem, "+++", 20, "\n"); Wait(500);
		pputs(modem, "ATH0", 10, "\n"); Wait(500);//Hung up 
		hangup(modem);
	}
	   
	// All files were successfully transmitted, therefore all files should be deleted. Impeccable logic.
	//if(num_files != 0){
	if(total_files_uploaded!= 0){
		for (LastTXstatus=1,fid=0;fid <MAXFILES; fid++) {//HM
		  if (fioName(fid,fname,FILENAME_MAX)>0)  {
			 if (strcmp(fname,config_path) != 0) {
				if (strcmp(fname, log_path) != 0) {
					if ((fioLen(fid) != 0)) {
						static cc format[] = "Remove all files %s\n";
						LogEntry(FuncName, format, fname);
						remove(fname);
					}
				}
			 }
		  }
		}
	   
		Err: /* collection point for aborts */

		/* report the number of files uploaded */
		if (LastTXstatus == 1 && (debuglevel>=2 || (debugbits&UPLOAD_H)))//HM added condition status == 1
		{
		  static cc format[]="Total files uploaded: %d\n";      
		  LogEntry(FuncName,format,total_files_uploaded);
		}
	   
		/* log the upload status */
		if (LastTXstatus==0)
		{
		  /* create the message */
		  static cc format[]="Abort upload: Reconnect"
			 "limit [%u] exceeded. Upload incomplete.\n";

		  /* make the logentry */
		  LogEntry(FuncName,format,MAX_RECONNECTS);
		}	   
	}
	//DspOff();//HM debug
	return LastTXstatus;
}

/*------------------------------------------------------------------------*/
/* function to upload a file from the Iridium float to the remote host    */
/*------------------------------------------------------------------------*/
/**
   Changed heavily to incorporate the NOAA Rudics protocol. LW, HM 10/22/2009
   This function uploads a data file from the Iridium float to the remote
   host.
     
      \begin{verbatim}
      input:
         modem.......A structure that contains pointers to machine dependent
                     primitive IO functions.  See the comment section of the
                     SerialPort structure for details.  The function checks
                     to be sure this pointer is not NULL.

         localpath...The filename on the Flash file system to be transferred to
                     the remote host.
 
         hostpath....The filename on the remote host where the file will be
                     stored. 
                     
      output:
         This function returns a positive value if successful, zero if it
         fails, and a negative value if exceptions were detected in the
         function's arguments.
      \end{verbatim}
*/
//int UpLoadFile(const struct SerialPort *modem, const char *localpath, const long filesize, unsigned char resend_bitmap[], int last_file, int fid)
int UpLoadFile(const struct SerialPort *modem, const char *localpath, const long filesize, unsigned char resend_bitmap[], int fid)
{
   /* function name for log entries */
   static cc FuncName[] = "UpLoadFile()";
   int buf_wait;	//wait length for the Irid modem buffer to clear, blk size dependent
   //static int test_num=0;
   
   /* initialize return value */
   int success=0;
            
   /* pet the watch dog */
   WatchDog(); 

   /* validate the port */
   if (!modem)
   {
      /* create the message */
      static cc msg[]="NULL serial port.\n";

      /* make the logentry */
      LogEntry(FuncName,msg);
   }
   
   /* validate the localpath */
   else if (!localpath)
   {
      /* create the message */
      static cc msg[]="NULL localpath.\n";

      /* make the logentry */
      LogEntry(FuncName,msg);
   } 
     
   /* check if carrier-dectect enabled and CD line not asserted */
   else if (modem->cd && !modem->cd())
   {
      /* create the message */
      static cc msg[]="No carrier detected.\n";

      /* make the logentry */
      LogEntry(FuncName,msg);

      /* indicate failure */
      success=0;
   }

   /* attempt to upload the file */
   else if(block_num <= last_blk)//skip after the last bad blk is sent to save time
   {  
		/* define a FILE pointer for uploading the data file */
		FILE *source=NULL;  	//commented out by HM Moved to the front as global     
		unsigned char *buf;  	// The file buffer Commented out by HM. Moved to the front as global
		int crc_calc;  			//Stores the calculated CRC
		int filenamesize = strlen(localpath); //The length of the file name
		int i, j, iterator; //Iterator variables
		int length, length5; 			//The length of the block
		int file_blknum; 		// The total number of blocks per file
		unsigned char temp2; 	// The bit in the bitmap that needs to be set for a particular block
		int skip  = 0;          // if =0, upload.  if =1, skip the block and no wait.      
		
		/* pet the watch dog */
		WatchDog(); 
   
		if ((source=fopen(localpath,"r"))!=0){
			file_blknum = (int)(filesize-1L) / BLOCK_SIZE + 1;
			//LogAdd("file_blknum=%d, File open %s\n", file_blknum, localpath);//HM
			for(j=1; j <= file_blknum; j++){
				/* set up format for log entry */
				//static cc format[] = "%s. %iB blk %i/%i total, %i/%i of file.\n";  
				static cc format[] = "%s. %iB blk %i/%i total.\n";  
				static cc format2[] = "Skip blk %i, not in resend rqst.\n";
				if(block_num <=last_blk){ //current blk num is less than the last blk num of resend request
					if (j == file_blknum) {  //If we are transmitting the last block in this file
						if (j == 1) {  //If the last block is the first block (ie, the file fits in one block)
							//Open a buffer with space for the data header and file name header
							//buf = (unsigned char *) malloc((sizeof(unsigned char)) * (filesize + 23 + filenamesize - BLOCK_SIZE*(j-1)));
							buf = (unsigned char *) malloc((sizeof(unsigned char)) * (filesize + 25 + filenamesize));
					  
							//Read the file into the buffer and get the length
							length = fread(buf+22+filenamesize, sizeof(unsigned char), filesize, source) + 17 + filenamesize;
							//LogAdd("a.Length=%d\n", length);
							//Make the buffer be null-terminated
							buf[filesize+22+filenamesize] = 0x00;
						} else { //This is not the first block
							//Open a buffer with space for only the data header
							//buf = (unsigned char *) malloc((sizeof(unsigned char)) * (filesize + 11 - BLOCK_SIZE*(j-1)));
							buf = (unsigned char *) malloc((sizeof(unsigned char)) * (filesize + 13 - BLOCK_SIZE*(j-1)));
						  
							//Read the file into the buffer and get the length
							length = fread(buf+10, sizeof(unsigned char), filesize - BLOCK_SIZE*(j-1), source) + 5;
							//LogAdd("b.Length=%d\n", length);
						
							//Make the buffer be null-terminated
							buf[filesize+10-BLOCK_SIZE*(j-1)] = 0x00;
						}
						fclose(source);   // it is the last block in this file... 
					} else if (j == 1) { // Transmitting the first block in a multi-block file
						//Open a buffer with space for the data header and file name header
						//buf = (unsigned char *) malloc((sizeof(unsigned char)) * (BLOCK_SIZE + 23 + filenamesize));
						buf = (unsigned char *) malloc((sizeof(unsigned char)) * (BLOCK_SIZE + 25 + filenamesize));
				   
						//Read the file into the buffer and get the length
						length = fread(buf+22+filenamesize, sizeof(unsigned char), BLOCK_SIZE, source) + 17 + filenamesize;
					  //LogAdd("c.Length=%d\n", length);
				   
						//Make the buffer be null-terminated
						buf[BLOCK_SIZE + 22 + filenamesize] = 0x00;
						
					} else {//2-nd and thereafter blocks reading
						//Open a buffer with space for just the data header
						//buf = (unsigned char *) malloc((sizeof(unsigned char)) * (11+BLOCK_SIZE*(j-1)));
						//buf = (unsigned char *) malloc((sizeof(unsigned char)) * (13+BLOCK_SIZE*(j-1)));
						buf = (unsigned char *) malloc((sizeof(unsigned char)) * (13+BLOCK_SIZE));
				   
						//Read the file into the buffer and get the length
						length = fread(buf+10, sizeof(unsigned char), BLOCK_SIZE, source) + 5;
						//LogAdd("d.Length=%d\n", length);
				   
						//Make the buffer be null-terminated
						//buf[10+BLOCK_SIZE*(j-1)] = 0x00;
						buf[10+BLOCK_SIZE] = 0x00;
			
						if(block_num == last_blk)fclose(source);//make sure file is closed
					}
				
					// Create a char with the bit we need to check for this particular block
					temp2 = (char) ((int) (pow((double) 2, (double) ((block_num-1) % 8))));
				
					if ((temp2 & resend_bitmap[(block_num-1)/8]) == temp2) { // Check if the block is set in the bitmap
						/* make the logentry */
						//LogEntry(FuncName,format,localpath, length, block_num, num_blocks, j, file_blknum);
						LogEntry(FuncName,format,localpath, length, block_num, num_blocks, j);
				
						buf[5] = (unsigned char)((length & 0xFF00)>>8); //length
						buf[6] = (unsigned char) (length & 0x00FF);     //length
						buf[7] = 'T';									//data type
						buf[8] = (unsigned char) (block_num & 0x00FF);  
						buf[9] = (unsigned char) (num_blocks & 0x00FF);
				   
						length5=length+5;
						
						// CREATE THE FILE HEADER
						//    Format: \n\n((((FILENAME))))\n\n
						//    Length: 17 chars + length of filename
						
						if (j == 1) {
							buf[10] = '\n';
							buf[11] = '\n';
							buf[12] = '(';
							buf[13] = '(';
							buf[14] = '(';
							buf[15] = '(';
					  
							for (i = 16; i < (17 + filenamesize); i++) {
								if ((i-16) < filenamesize) {
									buf[i] = localpath[i-16];
								} else {
									buf[i] = ')';
									buf[i+1] = ')';
									buf[i+2] = ')';
									buf[i+3] = ')';
									buf[i+4] = '\n';
									buf[i+5] = '\n';
								}
							}
						}
						
						WatchDog();						
						// Creating the NOAA buoy data header
						// Calculate the CRC 
						crc_calc = Calc_Crc(buf+5, length);

						buf[0] = '@';
						buf[1] = '@';
						buf[2] = '@';
						buf[3] = (unsigned char)((crc_calc & 0xFF00)>>8);//CRC
						buf[4] = (unsigned char)(crc_calc & 0x00FF);	 //CRC
						//LogAdd("crc%x%x length%x%x\n",buf[3],buf[4],buf[5],buf[6]); //debug HM
							
						if(pputs(modem,"@@@",3,""));Wait(200);
						if(pputb(modem, buf[3],1));Wait(50);
						if(pputb(modem, buf[4],1));Wait(100);
						for(iterator=5; iterator<length5; iterator++){pputb(modem, buf[iterator],100);}
					
						//	for (iterator = 0; iterator < length5; iterator++) {  // output the entire buffer, including NUL characters.
						//		pputb(modem, buf[iterator], 1); 				//changed iterator=0 to 5 by HM
						//	}
						skip=0; //HM 
						//}//
					} else {   // block not in bitmap, no need to transmit
						LogEntry(FuncName, format2, block_num);
						skip=1; //HM
					}
							
					//Apf9 serial is sending at 19500 but Iridium modem is 2400bps. Put wait between
					//the blocks depending on the size of blocks
					buf_wait=(length/240)*1000-2300; //HM Calculate wait period between blocks
					if(buf_wait<0)buf_wait=0;
					//if((last_file && (j == file_blknum)) || skip ==1 || num_blocks==block_num || block_num == last_blk) buf_wait=0; //if it is the last buffer, no wait. HM
					if((j == file_blknum) || skip ==1 || num_blocks==block_num || block_num == last_blk) buf_wait=0; //if it is the last buffer, no wait. HM
					//LogAdd("%d %d %d %d %d\n", last_file, j, file_blknum, buf_wait, last_blk);//HM debug Test only
					free(buf); // free up memory allocated
					//WatchDog();
					Wait(buf_wait); //wait ## miliseconds until Irid modem buffer clears except for the last block HM
					WatchDog();
					block_num++;
				}
			}
			success = 1; //All blks uploaded for this file! Still need to check if "done" or "Resend"
		} else {
			/* make a log entry that the file can't be opened for reading */

			/* create the message */
			static cc format[]="Unable to open \"%s\" for reading.\n";

			/* make the logentry */
			LogEntry(FuncName,format,localpath);
			RemoveFile(fid, fid, 0);
			success=0;
		}
	}
   
   return success;
}
int ChkResendReq(const struct SerialPort *modem, unsigned char resend_bitmap[], unsigned char resend_bytes[], unsigned char resend_crc[], int wait)
{
	/* function name for log entries */
	static cc FuncName[] = "ChkResendReq()";
	int i, n, flag;
	int ndelay = 1;//interval to check modem output
	time_t To=time(NULL),T=To;
	unsigned char byte;

	To= time(NULL);
	T = To;
	i = 0;
	flag = 0; //0=timeout (not done), 1=done, 2=resend, 3=cmd from land
	
	WatchDog();
	if(flag==0){
		static cc msg[]    = "Land rsp: ";
		LogEntry(FuncName, msg);
	}			 
	do 
	{//check response
		if (modem->getb(&byte)>0) {
			/* write the current byte to the logstream */
			if (debuglevel>=2 || (debugbits&EXPECT_H)) { //HM it was debuglevel >=3
				if (byte=='\r') LogAdd("\\r");
				else if (byte=='\n') LogAdd("\\n");
				else if (isprint(byte)) {
					if(byte=='@')byte='a'; //@@@ signs forbidden on the record
					LogAdd("%c",byte); 
					}
				else LogAdd("[0x%02x]",byte);
			}

			/* check if the current byte matches the expected byte from the prompt */
			if (i == 3 || i == 4) {         // CRC in
				resend_crc[i-3] = byte;
				i++;
			} else if (i == 5) {            // if retry, it should be "R"
				i++;
				resend_bytes[0] = byte;	   
			} else if (i >= 6 && i <= 13) { // read the resend request from 7-th to 14th byte
				resend_bitmap[13-i] = byte;
				resend_bytes[i-5] = byte;
				i++;
				if (i == 14) { 			   //end of resend request
					LogAdd("\n"); 
					flag = 2; 
					break; 
				}
			} else if (byte == '@') { 	   //Land is sending resend request
				i++;
			} else if (byte == 'c') { // Land is sending cmds, prepare to download file from Rudics
				LogAdd("%c", byte);
				if (expect(modem, "mds", "", 10, "") > 0) {
					flag = 3;
					LogAdd("mds\n");
					break;
				}
			} else if (byte == 'm') { 		//Land is sending "done" msg, success HM
				LogAdd("c%c", byte); 
				//if (expect(modem, "one", "ATH0", 10, "") > 0) {
				if (expect(modem, "ds", "", 10, "") > 0) { //HM
					flag = 3;
					//pputs(modem, "ATH0", 10, "\n");//Hung up statement added by HM
					LogAdd("ds\n");
					break;
				}	
			} else if (byte == 'd') { 	   //Land is sending "done" msg, success
				LogAdd("%c", byte); 
				//if (expect(modem, "one", "ATH0", 10, "") > 0) {
				if (expect(modem, "one", "", 10, "") > 0) { //HM
					flag = 1;
					//pputs(modem, "ATH0", 10, "\n");//Hung up statement added by HM
					LogAdd("one\n");
					break;
				}
			} else if (byte == 'o') { 		//Land is sending "done" msg, success HM
				LogAdd("d%c", byte); 
				//if (expect(modem, "one", "ATH0", 10, "") > 0) {
				if (expect(modem, "ne", "", 10, "") > 0) { //HM
					flag = 1;
					//pputs(modem, "ATH0", 10, "\n");//Hung up statement added by HM
					LogAdd("ne\n");
					break;
				}
			}	
			/* don't allow too many bytes to be read between time checks */
			if(i>0)ndelay=25;
			if (n<0 || n>ndelay) {T=time(NULL); n=0;} else n++; //HM it was n>25 
		}

		/* get the current time */
		else T=time(NULL); WatchDog();
	} while (T>=0 && To>=0 && difftime(T,To)< wait);  
	return flag;
}
void RemoveFile(int min_fid, int max_fid, int LastTXstatus)
{
static cc FuncName[] = "RemoveFile()";
int fid, fMaxfid, fMax=0;
char fname[FILENAME_MAX+1];
	for (fid=min_fid;fid <= max_fid; fid++) {
		WatchDog();
		if (fioName(fid,fname,FILENAME_MAX)>0)  {
			if (strcmp(fname,config_path) != 0) {
				if (strcmp(fname, log_path) != 0) {
					if(fioLen(fid)>fMax)fMaxfid=fid;//find the largest file
				} 	
			}
		}
	}
	if(fioName(fMaxfid, fname, FILENAME_MAX)>0){
		static cc format[] = "Upload problem. Remove file %s\n"; 
		static cc format2[]= "Upload OK. Remove file %s\n";
		if(LastTXstatus==0)LogEntry(FuncName, format, fname);
		else if(LastTXstatus==1)LogEntry(FuncName, format2, fname);
		remove(fname);
	}
}
