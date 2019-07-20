/******************************************************************************\
**	ProvSetting1.c	
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

#include	<CFxRecorder.h>	// BigIDEA Recorder Definitions
#include	<FloatSettings1.h>	// common for BigIDEADataLogger. changed by hm 8/27/99

bool	ModifyPermission = true;	// gets updated when password entered
char 	CFXNum[6];					// 5 digits + terminating zero
BIDLSettings	Settings[] = // see <FloatSettings1.h> and <CFxRecorder.h>
	{
	QUE_STARTUPS_NAME,	QUE_STARTUPS_DEFAULT,	QUE_STARTUPS_DESC,	"",
	QUE_MAXSTRTS_NAME,	QUE_MAXSTRTS_DEFAULT,	QUE_MAXSTRTS_DESC,	"",
	QUE_GOMONITOR_NAME,	QUE_GOMONITOR_DEFAULT,	QUE_GOMONITOR_DESC,	"",
	QUE_SRATEHZ_NAME,	QUE_SRATEHZ_DEFAULT,	QUE_SRATEHZ_DESC,	"",
	QUE_SAMPLES_NAME,	QUE_SAMPLES_DEFAULT,	QUE_SAMPLES_DESC,	"",
	QUE_PREPMIN_NAME,	QUE_PREPMIN_DEFAULT,	QUE_PREPMIN_DESC,	"",
	QUE_LOGFILE_NAME,	QUE_LOGFILE_DEFAULT,	QUE_LOGFILE_DESC,	"",
	QUE_PLTFRMID_NAME,	QUE_PLTFRMID_DEFAULT,	QUE_PLTFRMID_DESC,	"",
	QUE_LATITUDE_NAME,	QUE_LATITUDE_DEFAULT,	QUE_LATITUDE_DESC,	"",
	QUE_LONGITUDE_NAME,	QUE_LONGITUDE_DEFAULT,	QUE_LONGITUDE_DESC,	"",
	QUE_GAIN_NAME,		QUE_GAIN_DEFAULT,		QUE_GAIN_DESC,		"",
	QUE_EXPID_NAME,		QUE_EXPID_DEFAULT,		QUE_EXPID_DESC,		"",
	QUE_PROJID_NAME,	QUE_PROJID_DEFAULT,		QUE_PROJID_DESC,	"",
	QUE_PROG_NAME,		QUE_PROG_DEFAULT,		QUE_PROG_DESC,		"",	
	QUE_DRIFTLON_NAME,	QUE_DRIFTLON_DEFAULT,	QUE_DRIFTLON_DESC,	"",
	QUE_DRIFTLAT_NAME,	QUE_DRIFTLAT_DEFAULT,	QUE_DRIFTLAT_DESC,	"",
	QUE_BATHYF_NAME,	QUE_BATHYF_DEFAULT,		QUE_BATHYF_DESC,	"",
	QUE_MAXDEPTH_NAME,	QUE_MAXDEPTH_DEFAULT,	QUE_MAXDEPTH_DESC,	"",
	QUE_PARKDEPTH_NAME,	QUE_PARKDEPTH_DEFAULT,	QUE_PARKDEPTH_DESC,	"",
	QUE_LANDPHNUM_NAME, QUE_LANDPHNUM_DEFAULT,  QUE_LANDPHNUM_DESC, "",
	QUE_GPSWARM_NAME, 	QUE_GPSWARM_DEFAULT,	QUE_GPSWARM_DESC, 	"",
	QUE_LASTGPS_NAME, 	QUE_LASTGPS_DEFAULT,	QUE_LASTGPS_DESC, 	"",
	QUE_INGRID_NAME, 	QUE_INGRID_DEFAULT,		QUE_INGRID_DESC, 	"",
	QUE_GOTODIVE_NAME,  QUE_GOTODIVE_DEFAULT,	QUE_GOTODIVE_DESC,  "",
	QUE_ESTMDEPTH_NAME,	QUE_ESTMDEPTH_DEFAULT,	QUE_ESTMDEPTH_DESC,	"",
	QUE_FILECOUNT_NAME,	QUE_FILECOUNT_DEFAULT,	QUE_FILECOUNT_DESC,	"",
	QUE_FILENUM_NAME,	QUE_FILENUM_DEFAULT,	QUE_FILENUM_DESC,	"",
	QUE_MAXFILE_NAME,	QUE_MAXFILE_DEFAULT,	QUE_MAXFILE_DESC,	"",
	QUE_IRIDWARM_NAME,	QUE_IRIDWARM_DEFAULT,	QUE_IRIDWARM_DESC,	"",
	QUE_MAXCALLS_NAME, 	QUE_MAXCALLS_DEFAULT,	QUE_MAXCALLS_DESC,	"",
	QUE_SECDRFTCL_NAME,	QUE_SECDRFTCL_DEFAULT,	QUE_SECDRFTCL_DESC,	"",
	QUE_MAXDRFTCL_NAME,	QUE_MAXDRFTCL_DEFAULT,	QUE_MAXDRFTCL_DESC,	"",
	QUE_NUMDRFTCL_NAME,	QUE_NUMDRFTCL_DEFAULT,	QUE_NUMDRFTCL_DESC,	"",
	QUE_SURFSAMPL_NAME,	QUE_SURFSAMPL_DEFAULT,	QUE_SURFSAMPL_DESC,	"",
	QUE_PARKSAMPL_NAME,	QUE_PARKSAMPL_DEFAULT,	QUE_PARKSAMPL_DESC,	"",
	QUE_STATUS_NAME,	QUE_STATUS_DEFAULT,		QUE_STATUS_DESC,	"",
	QUE_LASTTRNS_NAME,	QUE_LASTTRNS_DEFAULT,	QUE_LASTTRNS_DESC,	"",
	QUE_NUMOFDIVE_NAME,	QUE_NUMOFDIVE_DEFAULT,	QUE_NUMOFDIVE_DESC, "",
	QUE_MAXNUMDIV_NAME,	QUE_MAXNUMDIV_DEFAULT,	QUE_MAXNUMDIV_DESC, "",
	QUE_PROVTERM_NAME,	QUE_PROVTERM_DEFAULT,	QUE_PROVTERM_DESC,  "",
	QUE_IRIDTERM_NAME,	QUE_IRIDTERM_DEFAULT,	QUE_IRIDTERM_DESC,  "",
	QUE_LOGONLY_NAME,	QUE_LOGONLY_DEFAULT,	QUE_LOGONLY_DESC,  "",
	QUE_GOPARK_NAME,	QUE_GOPARK_DEFAULT,		QUE_GOPARK_DESC,	"",
	QUE_GOBOTTOM_NAME,	QUE_GOBOTTOM_DEFAULT,	QUE_GOBOTTOM_DESC,	"",
	QUE_NUMOFGPS_NAME,	QUE_NUMOFGPS_DEFAULT,	QUE_NUMOFGPS_DESC,	"",
	QUE_NUMOFSAT_NAME,	QUE_NUMOFSAT_DEFAULT,	QUE_NUMOFSAT_DESC,	"",
	BIR_DIVESTART_NAME,	BIR_DIVESTART_DEFAULT,	BIR_DIVESTART_DESC,	"",
	BIR_PREVDEPTH_NAME,	BIR_PREVDEPTH_DEFAULT,	BIR_PREVDEPTH_DESC,	"",
	BIR_LATITUDE_NAME,	BIR_LATITUDE_DEFAULT,	BIR_LATITUDE_DESC,	"",
	BIR_LONGITUDE_NAME,	BIR_LONGITUDE_DEFAULT,	BIR_LONGITUDE_DESC,	"",
	QUE_CFPPBSZ_NAME,	QUE_CFPPBSZ_DEFAULT,	QUE_CFPPBSZ_DESC,	"",
	QUE_BLKLNGTH_NAME,	QUE_BLKLNGTH_DEFAULT,	QUE_BLKLNGTH_DESC,	"",
	QUE_RAMPPBSZ_NAME,	QUE_RAMPPBSZ_DEFAULT,	QUE_RAMPPBSZ_DESC,	"",
	QUE_MINFREEKB_NAME,	QUE_MINFREEKB_DEFAULT,	QUE_MINFREEKB_DESC,	"",
	BIR_NUMOFGPS_NAME,	BIR_NUMOFGPS_DEFAULT,	BIR_NUMOFGPS_DESC,	"",
	BIR_NUMOFSAT_NAME,	BIR_NUMOFSAT_DEFAULT,	BIR_NUMOFSAT_DESC,	"",
	BIR_STATUS_NAME,	BIR_STATUS_DEFAULT,		BIR_STATUS_DESC,	"",
	BIR_FLOGFLAG_NAME,	BIR_FLOGFLAG_DEFAULT,	BIR_FLOGFLAG_DESC,	"",
	BIR_UARTMONIT_NAME,	BIR_UARTMONIT_DEFAULT,	BIR_UARTMONIT_DESC,	"",
	BIR_RTCGPSFIX_NAME,	BIR_RTCGPSFIX_DEFAULT,	BIR_RTCGPSFIX_DESC,	"",
	0, 0, 0, 0		// terminating entry
	};

// PROTOTYPES
void main(void);
char *SetupHelpCmd(CmdInfoPtr cip);
char *SetupListCmd(CmdInfoPtr cip);
char *SetupDefaultsCmd(CmdInfoPtr cip);
//char *SetupStressCmd(CmdInfoPtr cip);
char *PrepCFCardCmd(CmdInfoPtr cip);
char *PrepDrivesCmd(CmdInfoPtr cip);
char *AllDirsCmd(CmdInfoPtr cip);
char *VDelete(CmdInfoPtr cip);


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
	,"VDELETE"		, VDelete,					0,	0,	0,	0,	"Delete a particular EPROM entry"	
	,"DEFAULTS"		, SetupDefaultsCmd,			0,	10,	0,	0,	"Reset to default settings"	
//	,"STRESS"		, SetupStressCmd,			0,	10,	0,	0,	"Insert settings to stress test"	
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
/*char *SetupStressCmd(CmdInfoPtr cip)
	{

	DosSwitch	psw = { "/", 'P', 0, 0 };		// persistor stress settings
	CmdExtractCIDosSwitches(cip, "P", &psw);

	printf("\nInstalling settings for stress testing\n");

	VEEStoreStr(QUE_SRATEHZ_NAME, "1000");	// 1000 hz
//	VEEStoreStr(QUE_NCHAN_NAME, "8");		// * 8 channels
//	VEEStoreStr(QUE_SAMPLES_NAME, "2");		// double it again for 16000 bps
	VEEStoreStr(QUE_SAMPLES_NAME, "3");		// double it again for 16000 bps

	VEEStoreStr(QUE_CFPPBSZ_NAME, "1048576");	// 1MB/2 files (approx 1 per minute)

	if (psw.pos)
		{
		VEEStoreStr(QUE_MINFREEKB_NAME, "58585000");	// 10 files for 60GB drive
		VEEStoreStr(QUE_STARTUPS_NAME, "1");	// skip extended setup
		}

	
	return 0;
	}	//____ SetupStressCmd() ____//
*/

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

	cfPPBsectors = VEEFetchLong(QUE_CFPPBSZ_NAME, atol(QUE_CFPPBSZ_DEFAULT)) / 512UL;

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

/******************************************************************************\
**	VDelete
\******************************************************************************/
char *VDelete(CmdInfoPtr cip)
	{

	cprintf("\nVEEDelete(%s) = %d\n", cip->argv[1].str, VEEDelete(cip->argv[1].str));
	return 0;

	}	//____ VDelete() ____//


