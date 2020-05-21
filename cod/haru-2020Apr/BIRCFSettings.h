/******************************************************************************\
** BIRCFSettings.h 9/18/2011 HM
** Revised from FloatSetting2.h
**	FloatSettings1.h	
**		* Merged noaasettings.c source into single combined project
** 8/13/2004 HM, NOAA, Newport		
\******************************************************************************/

void settings(void);	// renamed main from pre-FAT32 code

typedef struct	// Used to access runtime settings
	{
	char	*optName;		// VEE name for lookup
	char	*optDefault;	// default value
	char	*optDesc;		// detailed description text
	char	*optCurrent;	// current value
	} BIDLSettings;

#define	ACQ_STARTUPS_NAME		"ACQ.STARTUPS"
#define	ACQ_STARTUPS_DEFAULT	"0"
#define	ACQ_STARTUPS_DESC		\
	"This is used by the acquisition program to keep track of the number of\n" \
	"times the system has been power cycled or reset. It should normally be\n" \
	"set to zero at the start of deployement. It will be incremented by one\n" \
	"each time the system resets. This is compared to another variable named \n" \
	"ACQ.MAXSTRTS to prevent complete depletion of the batteries should\n" \
	"something unforeseen happen which forces the program to constantly fail.\n" \
	"The only time you would explicitly set this to non-zero is while bench\n" \
	"testing to confirm proper termination of a pathological system. \n" \

#define	ACQ_MAXSTRTS_NAME		"ACQ.MAXSTRTS"
#define	ACQ_MAXSTRTS_DEFAULT	"9999"
#define	ACQ_MAXSTRTS_DESC		\
	"This controls the number of times the acquisition program will attempt\n" \
	"to resume logging after a power cycle or reset. Ideally, there would be\n" \
	"only one power cycle, which would be when the experiment first starts,\n" \
	"and the program would continue uninterrupted until the experiment\n" \
	"completes. In practice, you may want to observe several successful\n" \
	"startups prior to a major deployment. You may also want to allow for a\n" \
	"few unexpected runtime failures.\n" \

#define	ACQ_NCHAN_NAME			"ACQ.NCHAN"
#define	ACQ_NCHAN_DEFAULT		"1"
#define	ACQ_NCHAN_DESC			\
	"This controls the number of analog channels sampled at each sampling\n" \
	"interval and must be 1, 2, 4, or 8.  For H3 use 1.\n" \

#define	ACQ_SRATEHZ_NAME		"ACQ.SRATEHZ"
#define	ACQ_SRATEHZ_DEFAULT		"2000"
#define	ACQ_SRATEHZ_DESC		\
	"This controls the basic sampling rate for the system, which refers to\n" \
	"the frequency at which a swath of one or more channels is sampled and\n" \
	"recorded. You must enter a value in Hz of80000,64000,50000,40000,32000\n"\
	",25000,20000,16000,12800,5000,2000,1000,500,250.\n" \

#define	ACQ_SAMPLES_NAME		"ACQ.SAMPLES"
#define	ACQ_SAMPLES_DEFAULT		"3"
#define	ACQ_SAMPLES_DESC		\
	"This variable defines the type of samples to record from the A-D converter.\n" \
	"A value of '0' means record at 1-byte resolution.  For both NOAADAQ1 and DAQ2.\n" \
	"A value of '1' is invalid and reserved for other purpose.\n" \
	"A value of '2' is only valid for DAQ1. 12 bits resolutin.\n" \
	"A value of '3' means full 16-bit 2-byte A/D.  NOAADAQ2 and Persistor.\n"\
	"Any other value will cause undefined (bad) behavior.\n" \

#define	ACQ_LOGFILE_NAME		"ACQ.LOGFILE"
#define	ACQ_LOGFILE_DEFAULT		"C:ACTIVITY.LOG"
#define	ACQ_LOGFILE_DESC		\
	"This variable defines the name of a separate logfile recorded on C: drive.\n" \
	"It contains the operation of the program during eahc mooring. If\n" \
	"this is an empty string, no runtime information logging is performed. If\n" \
	"this is a valid DOS filename on a CompactFlash partition, the\n" \
	"acquisition program will write a a status record each time the hard\n" \
	"drive spins up to transfer a buffer full of data from the CompactFlash\n" \
	"to the hard drive. Up to 14 chars.\n" \

