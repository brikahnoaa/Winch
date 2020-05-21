/******************************************************************************\
** BIRCFSettings.c (9/19/2011)
** For multiple CF cards logging using BigIDEAs.  Eacg BigIDEA has master and
** slave slots and it utilizes both as D: drive.
**
**	Revised to ProvSetting1.c	
**		
**	from noaasettings2.c			// Settings portion of BigIDEADataLogger suite
**	
**	1.1 release: 08/06/2004
*****************************************************************************
**	
**	Licensed by:	NOAA, PMEL Newport for the Peristor CFX
**	info@persistor.com - http://www.persistor.com
**	
*****************************************************************************
**	
**	Developed by:	Haru Matsumoto
**	
\******************************************************************************/
#define	PD_CMD_BUILD_LINKS		// this must be at the top of the file !!!

#include	<cfxpico.h>		// Persistor PicoDOS Definitions

#include	<stdio.h>
#include	<stdlib.h>
#include	<stdarg.h>
#include	<string.h>
#include	<limits.h>
#include	<dosdrive.h>	// PicoDOS DOS Drive and Directory Definitions

#include	<BIRCFRecorder.h>	// BigIDEA Recorder Definitions
#include	<BIRCFSettings.h>	// common for BigIDEADataLogger. changed by hm 8/27/99

