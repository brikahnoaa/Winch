/******************************************************************************\
**	FloatSettings1.h	
**		* Merged noaasettings.c source into single combined project
** 2/15/2006 HM, NOAA, Newport		
\******************************************************************************/

void settings(void);	// renamed main from pre-FAT32 code

typedef struct	// Used to access runtime settings
	{
	char	*optName;		// VEE name for lookup
	char	*optDefault;	// default value
	char	*optDesc;		// detailed description text
	char	*optCurrent;	// current value
	} BIDLSettings;

#define	QUE_STARTUPS_NAME		"QUE.STARTUPS"
#define	QUE_STARTUPS_DEFAULT	"0"
#define	QUE_STARTUPS_DESC		\
	"This is used by the acquisition program to keep track of the number of\n" \
	"times the system has been power cycled or reset. It should normally be\n" \
	"set to zero at the start of deployement. \n" \

#define	QUE_MAXSTRTS_NAME		"QUE.MAXSTRTS"
#define	QUE_MAXSTRTS_DEFAULT	"255"
#define	QUE_MAXSTRTS_DESC		\
	"This controls the number of times the acquisition program will attempt\n" \
	"to resume logging after a power cycle or reset. \n" \

#define	QUE_GOMONITOR_NAME			"QUE.GOMONITOR"
#define	QUE_GOMONITOR_DEFAULT		"0"
#define	QUE_GOMONITOR_DESC			\
	"If =1, monitoring starts immediately; =0 monitoring begins after making \n" \
	"sure it is grounded.\n" \

#define	QUE_IRIDTERM_NAME			"QUE.IRIDTERM"
#define	QUE_IRIDTERM_DEFAULT		"1"
#define	QUE_IRIDTERM_DESC			\
	"If =1, IRIDIUM/GPS are connected and in use. If !=1, test only. \n" \

#define	QUE_PROVTERM_NAME			"QUE.PROVTERM"
#define	QUE_PROVTERM_DEFAULT		"1"
#define	QUE_PROVTERM_DESC			\
	"If =1, PROVOR COM are connected and in use. If !=1, test only. \n" \

#define	QUE_LOGONLY_NAME			"QUE.LOGONLY"
#define	QUE_LOGONLY_DEFAULT		"0"
#define	QUE_LOGONLY_DESC			\
	"If =1, test only.  Allow to log without IRID or PROV; =0, would not log without\n" \
	"PROVTERM and IRIDTERM both = 1.\n"\

#define	QUE_SRATEHZ_NAME		"QUE.SRATEHZ"
#define	QUE_SRATEHZ_DEFAULT		"100"
#define	QUE_SRATEHZ_DESC		\
	"This controls the basic sampling rate for the system, which refers to\n" \
	"the frequency at which a swath of one or more channels is sampled and\n" \
	"recorded. You must enter a value in Hz of 2500,2000,1000, 500, 250,\n" \
	"200, 125, 100, 50, or 40. Any other values will give unpredictable\n" \
	"results.\n" \

#define	QUE_SAMPLES_NAME		"QUE.SAMPLES"
#define	QUE_SAMPLES_DEFAULT		"3"
#define	QUE_SAMPLES_DESC		\
	"This variable defines the type of samples to record from the A-D converter.\n" \
	"A value of '0' means record at 1-byte resolution.  For both NOAADAQ1 and DAQ2.\n" \
	"A value of '1' is invalid and reserved for other purpose.\n" \
	"A value of '2' is only valid for DAQ1. 12 bits resolutin.\n" \
	"A value of '3' means full 16-bit 2-byte A/D.  NOAADAQ2 only.\n"\
	"Any other value will cause undefined (bad) behavior.\n" \

#define	QUE_PREPMIN_NAME		"QUE.PREPMIN"
#define	QUE_PREPMIN_DEFAULT		"3"
#define	QUE_PREPMIN_DESC		\
	"This variable defines the number of minutes to prepare for launching.\n" \

#define	QUE_LOGFILE_NAME		"QUE.LOGFILE"
#define	QUE_LOGFILE_DEFAULT		"ACTIVITY.LOG"
#define	QUE_LOGFILE_DESC		\
	"This variable defines the name of a separate logfile recorded on C: drive.\n" \
	"It contains the operation of the program during eahc mooring. If\n" \
	"this is an empty string, no runtime information logging is performed. If\n" \
	"this is a valid DOS filename on a CompactFlash partition, the\n" \
	"acquisition program will write a a status record each time the hard\n" \
	"drive spins up to transfer a buffer full of data from the CompactFlash\n" \
	"to the hard drive. Up to 14 chars.\n" \