#define	ACQ_PLTFRMID_NAME		"ACQ.PLTFRMID"
#define	ACQ_PLTFRMID_DEFAULT	"4CHR"
#define	ACQ_PLTFRMID_DESC		\
	"Platform ID for this float.  Each float has a unique 4-char\n" \
	"long ID (e.g., Gld1).  This 4-char long ID is stamped to each data file to\n" \
	"identify the system for post-deployment ID purpose.\n" \

#define	ACQ_LATITUDE_NAME		"ACQ.LATITUDE"
#define	ACQ_LATITUDE_DEFAULT	"N30:04.503"
#define	ACQ_LATITUDE_DESC		\
	"Latitude. 10-char long. Starting with 3 char long degree with N or S, followed by min\n" \
	"4 char long with 3 decimal points.\n" \

#define	ACQ_LONGITUDE_NAME		"ACQ.LONGITUDE"
#define	ACQ_LONGITUDE_DEFAULT	"W130:13.862"
#define	ACQ_LONGITUDE_DESC		\
	"Longitude. 12-char long, starting by W or W with 3 digit degree, followed by \n" \
	"4 char long minute with 3 decimal points\n"\

#define	ACQ_GAIN_NAME		"ACQ.GAIN"
#define	ACQ_GAIN_DEFAULT	"2"
#define	ACQ_GAIN_DESC		\
	"No effect to H4 pre-amp.  Gain setting of pre-amp.  Choose the value between 0 \n"\
	"and 3, 0 is the lowest gain and 3 is the highest with 6 dB increment. \n" \

#define	ACQ_EXPID_NAME		"ACQ.EXPID"
#define	ACQ_EXPID_DEFAULT	"Up to 16 Char"
#define	ACQ_EXPID_DESC		\
	"This variable contains a string of up to 16 characters that will show up\n" \
	"in the header block of each file written to the hard drive.  Choose the name \n" \
	"associated with the cruise name and year for later ID purpose.\n" \

#define	ACQ_PROJID_NAME		"ACQ.PROJID"
#define	ACQ_PROJID_DEFAULT	"4CHR"
#define	ACQ_PROJID_DESC		\
	"This variable contains a string of up to 4 characters that will show up\n" \
	"in the header block of each file written to the hard drive.  Choose the name \n" \
	"associated with the system for later ID purpose.\n" \

#define	ACQ_PROG_NAME		"ACQ.PROGNAME"
#define	ACQ_PROG_DEFAULT	"CFxLgSP3i3_3"
#define	ACQ_PROG_DESC		\
	"Name of the program and version number (e.g., CFxLgSP3i3_1). Up to 12 characters.\n"\

#define	ACQ_FILECOUNT_NAME		"ACQ.FILECOUNT"
#define	ACQ_FILECOUNT_DEFAULT		"0"
#define	ACQ_FILECOUNT_DESC		\
	"File number count. Default=0 \n"

//#define	ACQ_MAXNUMFIL_NAME		"ACQ.MAXNUMFIL"
//#define	ACQ_MAXNUMFIL_DEFAULT		"12"		
//#define	ACQ_MAXNUMFIL_DESC			\
//	"Legacy of how many files to write.  Does not affect this program.\n" \

#define	ACQ_SLEEP_NAME		"ACQ.SLEEP"
#define	ACQ_SLEEP_DEFAULT	"0"
#define	ACQ_SLEEP_DESC		\
	"This is used by the acquisition program to put CFx low-power sleep mode until it's\n" \
	"ready to bigin A/D and logging data. Default value is 0. You should this zero so that\n" \
	"data logging begins immediately. Unit is in hours 0-65536.\n" \