bool	ModifyPermission = true;	// gets updated when password entered
char 	CFXNum[6];					// 5 digits + terminating zero
BIDLSettings	Settings[] = // see <settings.h> and <BIRecorder.h>
	{
	ACQ_STARTUPS_NAME,	ACQ_STARTUPS_DEFAULT,	ACQ_STARTUPS_DESC,	"",
	ACQ_MAXSTRTS_NAME,	ACQ_MAXSTRTS_DEFAULT,	ACQ_MAXSTRTS_DESC,	"",
	ACQ_NCHAN_NAME,		ACQ_NCHAN_DEFAULT,		ACQ_NCHAN_DESC,	"",
	ACQ_SRATEHZ_NAME,	   ACQ_SRATEHZ_DEFAULT,	   ACQ_SRATEHZ_DESC,	"",
	ACQ_SAMPLES_NAME,	   ACQ_SAMPLES_DEFAULT,	   ACQ_SAMPLES_DESC,	"",
	ACQ_LOGFILE_NAME,	   ACQ_LOGFILE_DEFAULT,	   ACQ_LOGFILE_DESC,	"",
	ACQ_PLTFRMID_NAME,	ACQ_PLTFRMID_DEFAULT,	ACQ_PLTFRMID_DESC,	"",
	ACQ_LATITUDE_NAME,	ACQ_LATITUDE_DEFAULT,	ACQ_LATITUDE_DESC,	"",
	ACQ_LONGITUDE_NAME,	ACQ_LONGITUDE_DEFAULT,	ACQ_LONGITUDE_DESC,	"",
	ACQ_GAIN_NAME,		   ACQ_GAIN_DEFAULT,		   ACQ_GAIN_DESC,		"",
	ACQ_EXPID_NAME,		ACQ_EXPID_DEFAULT,		ACQ_EXPID_DESC,		"",
	ACQ_PROJID_NAME,	   ACQ_PROJID_DEFAULT,		ACQ_PROJID_DESC,	"",
	ACQ_PROG_NAME,		   ACQ_PROG_DEFAULT,		   ACQ_PROG_DESC,		"",	
	ACQ_FILECOUNT_NAME,	ACQ_FILECOUNT_DEFAULT,	ACQ_FILECOUNT_DESC,	"",
	ACQ_DAQNAME_NAME,		ACQ_DAQNAME_DEFAULT,  	ACQ_DAQNAME_DESC, "",
	ACQ_SLEEP_NAME,	   ACQ_SLEEP_DEFAULT,	   ACQ_SLEEP_DESC,	"",
	ACQ_ACTIVESEC_NAME,	ACQ_ACTIVESEC_DEFAULT,	ACQ_ACTIVESEC_DESC,	"",
	ACQ_DUTYCYCLE_NAME,	ACQ_DUTYCYCLE_DEFAULT,	ACQ_DUTYCYCLE_DESC,	"",
	ACQ_HYDROSENS_NAME,	ACQ_HYDROSENS_DEFAULT,	ACQ_HYDROSENS_DESC,	"",
	ACQ_PRAMPNAME_NAME,  ACQ_PRAMPNAME_DEFAULT,  ACQ_PRAMPNAME_DESC, "",
	ACQ_WAKEUP_NAME,	   ACQ_WAKEUP_DEFAULT,		ACQ_WAKEUP_DESC,	"",
	ACQ_WARMUP_NAME,	   ACQ_WARMUP_DEFAULT,		ACQ_WARMUP_DESC,	"",
	ACQ_PWFILT_NAME,	   ACQ_PWFILT_DEFAULT,		ACQ_PWFILT_DESC,	"",
	ACQ_LOPASS_NAME,	   ACQ_LOPASS_DEFAULT,		ACQ_LOPASS_DESC,	"",
	//ACQ_DAQNAME_NAME,	   ACQ_DAQNAME_DEFAULT,		ACQ_DAQNAME_DESC,	"",
	ACQ_HYDROSRN_NAME,	ACQ_HYDROSRN_DEFAULT,	ACQ_HYDROSRN_DESC, "",
	ACQ_NCHAN_NAME,		ACQ_NCHAN_DEFAULT,		ACQ_NCHAN_DESC,   "",
	ACQ_TESTSEC_NAME,    ACQ_TESTSEC_DEFAULT,    ACQ_TESTSEC_DESC, "",
	BIR_CFPPBSZ_NAME,	   BIR_CFPPBSZ_DEFAULT,	   BIR_CFPPBSZ_DESC,	"",
	//BIR_RAMPPBSZ_NAME,	BIR_RAMPPBSZ_DEFAULT,	BIR_RAMPPBSZ_DESC,	"",
	//BIR_RAMHDBFSZ_NAME,	BIR_RAMHDBFSZ_DEFAULT,	BIR_RAMHDBFSZ_DESC,	"",
	//BIR_NODRVTEST_NAME,	BIR_NODRVTEST_DEFAULT,	BIR_NODRVTEST_DESC,	"",
	BIR_FLOGFLAG_NAME,	BIR_FLOGFLAG_DEFAULT,	BIR_FLOGFLAG_DESC,	"",
	//BIR_UARTMONIT_NAME,	BIR_UARTMONIT_DEFAULT,	BIR_UARTMONIT_DESC,	"",
	BIR_CFDOSDRV_NAME,	BIR_CFDOSDRV_DEFAULT,	BIR_CFDOSDRV_DESC,	"",
	BIR_MINFREEKB_NAME,	BIR_MINFREEKB_DEFAULT,	BIR_MINFREEKB_DESC,	"",
	BIR_CURBIA_NAME,	   BIR_CURBIA_DEFAULT,		BIR_CURBIA_DESC,	"",
	BIR_BIADEVICE_NAME,	BIR_BIADEVICE_DEFAULT,	BIR_BIADEVICE_DESC,	"",
	SYS_F32TRUST_VEENAME,	"",	"",	"",
	0, 0, 0, 0		// terminating entry
	};

// PROTOTYPES
void main(void);
char *SetupHelpCmd(CmdInfoPtr cip);
char *SetupListCmd(CmdInfoPtr cip);
char *SetupDefaultsCmd(CmdInfoPtr cip);
char *SetupStressCmd(CmdInfoPtr cip);
char *PrepCFCardCmd(CmdInfoPtr cip);
char *PrepDrivesCmd(CmdInfoPtr cip);
char *AllDirsCmd(CmdInfoPtr cip);