#define	QUE_PLTFRMID_NAME		"QUE.PLTFRMID"
#define	QUE_PLTFRMID_DEFAULT	"Q001"
#define	QUE_PLTFRMID_DESC		\
	"Platform ID for this float.  Each float has a unique 4-char\n" \
	"long ID (e.g., Q001).  This 4-char long ID is stamped to each data file to\n" \
	"identify the system for post-deployment ID purpose.\n" \

#define	QUE_LATITUDE_NAME		"QUE.LATITUDE"
#define	QUE_LATITUDE_DEFAULT	"N30:04.503"
#define	QUE_LATITUDE_DESC		\
	"Latitude in 10-char, starting 3 char long degree with N or S, followed by min\n" \
	"4 char long with 3 decimal points.\n" \

#define	QUE_LONGITUDE_NAME		"QUE.LONGITUDE"
#define	QUE_LONGITUDE_DEFAULT	"W130:13.862"
#define	QUE_LONGITUDE_DESC		\
	"Longitude in 11-char long, starting by W or W with 3 digit degree, followed by \n" \
	"4 char long minute with 3 decimal points\n"\

#define	QUE_GAIN_NAME		"QUE.GAIN"
#define	QUE_GAIN_DEFAULT	"0"
#define	QUE_GAIN_DESC		\
	"Gain setting of pre-amp.  Choose the value between 0 and 3, 0 is the lowest gain and 3\n" \
	"is the highest with 6 dB increment.  Chose 0 for 16-bit, and 3 for 8 bit A/D. \n" \

#define	QUE_RTCTIME_NAME		"QUE.RTCTIME"
#define	QUE_RTCTIME_DEFAULT	"0L"
#define	QUE_RTCTIME_DESC		\
	"RTC time when synched to GPS at the surface.\n" 

#define	QUE_RTCMSEC_NAME		"QUE.RTCMSEC"
#define	QUE_RTCMSEC_DEFAULT	    "0"
#define	QUE_RTCMSEC_DESC		\
	"RTC msec when synched to GPS at the surface.\n" 

#define	QUE_GPSTIME_NAME		"QUE.GPSTIME"
#define	QUE_GPSTIME_DEFAULT		"0L"
#define	QUE_GPSTIME_DESC		\
	"GPS time when RTC was synched to GPS at the surface.\n" 

#define	QUE_GPSMSEC_NAME		"QUE.GPSMSEC"
#define	QUE_GPSMSEC_DEFAULT		"0"
#define	QUE_GPSMSEC_DESC		\
	"GPS msec when RTC was synched to GPS at the surface.\n" 

#define	QUE_PREVDEPTH_NAME		"QUE.PREVDEPTH"
#define	QUE_PREVDEPTH_DEFAULT	"0"
#define	QUE_PREVDEPTH_DESC		\
	"Bottom or parking depth in meter in the previous dive. Default =0.\n" 

#define	QUE_EXPID_NAME		"QUE.EXPID"
#define	QUE_EXPID_DEFAULT	"05"
#define	QUE_EXPID_DESC		\
	"This variable contains a string of up to 2 characters that will show up\n" \
	"in the header block of each file written to the hard drive.  Choose the name \n" \
	"associated with the cruise name and year for later ID purpose.\n" \

#define	QUE_PROJID_NAME		"QUE.PROJID"
#define	QUE_PROJID_DEFAULT	"QUEH"
#define	QUE_PROJID_DESC		\
	"This variable contains a string of up to 4 characters that will show up\n" \
	"in the header block of each file written to the hard drive.  Choose the name \n" \
	"associated with the system for later ID purpose.\n" \

#define	QUE_PROG_NAME		"QUE.PROGNAME"
#define	QUE_PROG_DEFAULT	"QUECont3_3"
#define	QUE_PROG_DESC		\
	"Name of the program and version number (e.g., ProvLogger1). Up to 16 characters.\n"\

#define	QUE_BATHYF_NAME		"QUE.BATHYF"
#define	QUE_BATHYF_DEFAULT		"AXIBATHY.DAT"
#define	QUE_BATHYF_DESC		\
	"Bathymetry data file name. Up to 14 characters.\n"\

#define	QUE_DRIFTLAT_NAME	"QUE.DRIFTLAT"
#define	QUE_DRIFTLAT_DEFAULT	"0"
#define	QUE_DRIFTLAT_DESC		\
	"Drift in latitude in seconds.\n"\

#define	QUE_DRIFTLON_NAME	"QUE.DRIFTLON"
#define	QUE_DRIFTLON_DEFAULT "0"
#define	QUE_DRIFTLON_DESC		\
	"Drift in longitude in seconds.\n"\