#define	ACQ_TESTSEC_NAME		"ACQ.TESTSEC"
#define	ACQ_TESTSEC_DEFAULT	 	"10"
#define	ACQ_TESTSEC_DESC		\
	"The variable contains the period in seconds of test logging. A new file test.dat is \n" \
	"created during pre-launching phase.  If A/D has 0s, it allows operator to stop logging. \n" \

#define	ACQ_ACTIVESEC_NAME		"ACQ.ACTIVESEC"
#define	ACQ_ACTIVESEC_DEFAULT	"1800"
#define	ACQ_ACTIVESEC_DESC		\
	"It sets duration of active logging period in sec for each hour. It determines the \n" \
	"size of data file (=BIR.CFPPBSZ).\n" \

#define	ACQ_DUTYCYCLE_NAME		"ACQ.DUTYCYCLE"
#define	ACQ_DUTYCYCLE_DEFAULT	 "1800"
#define	ACQ_DUTYCYCLE_DESC		\
	"The variable contains the interval in seconds when the logging cycle repeats. If for example,\n" \
	"it is 1820 sec, a new logging occurs at every 1820 sec, and new data file is created. \n"\
	"Must be > ACQ.ACTIVESEC. Program goes to low-power hybernation in between.\n" \

#define	ACQ_HYDROSENS_NAME		"ACQ.HYDROSENS"
#define	ACQ_HYDROSENS_DEFAULT	"-174"
#define	ACQ_HYDROSENS_DESC		\
	"Hydrophone sensitivity in dBV re uPa. -192 is standard for ITC1032.\n" 

#define	ACQ_HYDROSRN_NAME		"ACQ.HYDROSRN"
#define	ACQ_HYDROSRN_DEFAULT	"####"
#define	ACQ_HYDROSRN_DESC		\
	"Hydrophone's 6-digit serial number.\n" 

#define	ACQ_PRAMPNAME_NAME		"ACQ.PRAMPNAME"
#define	ACQ_PRAMPNAME_DEFAULT	"H4.1A020##"
#define	ACQ_PRAMPNAME_DESC		\
	"Pre-Amp model, revision # and board number (e.g.,H3R4.1A020). Up to 10 characters.\n"\

//#define	ACQ_PRAMPNUM_NAME		"ACQ.PRAMPNUM"
//#define	ACQ_PRAMPNUM_DEFAULT	"42"
//#define	ACQ_PRAMPNUM_DESC		\
//	"Pre-amp board number.\n" 

#define	ACQ_WARMUP_NAME		"ACQ.WARMUP"
#define	ACQ_WARMUP_DEFAULT		"0"
#define	ACQ_WARMUP_DESC		\
	"Pre-amp warm seconds. Default=0sec. If duty cycle logging, set to 5. \n"

#define	ACQ_DAQNAME_NAME		"ACQ.DAQNAME"
#define	ACQ_DAQNAME_DEFAULT	"CSACDAQ000"
#define	ACQ_DAQNAME_DESC		\
	"DAQ board name, revision and board number. Up to 10 characters, e.g., CSACDQ001 is CSACDAQ\n" \
	"board.\n"\

#define	ACQ_WAKEUP_NAME		"ACQ.WAKEUP"
#define	ACQ_WAKEUP_DEFAULT	 "0"
#define	ACQ_WAKEUP_DESC		\
	"This variable contains wake-up-time in seconds since 1970 Jan 1.  The default is 0\n" \
	"and CFx will write a wake-up time when the logging program is initiated.  Program \n" \
	"sets the value automatically.\n"

#define	ACQ_PWFILT_NAME		"ACQ.PWFILT"
#define	ACQ_PWFILT_DEFAULT		"2"
#define	ACQ_PWFILT_DESC		\
	"Pre-whitening filter switch setting (1 to 4). \n"\

#define	ACQ_LOPASS_NAME		"ACQ.LOPASS"
#define	ACQ_LOPASS_DEFAULT		"840"
#define	ACQ_LOPASS_DESC		\
	"Low-pass filter setting. 45% of the sampling rate is appropriate.\n"\