CmdTable		SetupCmdTable[] = 
	{
//	PROMPT			HANDLER					2COLS DEF  CR  ABV	HEADER TEXT
	"SETUP>"		, PDCCmdStdInteractive,
												0,	0,	1,	1,	"BigIDEADataLogger Settings Commands\n"
//	 COMMAND		COMMAND				 	  MIN PRV  CR NUM	SHORT HELP TEXT
//	  NAME			FUNCTION				  REQ LEV REP BASE	EMPTY STRING "" TO 
//	(CONSTANT)		POINTER					  ARG REQ  OK 0=NO	OMIT FROM HELP LIST
	,"LIST"			, SetupListCmd,				0,	0,	0,	0,	"List settings [/V] (verbose)"	
	,"SET"			, PDCSetCmd,				0,	0,	0,	0,	"[var=[str]] [/SLFE?]"
	,"DEFAULTS"		, SetupDefaultsCmd,			0,	10,	0,	0,	"Reset to default settings"	
	,"STRESS"		, SetupStressCmd,			0,	10,	0,	0,	"Insert settings to stress test"	
	,"PREPCF"		, PrepCFCardCmd,			0,	10,	0,	0,	"Prepare CF card for BIRecorder"	
	,"PREPDRVS"		, PrepDrivesCmd,			0,	10,	0,	0,	"Prepare drives for BIRecorder"	
	,"ALLDIRS"		, AllDirsCmd,				0,	10,	0,	0,	"Get directories from all drives"	

	,"DATE"			, PDCDateCmd,				0,	0,	0,	0,	"[mm-dd-yy[yy] [time]"	
	,"QUIT"			, PDCResetCmd,				0,	0,	0,	0,	"QUIT, RES, EXIT all reset the CFX"	
	,"RES"			, PDCResetCmd,				0,	0,	0,	0,	""	
	,"EXIT"			, PDCResetCmd,				0,	0,	0,	0,	""	
	,"HELP"			, SetupHelpCmd,				0,	0,	0,	0,	"HELP, HE, H, ? all show help"
	,"HE"			, SetupHelpCmd,				0,	0,	0,	0,	""
	,"H"			, SetupHelpCmd,				0,	0,	0,	0,	""
	,"?"			, SetupHelpCmd,				0,	0,	0,	0,	""

	//TERMINATING ENTRY (!!!!This must be present at end of table !!!!)
	,""				, 0,						0,	0,	0,	0,	0
	};


/******************************************************************************\
**	settings		Main Entry Point
**	
**	This is just the launching point for the major portions of the program.
\******************************************************************************/
void settings(void)
	{
	BIDLSettings		*setp = Settings;
	VEEData				vdp;
	CmdInfo				ci, *cip = &ci;

	char 				*ProgramDescription = 
		{
		"\n"
		"Type HELP from the SETUP> prompt for a list of the commands available\n"
		"in the program. Type QUIT from the SETUP> to exit the program.\n"
		"\n"
		"Type LIST /V from the SETUP> prompt for a complete description\n"
		"of each of the settings names, values, and purpose.\n"
		"\n"
		"Type QUIT from the SETUP> to exit the program.\n"
		"\n"
		};
	sprintf(CFXNum,"%05ld",BIOSGVT.CF1SerNum);
	//printf("CFX %s\n",CFXNum);

	printf("\nProgram: %s: %s %s \n", __FILE__, __DATE__, __TIME__);
	printf(ProgramDescription);

//
//	READ IN THE CURRENT VARIABLES
//
	printf("\nVARIABLE:           CURRENT:            DEFAULT:            DIFF:\n");
	while (setp->optName)
		{
		vdp = VEEFetchData(setp->optName);
		if (vdp.str == 0)	// not found, create it
			{
			if (! ModifyPermission)
				{
				printf("Didn't find %s, do not use this system\n", setp->optName);
				setp++;
				continue;
				}
			printf("Creating %s with default setting...\n", setp->optName);
			if (! VEEStoreStr(setp->optName, setp->optDefault))
				{
				printf("!!! VEE store failed, do not use this system\n");
				break;
				}
			else
				continue;	// without increment, to see setting
			}
		setp->optCurrent = vdp.str;
		printf("%-20s%-20s%-20s%s\n", setp->optName, setp->optCurrent, setp->optDefault,
			strcmp(setp->optCurrent, setp->optDefault) ? "*!*" : "");
		setp++;
		}

//
//	ENTER INTERACTIVE COMMAND PROCESSING MODE
//
	CmdStdSetup(&ci, SetupCmdTable, 0);
	cip->privLevel = ModifyPermission ? 100 : 1;

	while (CmdStdRun(&ci) != CMD_BREAK)
		{
		printf("\n%s", ci.errmes);
		fflush(stdout);
		}

	BIOSReset();	// clean restart
	
	}	//____ main() ____//