#define	QUE_MAXDEPTH_NAME		"QUE.MAXDEPTH"
#define	QUE_MAXDEPTH_DEFAULT		"1950"
#define	QUE_MAXDEPTH_DESC		\
	"Max depth in meters that QUEphone can dive. Up to 1950m.\n"\

#define	QUE_PARKDEPTH_NAME		"QUE.PARKDEPTH"
#define	QUE_PARKDEPTH_DEFAULT		"1800"
#define	QUE_PARKDEPTH_DESC		\
	"Parking depth in meters. Up to 1950m.\n"\
	
#define	QUE_LANDPHNUM_NAME		"QUE.LANDPHNUM"
#define	QUE_LANDPHNUM_DEFAULT		"0012065264495"//PMEL
//#define	QUE_LANDPHNUM_DEFAULT		"0088160000519"//Rudic
#define	QUE_LANDPHNUM_DESC		\
	"Land station modem phone number. Up to 14 characters.\n"\
	
#define	QUE_GPSWARM_NAME		"QUE.GPSWARM"
#define	QUE_GPSWARM_DEFAULT		"60"
#define	QUE_GPSWARM_DESC		\
	"GPS power warm up time in seconds. Up to 32768. Typically 60.\n"\

#define	QUE_LASTGPS_NAME		"QUE.LASTGPS"
#define	QUE_LASTGPS_DEFAULT		"0"
#define	QUE_LASTGPS_DESC		\
	"Status of last GPS fetch. Init value=0. \n"\

#define	QUE_INGRID_NAME		"QUE.INGRID"
#define	QUE_INGRID_DEFAULT		"0"
#define	QUE_INGRID_DESC		\
	"Within the bathy grid =1, out of grid =0. \n"\

#define	QUE_GOTODIVE_NAME		"QUE.GOTODIVE"
#define	QUE_GOTODIVE_DEFAULT		"1"
#define	QUE_GOTODIVE_DESC		\
	"Ready to dive =1, not ready 0. \n"\

#define	QUE_MAXCALLS_NAME		"QUE.MAXCALLS"
#define	QUE_MAXCALLS_DEFAULT		"5"
#define	QUE_MAXCALLS_DESC		\
	"Max number of phone calls IRIDIUM modem can make per surface.\n"\

#define	QUE_NUMDRFTCL_NAME		"QUE.NUMDRFTCL"
#define	QUE_NUMDRFTCL_DEFAULT		"0"
#define	QUE_NUMDRFTCL_DESC		\
	"Number of phone calls for position report made after mission ended.\n"\

#define	QUE_MAXDRFTCL_NAME		"QUE.MAXDRFTCL"
#define	QUE_MAXDRFTCL_DEFAULT		"256"
#define	QUE_MAXDRFTCL_DESC		\
	"Max number of drift calls allowed after mission ended.\n"\

#define	QUE_SECDRFTCL_NAME		"QUE.SECDRFTCL"
#define	QUE_SECDRFTCL_DEFAULT		"1800"
#define	QUE_SECDRFTCL_DESC		\
	"Number of seconds interval float calls after finishing mission while drifting.\n"\

#define	QUE_PARKSAMPL_NAME		"QUE.PARKSAMPL"
#define	QUE_PARKSAMPL_DEFAULT		"30"
#define	QUE_PARKSAMPL_DESC		\
	"Sampling periond in minute during parking.\n"\

#define	QUE_SURFSAMPL_NAME		"QUE.SURFSAMPL"
#define	QUE_SURFSAMPL_DEFAULT		"30"
#define	QUE_SURFSAMPL_DESC		\
	"Sampling periond in minute during at surface.\n"\

#define	QUE_STATUS_NAME		"QUE.STATUS"
#define	QUE_STATUS_DEFAULT		"0"
#define	QUE_STATUS_DESC		\
	"Status of what float is doing 0=INIT, 1=SURFACE, 2=SURFACE_DIVE, 3=DIVE ..9.\n"\


#define QUE_LASTTRNS_NAME   "QUE.LASTTRNS"
#define	QUE_LASTTRNS_DEFAULT		"1"
#define	QUE_LASTTRNS_DESC		\
	"1=Transmssion success; 0=Transmission failed.\n"\

#define	QUE_ESTMDEPTH_NAME		"QUE.ESTMDEPTH"
#define	QUE_ESTMDEPTH_DEFAULT		"0"
#define	QUE_ESTMDEPTH_DESC		\
	"Estimated Bathy depth based on bathy and GPS. If <0, not valid \n"