/******************************************************************************\
**	SetupHelpCmd
\******************************************************************************/
char *SetupHelpCmd(CmdInfoPtr cip)
	{
	char *HELPhelpText =
		{
		"\n"
		"Type HELP without parameters to display the list of Setup commands\n"
		"and abbreviated syntax. Type HELP followed by a command or setting\n"
		"name to displaycomprehensive usage information.\n"
		"\n"
		};
	enum				{ help, cmd };
	char				cmdbuf[32];
	BIDLSettings		*setp = Settings;
	
	DosSwitch	QMsw = { "/", '?', 0, 0 };		// help

	CmdExtractCIDosSwitches(cip, "?", &QMsw);

// HELP SWITCH SELECTED (common format to many commands)
	if (QMsw.pos)
		return HELPhelpText;

	if (ARGS == help)
		return PDCCmdStdHelp(cip);

//	FIRST CHECK SETTING MATCH
	while (setp->optName)
		{
		if (strcmp(setp->optName, cip->argv[cmd].str) == 0)
			{
			printf("\n%s\n", setp->optDesc);
			return 0;
			}
		setp++;
		}

//	THAT FAILEDS, TRY EXTENDED COMMAND HELP

	strncpy(cmdbuf, cip->argv[cmd].str, sizeof(cmdbuf));
	sprintf(cip->line, "%s /?", cmdbuf);

	CmdParse(cip);		// gives us args for future initargs call
	CmdDispatch(cip);
	CmdSetNextCmd(cip, 0);	// don't repeat
	if (cip->errmes != CmdErrUnknownCommand)
		return cip->errmes;

	return 0;

	}	//____ SetupHelpCmd() ____//



/******************************************************************************\
**	SetupListCmd
\******************************************************************************/
char *SetupListCmd(CmdInfoPtr cip)
	{
	char *LISThelpText =
		{
		"\n"
		"Type LIST to display a list of the current settings\n"
		"\n"
		"  /V          Show detailed descriptions of each settings function.\n"
		"\n"
		};

	BIDLSettings		*setp = Settings;
	VEEData				vdp;
	
	DosSwitch	QMsw = { "/", '?', 0, 0 };		// help
	DosSwitch	vsw = { "/", 'V', 0, 0 };		// verbose

	CmdExtractCIDosSwitches(cip, "?v", &QMsw, &vsw);

// HELP SWITCH SELECTED (common format to many commands)
	if (QMsw.pos)
		return LISThelpText;

	printf("\nVARIABLE:           CURRENT:            DEFAULT:            DIFF:\n");
	while (setp->optName)
		{
		vdp = VEEFetchData(setp->optName);
		if (vdp.str == 0)	// not found, create it
			{
			printf("Didn't find %s, do not use this system\n", setp->optName);
			setp++;
			continue;
			}

		setp->optCurrent = vdp.str;

		printf("%-20s%-20s%-20s%s\n", setp->optName, setp->optCurrent, setp->optDefault,
			strcmp(setp->optCurrent, setp->optDefault) ? "*!*" : "");
		if (vsw.pos)
			printf("%s\n", setp->optDesc);

		setp++;
		}

	return 0;

	}	//____ SetupListCmd() ____//


/******************************************************************************\
**	SetupStressCmd
\******************************************************************************/
char *SetupStressCmd(CmdInfoPtr cip)
	{

	DosSwitch	psw = { "/", 'P', 0, 0 };		// persistor stress settings
	CmdExtractCIDosSwitches(cip, "P", &psw);

	printf("\nInstalling settings for stress testing\n");

	VEEStoreStr(ACQ_SRATEHZ_NAME, "1000");	// 1000 hz
	VEEStoreStr(ACQ_NCHAN_NAME, "8");		// * 8 channels
//	VEEStoreStr(ACQ_SAMPLES_NAME, "2");		// double it again for 16000 bps
	VEEStoreStr(ACQ_SAMPLES_NAME, "3");		// double it again for 16000 bps

	VEEStoreStr(BIR_CFPPBSZ_NAME, "1048576");	// 1MB/2 files (approx 1 per minute)

	if (psw.pos)
		{
		VEEStoreStr(BIR_MINFREEKB_NAME, "58585000");	// 10 files for 60GB drive
		VEEStoreStr(ACQ_STARTUPS_NAME, "1");	// skip extended setup
		}

	
	return 0;
	}	//____ SetupStressCmd() ____//


/******************************************************************************\
**	SetupDefaultsCmd
\******************************************************************************/
char *SetupDefaultsCmd(CmdInfoPtr cip)
	{
	
	char *SEThelpText =
		{
		"\n"
		"Resets one or all of the settings to their default states\n"
		"\n"
		};

	bool				reset;
	BIDLSettings		*setp = Settings;
	VEEData				vdp;
	
	DosSwitch	QMsw = { "/", '?', 0, 0 };		// help

	CmdExtractCIDosSwitches(cip, "?", &QMsw);

// HELP SWITCH SELECTED (common format to many commands)
	if (QMsw.pos)
		return SEThelpText;

	printf("\nVARIABLE:           CURRENT:            DEFAULT:            RESET:\n");
	while (setp->optName)
		{
		vdp = VEEFetchData(setp->optName);
		if (vdp.str == 0)	// not found, create it
			{
			printf("Creating %s with default setting...\n", setp->optName);
			if (! VEEStoreStr(setp->optName, setp->optDefault))
				{
				printf("!!! VEE store failed, do not use this system\n");
				break;
				}
			else
				continue;	// without increment, to see setting
			}
		reset = false;	// assume we won't need to
		if (strcmp(setp->optCurrent, setp->optDefault) != 0)
			{
			if (! VEEStoreStr(setp->optName, setp->optDefault))
				printf("!!! VEE store failed, do not use this system\n");
			else
				{
				reset = true;
				setp->optCurrent = setp->optDefault;
				}
			}
		printf("%-20s%-20s%-20s%s\n", setp->optName, setp->optCurrent, setp->optDefault,
			reset ? "*!*" : "");
		setp++;
		}
	
	printf("\n");
	return 0;

	}	//____ SetupDefaultsCmd() ____//


/******************************************************************************\
**	PrepCFCardCmd
\******************************************************************************/
char *PrepCFCardCmd(CmdInfoPtr cip)
	{
	short			err;
	ulong			total_sectors, dos_sectors;

	short	ssdid;
	short	prtn;
	short	ptype;
	ulong	firstSect;
	ulong	lastSect;
	long	cfPPBsectors;
	DOSPartitionTable	dtab;

	static char *PrepCFCardCmdhelpText =
		{
		"\n"
		"PREPCF fdisks and formats the CompactFlash card to have two\n"
		"partitions. The first is a DOS partion equal to the capacitye\n"
		"of the card minus the space defined by BIR.CFPPBSZ / 512 which\n"
		"becomes the second non-DOS partion used as the ping-pong buffer\n"
		"\n"
		"Make sure that BIR.CFPPBSZ is the correct size before running\n"
		"PREPCF, and make sure to run it again if you make any changes\n"
		"to BIR.CFPPBSZ\n"
		"\n"
		};
		
	cprintf(PrepCFCardCmdhelpText);
	if (! QRconfirm("Do you really want to continue", false, true))
		return 0;

	PZCacheRelease(SHRT_MIN);
	DSDReset(0);	// Dismount all drives
	BSDReset(0);	// reset all
	BIAShutDown();
	DSDSetCurrentDrive(-1);

	if ((err = ATACapacity(CFGetDriver(), &total_sectors, 0, 0, 0)) != 0)
		return CmdErrf(cip, err, "Can't get CF capacity");

	cfPPBsectors = VEEFetchLong(BIR_CFPPBSZ_NAME, atol(BIR_CFPPBSZ_DEFAULT)) / 512UL;

	if (cfPPBsectors > total_sectors - (5000000 / 512UL))
		return CmdErrf(cip, -1, "CF card too small");
	dos_sectors = total_sectors - (cfPPBsectors + 256);// give us some slop for reserved sectors
	
	printf("\nPreparing CF with %5.1fMB DOS partition and %5.1fMB ping-pong buffer\n",
		((float) dos_sectors * 512.0) / (float) MB_SIZE, ((float) cfPPBsectors * 512.0) / (float) MB_SIZE);
		
	execstr("FDISK /SCF /P%lu /Q", dos_sectors * 512UL);
	execstr("MOUNT CF");
	execstr("FORMAT C: /P /Q");
	execstr("FDISK");

// find the multi-MB partition
	if ((ssdid = SSDLookupID("CF")) < 0)
		return CmdErrf(cip, birFindCFSSDFailed, "problem finding any CF buffer SSD");

	if ((err = MBRReadPartitionTable(ssdid, 0, dtab)) != 0)
		return CmdErrf(cip, -err, "problem reading partition table");
	dtab[1].ptype = nondos;		// fdisk made it a dos partition
	if ((err = MBRWritePartitionTable(ssdid, 0, dtab)) != 0)
		return CmdErrf(cip, -err, "problem writing partition table");
	execstr("FDISK");
	
// perform the same lookup that BIRecorder will perform

	for (prtn = MBR_MAX_PARTS - 1; prtn >= 0; prtn--)	// scan backwards looking for one big enough
		{
		ptype = MBRGetPartitionSectors(ssdid, 0, prtn, false, &firstSect, &lastSect);
		//DBG( cprintf("\n ssdid=%d, prtn=%d, firstSect=%lx, lastSect=%lx", ssdid, prtn, firstSect, lastSect);  cdrain();)
		if (ptype == nondos && lastSect - firstSect + 1 >= cfPPBsectors)
			break;
		}
	if (prtn < 0)		// went through without finding it
		return CmdErrf(cip, -1, "SEVERE PROBLEM - CANNOT FIND PING-PONG BUFFER PARTITION!!!");

	cprintf("\nComplete, found 0x%lX sectors in partition %d\n", lastSect - firstSect + 1, prtn);

	return 0;

	}	//____ PrepCFCardCmd() ____//