#define	QUE_FILECOUNT_NAME		"QUE.FILECOUNT"
#define	QUE_FILECOUNT_DEFAULT		"0"
#define	QUE_FILECOUNT_DESC		\
	"Total file number count. Default=0 \n"

#define	QUE_FILENUM_NAME		"QUE.FILENUM"
#define	QUE_FILENUM_DEFAULT		"1"
#define	QUE_FILENUM_DESC		\
	"File number count per dive. Default=1 \n"

#define	QUE_MAXFILE_NAME		"QUE.MAXFILE"
#define	QUE_MAXFILE_DEFAULT		"5"
#define	QUE_MAXFILE_DESC		\
	"Max file number count per dive. Default=5 \n"

#define	QUE_DIVEHOURS_NAME			"QUE.DIVEHOURS"
#define	QUE_DIVEHOURS_DEFAULT		"720"		
#define	QUE_DIVEHOURS_DESC			\
	"How many hours per dive/  Up to 65536 hours.\n" \

#define	QUE_NUMOFDIVE_NAME			"QUE.NUMOFDIVE"
#define	QUE_NUMOFDIVE_DEFAULT		"0"		
#define	QUE_NUMOFDIVE_DESC			\
	"How many dives so far/  Up to 65536 dives.\n" \

#define	QUE_MAXNUMDIV_NAME		"QUE.MAXNUMDIV"
#define	QUE_MAXNUMDIV_DEFAULT		"12"		
#define	QUE_MAXNUMDIV_DESC	\
	"Max many dives allowed/  Up to 65536.\n" \

#define	QUE_IRIDWARM_NAME		"QUE.IRIDWARM"
#define	QUE_IRIDWARM_DEFAULT		"30"		
#define	QUE_IRIDWARM_DESC	\
	"Number of seconds for IRIDIUM to warm up. Typically 30.\n" 

#define	QUE_NUMOFSAT_NAME		"QUE.NUMOFSAT"
#define	QUE_NUMOFSAT_DEFAULT		"0"		
#define	QUE_NUMOFSAT_DESC	\
	"Number of satellites for GPS.\n" \

#define	QUE_GOPARK_NAME		"QUE.GOPARK"
#define	QUE_GOPARK_DEFAULT	"0"
#define	QUE_GOPARK_DESC		\
	"1= go park; 0= go dive to the bottom or stay at the surface.\n" 

#define	QUE_GOBOTTOM_NAME		"QUE.GOBOTTOM"
#define	QUE_GOBOTTOM_DEFAULT	"0"
#define	QUE_GOBOTTOM_DESC 	\
"1= go to bottom; 0= stay surface or parking depth\n"

#define	QUE_NUMOFGPS_NAME		"QUE.NUMOFGPS"
#define	QUE_NUMOFGPS_DEFAULT	"0"
#define	QUE_NUMOFGPS_DESC 	\
"Number of attempts to fetch GPS satellites\n"

#define	QUE_CFPPBSZ_NAME	"QUE.CFPPBSZ"
#define	QUE_CFPPBSZ_DEFAULT	"65536" //For 1KHz  sampling rate (5.8254hrs) Now, back to this line 1/11/00 HM
#define	QUE_CFPPBSZ_DESC		\
	"This variable controls the size of the logging file in CompactFlash C:\n" \
	"The file size = CFPPBSZ+256.\n" \

#define	QUE_BLKLNGTH_NAME		"QUE.BLKLNGTH"
//#define	QUE_RAMPPBSZ_DEFAULT	"65536"
#define	QUE_BLKLNGTH_DEFAULT	"1024"
#define	QUE_BLKLNGTH_DESC		\
	"This variable controls the size of the transmission block size in byte\n" \
	"Total byte size transmitted is BLKLNGTH*64.  CFPPBSZ+256 must be equal \n" \
	"or less than BLKLNGTH*64\n" \

#define	QUE_RAMPPBSZ_NAME	    "QUE.RAMPPBSZ"
#define	QUE_RAMPPBSZ_DEFAULT	"1024"
#define	QUE_RAMPPBSZ_DESC		\
	"This variable controls the size of the RAM buffer size in byte\n" \
	"ADdata buffer (256 bytes) is stored in this buffer, and copied to \n" \
	"a file on CF.\n" \

#define	QUE_MINFREEKB_NAME		"QUE.MINFREEKB"
#define	QUE_MINFREEKB_DEFAULT	"0"
#define	QUE_MINFREEKB_DESC		\
	"Minimum CF drive space = CFPPBSZ. \n" \