/******************************************************************************\
**	PrepDrivesCmd
\******************************************************************************/
char *PrepDrivesCmd(CmdInfoPtr cip)
	{
	short		i, count;
	DosSwitch	ysw = { "/", 'Y', 0, 0 };
	DosSwitch	esw = { "/", 'E', 0, 0 };
	DosSwitch	ssw = { "/", 'S', 0, 0 };

	static char *WarnText =
		{
		"\n"
		"#!#!#  THIS COMMAND WILL DESTROY ALL DATA ON ALL HARD DRIVES #!#!#\n"
		"\n"
		};
		
	static char *PrepDrivesCmdhelpText =
		{
		"\n"
		"The PREPDRVS command cycles through all of the attached BigIDEAs\n"
		"and hard drives to fdisk and format them properly for the logger\n"
		"program.\n"
		"\n"
		"Because this has so much potential for destroying the results of\n"
		"a prior deployment, you must invoke the command as follows:\n"
		"\n"
		"   \"PREPDRVS /Y /E /S\"<enter>\n"
		"\n"
		"You will then be given one final warning and opportunity to cancel.\n"
		"\n"
		};
	CmdExtractCIDosSwitches(cip, "YES", &ysw, &esw, &ssw);

	if (! (ysw.pos && ysw.pos < esw.pos && esw.pos < ssw.pos))
		{
		cprintf(PrepDrivesCmdhelpText);
		return 0;
		}

	cprintf(WarnText);
	if (! QRconfirm("Do you really want to do this", false, true))
		return 0;
	cprintf("\nPreparing drives ... \n");
	PZCacheRelease(SHRT_MIN);
	DSDReset(0);	// Dismount all drives
	BSDReset(0);	// reset all
	BIAShutDown();
	DSDSetCurrentDrive(-1);

	VEEStoreStr(SYS_F32TRUST_VEENAME, "D");	// enable trust for D:
	
	for (i = count = 0; i < 10; i++)
		{
		if (execstr("MOUNT BIHD-%u /Q", i) == 0)
			{
			cprintf("\nFormatting BIHD-%u ...", i);
			execstr("FORMAT /Q");
			execstr("DIR /V");
			count++;
			}
		execstr("MOUNT BIHD-%u /D /Q", i);
		}

	return 0;

	}	//____ PrepDrivesCmd() ____//



/******************************************************************************\
**	AllDirsCmd
\******************************************************************************/
char *AllDirsCmd(CmdInfoPtr )
	{
	short		i;

cprintf("\n\n ============================= ALL DIRS ============================\n");

	execstr("DIR C: /V");
	
	for (i = 0; i < 10; i++)
		{
		if (kbhit())
			if (cgetc() == '.')
				break;
		if (execstr("MOUNT BIHD-%u /V", i) == 0)
			execstr("DIR D: /V");
		else
			break;
		}

	return 0;

	}	//____ AllDirsCmd() ____//


