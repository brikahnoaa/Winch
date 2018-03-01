!_TAG_FILE_FORMAT	2	/extended format; --format=1 will not append ;" to lines/
!_TAG_FILE_SORTED	1	/0=unsorted, 1=sorted, 2=foldcase/
!_TAG_PROGRAM_AUTHOR	Darren Hiebert	/dhiebert@users.sourceforge.net/
!_TAG_PROGRAM_NAME	Exuberant Ctags	//
!_TAG_PROGRAM_URL	http://ctags.sourceforge.net	/official site/
!_TAG_PROGRAM_VERSION	5.8	//
ADS8344BIP	hdr/CFX/Headers/Drivers/ADS8344.h	/^enum { 	ADS8344UNI		= 0x08, 		ADS8344BIP		= 0x00	};$/;"	e	enum:__anon9
ADS8344CMD	hdr/CFX/Headers/Drivers/ADS8344.h	/^enum { 	ADS8344CMD		= 0x80	};$/;"	e	enum:__anon7
ADS8344DIF	hdr/CFX/Headers/Drivers/ADS8344.h	/^enum { 	ADS8344SGL		= 0x04, 		ADS8344DIF		= 0x00	};$/;"	e	enum:__anon8
ADS8344EXT	hdr/CFX/Headers/Drivers/ADS8344.h	/^		ADS8344INT		= 0x02, 		ADS8344EXT		= 0x03	};$/;"	e	enum:__anon10
ADS8344INT	hdr/CFX/Headers/Drivers/ADS8344.h	/^		ADS8344INT		= 0x02, 		ADS8344EXT		= 0x03	};$/;"	e	enum:__anon10
ADS8344PDFull	hdr/CFX/Headers/Drivers/ADS8344.h	/^enum { 	ADS8344PDFull	= 0x00,			ADS8344PDFuture	= 0x01,$/;"	e	enum:__anon10
ADS8344PDFuture	hdr/CFX/Headers/Drivers/ADS8344.h	/^enum { 	ADS8344PDFull	= 0x00,			ADS8344PDFuture	= 0x01,$/;"	e	enum:__anon10
ADS8344SGL	hdr/CFX/Headers/Drivers/ADS8344.h	/^enum { 	ADS8344SGL		= 0x04, 		ADS8344DIF		= 0x00	};$/;"	e	enum:__anon8
ADS8344UNI	hdr/CFX/Headers/Drivers/ADS8344.h	/^enum { 	ADS8344UNI		= 0x08, 		ADS8344BIP		= 0x00	};$/;"	e	enum:__anon9
AD_REF_SHDN_PIN	hdr/CFX/Headers/Drivers/ADS8344.h	53;"	d
ADisADS8344	hdr/CFX/Headers/cfxad.h	53;"	d
ADisFirstUnknown	hdr/CFX/Headers/cfxad.h	51;"	d
ADisMAX146	hdr/CFX/Headers/cfxad.h	52;"	d
APP_STACK_BASE	hdr/CFX/Headers/_cfx_memmap.h	99;"	d
APP_USER_NVRAM_BASE	hdr/CFX/Headers/_cfx_memmap.h	103;"	d
APP_USER_NVRAM_SIZE	hdr/CFX/Headers/_cfx_memmap.h	104;"	d
ARGS	hdr/CFX/Headers/_cfx_console.h	221;"	d
ASM_BIOS_CALL	hdr/CFX/Headers/_cfx_internals.h	91;"	d
ASM_BIOS_JMP	hdr/CFX/Headers/_cfx_internals.h	92;"	d
ASM_PICO_CALL	hdr/CFX/Headers/_cfx_internals.h	94;"	d
ASM_PICO_JMP	hdr/CFX/Headers/_cfx_internals.h	95;"	d
ATACapacity	hdr/CFX/Headers/_cfx_drives.h	/^				struct ATADriveID **info)			BIOS_CALL(ATACapacity);$/;"	v
ATADriveID	hdr/CFX/Headers/dosdrive.h	/^	} ATADriveID;$/;"	t	typeref:struct:ATADriveID
ATADriveID	hdr/CFX/Headers/dosdrive.h	/^typedef struct ATADriveID	\/\/ the info structure returned from ATACapacity()$/;"	s
ATADvr	hdr/CFX/Headers/_cfx_drives.h	/^typedef short	(*ATADvr)(void *);$/;"	t
ATAFLashChkBsyDelay	hdr/CFX/Headers/_cfx_internals.h	/^	, ATAFLashChkBsyDelay			= 1000000	\/\/ 1 second (typ 150-250us)$/;"	e	enum:ResetResume::__anon65
ATAFLashStartupTimeout	hdr/CFX/Headers/_cfx_internals.h	/^	, ATAFLashStartupTimeout		= 5000000	\/\/ 5 seconds (>1s for NAND cards)$/;"	e	enum:ResetResume::__anon65
ATAReadSectors	hdr/CFX/Headers/_cfx_drives.h	/^				void *buffer, short count)			BIOS_CALL(ATAReadSectors);$/;"	v
ATAWriteSectors	hdr/CFX/Headers/_cfx_drives.h	/^				void *buffer, short count)			BIOS_CALL(ATAWriteSectors);$/;"	v
ATA_SECTOR_SIZE	hdr/CFX/Headers/_cfx_drives.h	50;"	d
AttribCmd	hdr/CFX/Headers/_cfx_console.h	/^char	*AttribCmd(CmdInfoPtr cip)					PICO_CALL(AttribCmd);$/;"	v
BBP	hdr/CFX/Headers/_cfx_internals.h	/^	} ByteBit, *BBP;$/;"	t	struct:ResetResume	typeref:struct:ResetResume::__anon61
BIA18	hdr/CFX/Headers/_cfx_drives.h	/^	, BIA18			\/\/ 1.8" PCMCIA hard drive$/;"	e	enum:__anon42
BIA25	hdr/CFX/Headers/_cfx_drives.h	/^	, BIA25			\/\/ 2.5" hard drive$/;"	e	enum:__anon42
BIACheckReady	hdr/CFX/Headers/_cfx_drives.h	/^bool	BIACheckReady(bool waitready)				PICO_CALL(BIACheckReady);$/;"	v
BIADEV	hdr/CFX/Headers/_cfx_drives.h	/^	}	BIADEV;$/;"	t	typeref:enum:__anon42
BIAFCM	hdr/CFX/Headers/_cfx_drives.h	/^	, BIAFCM		\/\/ ATA flash card master$/;"	e	enum:__anon42
BIAFCS	hdr/CFX/Headers/_cfx_drives.h	/^	, BIAFCS		\/\/ ATA flash card slave$/;"	e	enum:__anon42
BIAGetDriver	hdr/CFX/Headers/_cfx_drives.h	/^ATADvr	BIAGetDriver(BIADEV device)					PICO_CALL(BIAGetDriver);$/;"	v
BIAGetStatusString	hdr/CFX/Headers/_cfx_drives.h	/^char	*BIAGetStatusString(void)					PICO_CALL(BIAGetStatusString);$/;"	v
BIAHD	hdr/CFX/Headers/_cfx_drives.h	/^	  BIAHD			\/\/ generic IDE hard drive$/;"	e	enum:__anon42
BIAPort	hdr/CFX/Headers/_cfx_internals.h	/^	}	BIAPort;$/;"	t	struct:ResetResume	typeref:struct:ResetResume::__anon67
BIAPowerUp	hdr/CFX/Headers/_cfx_drives.h	/^bool	BIAPowerUp(bool waitready)					PICO_CALL(BIAPowerUp);$/;"	v
BIAST	hdr/CFX/Headers/_cfx_internals.h	/^	}	BIAST;$/;"	t	struct:ResetResume	typeref:struct:ResetResume::__anon66
BIAShutDown	hdr/CFX/Headers/_cfx_drives.h	/^void	BIAShutDown(void)							PICO_CALL(BIAShutDown);$/;"	v
BIA_18_DRIVER_NAME	hdr/CFX/Headers/_cfx_drives.h	91;"	d
BIA_25_DRIVER_NAME	hdr/CFX/Headers/_cfx_drives.h	90;"	d
BIA_DEFAULT_NS	hdr/CFX/Headers/_cfx_internals.h	974;"	d
BIA_DEFAULT_SCB	hdr/CFX/Headers/_cfx_internals.h	972;"	d
BIA_DEV_TYPES	hdr/CFX/Headers/_cfx_drives.h	/^	, BIA_DEV_TYPES	\/\/ number of know driver types$/;"	e	enum:__anon42
BIA_ERRORS	hdr/CFX/Headers/_cfx_errors.h	177;"	d
BIA_FCM_DRIVER_NAME	hdr/CFX/Headers/_cfx_drives.h	92;"	d
BIA_FCS_DRIVER_NAME	hdr/CFX/Headers/_cfx_drives.h	93;"	d
BIA_HD_DRIVER_NAME	hdr/CFX/Headers/_cfx_drives.h	89;"	d
BIA_SCS_TYPE_ID	hdr/CFX/Headers/_cfx_internals.h	973;"	d
BIOSBuildDate	hdr/CFX/Headers/_cfx_globals.h	/^	char	BIOSBuildDate[20];	\/\/ for sanity checks$/;"	m	struct:__anon49
BIOSCRC32	hdr/CFX/Headers/_cfx_globals.h	/^	ulong	BIOSCRC32;			\/\/ CRC32 of the 16KB BIOS sector$/;"	m	struct:__anon49
BIOSChore	hdr/CFX/Headers/_cfx_internals.h	/^	} BIOSChore;$/;"	t	struct:ResetResume	typeref:struct:ResetResume::__anon53
BIOSCopyright	hdr/CFX/Headers/_cfx_globals.h	/^	char	BIOSCopyright[12];	\/\/ brief copyright notice$/;"	m	struct:__anon49
BIOSGlobalVarTable	hdr/CFX/Headers/_cfx_globals.h	/^	} BIOSGlobalVarTable;$/;"	t	typeref:struct:__anon49
BIOSHandlerAddress	hdr/CFX/Headers/_cfx_cpulevel.h	52;"	d
BIOSLIST	hdr/CFX/Headers/_cfx_linktab.h	52;"	d
BIOSPatchInsert	hdr/CFX/Headers/_cfx_cpulevel.h	51;"	d
BIOSRelease	hdr/CFX/Headers/_cfx_globals.h	/^	uchar	BIOSRelease;		\/\/ fixes, no table changes REL*10+SUB$/;"	m	struct:__anon49
BIOSRelease	hdr/CFX/Headers/cfxpatch.h	/^	uchar	BIOSRelease;		\/\/ 0 for BIOS x.00, 25 for x.25, etc.$/;"	m	struct:__anon2
BIOSReset	hdr/CFX/Headers/_cfx_util.h	/^void	BIOSReset(void)								BIOS_CALL(BIOSReset);$/;"	v
BIOSResetToPBM	hdr/CFX/Headers/_cfx_util.h	/^void	BIOSResetToPBM(void)						BIOS_CALL(BIOSResetToPBM);$/;"	v
BIOSResetToPicoDOS	hdr/CFX/Headers/_cfx_util.h	/^void	BIOSResetToPicoDOS(void)					BIOS_CALL(BIOSResetToPicoDOS);$/;"	v
BIOSVersion	hdr/CFX/Headers/_cfx_globals.h	/^	uchar	BIOSVersion;		\/\/ must be equal or greater than your app$/;"	m	struct:__anon49
BIOSVersion	hdr/CFX/Headers/cfxpatch.h	/^	uchar	BIOSVersion;		\/\/ 1 for BIOS 1.xx, 2 for 2.xx, etc.$/;"	m	struct:__anon2
BIOS_BUILD	hdr/CFX/Headers/_cfx_internals.h	50;"	d
BIOS_CALL	hdr/CFX/Headers/_cfx_linktab.h	55;"	d
BIOS_DEF_BEGIN	hdr/CFX/Headers/_cfx_linktab.h	48;"	d
BIOS_DEF_END	hdr/CFX/Headers/_cfx_linktab.h	49;"	d
BIOS_FDTAB_BASE	hdr/CFX/Headers/_cfx_memmap.h	72;"	d
BIOS_FDTAB_BASE	hdr/CFX/Headers/_cfx_memmap.h	85;"	d
BIOS_FDTAB_SIZE	hdr/CFX/Headers/_cfx_memmap.h	73;"	d
BIOS_FDTAB_SIZE	hdr/CFX/Headers/_cfx_memmap.h	86;"	d
BIOS_GLOB_BASE	hdr/CFX/Headers/_cfx_globals.h	/^const BIOSGlobalVarTable	BIOSGVT : BIOS_GLOB_BASE;	\/\/ BIOS Global Variables$/;"	v
BIOS_GLOB_BASE	hdr/CFX/Headers/_cfx_memmap.h	61;"	d
BIOS_GLOB_BASE	hdr/CFX/Headers/_cfx_memmap.h	79;"	d
BIOS_GLOB_SIZE	hdr/CFX/Headers/_cfx_memmap.h	62;"	d
BIOS_GLOB_SIZE	hdr/CFX/Headers/_cfx_memmap.h	80;"	d
BIOS_PATCH	hdr/CFX/Headers/_cfx_internals.h	51;"	d
BIOS_RELEASE	hdr/CFX/Headers/_cfx_internals.h	48;"	d
BIOS_SUBREL	hdr/CFX/Headers/_cfx_internals.h	49;"	d
BIOS_VBR_BASE	hdr/CFX/Headers/_cfx_memmap.h	55;"	d
BIOS_VBR_SIZE	hdr/CFX/Headers/_cfx_memmap.h	56;"	d
BIOS_VERSION	hdr/CFX/Headers/_cfx_internals.h	47;"	d
BIUMCR	hdr/CFX/Headers/mc68338.h	/^	vushort		BIUMCR;		\/\/ $YFF400 BIUSM Module Configuration Register $/;"	m	struct:__anon22
BIUTBR	hdr/CFX/Headers/mc68338.h	/^	vushort		BIUTBR;		\/\/ $YFF404 BIUSM Time Base Register $/;"	m	struct:__anon22
BIUTEST	hdr/CFX/Headers/mc68338.h	/^	vushort		BIUTEST;	\/\/ $YFF402 BIUSM Test Register $/;"	m	struct:__anon22
BMT16	hdr/CFX/Headers/_cfx_internals.h	/^			BMT64 = 0x00,	BMT32 = 0x01,	BMT16 = 0x02,	BMT8 = 0x03 };$/;"	e	enum:ResetResume::__anon51
BMT32	hdr/CFX/Headers/_cfx_internals.h	/^			BMT64 = 0x00,	BMT32 = 0x01,	BMT16 = 0x02,	BMT8 = 0x03 };$/;"	e	enum:ResetResume::__anon51
BMT64	hdr/CFX/Headers/_cfx_internals.h	/^			BMT64 = 0x00,	BMT32 = 0x01,	BMT16 = 0x02,	BMT8 = 0x03 };$/;"	e	enum:ResetResume::__anon51
BMT8	hdr/CFX/Headers/_cfx_internals.h	/^			BMT64 = 0x00,	BMT32 = 0x01,	BMT16 = 0x02,	BMT8 = 0x03 };$/;"	e	enum:ResetResume::__anon51
BOOL	hdr/CFX/Headers/_cfx_types.h	84;"	d
BOOL	hdr/CFX/Headers/mxcfxstd.h	102;"	d
BPB_BkBootSec	hdr/CFX/Headers/dosdrive.h	/^	ushort		BPB_BkBootSec;			\/\/ sector number for backup boot$/;"	m	struct:BS
BPB_BkBootSec	hdr/CFX/Headers/dosdrive.h	/^	ushort		BPB_BkBootSec;			\/\/ sector number for backup boot$/;"	m	struct:BS32
BPB_BytsPerSec	hdr/CFX/Headers/dosdrive.h	/^	ushort		BPB_BytsPerSec;			\/\/ bytes per sector$/;"	m	struct:DPBPB
BPB_BytsPerSec	hdr/CFX/Headers/dosdrive.h	/^	ushort		BPB_BytsPerSec;			\/\/ bytes per sector$/;"	m	struct:DWBPB
BPB_ExtFlags	hdr/CFX/Headers/dosdrive.h	/^	ushort		BPB_ExtFlags;			\/\/ bits 0-3 active fat$/;"	m	struct:BS
BPB_ExtFlags	hdr/CFX/Headers/dosdrive.h	/^	ushort		BPB_ExtFlags;			\/\/ bits 0-3 active fat$/;"	m	struct:BS32
BPB_FATSz16	hdr/CFX/Headers/dosdrive.h	/^	ushort		BPB_FATSz16;			\/\/ sectors per FAT$/;"	m	struct:DPBPB
BPB_FATSz16	hdr/CFX/Headers/dosdrive.h	/^	ushort		BPB_FATSz16;			\/\/ sectors per FAT$/;"	m	struct:DWBPB
BPB_FATSz32	hdr/CFX/Headers/dosdrive.h	/^	ulong		BPB_FATSz32;			\/\/ sectors per fat$/;"	m	struct:BS
BPB_FATSz32	hdr/CFX/Headers/dosdrive.h	/^	ulong		BPB_FATSz32;			\/\/ sectors per fat$/;"	m	struct:BS32
BPB_FSInfo	hdr/CFX/Headers/dosdrive.h	/^	ushort		BPB_FSInfo;				\/\/ sector number for info sector$/;"	m	struct:BS
BPB_FSInfo	hdr/CFX/Headers/dosdrive.h	/^	ushort		BPB_FSInfo;				\/\/ sector number for info sector$/;"	m	struct:BS32
BPB_FSVer	hdr/CFX/Headers/dosdrive.h	/^	ushort		BPB_FSVer;				\/\/ 0:0 is current FAT32 version$/;"	m	struct:BS
BPB_FSVer	hdr/CFX/Headers/dosdrive.h	/^	ushort		BPB_FSVer;				\/\/ 0:0 is current FAT32 version$/;"	m	struct:BS32
BPB_HiddSec	hdr/CFX/Headers/dosdrive.h	/^	ulong		BPB_HiddSec;			\/\/ hidden sectors$/;"	m	struct:DPBPB
BPB_HiddSec	hdr/CFX/Headers/dosdrive.h	/^	ulong		BPB_HiddSec;			\/\/ hidden sectors$/;"	m	struct:DWBPB
BPB_Media	hdr/CFX/Headers/dosdrive.h	/^	uchar		BPB_Media;				\/\/ media descriptor$/;"	m	struct:DPBPB
BPB_Media	hdr/CFX/Headers/dosdrive.h	/^	uchar		BPB_Media;				\/\/ media descriptor$/;"	m	struct:DWBPB
BPB_NumFATs	hdr/CFX/Headers/dosdrive.h	/^	uchar		BPB_NumFATs;			\/\/ number of FATs$/;"	m	struct:DPBPB
BPB_NumFATs	hdr/CFX/Headers/dosdrive.h	/^	uchar		BPB_NumFATs;			\/\/ number of FATs$/;"	m	struct:DWBPB
BPB_NumHeads	hdr/CFX/Headers/dosdrive.h	/^	ushort		BPB_NumHeads;			\/\/ number of heads$/;"	m	struct:DPBPB
BPB_NumHeads	hdr/CFX/Headers/dosdrive.h	/^	ushort		BPB_NumHeads;			\/\/ number of heads$/;"	m	struct:DWBPB
BPB_Reserved	hdr/CFX/Headers/dosdrive.h	/^	char		BPB_Reserved[12];		\/\/ zeros$/;"	m	struct:BS
BPB_Reserved	hdr/CFX/Headers/dosdrive.h	/^	char		BPB_Reserved[12];		\/\/ zeros$/;"	m	struct:BS32
BPB_RootClus	hdr/CFX/Headers/dosdrive.h	/^	ulong		BPB_RootClus;			\/\/ root directory first cluster$/;"	m	struct:BS
BPB_RootClus	hdr/CFX/Headers/dosdrive.h	/^	ulong		BPB_RootClus;			\/\/ root directory first cluster$/;"	m	struct:BS32
BPB_RootEntCnt	hdr/CFX/Headers/dosdrive.h	/^	ushort		BPB_RootEntCnt;			\/\/ root directory entries$/;"	m	struct:DPBPB
BPB_RootEntCnt	hdr/CFX/Headers/dosdrive.h	/^	ushort		BPB_RootEntCnt;			\/\/ root directory entries$/;"	m	struct:DWBPB
BPB_RsvdSecCnt	hdr/CFX/Headers/dosdrive.h	/^	ushort		BPB_RsvdSecCnt;			\/\/ reserved sectors$/;"	m	struct:DPBPB
BPB_RsvdSecCnt	hdr/CFX/Headers/dosdrive.h	/^	ushort		BPB_RsvdSecCnt;			\/\/ reserved sectors$/;"	m	struct:DWBPB
BPB_SecPerClus	hdr/CFX/Headers/dosdrive.h	/^	uchar		BPB_SecPerClus;			\/\/ sectors per cluster$/;"	m	struct:DPBPB
BPB_SecPerClus	hdr/CFX/Headers/dosdrive.h	/^	uchar		BPB_SecPerClus;			\/\/ sectors per cluster$/;"	m	struct:DWBPB
BPB_SecPerTrk	hdr/CFX/Headers/dosdrive.h	/^	ushort		BPB_SecPerTrk;			\/\/ sectors per track$/;"	m	struct:DPBPB
BPB_SecPerTrk	hdr/CFX/Headers/dosdrive.h	/^	ushort		BPB_SecPerTrk;			\/\/ sectors per track$/;"	m	struct:DWBPB
BPB_TotSec16	hdr/CFX/Headers/dosdrive.h	/^	ushort		BPB_TotSec16;			\/\/ total sectors$/;"	m	struct:DPBPB
BPB_TotSec16	hdr/CFX/Headers/dosdrive.h	/^	ushort		BPB_TotSec16;			\/\/ total sectors$/;"	m	struct:DWBPB
BPB_TotSec32	hdr/CFX/Headers/dosdrive.h	/^	ulong		BPB_TotSec32;			\/\/ extended total sectors$/;"	m	struct:DPBPB
BPB_TotSec32	hdr/CFX/Headers/dosdrive.h	/^	ulong		BPB_TotSec32;			\/\/ extended total sectors$/;"	m	struct:DWBPB
BS	hdr/CFX/Headers/dosdrive.h	/^	}	BS;$/;"	t	typeref:struct:BS
BS	hdr/CFX/Headers/dosdrive.h	/^typedef struct BS	\/\/ unpacked version of BS32 for both FAT16\/FAT32$/;"	s
BS16	hdr/CFX/Headers/dosdrive.h	/^	}	BS16;$/;"	t	typeref:struct:BS16
BS16	hdr/CFX/Headers/dosdrive.h	/^typedef struct BS16$/;"	s
BS32	hdr/CFX/Headers/dosdrive.h	/^	}	BS32;$/;"	t	typeref:struct:BS32
BS32	hdr/CFX/Headers/dosdrive.h	/^typedef struct BS32$/;"	s
BSD_ERRORS	hdr/CFX/Headers/_cfx_errors.h	118;"	d
BSD_FIRST_DEV	hdr/CFX/Headers/_cfx_internals.h	1050;"	d
BSD_LAST_DEV	hdr/CFX/Headers/_cfx_internals.h	1051;"	d
BSD_MAX_DEVS	hdr/CFX/Headers/_cfx_internals.h	1052;"	d
BS_BootSig	hdr/CFX/Headers/dosdrive.h	/^	uchar		BS_BootSig;				\/\/ 0x29$/;"	m	struct:BS
BS_BootSig	hdr/CFX/Headers/dosdrive.h	/^	uchar		BS_BootSig;				\/\/ 0x29$/;"	m	struct:BS16
BS_BootSig	hdr/CFX/Headers/dosdrive.h	/^	uchar		BS_BootSig;				\/\/ 0x29$/;"	m	struct:BS32
BS_DrvNum	hdr/CFX/Headers/dosdrive.h	/^	uchar		BS_DrvNum;				\/\/ 0x80$/;"	m	struct:BS
BS_DrvNum	hdr/CFX/Headers/dosdrive.h	/^	uchar		BS_DrvNum;				\/\/ 0x80$/;"	m	struct:BS16
BS_DrvNum	hdr/CFX/Headers/dosdrive.h	/^	uchar		BS_DrvNum;				\/\/ 0x80$/;"	m	struct:BS32
BS_FilSysType	hdr/CFX/Headers/dosdrive.h	/^	char		BS_FilSysType[8];		\/\/ FAT type$/;"	m	struct:BS
BS_FilSysType	hdr/CFX/Headers/dosdrive.h	/^	char		BS_FilSysType[8];		\/\/ FAT type$/;"	m	struct:BS16
BS_FilSysType	hdr/CFX/Headers/dosdrive.h	/^	char		BS_FilSysType[8];		\/\/ FAT type$/;"	m	struct:BS32
BS_Reserved1	hdr/CFX/Headers/dosdrive.h	/^	uchar		BS_Reserved1;			\/\/ 0x00$/;"	m	struct:BS
BS_Reserved1	hdr/CFX/Headers/dosdrive.h	/^	uchar		BS_Reserved1;			\/\/ 0x00$/;"	m	struct:BS16
BS_Reserved1	hdr/CFX/Headers/dosdrive.h	/^	uchar		BS_Reserved1;			\/\/ 0x00$/;"	m	struct:BS32
BS_VolID	hdr/CFX/Headers/dosdrive.h	/^	ulong		BS_VolID;				\/\/ ID code$/;"	m	struct:BS
BS_VolID	hdr/CFX/Headers/dosdrive.h	/^	ulong		BS_VolID;				\/\/ ID code$/;"	m	struct:BS16
BS_VolID	hdr/CFX/Headers/dosdrive.h	/^	ulong		BS_VolID;				\/\/ ID code$/;"	m	struct:BS32
BS_VolLab	hdr/CFX/Headers/dosdrive.h	/^	char		BS_VolLab[11];			\/\/ volume label$/;"	m	struct:BS
BS_VolLab	hdr/CFX/Headers/dosdrive.h	/^	char		BS_VolLab[11];			\/\/ volume label$/;"	m	struct:BS16
BS_VolLab	hdr/CFX/Headers/dosdrive.h	/^	char		BS_VolLab[11];			\/\/ volume label$/;"	m	struct:BS32
BUFSIZ	hdr/CPU32/Headers/StdC/stdio.h	24;"	d
BW	hdr/CFX/Headers/_cfx_util.h	/^typedef	union	{ uchar	b[2]; ushort w; } BW, *pBW;$/;"	t	typeref:union:__anon98
BWL	hdr/CFX/Headers/_cfx_util.h	/^typedef	union	{ uchar	b[4]; ushort w[2]; ulong l; } BWL, *pBWL;$/;"	t	typeref:union:__anon99
BackROMCmd	hdr/CFX/Headers/_cfx_console.h	/^char	*BackROMCmd(CmdInfoPtr cip)					PICO_CALL(BackROMCmd);$/;"	v
BaudCmd	hdr/CFX/Headers/_cfx_console.h	/^char	*BaudCmd(CmdInfoPtr cip)					PICO_CALL(BaudCmd);$/;"	v
BootCmd	hdr/CFX/Headers/_cfx_console.h	/^char	*BootCmd(CmdInfoPtr cip)					PICO_CALL(BootCmd);$/;"	v
BreakCmd	hdr/CFX/Headers/_cfx_console.h	/^char	*BreakCmd(CmdInfoPtr cip)					PICO_CALL(BreakCmd);$/;"	v
BusMonEnable	hdr/CFX/Headers/_cfx_internals.h	/^enum	{	HaltMonEnable = 0x08,		BusMonEnable = 0x04,	$/;"	e	enum:ResetResume::__anon51
Byte2Swap	hdr/CFX/Headers/_cfx_util.h	53;"	d
Byte4Swap	hdr/CFX/Headers/_cfx_util.h	55;"	d
ByteBit	hdr/CFX/Headers/_cfx_internals.h	/^	} ByteBit, *BBP;$/;"	t	struct:ResetResume	typeref:struct:ResetResume::__anon61
ByteBitClear	hdr/CFX/Headers/_cfx_internals.h	817;"	d
ByteBitSet	hdr/CFX/Headers/_cfx_internals.h	816;"	d
ByteBitTest	hdr/CFX/Headers/_cfx_internals.h	818;"	d
CF1	hdr/CFX/Headers/mxcfxstd.h	88;"	d
CF1SerNum	hdr/CFX/Headers/_cfx_internals.h	65;"	d
CF1_CLOCKS_PER_SEC	hdr/CFX/Headers/_cfx_time.h	104;"	d
CF2	hdr/CFX/Headers/mxcfxstd.h	91;"	d
CFCardDetect	hdr/CFX/Headers/_cfx_drives.h	/^bool	CFCardDetect(void)							BIOS_CALL(CFCardDetect);$/;"	v
CFDvrRdyTMTms	hdr/CFX/Headers/_cfx_globals.h	/^	ushort	CFDvrRdyTMTms;		\/\/ CF driver ready timeout in ms$/;"	m	struct:__anon49
CFDvrRdyTMTmsDefault	hdr/CFX/Headers/_cfx_internals.h	179;"	d
CFDvrRdyTMTmsIBMuD	hdr/CFX/Headers/_cfx_internals.h	180;"	d
CFEnable	hdr/CFX/Headers/_cfx_drives.h	/^bool	CFEnable(bool on)							BIOS_CALL(CFEnable);$/;"	v
CFGetDriver	hdr/CFX/Headers/_cfx_drives.h	/^ATADvr	CFGetDriver(void)							BIOS_CALL(CFGetDriver);$/;"	v
CFMaster	hdr/CFX/Headers/_cfx_internals.h	/^enum { NoEraXlateFlag = 0x01, CFMaster = 0x10 };	\/\/ > 228r1$/;"	e	enum:ResetResume::__anon54
CFSR0	hdr/CFX/Headers/mc68332.h	/^	vushort		CFSR0;		\/\/ $YFFE0C	Channel Function Selection Register 0 [S]$/;"	m	struct:__anon18
CFSR1	hdr/CFX/Headers/mc68332.h	/^	vushort		CFSR1;		\/\/ $YFFE0E	Channel Function Selection Register 1 [S]$/;"	m	struct:__anon18
CFSR2	hdr/CFX/Headers/mc68332.h	/^	vushort		CFSR2;		\/\/ $YFFE10	Channel Function Selection Register 2 [S]$/;"	m	struct:__anon18
CFSR3	hdr/CFX/Headers/mc68332.h	/^	vushort		CFSR3;		\/\/ $YFFE12	Channel Function Selection Register 3 [S]$/;"	m	struct:__anon18
CFX	hdr/CFX/(CF1)/cfx.h	71;"	d
CFX	hdr/CFX/(CF2)/cfx.h	72;"	d
CFX	hdr/CFX/Headers/cfx.h	72;"	d
CFX_CLOCKS_PER_SEC	hdr/CFX/Headers/_cfx_time.h	105;"	d
CFX_CLOCKS_PER_SEC	hdr/CFX/Headers/_cfx_time.h	107;"	d
CF_DRIVER_NAME	hdr/CFX/Headers/_cfx_drives.h	73;"	d
CF_ERRORS	hdr/CFX/Headers/_cfx_errors.h	92;"	d
CFxAD	hdr/CFX/Headers/cfxad.h	/^	}	CFxAD;$/;"	t	typeref:struct:__anon1
CFxADLock	hdr/CFX/Headers/cfxad.h	73;"	d
CFxADPowerDown	hdr/CFX/Headers/cfxad.h	76;"	d
CFxADQueueToArray	hdr/CFX/Headers/cfxad.h	81;"	d
CFxADRawToVolts	hdr/CFX/Headers/cfxad.h	80;"	d
CFxADRepeat	hdr/CFX/Headers/cfxad.h	75;"	d
CFxADSample	hdr/CFX/Headers/cfxad.h	77;"	d
CFxADSampleBlock	hdr/CFX/Headers/cfxad.h	78;"	d
CFxADUnlock	hdr/CFX/Headers/cfxad.h	74;"	d
CFxPatchInstall	hdr/CFX/Headers/cfxpatch.h	49;"	d
CFxPatchLevel	hdr/CFX/Headers/cfxpatch.h	51;"	d
CFxPatchesList	hdr/CFX/Headers/cfxpatch.h	50;"	d
CFxSerNum	hdr/CFX/Headers/_cfx_globals.h	/^	long	CFxSerNum;			\/\/ serial number of this CF1$/;"	m	struct:__anon49
CHAR_BIT	hdr/CPU32/Headers/StdC/limits.h	8;"	d
CHAR_MAX	hdr/CPU32/Headers/StdC/limits.h	9;"	d
CHAR_MIN	hdr/CPU32/Headers/StdC/limits.h	10;"	d
CHDirCmd	hdr/CFX/Headers/_cfx_console.h	/^char	*CHDirCmd(CmdInfoPtr cip)					PICO_CALL(CHDirCmd);$/;"	v
CIER	hdr/CFX/Headers/mc68332.h	/^	vushort		CIER;		\/\/ $YFFE0A	Channel Interrupt Enable Register [S]$/;"	m	struct:__anon18
CISR	hdr/CFX/Headers/mc68332.h	/^	vushort		CISR;		\/\/ $YFFE20	Channel Interrupt Status Register [S]$/;"	m	struct:__anon18
CLEAR_OBJECT	hdr/CFX/Headers/_cfx_internals.h	810;"	d
CLOCKS_PER_SEC	hdr/CPU32/Headers/StdC/time.h	11;"	d
CLSCmd	hdr/CFX/Headers/_cfx_console.h	/^char	*CLSCmd(CmdInfoPtr cip)						PICO_CALL(CLSCmd);$/;"	v
CMDLINELEN	hdr/CFX/Headers/_cfx_console.h	206;"	d
CMDMAXARGS	hdr/CFX/Headers/_cfx_console.h	207;"	d
CMD_ARG_DELIMS	hdr/CFX/Headers/_cfx_console.h	233;"	d
CMD_BREAK	hdr/CFX/Headers/_cfx_console.h	232;"	d
CMD_ERRORS	hdr/CFX/Headers/_cfx_errors.h	96;"	d
CMD_LINE_DELIM	hdr/CFX/Headers/_cfx_console.h	235;"	d
CMD_OPT_DELIM	hdr/CFX/Headers/_cfx_console.h	234;"	d
CMD_RANGE_CHAR	hdr/CFX/Headers/_cfx_console.h	237;"	d
CMD_REP_CHAR	hdr/CFX/Headers/_cfx_console.h	236;"	d
CPCR	hdr/CFX/Headers/mc68338.h	/^	vushort		CPCR;		\/\/ $YFF408 CPSM Control Register $/;"	m	struct:__anon22
CPR0	hdr/CFX/Headers/mc68332.h	/^	vushort		CPR0;		\/\/ $YFFE1C	Channel Priority Register 0 [S]$/;"	m	struct:__anon18
CPR1	hdr/CFX/Headers/mc68332.h	/^	vushort		CPR1;		\/\/ $YFFE1E	Channel Priority Register 1 [S]$/;"	m	struct:__anon18
CPSMDivisor	hdr/CFX/Headers/_cf1_ctm.h	/^typedef enum	{	DivideBy2,	DivideBy3	} CPSMDivisor;$/;"	t	typeref:enum:__anon27
CPTR	hdr/CFX/Headers/mc68338.h	/^	vushort		CPTR;		\/\/ $YFF40A CPSM Test Register $/;"	m	struct:__anon22
CPUStop	hdr/CFX/Headers/_cfx_power.h	/^enum { 	FullStop = 0x00,	FastStop = 0x02,		CPUStop = 0x42 };$/;"	e	enum:__anon76
CPUTestAndSet	hdr/CFX/Headers/_cfx_cpulevel.h	73;"	d
CRC16	hdr/CFX/Headers/_cfx_util.h	/^ushort	CRC16(uchar value, ushort runningCRC)		BIOS_CALL(CRC16);$/;"	v
CRC16Block	hdr/CFX/Headers/_cfx_util.h	/^				ulong len, ushort runningCRC)		BIOS_CALL(CRC16Block);$/;"	v
CRC32	hdr/CFX/Headers/_cfx_util.h	/^ulong	CRC32(uchar value, ulong runningCRC)		BIOS_CALL(CRC32);$/;"	v
CRC32Block	hdr/CFX/Headers/_cfx_util.h	/^				ulong len, ulong runningCRC)		BIOS_CALL(CRC32Block);$/;"	v
CRCCmd	hdr/CFX/Headers/_cfx_console.h	/^char	*CRCCmd(CmdInfoPtr cip)						PICO_CALL(CRCCmd);$/;"	v
CREG	hdr/CFX/Headers/mc68332.h	/^	vushort		CREG;		\/\/ $YFFA38 Test Module Control [S]$/;"	m	struct:__anon16
CREG	hdr/CFX/Headers/mc68338.h	/^	vushort		CREG;		\/\/ $YFFA38 Test Module Control [S]$/;"	m	struct:__anon20
CS10GetWaits	hdr/CFX/Headers/_cfx_expand.h	/^short	CS10GetWaits(void)							BIOS_CALL(CS10GetWaits);$/;"	v
CS10Options	hdr/CFX/Headers/_cfx_expand.h	/^				bool dsSync, short waits)			BIOS_CALL(CS10Options);$/;"	v
CS10Setup	hdr/CFX/Headers/_cfx_expand.h	/^				bool is16bit)					 	BIOS_CALL(CS10Setup);$/;"	v
CS10isEClock	hdr/CFX/Headers/_cfx_expand.h	/^void	CS10isEClock(bool isECLK)					BIOS_CALL(CS10isEClock);$/;"	v
CS8GetWaits	hdr/CFX/Headers/_cfx_expand.h	/^short	CS8GetWaits(void)							BIOS_CALL(CS8GetWaits);$/;"	v
CS8Options	hdr/CFX/Headers/_cfx_expand.h	/^				bool dsSync, short waits)			BIOS_CALL(CS8Options);$/;"	v
CS8Setup	hdr/CFX/Headers/_cfx_expand.h	/^				bool is16bit)				 		BIOS_CALL(CS8Setup);$/;"	v
CSBAR0	hdr/CFX/Headers/mc68332.h	/^	vushort		CSBAR0;		\/\/ $YFFA4C Chip-Select Base 0 [S]$/;"	m	struct:__anon16
CSBAR0	hdr/CFX/Headers/mc68338.h	/^	vushort		CSBAR0;		\/\/ $YFFA4C Chip-Select Base 0 [S]$/;"	m	struct:__anon20
CSBAR1	hdr/CFX/Headers/mc68332.h	/^	vushort		CSBAR1;		\/\/ $YFFA50 Chip-Select Base 1 [S]$/;"	m	struct:__anon16
CSBAR1	hdr/CFX/Headers/mc68338.h	/^	vushort		CSBAR1;		\/\/ $YFFA50 Chip-Select Base 1 [S]$/;"	m	struct:__anon20
CSBAR10	hdr/CFX/Headers/mc68332.h	/^	vushort		CSBAR10;	\/\/ $YFFA74 Chip-Select Base 10 [S]$/;"	m	struct:__anon16
CSBAR10	hdr/CFX/Headers/mc68338.h	/^	vushort		CSBAR10;	\/\/ $YFFA74 Chip-Select Base 10 [S]$/;"	m	struct:__anon20
CSBAR2	hdr/CFX/Headers/mc68332.h	/^	vushort		CSBAR2;		\/\/ $YFFA54 Chip-Select Base 2 [S]$/;"	m	struct:__anon16
CSBAR2	hdr/CFX/Headers/mc68338.h	/^	vushort		CSBAR2;		\/\/ $YFFA54 Chip-Select Base 2 [S]$/;"	m	struct:__anon20
CSBAR3	hdr/CFX/Headers/mc68332.h	/^	vushort		CSBAR3;		\/\/ $YFFA58 Chip-Select Base 3 [S]$/;"	m	struct:__anon16
CSBAR3	hdr/CFX/Headers/mc68338.h	/^	vushort		CSBAR3;		\/\/ $YFFA58 Chip-Select Base 3 [S]$/;"	m	struct:__anon20
CSBAR4	hdr/CFX/Headers/mc68332.h	/^	vushort		CSBAR4;		\/\/ $YFFA5C Chip-Select Base 4 [S]$/;"	m	struct:__anon16
CSBAR4	hdr/CFX/Headers/mc68338.h	/^	vushort		CSBAR4;		\/\/ $YFFA5C Chip-Select Base 4 [S]$/;"	m	struct:__anon20
CSBAR5	hdr/CFX/Headers/mc68332.h	/^	vushort		CSBAR5;		\/\/ $YFFA60 Chip-Select Base 5 [S]$/;"	m	struct:__anon16
CSBAR5	hdr/CFX/Headers/mc68338.h	/^	vushort		CSBAR5;		\/\/ $YFFA60 Chip-Select Base 5 [S]$/;"	m	struct:__anon20
CSBAR6	hdr/CFX/Headers/mc68332.h	/^	vushort		CSBAR6;		\/\/ $YFFA64 Chip-Select Base 6 [S]$/;"	m	struct:__anon16
CSBAR6	hdr/CFX/Headers/mc68338.h	/^	vushort		CSBAR6;		\/\/ $YFFA64 Chip-Select Base 6 [S]$/;"	m	struct:__anon20
CSBAR7	hdr/CFX/Headers/mc68332.h	/^	vushort		CSBAR7;		\/\/ $YFFA68 Chip-Select Base 7 [S]$/;"	m	struct:__anon16
CSBAR7	hdr/CFX/Headers/mc68338.h	/^	vushort		CSBAR7;		\/\/ $YFFA68 Chip-Select Base 7 [S]$/;"	m	struct:__anon20
CSBAR8	hdr/CFX/Headers/mc68332.h	/^	vushort		CSBAR8;		\/\/ $YFFA6C Chip-Select Base 8 [S]$/;"	m	struct:__anon16
CSBAR8	hdr/CFX/Headers/mc68338.h	/^	vushort		CSBAR8;		\/\/ $YFFA6C Chip-Select Base 8 [S]$/;"	m	struct:__anon20
CSBAR9	hdr/CFX/Headers/mc68332.h	/^	vushort		CSBAR9;		\/\/ $YFFA70 Chip-Select Base 9 [S]$/;"	m	struct:__anon16
CSBAR9	hdr/CFX/Headers/mc68338.h	/^	vushort		CSBAR9;		\/\/ $YFFA70 Chip-Select Base 9 [S]$/;"	m	struct:__anon20
CSBARBT	hdr/CFX/Headers/mc68332.h	/^	vushort		CSBARBT;	\/\/ $YFFA48 Chip-Select Base Boot [S]$/;"	m	struct:__anon16
CSBARBT	hdr/CFX/Headers/mc68338.h	/^	vushort		CSBARBT;	\/\/ $YFFA48 Chip-Select Base Boot [S]$/;"	m	struct:__anon20
CSDontChange	hdr/CFX/Headers/_cfx_expand.h	51;"	d
CSFastTermWaits	hdr/CFX/Headers/_cfx_expand.h	52;"	d
CSOR0	hdr/CFX/Headers/mc68332.h	/^	vushort		CSOR0;		\/\/ $YFFA4E Chip-Select Option 0 [S]$/;"	m	struct:__anon16
CSOR0	hdr/CFX/Headers/mc68338.h	/^	vushort		CSOR0;		\/\/ $YFFA4E Chip-Select Option 0 [S]$/;"	m	struct:__anon20
CSOR1	hdr/CFX/Headers/mc68332.h	/^	vushort		CSOR1;		\/\/ $YFFA52 Chip-Select Option 1 [S]$/;"	m	struct:__anon16
CSOR1	hdr/CFX/Headers/mc68338.h	/^	vushort		CSOR1;		\/\/ $YFFA52 Chip-Select Option 1 [S]$/;"	m	struct:__anon20
CSOR10	hdr/CFX/Headers/mc68332.h	/^	vushort		CSOR10;		\/\/ $YFFA76 Chip-Select Option 10 [S]$/;"	m	struct:__anon16
CSOR10	hdr/CFX/Headers/mc68338.h	/^	vushort		CSOR10;		\/\/ $YFFA76 Chip-Select Option 10 [S]$/;"	m	struct:__anon20
CSOR2	hdr/CFX/Headers/mc68332.h	/^	vushort		CSOR2;		\/\/ $YFFA56 Chip-Select Option 2 [S]$/;"	m	struct:__anon16
CSOR2	hdr/CFX/Headers/mc68338.h	/^	vushort		CSOR2;		\/\/ $YFFA56 Chip-Select Option 2 [S]$/;"	m	struct:__anon20
CSOR3	hdr/CFX/Headers/mc68332.h	/^	vushort		CSOR3;		\/\/ $YFFA5A Chip-Select Option 3 [S]$/;"	m	struct:__anon16
CSOR3	hdr/CFX/Headers/mc68338.h	/^	vushort		CSOR3;		\/\/ $YFFA5A Chip-Select Option 3 [S]$/;"	m	struct:__anon20
CSOR4	hdr/CFX/Headers/mc68332.h	/^	vushort		CSOR4;		\/\/ $YFFA5E Chip-Select Option 4 [S]$/;"	m	struct:__anon16
CSOR4	hdr/CFX/Headers/mc68338.h	/^	vushort		CSOR4;		\/\/ $YFFA5E Chip-Select Option 4 [S]$/;"	m	struct:__anon20
CSOR5	hdr/CFX/Headers/mc68332.h	/^	vushort		CSOR5;		\/\/ $YFFA62 Chip-Select Option 5 [S]$/;"	m	struct:__anon16
CSOR5	hdr/CFX/Headers/mc68338.h	/^	vushort		CSOR5;		\/\/ $YFFA62 Chip-Select Option 5 [S]$/;"	m	struct:__anon20
CSOR6	hdr/CFX/Headers/mc68332.h	/^	vushort		CSOR6;		\/\/ $YFFA66 Chip-Select Option 6 [S]$/;"	m	struct:__anon16
CSOR6	hdr/CFX/Headers/mc68338.h	/^	vushort		CSOR6;		\/\/ $YFFA66 Chip-Select Option 6 [S]$/;"	m	struct:__anon20
CSOR7	hdr/CFX/Headers/mc68332.h	/^	vushort		CSOR7;		\/\/ $YFFA6A Chip-Select Option 7 [S]$/;"	m	struct:__anon16
CSOR7	hdr/CFX/Headers/mc68338.h	/^	vushort		CSOR7;		\/\/ $YFFA6A Chip-Select Option 7 [S]$/;"	m	struct:__anon20
CSOR8	hdr/CFX/Headers/mc68332.h	/^	vushort		CSOR8;		\/\/ $YFFA6E Chip-Select Option 8 [S]$/;"	m	struct:__anon16
CSOR8	hdr/CFX/Headers/mc68338.h	/^	vushort		CSOR8;		\/\/ $YFFA6E Chip-Select Option 8 [S]$/;"	m	struct:__anon20
CSOR9	hdr/CFX/Headers/mc68332.h	/^	vushort		CSOR9;		\/\/ $YFFA72 Chip-Select Option 9 [S]$/;"	m	struct:__anon16
CSOR9	hdr/CFX/Headers/mc68338.h	/^	vushort		CSOR9;		\/\/ $YFFA72 Chip-Select Option 9 [S]$/;"	m	struct:__anon20
CSORBT	hdr/CFX/Headers/mc68332.h	/^	vushort		CSORBT;		\/\/ $YFFA4A Chip-Select Option Boot [S]$/;"	m	struct:__anon16
CSORBT	hdr/CFX/Headers/mc68338.h	/^	vushort		CSORBT;		\/\/ $YFFA4A Chip-Select Option Boot [S]$/;"	m	struct:__anon20
CSPAR0	hdr/CFX/Headers/mc68332.h	/^	vushort		CSPAR0;		\/\/ $YFFA44 Chip-Select Pin Assignment [S]$/;"	m	struct:__anon16
CSPAR0	hdr/CFX/Headers/mc68338.h	/^	vushort		CSPAR0;		\/\/ $YFFA44 Chip-Select Pin Assignment [S]$/;"	m	struct:__anon20
CSPAR1	hdr/CFX/Headers/mc68332.h	/^	vushort		CSPAR1;		\/\/ $YFFA46 Chip-Select Pin Assignment [S]$/;"	m	struct:__anon16
CSPAR1	hdr/CFX/Headers/mc68338.h	/^	vushort		CSPAR1;		\/\/ $YFFA46 Chip-Select Pin Assignment [S]$/;"	m	struct:__anon20
CTD10	hdr/CFX/Headers/_cf1_pins.h	/^	, CTD10	= 22	\/\/ Double Action Timer	I\/O		GPIO\/TMR		I?	I?$/;"	e	enum:__anon33
CTD26	hdr/CFX/Headers/_cf1_pins.h	/^	, CTD26	= 37	\/\/ Double Action Timer	I\/O		GPIO\/TMR		I?	I?$/;"	e	enum:__anon33
CTD27	hdr/CFX/Headers/_cf1_pins.h	/^	, CTD27	= 35	\/\/ Double Action Timer	I\/O		GPIO\/TMR		I?	I?$/;"	e	enum:__anon33
CTD28	hdr/CFX/Headers/_cf1_pins.h	/^	, CTD28	= 34	\/\/ Double Action Timer	I\/O		GPIO\/TMR		I?	I?$/;"	e	enum:__anon33
CTD29	hdr/CFX/Headers/_cf1_pins.h	/^	, CTD29	= 33	\/\/ Double Action Timer	I\/O		GPIO\/TMR		I?	I?$/;"	e	enum:__anon33
CTD4	hdr/CFX/Headers/_cf1_pins.h	/^	, CTD4	= 29	\/\/ Double Action Timer	I\/O		GPIO\/TMR		I?	I?$/;"	e	enum:__anon33
CTD5	hdr/CFX/Headers/_cf1_pins.h	/^	, CTD5	= 27	\/\/ Double Action Timer	I\/O		GPIO\/TMR		I?	I?$/;"	e	enum:__anon33
CTD6	hdr/CFX/Headers/_cf1_pins.h	/^	, CTD6	= 26	\/\/ Double Action Timer	I\/O		GPIO\/TMR		I?	I?$/;"	e	enum:__anon33
CTD7	hdr/CFX/Headers/_cf1_pins.h	/^	, CTD7	= 24	\/\/ Double Action Timer	I\/O		GPIO\/TMR		I?	I?$/;"	e	enum:__anon33
CTD8	hdr/CFX/Headers/_cf1_pins.h	/^	, CTD8	= 25	\/\/ Double Action Timer	I\/O		GPIO\/TMR		I?	I?$/;"	e	enum:__anon33
CTD9	hdr/CFX/Headers/_cf1_pins.h	/^	, CTD9	= 23	\/\/ Double Action Timer	I\/O		GPIO\/TMR		I?	I?$/;"	e	enum:__anon33
CTDAClearEventFlag	hdr/CFX/Headers/_cf1_ctm.h	/^void	CTDAClearEventFlag(DoubleActionID sm)		BIOS_CALL(CTDAClearEventFlag);$/;"	v
CTDADataRegA	hdr/CFX/Headers/_cf1_ctm.h	/^vushort	*CTDADataRegA(DoubleActionID sm)			BIOS_CALL(CTDADataRegA);$/;"	v
CTDADataRegB	hdr/CFX/Headers/_cf1_ctm.h	/^vushort	*CTDADataRegB(DoubleActionID sm)			BIOS_CALL(CTDADataRegB);$/;"	v
CTDAEdgeSelect	hdr/CFX/Headers/_cf1_ctm.h	/^			bool posB)								BIOS_CALL(CTDAEdgeSelect);$/;"	v
CTDAFlipFlops	hdr/CFX/Headers/_cf1_ctm.h	/^			bool forceA, bool forceB)				BIOS_CALL(CTDAFlipFlops);$/;"	v
CTDAGetEventFlag	hdr/CFX/Headers/_cf1_ctm.h	/^bool	CTDAGetEventFlag(DoubleActionID sm)			BIOS_CALL(CTDAGetEventFlag);$/;"	v
CTDAGetIDFromPin	hdr/CFX/Headers/_cf1_ctm.h	/^DoubleActionID	CTDAGetIDFromPin(short pin)			BIOS_CALL(CTDAGetIDFromPin);$/;"	v
CTDAGetPinFromID	hdr/CFX/Headers/_cf1_ctm.h	/^short	CTDAGetPinFromID(DoubleActionID sm)			BIOS_CALL(CTDAGetPinFromID);$/;"	v
CTDAInterruptFunction	hdr/CFX/Headers/_cf1_ctm.h	/^		vfptr ifp)									BIOS_CALL(CTDAInterruptFunction);$/;"	v
CTDAInterruptLevel	hdr/CFX/Headers/_cf1_ctm.h	/^			short il, short iarb3)					BIOS_CALL(CTDAInterruptLevel);$/;"	v
CTDAModeSelect	hdr/CFX/Headers/_cf1_ctm.h	/^			DASMModeID mode)						BIOS_CALL(CTDAModeSelect);$/;"	v
CTDASetupOPWM	hdr/CFX/Headers/_cf1_ctm.h	/^			ushort le, ushort te)					BIOS_CALL(CTDASetupOPWM);$/;"	v
CTDATimeBaseBusSelect	hdr/CFX/Headers/_cf1_ctm.h	/^			bool busB)								BIOS_CALL(CTDATimeBaseBusSelect);$/;"	v
CTDAWiredOrMode	hdr/CFX/Headers/_cf1_ctm.h	/^			bool wom)								BIOS_CALL(CTDAWiredOrMode);$/;"	v
CTFRClearCOFFlag	hdr/CFX/Headers/_cf1_ctm.h	/^void	CTFRClearCOFFlag(void)						BIOS_CALL(CTFRClearCOFFlag);$/;"	v
CTFRClockSource	hdr/CFX/Headers/_cf1_ctm.h	/^CounterClockSourceID CTFRClockSource(void)			BIOS_CALL(CTFRClockSource);$/;"	v
CTFRCounterReg	hdr/CFX/Headers/_cf1_ctm.h	/^vushort	*CTFRCounterReg(void)						BIOS_CALL(CTFRCounterReg);$/;"	v
CTFRDriveTimeBase	hdr/CFX/Headers/_cf1_ctm.h	/^void	CTFRDriveTimeBase(bool drive, bool busB)	BIOS_CALL(CTFRDriveTimeBase);$/;"	v
CTFRDrivesBuses	hdr/CFX/Headers/_cf1_ctm.h	/^TimeBaseBusID CTFRDrivesBuses(void)					BIOS_CALL(CTFRDrivesBuses);$/;"	v
CTFRGetCOFFlag	hdr/CFX/Headers/_cf1_ctm.h	/^bool	CTFRGetCOFFlag(void)						BIOS_CALL(CTFRGetCOFFlag);$/;"	v
CTFRInterruptFunction	hdr/CFX/Headers/_cf1_ctm.h	/^void 	CTFRInterruptFunction(vfptr ifp)			BIOS_CALL(CTFRInterruptFunction);$/;"	v
CTFRInterruptLevel	hdr/CFX/Headers/_cf1_ctm.h	/^void	CTFRInterruptLevel(short il, short iarb3)	BIOS_CALL(CTFRInterruptLevel);$/;"	v
CTFRSelectClockSource	hdr/CFX/Headers/_cf1_ctm.h	/^			(CounterClockSourceID clksel)			BIOS_CALL(CTFRSelectClockSource);$/;"	v
CTM31L	hdr/CFX/Headers/_cf1_pins.h	/^	, CTM31L= 36	\/\/ Timer Load\/Clock		In		GPIO\/TMR	1M	I+	I+$/;"	e	enum:__anon33
CTM6_BASE_ADDR	hdr/CFX/Headers/mc68338.h	52;"	d
CTMCClearCOFFlag	hdr/CFX/Headers/_cf1_ctm.h	/^void	CTMCClearCOFFlag(ModulusCounterID sm)		BIOS_CALL(CTMCClearCOFFlag);$/;"	v
CTMCClkSource	hdr/CFX/Headers/_cf1_ctm.h	/^		ModulusCounterID sm)						BIOS_CALL(CTMCClkSource);$/;"	v
CTMCCounterReg	hdr/CFX/Headers/_cf1_ctm.h	/^vushort	*CTMCCounterReg(ModulusCounterID sm)		BIOS_CALL(CTMCCounterReg);$/;"	v
CTMCDriveTimeBase	hdr/CFX/Headers/_cf1_ctm.h	/^			bool drive, bool busB)					BIOS_CALL(CTMCDriveTimeBase);$/;"	v
CTMCDrivesBuses	hdr/CFX/Headers/_cf1_ctm.h	/^		ModulusCounterID sm)						BIOS_CALL(CTMCDrivesBuses);$/;"	v
CTMCFallingEdge	hdr/CFX/Headers/_cf1_ctm.h	/^	CTMCFallingEdge,	CTMCRisingEdge				} CounterClockSourceID;$/;"	e	enum:__anon24
CTMCGetCOFFlag	hdr/CFX/Headers/_cf1_ctm.h	/^bool	CTMCGetCOFFlag(ModulusCounterID sm)			BIOS_CALL(CTMCGetCOFFlag);$/;"	v
CTMCInterruptFunction	hdr/CFX/Headers/_cf1_ctm.h	/^			vfptr ifp)								BIOS_CALL(CTMCInterruptFunction);$/;"	v
CTMCInterruptLevel	hdr/CFX/Headers/_cf1_ctm.h	/^			short il, short iarb3)					BIOS_CALL(CTMCInterruptLevel);$/;"	v
CTMCModulusReg	hdr/CFX/Headers/_cf1_ctm.h	/^vushort	*CTMCModulusReg(ModulusCounterID sm)		BIOS_CALL(CTMCModulusReg);$/;"	v
CTMCRisingEdge	hdr/CFX/Headers/_cf1_ctm.h	/^	CTMCFallingEdge,	CTMCRisingEdge				} CounterClockSourceID;$/;"	e	enum:__anon24
CTMCSelectClockSource	hdr/CFX/Headers/_cf1_ctm.h	/^			CounterClockSourceID clksel)			BIOS_CALL(CTMCSelectClockSource);$/;"	v
CTMCSelectEdges	hdr/CFX/Headers/_cf1_ctm.h	/^			bool pos, bool neg)						BIOS_CALL(CTMCSelectEdges);$/;"	v
CTMGetPrescaleP6	hdr/CFX/Headers/_cf1_ctm.h	/^P6DivisorID	CTMGetPrescaleP6(void)					BIOS_CALL(CTMGetPrescaleP6);$/;"	v
CTMInit	hdr/CFX/Headers/_cf1_ctm.h	/^void	CTMInit(void)								BIOS_CALL(CTMInit);$/;"	v
CTMPrescaleIsDiv3	hdr/CFX/Headers/_cf1_ctm.h	/^bool	CTMPrescaleIsDiv3(void)						BIOS_CALL(CTMPrescaleIsDiv3);$/;"	v
CTMPrescalerRun	hdr/CFX/Headers/_cf1_ctm.h	/^void	CTMPrescalerRun(bool run)					BIOS_CALL(CTMPrescalerRun);$/;"	v
CTMRun	hdr/CFX/Headers/_cf1_ctm.h	/^void	CTMRun(bool run)							BIOS_CALL(CTMRun);$/;"	v
CTMRun	hdr/CFX/Headers/_cfx_tpu.h	/^void	CTMRun(bool run)							BIOS_CALL(CTMRun);$/;"	v
CTMSetPrescale	hdr/CFX/Headers/_cf1_ctm.h	/^		P6DivisorID p6select)						BIOS_CALL(CTMSetPrescale);$/;"	v
CTMTimeBaseReg	hdr/CFX/Headers/_cf1_ctm.h	/^vushort *CTMTimeBaseReg(TimeBaseBusID timebase)		BIOS_CALL(CTMTimeBaseReg);$/;"	v
CTM_DEF_IARB	hdr/CFX/Headers/_cfx_internals.h	450;"	d
CTM_DEF_VECT	hdr/CFX/Headers/_cfx_internals.h	451;"	d
CTM_RTC_DEF_IARB	hdr/CFX/Headers/_cfx_internals.h	453;"	d
CTM_RTC_DEF_IPL	hdr/CFX/Headers/_cfx_internals.h	452;"	d
CTS14A	hdr/CFX/Headers/_cf1_pins.h	/^	, CTS14A= 30	\/\/ Single Action Timer	I\/O		GPIO\/TMR		I?	I?$/;"	e	enum:__anon33
CTS14B	hdr/CFX/Headers/_cf1_pins.h	/^	, CTS14B= 28	\/\/ Single Action Timer	I\/O		GPIO\/TMR		I?	I?$/;"	e	enum:__anon33
CTS18A	hdr/CFX/Headers/_cf1_pins.h	/^	, CTS18A= 31	\/\/ Single Action Timer	I\/O		GPIO\/TMR		I?	I?$/;"	e	enum:__anon33
CTS18B	hdr/CFX/Headers/_cf1_pins.h	/^	, CTS18B= 32	\/\/ Single Action Timer	I\/O		GPIO\/TMR		I?	I?$/;"	e	enum:__anon33
CTSAClearEventFlag	hdr/CFX/Headers/_cf1_ctm.h	/^void	CTSAClearEventFlag(SingleActionID sm)		BIOS_CALL(CTSAClearEventFlag);$/;"	v
CTSADataReg	hdr/CFX/Headers/_cf1_ctm.h	/^vushort	*CTSADataReg(SingleActionID sm)				BIOS_CALL(CTSADataReg);$/;"	v
CTSAEdgeSelect	hdr/CFX/Headers/_cf1_ctm.h	/^void	CTSAEdgeSelect(SingleActionID sm, bool pos)	BIOS_CALL(CTSAEdgeSelect);$/;"	v
CTSAFlipFlop	hdr/CFX/Headers/_cf1_ctm.h	/^void	CTSAFlipFlop(SingleActionID sm, bool force)	BIOS_CALL(CTSAFlipFlop);$/;"	v
CTSAGetEventFlag	hdr/CFX/Headers/_cf1_ctm.h	/^bool	CTSAGetEventFlag(SingleActionID sm)			BIOS_CALL(CTSAGetEventFlag);$/;"	v
CTSAGetIDFromPin	hdr/CFX/Headers/_cf1_ctm.h	/^SingleActionID	CTSAGetIDFromPin(short pin)			BIOS_CALL(CTSAGetIDFromPin);$/;"	v
CTSAGetPinFromID	hdr/CFX/Headers/_cf1_ctm.h	/^short	CTSAGetPinFromID(SingleActionID sm)			BIOS_CALL(CTSAGetPinFromID);$/;"	v
CTSAInterruptEnable	hdr/CFX/Headers/_cf1_ctm.h	/^			bool enable)							BIOS_CALL(CTSAInterruptEnable);$/;"	v
CTSAInterruptFunction	hdr/CFX/Headers/_cf1_ctm.h	/^			vfptr ifp)								BIOS_CALL(CTSAInterruptFunction);$/;"	v
CTSAInterruptLevel	hdr/CFX/Headers/_cf1_ctm.h	/^			short il, short iarb3)					BIOS_CALL(CTSAInterruptLevel);$/;"	v
CTSAModeSelect	hdr/CFX/Headers/_cf1_ctm.h	/^			SASMModeID mode)						BIOS_CALL(CTSAModeSelect);$/;"	v
CTSATimeBaseBusSelect	hdr/CFX/Headers/_cf1_ctm.h	/^			bool busB)								BIOS_CALL(CTSATimeBaseBusSelect);$/;"	v
CUSTOM_SYPCR	hdr/CFX/Headers/mxcfxwdt.h	51;"	d
CallCmd	hdr/CFX/Headers/_cfx_console.h	/^char	*CallCmd(CmdInfoPtr cip)					PICO_CALL(CallCmd);$/;"	v
Callocf	hdr/CFX/Headers/_cfx_types.h	/^typedef void	*Callocf(ulong nmemb, ulong size);$/;"	t
CaptureCmd	hdr/CFX/Headers/_cfx_console.h	/^char	*CaptureCmd(CmdInfoPtr cip)					PICO_CALL(CaptureCmd);$/;"	v
CardChangeCmd	hdr/CFX/Headers/_cfx_console.h	/^char	*CardChangeCmd(CmdInfoPtr cip)				PICO_CALL(CardChangeCmd);$/;"	v
CheckSum16	hdr/CFX/Headers/_cfx_util.h	/^ushort	CheckSum16(uchar value, ushort runningSum)	BIOS_CALL(CheckSum16);$/;"	v
CheckSum16Block	hdr/CFX/Headers/_cfx_util.h	/^				ulong len, ushort runningSum)		BIOS_CALL(CheckSum16Block);$/;"	v
CheckSum32	hdr/CFX/Headers/_cfx_util.h	/^ulong	CheckSum32(uchar value, ulong runningSum)	BIOS_CALL(CheckSum32);$/;"	v
CheckSum32Block	hdr/CFX/Headers/_cfx_util.h	/^				ulong len, ulong runningSum)		BIOS_CALL(CheckSum32Block);$/;"	v
ChkdskCmd	hdr/CFX/Headers/_cfx_console.h	/^char	*ChkdskCmd(CmdInfoPtr cip)					PICO_CALL(ChkdskCmd);$/;"	v
CmdApplicationError	hdr/CFX/Headers/_cfx_errors.h	209;"	d
CmdCancelled	hdr/CFX/Headers/_cfx_errors.h	212;"	d
CmdCantWithPicoZOOM	hdr/CFX/Headers/_cfx_errors.h	210;"	d
CmdCheckOptionChar	hdr/CFX/Headers/_cfx_console.h	/^int		CmdCheckOptionChar(CmdInfoPtr cip, char c)	PICO_CALL(CmdCheckOptionChar);	$/;"	v
CmdConfirm	hdr/CFX/Headers/_cfx_console.h	/^bool	CmdConfirm(char *prompt, char trueReply)	PICO_CALL(CmdConfirm);$/;"	v
CmdDispatch	hdr/CFX/Headers/_cfx_console.h	/^char	*CmdDispatch(CmdInfoPtr cip)				PICO_CALL(CmdDispatch);$/;"	v
CmdDriveError	hdr/CFX/Headers/_cfx_errors.h	208;"	d
CmdDriveNotFound	hdr/CFX/Headers/_cfx_errors.h	207;"	d
CmdErrGeneralFailure	hdr/CFX/Headers/_cfx_errors.h	195;"	d
CmdErrInvalidParam	hdr/CFX/Headers/_cfx_errors.h	199;"	d
CmdErrParse	hdr/CFX/Headers/_cfx_console.h	/^char	*CmdErrParse(char *errfstr, long *errnum)	PICO_CALL(CmdErrParse);$/;"	v
CmdErrPrivViolation	hdr/CFX/Headers/_cfx_errors.h	198;"	d
CmdErrUnknownCommand	hdr/CFX/Headers/_cfx_errors.h	197;"	d
CmdErrWrongParmCount	hdr/CFX/Headers/_cfx_errors.h	196;"	d
CmdErrf	hdr/CFX/Headers/_cfx_console.h	/^			char *str, ...)							PICO_CALL(CmdErrf);$/;"	v
CmdExecf	hdr/CFX/Headers/_cfx_console.h	/^char	*CmdExecf(CmdInfoPtr cip, char *str, ...)	PICO_CALL(CmdExecf);$/;"	v
CmdExpectRange	hdr/CFX/Headers/_cfx_console.h	/^			long *start, long *end)					PICO_CALL(CmdExpectRange);$/;"	v
CmdExpectValue	hdr/CFX/Headers/_cfx_console.h	/^			long *value)							PICO_CALL(CmdExpectValue);$/;"	v
CmdExpectingValue	hdr/CFX/Headers/_cfx_errors.h	201;"	d
CmdExtractAVDosSwitches	hdr/CFX/Headers/_cfx_console.h	/^			char **argv, char *fmt, ...)			PICO_CALL(CmdExtractAVDosSwitches);$/;"	v
CmdExtractArgValues	hdr/CFX/Headers/_cfx_console.h	/^			short first, short last, short radix)	PICO_CALL(CmdExtractArgValues);	$/;"	v
CmdExtractCIDosSwitches	hdr/CFX/Headers/_cfx_console.h	/^			char *fmt, ...)							PICO_CALL(CmdExtractCIDosSwitches);$/;"	v
CmdFileNotFound	hdr/CFX/Headers/_cfx_errors.h	206;"	d
CmdInfo	hdr/CFX/Headers/_cfx_console.h	/^	}	CmdInfo, *CmdInfoPtr;$/;"	t	typeref:struct:CmdInfo
CmdInfo	hdr/CFX/Headers/_cfx_console.h	/^typedef struct CmdInfo$/;"	s
CmdInfoPtr	hdr/CFX/Headers/_cfx_console.h	/^	}	CmdInfo, *CmdInfoPtr;$/;"	t	typeref:struct:CmdInfo
CmdInvalidAddress	hdr/CFX/Headers/_cfx_errors.h	203;"	d
CmdInvalidRange	hdr/CFX/Headers/_cfx_errors.h	202;"	d
CmdIsDigit	hdr/CFX/Headers/_cfx_console.h	/^int		CmdIsDigit(short c, short base, short *val)	PICO_CALL(CmdIsDigit);$/;"	v
CmdIsNumber	hdr/CFX/Headers/_cfx_console.h	/^			short base)								PICO_CALL(CmdIsNumber);	$/;"	v
CmdNotEnoughMemory	hdr/CFX/Headers/_cfx_errors.h	211;"	d
CmdOddAddress	hdr/CFX/Headers/_cfx_errors.h	205;"	d
CmdParam	hdr/CFX/Headers/_cfx_console.h	/^	}	CmdParam, *CmdParamPtr;$/;"	t	typeref:struct:__anon36
CmdParamPtr	hdr/CFX/Headers/_cfx_console.h	/^	}	CmdParam, *CmdParamPtr;$/;"	t	typeref:struct:__anon36
CmdParse	hdr/CFX/Headers/_cfx_console.h	/^char	*CmdParse(CmdInfoPtr cip)					PICO_CALL(CmdParse);$/;"	v
CmdReqParamMissing	hdr/CFX/Headers/_cfx_errors.h	200;"	d
CmdSetDateTime	hdr/CFX/Headers/_cfx_console.h	/^			ulong nowsecs)							PICO_CALL(CmdSetDateTime);$/;"	v
CmdSetNextCmd	hdr/CFX/Headers/_cfx_console.h	/^			char *nextcmd)							PICO_CALL(CmdSetNextCmd);$/;"	v
CmdStdBreak	hdr/CFX/Headers/_cfx_console.h	/^char	*CmdStdBreak(CmdInfoPtr cip)				PICO_CALL(CmdStdBreak);$/;"	v
CmdStdCmdTest	hdr/CFX/Headers/_cfx_console.h	/^char	*CmdStdCmdTest(CmdInfoPtr cip)				PICO_CALL(CmdStdCmdTest);$/;"	v
CmdStdErrText	hdr/CFX/Headers/_cfx_console.h	/^char	*CmdStdErrText(short errID)					PICO_CALL(CmdStdErrText);$/;"	v
CmdStdHelp	hdr/CFX/Headers/_cfx_console.h	/^char	*CmdStdHelp(CmdInfoPtr cip)					PICO_CALL(CmdStdHelp);$/;"	v
CmdStdInteractive	hdr/CFX/Headers/_cfx_console.h	/^char	*CmdStdInteractive(CmdInfoPtr cip)			PICO_CALL(CmdStdInteractive);$/;"	v
CmdStdLPGets	hdr/CFX/Headers/_cfx_console.h	/^short	CmdStdLPGets(char *linebuf, short linelen)	PICO_CALL(CmdStdLPGets);$/;"	v
CmdStdPicoRun	hdr/CFX/Headers/_cfx_console.h	/^char	*CmdStdPicoRun(CmdInfoPtr cip)				PICO_CALL(CmdStdPicoRun);$/;"	v
CmdStdRun	hdr/CFX/Headers/_cfx_console.h	/^char	*CmdStdRun(CmdInfoPtr cip)					PICO_CALL(CmdStdRun);$/;"	v
CmdStdSetup	hdr/CFX/Headers/_cfx_console.h	/^			short (*altgets)(char *, short))		PICO_CALL(CmdStdSetup);$/;"	v
CmdStringUpper	hdr/CFX/Headers/_cfx_console.h	/^char	*CmdStringUpper(char *s)					PICO_CALL(CmdStringUpper);$/;"	v
CmdTable	hdr/CFX/Headers/_cfx_console.h	/^	}	CmdTable, *CmdTablePtr;$/;"	t	typeref:struct:CmdTable
CmdTable	hdr/CFX/Headers/_cfx_console.h	/^typedef struct CmdTable$/;"	s
CmdTablePtr	hdr/CFX/Headers/_cfx_console.h	/^	}	CmdTable, *CmdTablePtr;$/;"	t	typeref:struct:CmdTable
CmdWrongParmType	hdr/CFX/Headers/_cfx_errors.h	204;"	d
CopyCmd	hdr/CFX/Headers/_cfx_console.h	/^char	*CopyCmd(CmdInfoPtr cip)					PICO_CALL(CopyCmd);$/;"	v
CounterClockSourceID	hdr/CFX/Headers/_cf1_ctm.h	/^	CTMCFallingEdge,	CTMCRisingEdge				} CounterClockSourceID;$/;"	t	typeref:enum:__anon24
DA10A	hdr/CFX/Headers/mc68338.h	/^	vushort		DA10A;		\/\/ $YFF452 DASM10 Register A $/;"	m	struct:__anon22
DA10B	hdr/CFX/Headers/mc68338.h	/^	vushort		DA10B;		\/\/ $YFF454 DASM10 Register B $/;"	m	struct:__anon22
DA10SIC	hdr/CFX/Headers/mc68338.h	/^	vushort		DA10SIC;	\/\/ $YFF450 DASM10 Status\/Interrupt\/Control Register $/;"	m	struct:__anon22
DA26A	hdr/CFX/Headers/mc68338.h	/^	vushort		DA26A;		\/\/ $YFF4D2 DASM26 Register A $/;"	m	struct:__anon22
DA26B	hdr/CFX/Headers/mc68338.h	/^	vushort		DA26B;		\/\/ $YFF4D4 DASM26 Register B $/;"	m	struct:__anon22
DA26SIC	hdr/CFX/Headers/mc68338.h	/^	vushort		DA26SIC;	\/\/ $YFF4D0 DASM26 Status\/Interrupt\/Control Register $/;"	m	struct:__anon22
DA27A	hdr/CFX/Headers/mc68338.h	/^	vushort		DA27A;		\/\/ $YFF4DA DASM27 Register A $/;"	m	struct:__anon22
DA27B	hdr/CFX/Headers/mc68338.h	/^	vushort		DA27B;		\/\/ $YFF4DC DASM27 Register B $/;"	m	struct:__anon22
DA27SIC	hdr/CFX/Headers/mc68338.h	/^	vushort		DA27SIC;	\/\/ $YFF4D8 DASM27 Status\/Interrupt\/Control Register $/;"	m	struct:__anon22
DA28A	hdr/CFX/Headers/mc68338.h	/^	vushort		DA28A;		\/\/ $YFF4E2 DASM28 Register A $/;"	m	struct:__anon22
DA28B	hdr/CFX/Headers/mc68338.h	/^	vushort		DA28B;		\/\/ $YFF4E4 DASM28 Register B $/;"	m	struct:__anon22
DA28SIC	hdr/CFX/Headers/mc68338.h	/^	vushort		DA28SIC;	\/\/ $YFF4E0 DASM28 Status\/Interrupt\/Control Register $/;"	m	struct:__anon22
DA29A	hdr/CFX/Headers/mc68338.h	/^	vushort		DA29A;		\/\/ $YFF4EA DASM29 Register A $/;"	m	struct:__anon22
DA29B	hdr/CFX/Headers/mc68338.h	/^	vushort		DA29B;		\/\/ $YFF4EC DASM29 Register B $/;"	m	struct:__anon22
DA29SIC	hdr/CFX/Headers/mc68338.h	/^	vushort		DA29SIC;	\/\/ $YFF4E8 DASM29 Status\/Interrupt\/Control Register $/;"	m	struct:__anon22
DA4A	hdr/CFX/Headers/mc68338.h	/^	vushort		DA4A;		\/\/ $YFF422 DASM4 Register A $/;"	m	struct:__anon22
DA4B	hdr/CFX/Headers/mc68338.h	/^	vushort		DA4B;		\/\/ $YFF424 DASM4 Register B $/;"	m	struct:__anon22
DA4SIC	hdr/CFX/Headers/mc68338.h	/^	vushort		DA4SIC;		\/\/ $YFF420 DASM4 Status\/Interrupt\/Control Register $/;"	m	struct:__anon22
DA5A	hdr/CFX/Headers/mc68338.h	/^	vushort		DA5A;		\/\/ $YFF42A DASM5 Register A $/;"	m	struct:__anon22
DA5B	hdr/CFX/Headers/mc68338.h	/^	vushort		DA5B;		\/\/ $YFF42C DASM5 Register B $/;"	m	struct:__anon22
DA5SIC	hdr/CFX/Headers/mc68338.h	/^	vushort		DA5SIC;		\/\/ $YFF428 DASM5 Status\/Interrupt\/Control Register $/;"	m	struct:__anon22
DA6A	hdr/CFX/Headers/mc68338.h	/^	vushort		DA6A;		\/\/ $YFF432 DASM6 Register A $/;"	m	struct:__anon22
DA6B	hdr/CFX/Headers/mc68338.h	/^	vushort		DA6B;		\/\/ $YFF434 DASM6 Register B $/;"	m	struct:__anon22
DA6SIC	hdr/CFX/Headers/mc68338.h	/^	vushort		DA6SIC;		\/\/ $YFF430 DASM6 Status\/Interrupt\/Control Register $/;"	m	struct:__anon22
DA7A	hdr/CFX/Headers/mc68338.h	/^	vushort		DA7A;		\/\/ $YFF43A DASM7 Register A $/;"	m	struct:__anon22
DA7B	hdr/CFX/Headers/mc68338.h	/^	vushort		DA7B;		\/\/ $YFF43C DASM7 Register B $/;"	m	struct:__anon22
DA7SIC	hdr/CFX/Headers/mc68338.h	/^	vushort		DA7SIC;		\/\/ $YFF438 DASM7 Status\/Interrupt\/Control Register $/;"	m	struct:__anon22
DA8A	hdr/CFX/Headers/mc68338.h	/^	vushort		DA8A;		\/\/ $YFF442 DASM8 Register A $/;"	m	struct:__anon22
DA8B	hdr/CFX/Headers/mc68338.h	/^	vushort		DA8B;		\/\/ $YFF444 DASM8 Register B $/;"	m	struct:__anon22
DA8SIC	hdr/CFX/Headers/mc68338.h	/^	vushort		DA8SIC;		\/\/ $YFF440 DASM8 Status\/Interrupt\/Control Register $/;"	m	struct:__anon22
DA9A	hdr/CFX/Headers/mc68338.h	/^	vushort		DA9A;		\/\/ $YFF44A DASM9 Register A $/;"	m	struct:__anon22
DA9B	hdr/CFX/Headers/mc68338.h	/^	vushort		DA9B;		\/\/ $YFF44C DASM9 Register B $/;"	m	struct:__anon22
DA9SIC	hdr/CFX/Headers/mc68338.h	/^	vushort		DA9SIC;		\/\/ $YFF448 DASM9 Status\/Interrupt\/Control Register $/;"	m	struct:__anon22
DASM10	hdr/CFX/Headers/_cf1_ctm.h	/^	DASM9 = 9, DASM10 = 10, DASM26 = 26,DASM27 = 27,DASM28 = 28,$/;"	e	enum:__anon26
DASM26	hdr/CFX/Headers/_cf1_ctm.h	/^	DASM9 = 9, DASM10 = 10, DASM26 = 26,DASM27 = 27,DASM28 = 28,$/;"	e	enum:__anon26
DASM27	hdr/CFX/Headers/_cf1_ctm.h	/^	DASM9 = 9, DASM10 = 10, DASM26 = 26,DASM27 = 27,DASM28 = 28,$/;"	e	enum:__anon26
DASM28	hdr/CFX/Headers/_cf1_ctm.h	/^	DASM9 = 9, DASM10 = 10, DASM26 = 26,DASM27 = 27,DASM28 = 28,$/;"	e	enum:__anon26
DASM29	hdr/CFX/Headers/_cf1_ctm.h	/^	DASM29 = 29$/;"	e	enum:__anon26
DASM4	hdr/CFX/Headers/_cf1_ctm.h	/^	DASM4 = 4,	DASM5 = 5,	DASM6 = 6,	DASM7 = 7,	DASM8 = 8,	$/;"	e	enum:__anon26
DASM5	hdr/CFX/Headers/_cf1_ctm.h	/^	DASM4 = 4,	DASM5 = 5,	DASM6 = 6,	DASM7 = 7,	DASM8 = 8,	$/;"	e	enum:__anon26
DASM6	hdr/CFX/Headers/_cf1_ctm.h	/^	DASM4 = 4,	DASM5 = 5,	DASM6 = 6,	DASM7 = 7,	DASM8 = 8,	$/;"	e	enum:__anon26
DASM7	hdr/CFX/Headers/_cf1_ctm.h	/^	DASM4 = 4,	DASM5 = 5,	DASM6 = 6,	DASM7 = 7,	DASM8 = 8,	$/;"	e	enum:__anon26
DASM8	hdr/CFX/Headers/_cf1_ctm.h	/^	DASM4 = 4,	DASM5 = 5,	DASM6 = 6,	DASM7 = 7,	DASM8 = 8,	$/;"	e	enum:__anon26
DASM9	hdr/CFX/Headers/_cf1_ctm.h	/^	DASM9 = 9, DASM10 = 10, DASM26 = 26,DASM27 = 27,DASM28 = 28,$/;"	e	enum:__anon26
DASMDisable	hdr/CFX/Headers/_cf1_ctm.h	/^	  DASMDisable	$/;"	e	enum:__anon31
DASMIC	hdr/CFX/Headers/_cf1_ctm.h	/^	, DASMIC 			\/\/ Input Capture$/;"	e	enum:__anon31
DASMIPM	hdr/CFX/Headers/_cf1_ctm.h	/^	, DASMIPM 			\/\/ Input Period Measurement$/;"	e	enum:__anon31
DASMIPWM	hdr/CFX/Headers/_cf1_ctm.h	/^	, DASMIPWM 			\/\/ Input Pulse Width Measurement$/;"	e	enum:__anon31
DASMModeID	hdr/CFX/Headers/_cf1_ctm.h	/^	}	DASMModeID;$/;"	t	typeref:enum:__anon31
DASMOCAB	hdr/CFX/Headers/_cf1_ctm.h	/^	, DASMOCAB			\/\/ Output Compare Flag on A and B$/;"	e	enum:__anon31
DASMOCB	hdr/CFX/Headers/_cf1_ctm.h	/^	, DASMOCB			\/\/ Output Compare Flag on Channel B$/;"	e	enum:__anon31
DASMOPWM11	hdr/CFX/Headers/_cf1_ctm.h	/^	, DASMOPWM11		\/\/ Output PWM 11 Bit Resolution$/;"	e	enum:__anon31
DASMOPWM12	hdr/CFX/Headers/_cf1_ctm.h	/^	, DASMOPWM12		\/\/ Output PWM 12 Bit Resolution$/;"	e	enum:__anon31
DASMOPWM13	hdr/CFX/Headers/_cf1_ctm.h	/^	, DASMOPWM13		\/\/ Output PWM 13 Bit Resolution$/;"	e	enum:__anon31
DASMOPWM14	hdr/CFX/Headers/_cf1_ctm.h	/^	, DASMOPWM14		\/\/ Output PWM 14 Bit Resolution$/;"	e	enum:__anon31
DASMOPWM15	hdr/CFX/Headers/_cf1_ctm.h	/^	, DASMOPWM15		\/\/ Output PWM 15 Bit Resolution$/;"	e	enum:__anon31
DASMOPWM16	hdr/CFX/Headers/_cf1_ctm.h	/^	, DASMOPWM16 = 8	\/\/ Output PWM 16 Bit Resolution$/;"	e	enum:__anon31
DASMOPWM7	hdr/CFX/Headers/_cf1_ctm.h	/^	, DASMOPWM7			\/\/ Output PWM 7 Bit Resolution$/;"	e	enum:__anon31
DASMOPWM9	hdr/CFX/Headers/_cf1_ctm.h	/^	, DASMOPWM9			\/\/ Output PWM 9 Bit Resolution$/;"	e	enum:__anon31
DBL_DIG	hdr/CPU32/Headers/StdC/float.h	20;"	d
DBL_EPSILON	hdr/CPU32/Headers/StdC/float.h	21;"	d
DBL_MANT_DIG	hdr/CPU32/Headers/StdC/float.h	22;"	d
DBL_MAX	hdr/CPU32/Headers/StdC/float.h	23;"	d
DBL_MAX_10_EXP	hdr/CPU32/Headers/StdC/float.h	24;"	d
DBL_MAX_EXP	hdr/CPU32/Headers/StdC/float.h	25;"	d
DBL_MIN	hdr/CPU32/Headers/StdC/float.h	26;"	d
DBL_MIN_10_EXP	hdr/CPU32/Headers/StdC/float.h	27;"	d
DBL_MIN_EXP	hdr/CPU32/Headers/StdC/float.h	28;"	d
DBR_ERRORS	hdr/CFX/Headers/_cfx_errors.h	134;"	d
DCNR	hdr/CFX/Headers/mc68332.h	/^	vushort		DCNR;		\/\/ $YFFE26	Decoded Channel Number Register [S]$/;"	m	struct:__anon18
DDMMYY	hdr/CFX/Headers/_cfx_types.h	/^						, DDMMYY	\/\/ European$/;"	e	enum:__anon97
DDRE	hdr/CFX/Headers/mc68332.h	/^	vuchar		DDRE;		\/\/ $YFFA15 Port E Data Direction [S\/U]$/;"	m	struct:__anon16
DDRE	hdr/CFX/Headers/mc68338.h	/^	vuchar		DDRE;		\/\/ $YFFA15 Port E Data Direction [S\/U]$/;"	m	struct:__anon20
DDRF	hdr/CFX/Headers/mc68332.h	/^	vuchar		DDRF;		\/\/ $YFFA1D Port F Data Direction [S\/U]$/;"	m	struct:__anon16
DDRF	hdr/CFX/Headers/mc68338.h	/^	vuchar		DDRF;		\/\/ $YFFA1D Port F Data Direction [S\/U]$/;"	m	struct:__anon20
DDRQS	hdr/CFX/Headers/mc68332.h	/^	vuchar		DDRQS;		\/\/ $YFFC17 PQS Data Direction Register [S\/U]$/;"	m	struct:__anon17
DDRQS	hdr/CFX/Headers/mc68338.h	/^	vuchar		DDRQS;		\/\/ $YFFC17 PQS Data Direction Register [S\/U]$/;"	m	struct:__anon21
DEBUG_TRAP	hdr/CFX/Headers/_cfx_errors.h	/^	  DEBUG_TRAP				\/\/ 0 for inserting debugging traps$/;"	e	enum:__anon44
DECIMAL_DIG	hdr/CFX/Headers/math.mx.h	70;"	d
DFS_ERRORS	hdr/CFX/Headers/_cfx_errors.h	151;"	d
DIO	hdr/CFX/Headers/_cfx_tpu.h	/^	DIO		= 0xD,	\/\/*REQ*	21	Discrete Input\/Output$/;"	e	enum:__anon88
DIOINIT_0	hdr/CFX/Headers/tpu68332.h	166;"	d
DIOINIT_1	hdr/CFX/Headers/tpu68332.h	164;"	d
DIOINIT_10	hdr/CFX/Headers/tpu68332.h	161;"	d
DIOINIT_11	hdr/CFX/Headers/tpu68332.h	159;"	d
DIOINIT_12	hdr/CFX/Headers/tpu68332.h	157;"	d
DIOINIT_13	hdr/CFX/Headers/tpu68332.h	155;"	d
DIOINIT_14	hdr/CFX/Headers/tpu68332.h	153;"	d
DIOINIT_15	hdr/CFX/Headers/tpu68332.h	151;"	d
DIOINIT_2	hdr/CFX/Headers/tpu68332.h	162;"	d
DIOINIT_3	hdr/CFX/Headers/tpu68332.h	160;"	d
DIOINIT_4	hdr/CFX/Headers/tpu68332.h	158;"	d
DIOINIT_5	hdr/CFX/Headers/tpu68332.h	156;"	d
DIOINIT_6	hdr/CFX/Headers/tpu68332.h	154;"	d
DIOINIT_7	hdr/CFX/Headers/tpu68332.h	152;"	d
DIOINIT_8	hdr/CFX/Headers/tpu68332.h	165;"	d
DIOINIT_9	hdr/CFX/Headers/tpu68332.h	163;"	d
DIOPCLR_0	hdr/CFX/Headers/tpu68332.h	132;"	d
DIOPCLR_1	hdr/CFX/Headers/tpu68332.h	130;"	d
DIOPCLR_10	hdr/CFX/Headers/tpu68332.h	127;"	d
DIOPCLR_11	hdr/CFX/Headers/tpu68332.h	125;"	d
DIOPCLR_12	hdr/CFX/Headers/tpu68332.h	123;"	d
DIOPCLR_13	hdr/CFX/Headers/tpu68332.h	121;"	d
DIOPCLR_14	hdr/CFX/Headers/tpu68332.h	119;"	d
DIOPCLR_15	hdr/CFX/Headers/tpu68332.h	117;"	d
DIOPCLR_2	hdr/CFX/Headers/tpu68332.h	128;"	d
DIOPCLR_3	hdr/CFX/Headers/tpu68332.h	126;"	d
DIOPCLR_4	hdr/CFX/Headers/tpu68332.h	124;"	d
DIOPCLR_5	hdr/CFX/Headers/tpu68332.h	122;"	d
DIOPCLR_6	hdr/CFX/Headers/tpu68332.h	120;"	d
DIOPCLR_7	hdr/CFX/Headers/tpu68332.h	118;"	d
DIOPCLR_8	hdr/CFX/Headers/tpu68332.h	131;"	d
DIOPCLR_9	hdr/CFX/Headers/tpu68332.h	129;"	d
DIOPSET_0	hdr/CFX/Headers/tpu68332.h	115;"	d
DIOPSET_1	hdr/CFX/Headers/tpu68332.h	113;"	d
DIOPSET_10	hdr/CFX/Headers/tpu68332.h	110;"	d
DIOPSET_11	hdr/CFX/Headers/tpu68332.h	108;"	d
DIOPSET_12	hdr/CFX/Headers/tpu68332.h	106;"	d
DIOPSET_13	hdr/CFX/Headers/tpu68332.h	104;"	d
DIOPSET_14	hdr/CFX/Headers/tpu68332.h	102;"	d
DIOPSET_15	hdr/CFX/Headers/tpu68332.h	100;"	d
DIOPSET_2	hdr/CFX/Headers/tpu68332.h	111;"	d
DIOPSET_3	hdr/CFX/Headers/tpu68332.h	109;"	d
DIOPSET_4	hdr/CFX/Headers/tpu68332.h	107;"	d
DIOPSET_5	hdr/CFX/Headers/tpu68332.h	105;"	d
DIOPSET_6	hdr/CFX/Headers/tpu68332.h	103;"	d
DIOPSET_7	hdr/CFX/Headers/tpu68332.h	101;"	d
DIOPSET_8	hdr/CFX/Headers/tpu68332.h	114;"	d
DIOPSET_9	hdr/CFX/Headers/tpu68332.h	112;"	d
DIOUPDSQ_0	hdr/CFX/Headers/tpu68332.h	149;"	d
DIOUPDSQ_1	hdr/CFX/Headers/tpu68332.h	147;"	d
DIOUPDSQ_10	hdr/CFX/Headers/tpu68332.h	144;"	d
DIOUPDSQ_11	hdr/CFX/Headers/tpu68332.h	142;"	d
DIOUPDSQ_12	hdr/CFX/Headers/tpu68332.h	140;"	d
DIOUPDSQ_13	hdr/CFX/Headers/tpu68332.h	138;"	d
DIOUPDSQ_14	hdr/CFX/Headers/tpu68332.h	136;"	d
DIOUPDSQ_15	hdr/CFX/Headers/tpu68332.h	134;"	d
DIOUPDSQ_2	hdr/CFX/Headers/tpu68332.h	145;"	d
DIOUPDSQ_3	hdr/CFX/Headers/tpu68332.h	143;"	d
DIOUPDSQ_4	hdr/CFX/Headers/tpu68332.h	141;"	d
DIOUPDSQ_5	hdr/CFX/Headers/tpu68332.h	139;"	d
DIOUPDSQ_6	hdr/CFX/Headers/tpu68332.h	137;"	d
DIOUPDSQ_7	hdr/CFX/Headers/tpu68332.h	135;"	d
DIOUPDSQ_8	hdr/CFX/Headers/tpu68332.h	148;"	d
DIOUPDSQ_9	hdr/CFX/Headers/tpu68332.h	146;"	d
DIR	hdr/CFX/Headers/dirent.h	/^	} DIR;$/;"	t	typeref:struct:dir
DIRENT	hdr/CFX/Headers/dirent.h	/^	} DIRENT;$/;"	t	typeref:struct:dirent
DIRFindEnd	hdr/CFX/Headers/_cfx_drives.h	/^short	DIRFindEnd(struct dirent *dp)				PICO_CALL(DIRFindEnd);$/;"	v
DIRFindFirst	hdr/CFX/Headers/_cfx_drives.h	/^			struct dirent *dp)						PICO_CALL(DIRFindFirst);$/;"	v
DIRFindNext	hdr/CFX/Headers/_cfx_drives.h	/^short	DIRFindNext(struct dirent *dp)				PICO_CALL(DIRFindNext);$/;"	v
DIRFreeSpace	hdr/CFX/Headers/_cfx_drives.h	/^long	DIRFreeSpace(char *drive)					PICO_CALL(DIRFreeSpace);$/;"	v
DIRMatchName	hdr/CFX/Headers/_cfx_drives.h	/^bool	DIRMatchName(char *filename, char *pattern)	PICO_CALL(DIRMatchName);$/;"	v
DIRTotalSpace	hdr/CFX/Headers/_cfx_drives.h	/^long	DIRTotalSpace(char *drive)					PICO_CALL(DIRTotalSpace);$/;"	v
DIR_ENTRY_SIZE	hdr/CFX/Headers/dosdrive.h	90;"	d
DIR_ERRORS	hdr/CFX/Headers/_cfx_errors.h	147;"	d
DMA_LBA_Support	hdr/CFX/Headers/dosdrive.h	/^	ushort		DMA_LBA_Support;		\/\/ supports DMA (b8) and LBA (b9)$/;"	m	struct:ATADriveID
DOSPackedBootParamBlock	hdr/CFX/Headers/dosdrive.h	/^	}	DOSPackedBootParamBlock, DPBPB;$/;"	t	typeref:struct:DPBPB
DOSPartitionEntry	hdr/CFX/Headers/dosdrive.h	/^	}	DOSPartitionEntry, DOSPartitionTable[4];$/;"	t	typeref:struct:__anon6
DOSPartitionTable	hdr/CFX/Headers/dosdrive.h	/^	}	DOSPartitionEntry, DOSPartitionTable[4];$/;"	t	typeref:struct:__anon6
DOSWorkingBootParamBlock	hdr/CFX/Headers/dosdrive.h	/^	}	DOSWorkingBootParamBlock, DWBPB;$/;"	t	typeref:struct:DWBPB
DO_NOT_AUTOEXEC	hdr/CFX/Headers/_cfx_globals.h	131;"	d
DPBPB	hdr/CFX/Headers/dosdrive.h	/^	}	DOSPackedBootParamBlock, DPBPB;$/;"	t	typeref:struct:DPBPB
DPBPB	hdr/CFX/Headers/dosdrive.h	/^typedef struct DPBPB$/;"	s
DREG	hdr/CFX/Headers/mc68332.h	/^	vushort		DREG;		\/\/ $YFFA3A Test Module Distributed Register [S\/U]$/;"	m	struct:__anon16
DREG	hdr/CFX/Headers/mc68338.h	/^	vushort		DREG;		\/\/ $YFFA3A Test Module Distributed Register [S\/U]$/;"	m	struct:__anon20
DS	hdr/CFX/Headers/_cf1_pins.h	/^	  DS	=  1	\/\/ Data Strobe\/GPIO		Out		GPIO\/BUS		OB	OB$/;"	e	enum:__anon33
DS	hdr/CFX/Headers/_cfx_pins.h	/^	  DS	=  1	\/\/ Data Strobe\/GPIO		Out		GPIO\/BUS		OB	OB$/;"	e	enum:__anon75
DSCR	hdr/CFX/Headers/mc68332.h	/^	vushort		DSCR;		\/\/ $YFFE04	Development Support Control Register [S]$/;"	m	struct:__anon18
DSDDataSectors	hdr/CFX/Headers/_cfx_drives.h	/^long	DSDDataSectors(short logdrv)				PICO_CALL(DSDDataSectors);$/;"	v
DSDFreeSectors	hdr/CFX/Headers/_cfx_drives.h	/^long	DSDFreeSectors(short logdrv)				PICO_CALL(DSDFreeSectors);$/;"	v
DSD_COMMIT_HINTS_MASK	hdr/CFX/Headers/_cfx_internals.h	1200;"	d
DSD_ERRORS	hdr/CFX/Headers/_cfx_errors.h	124;"	d
DSD_FIRST_DEV	hdr/CFX/Headers/_cfx_internals.h	1145;"	d
DSD_LAST_DEV	hdr/CFX/Headers/_cfx_internals.h	1146;"	d
DSD_MAX_DEVS	hdr/CFX/Headers/_cfx_internals.h	1147;"	d
DSSR	hdr/CFX/Headers/mc68332.h	/^	vushort		DSSR;		\/\/ $YFFE06	Development Support Status Register [S]$/;"	m	struct:__anon18
DWBPB	hdr/CFX/Headers/dosdrive.h	/^	}	DOSWorkingBootParamBlock, DWBPB;$/;"	t	typeref:struct:DWBPB
DWBPB	hdr/CFX/Headers/dosdrive.h	/^typedef struct DWBPB$/;"	s
DW_CountOfClusters	hdr/CFX/Headers/dosdrive.h	/^	ulong		DW_CountOfClusters;		\/\/ max cluster number (tot-1)$/;"	m	struct:DWBPB
DW_DataSectors	hdr/CFX/Headers/dosdrive.h	/^	ulong		DW_DataSectors;			\/\/ working data sectors (FAT32 includes root dir)$/;"	m	struct:DWBPB
DW_FATType	hdr/CFX/Headers/dosdrive.h	/^	FATType		DW_FATType;				\/\/ computed from capacity (uSoft)$/;"	m	struct:DWBPB
DW_FSI_Free_Count	hdr/CFX/Headers/dosdrive.h	/^	long		*DW_FSI_Free_Count;		\/\/ last known free cluster count$/;"	m	struct:DWBPB
DW_FSI_Nxt_Free	hdr/CFX/Headers/dosdrive.h	/^	long		*DW_FSI_Nxt_Free;		\/\/ cluster number at to start free search$/;"	m	struct:DWBPB
DW_FirstDataSector	hdr/CFX/Headers/dosdrive.h	/^	ulong		DW_FirstDataSector;		\/\/ first data sector (includes root for FAT32)$/;"	m	struct:DWBPB
DW_FirstDirSector	hdr/CFX/Headers/dosdrive.h	/^	ulong		DW_FirstDirSector;		\/\/ first dir sector (root for FAT32)$/;"	m	struct:DWBPB
DW_FirstFatSector	hdr/CFX/Headers/dosdrive.h	/^	ulong		DW_FirstFatSector;		\/\/ first fat sector$/;"	m	struct:DWBPB
DW_LastClusterMark	hdr/CFX/Headers/dosdrive.h	/^	ulong		DW_LastClusterMark;		\/\/ last cluster mark$/;"	m	struct:DWBPB
DW_LastDataSector	hdr/CFX/Headers/dosdrive.h	/^	ulong		DW_LastDataSector;		\/\/ last data sector$/;"	m	struct:DWBPB
DW_RootDirSectors	hdr/CFX/Headers/dosdrive.h	/^	ulong		DW_RootDirSectors;		\/\/ root dir sectors (0 for FAT32)$/;"	m	struct:DWBPB
DW_SectorsPerFAT	hdr/CFX/Headers/dosdrive.h	/^	ulong		DW_SectorsPerFAT;		\/\/ sectors per fat$/;"	m	struct:DWBPB
DW_TotalSectors	hdr/CFX/Headers/dosdrive.h	/^	ulong		DW_TotalSectors;		\/\/ working total sectors$/;"	m	struct:DWBPB
DateCmd	hdr/CFX/Headers/_cfx_console.h	/^char	*DateCmd(CmdInfoPtr cip)					PICO_CALL(DateCmd);$/;"	v
DateFieldOrder	hdr/CFX/Headers/_cfx_types.h	/^	} DateFieldOrder;$/;"	t	typeref:enum:__anon97
DefaultChkBsyDelay	hdr/CFX/Headers/_cfx_internals.h	/^	, DefaultChkBsyDelay			= 1000000	\/\/ 1 second$/;"	e	enum:ResetResume::__anon65
DefaultStartupTimeout	hdr/CFX/Headers/_cfx_internals.h	/^	  DefaultStartupTimeout			= 30000000	\/\/ 30 seconds$/;"	e	enum:ResetResume::__anon65
DelCmd	hdr/CFX/Headers/_cfx_console.h	/^char	*DelCmd(CmdInfoPtr cip)						PICO_CALL(DelCmd);$/;"	v
Delay100us	hdr/CFX/Headers/_cfx_time.h	57;"	d
Delay10us	hdr/CFX/Headers/_cfx_time.h	54;"	d
Delay1ms	hdr/CFX/Headers/_cfx_time.h	60;"	d
Delay1us	hdr/CFX/Headers/_cfx_time.h	51;"	d
Delay200us	hdr/CFX/Headers/_cfx_time.h	58;"	d
Delay20us	hdr/CFX/Headers/_cfx_time.h	55;"	d
Delay2us	hdr/CFX/Headers/_cfx_time.h	52;"	d
Delay500us	hdr/CFX/Headers/_cfx_time.h	59;"	d
Delay50us	hdr/CFX/Headers/_cfx_time.h	56;"	d
Delay5us	hdr/CFX/Headers/_cfx_time.h	53;"	d
DeviceCmd	hdr/CFX/Headers/_cfx_console.h	/^char	*DeviceCmd(CmdInfoPtr cip)					PICO_CALL(DeviceCmd);$/;"	v
DirCmd	hdr/CFX/Headers/_cfx_console.h	/^char	*DirCmd(CmdInfoPtr cip)						PICO_CALL(DirCmd);$/;"	v
DivideBy2	hdr/CFX/Headers/_cf1_ctm.h	/^typedef enum	{	DivideBy2,	DivideBy3	} CPSMDivisor;$/;"	e	enum:__anon27
DivideBy3	hdr/CFX/Headers/_cf1_ctm.h	/^typedef enum	{	DivideBy2,	DivideBy3	} CPSMDivisor;$/;"	e	enum:__anon27
DontOpenClose	hdr/CFX/Headers/_cfx_console.h	/^enum { EchoToConsole = 0x01, LogPicoCommands = 0x10, DontOpenClose = 0x20 };$/;"	e	enum:__anon35
DosSwitch	hdr/CFX/Headers/_cfx_console.h	/^	} DosSwitch;$/;"	t	typeref:struct:__anon37
DoubleActionID	hdr/CFX/Headers/_cf1_ctm.h	/^	} DoubleActionID;$/;"	t	typeref:enum:__anon26
DumpCmd	hdr/CFX/Headers/_cfx_console.h	/^char	*DumpCmd(CmdInfoPtr cip)					PICO_CALL(DumpCmd);$/;"	v
EBADF	hdr/CFX/Headers/_cfx_errors.h	/^	, EBADF						\/\/ invalid file descriptor (11)$/;"	e	enum:__anon43
EBADF	hdr/CPU32/Headers/StdC/errno.h	9;"	d
EDOM	hdr/CFX/Headers/_cfx_errors.h	/^	, EDOM = 33					\/\/ domain error (33)$/;"	e	enum:__anon43
EDOM	hdr/CPU32/Headers/StdC/errno.h	22;"	d
EEXIST	hdr/CFX/Headers/_cfx_errors.h	/^	, EEXIST					\/\/ named file already exists (12)$/;"	e	enum:__anon43
EEXIST	hdr/CPU32/Headers/StdC/errno.h	10;"	d
EFPOS	hdr/CFX/Headers/_cfx_errors.h	/^	, EFPOS						\/\/ file position error (35)$/;"	e	enum:__anon43
EFPOS	hdr/CPU32/Headers/StdC/errno.h	24;"	d
EIAAssertTXX	hdr/CFX/Headers/_cfx_sercomm.h	/^bool	EIAAssertTXX(bool set)						BIOS_CALL(EIAAssertTXX);$/;"	v
EIACheckRXX	hdr/CFX/Headers/_cfx_sercomm.h	/^bool	EIACheckRXX(void)							BIOS_CALL(EIACheckRXX);$/;"	v
EIAEnableRx	hdr/CFX/Headers/_cfx_sercomm.h	/^bool	EIAEnableRx(bool enable)					BIOS_CALL(EIAEnableRx);$/;"	v
EIAForceOff	hdr/CFX/Headers/_cfx_sercomm.h	/^bool	EIAForceOff(bool forceoff)					BIOS_CALL(EIAForceOff);$/;"	v
EINVAL	hdr/CFX/Headers/_cfx_errors.h	/^	, EINVAL					\/\/ invalid argument (13)$/;"	e	enum:__anon43
EINVAL	hdr/CPU32/Headers/StdC/errno.h	11;"	d
EIO	hdr/CFX/Headers/_cfx_errors.h	/^	, EIO						\/\/ input or output error (14)$/;"	e	enum:__anon43
EIO	hdr/CPU32/Headers/StdC/errno.h	12;"	d
EISDIR	hdr/CFX/Headers/_cfx_errors.h	/^	, EISDIR					\/\/ attempt open dir as file (15)$/;"	e	enum:__anon43
EISDIR	hdr/CPU32/Headers/StdC/errno.h	13;"	d
EMPTY_CLUSTER	hdr/CFX/Headers/dosdrive.h	108;"	d
ENFILE	hdr/CFX/Headers/_cfx_errors.h	/^	, ENFILE					\/\/ too many files are open (16)$/;"	e	enum:__anon43
ENFILE	hdr/CPU32/Headers/StdC/errno.h	14;"	d
ENOENT	hdr/CFX/Headers/_cfx_errors.h	/^	, ENOENT					\/\/ file or directory does not exist (17)$/;"	e	enum:__anon43
ENOENT	hdr/CPU32/Headers/StdC/errno.h	15;"	d
ENOERR	hdr/CFX/Headers/_cfx_errors.h	/^	  ENOERR = 0				\/\/ no error (0) <errno.h>$/;"	e	enum:__anon43
ENOERR	hdr/CPU32/Headers/StdC/errno.h	8;"	d
ENOMEM	hdr/CFX/Headers/_cfx_errors.h	/^	, ENOMEM					\/\/ no memory available (18)$/;"	e	enum:__anon43
ENOMEM	hdr/CPU32/Headers/StdC/errno.h	16;"	d
ENOSPC	hdr/CFX/Headers/_cfx_errors.h	/^	, ENOSPC					\/\/ no space on disk or directory (19)$/;"	e	enum:__anon43
ENOSPC	hdr/CPU32/Headers/StdC/errno.h	17;"	d
ENOTDIR	hdr/CFX/Headers/_cfx_errors.h	/^	, ENOTDIR					\/\/ part of path was not directory (20)$/;"	e	enum:__anon43
ENOTDIR	hdr/CPU32/Headers/StdC/errno.h	18;"	d
ENOTEMPTY	hdr/CFX/Headers/_cfx_errors.h	/^	, ENOTEMPTY					\/\/ attempt to delete non-empty directory (21)$/;"	e	enum:__anon43
ENOTEMPTY	hdr/CPU32/Headers/StdC/errno.h	19;"	d
ENXIO	hdr/CFX/Headers/_cfx_errors.h	/^	, ENXIO						\/\/ no such device (22)$/;"	e	enum:__anon43
ENXIO	hdr/CPU32/Headers/StdC/errno.h	20;"	d
EOF	hdr/CPU32/Headers/StdC/stdio.h	25;"	d
EPERM	hdr/CFX/Headers/_cfx_errors.h	/^	, EPERM						\/\/ operation is not permited (23)$/;"	e	enum:__anon43
EPERM	hdr/CPU32/Headers/StdC/errno.h	21;"	d
ERANGE	hdr/CFX/Headers/_cfx_errors.h	/^	, ERANGE					\/\/ range error (34)$/;"	e	enum:__anon43
ERANGE	hdr/CPU32/Headers/StdC/errno.h	23;"	d
ESIGPARM	hdr/CFX/Headers/_cfx_errors.h	/^	, ESIGPARM					\/\/ signal parameter error (36)$/;"	e	enum:__anon43
ESIGPARM	hdr/CPU32/Headers/StdC/errno.h	25;"	d
EXEC_MUST_NOT_FAIL	hdr/CFX/Headers/_cfx_globals.h	132;"	d
EXIT_FAILURE	hdr/CPU32/Headers/StdC/stdlib.h	12;"	d
EXIT_SUCCESS	hdr/CPU32/Headers/StdC/stdlib.h	13;"	d
EXT_BOOT_SIG	hdr/CFX/Headers/dosdrive.h	93;"	d
EchoCmd	hdr/CFX/Headers/_cfx_console.h	/^char	*EchoCmd(CmdInfoPtr cip)					PICO_CALL(EchoCmd);$/;"	v
EchoToConsole	hdr/CFX/Headers/_cfx_console.h	/^enum { EchoToConsole = 0x01, LogPicoCommands = 0x10, DontOpenClose = 0x20 };$/;"	e	enum:__anon35
ExitCmd	hdr/CFX/Headers/_cfx_console.h	/^char	*ExitCmd(CmdInfoPtr cip)					PICO_CALL(ExitCmd);$/;"	v
FAT12	hdr/CFX/Headers/dosdrive.h	/^typedef enum { FAT12 = -1, FAT16, FAT32, FATBAD } FATType;$/;"	e	enum:__anon3
FAT16	hdr/CFX/Headers/dosdrive.h	/^typedef enum { FAT12 = -1, FAT16, FAT32, FATBAD } FATType;$/;"	e	enum:__anon3
FAT16_RESERVED_SECTORS	hdr/CFX/Headers/dosdrive.h	104;"	d
FAT16_ROOT_DIR_ENTRIES	hdr/CFX/Headers/dosdrive.h	91;"	d
FAT32	hdr/CFX/Headers/dosdrive.h	/^typedef enum { FAT12 = -1, FAT16, FAT32, FATBAD } FATType;$/;"	e	enum:__anon3
FAT32_BACKUP_SECTOR	hdr/CFX/Headers/dosdrive.h	97;"	d
FAT32_FSINFO_SECTOR	hdr/CFX/Headers/dosdrive.h	96;"	d
FAT32_RESERVED_SECTORS	hdr/CFX/Headers/dosdrive.h	105;"	d
FAT32_ROOT_CLUSTER	hdr/CFX/Headers/dosdrive.h	95;"	d
FAT32_ROOT_DIR_ENTRIES	hdr/CFX/Headers/dosdrive.h	92;"	d
FAT32_VERSION	hdr/CFX/Headers/dosdrive.h	94;"	d
FATBAD	hdr/CFX/Headers/dosdrive.h	/^typedef enum { FAT12 = -1, FAT16, FAT32, FATBAD } FATType;$/;"	e	enum:__anon3
FATType	hdr/CFX/Headers/dosdrive.h	/^typedef enum { FAT12 = -1, FAT16, FAT32, FATBAD } FATType;$/;"	t	typeref:enum:__anon3
FAT_DEFAULT_NUMBER	hdr/CFX/Headers/dosdrive.h	106;"	d
FAT_MEDIA_DESCRIPTOR	hdr/CFX/Headers/dosdrive.h	107;"	d
FC3CNT	hdr/CFX/Headers/mc68338.h	/^	vushort		FC3CNT;		\/\/ $YFF41A FCSM3 Counter Register $/;"	m	struct:__anon22
FC3SIC	hdr/CFX/Headers/mc68338.h	/^	vushort		FC3SIC;		\/\/ $YFF418 FCSM3 Status\/Interrupt\/Control Register $/;"	m	struct:__anon22
FILE	hdr/CPU32/Headers/StdC/stdio.h	/^typedef struct _FILE FILE;$/;"	t	typeref:struct:_FILE
FILENAME_MAX	hdr/CPU32/Headers/StdC/stdio.h	13;"	d
FLASH_ERRORS	hdr/CFX/Headers/_cfx_errors.h	75;"	d
FLASH_ERR_TRAP	hdr/CFX/Headers/_cfx_errors.h	/^	, FLASH_ERR_TRAP			\/\/ 4 for detecting flash errors$/;"	e	enum:__anon44
FLT_DIG	hdr/CPU32/Headers/StdC/float.h	10;"	d
FLT_EPSILON	hdr/CPU32/Headers/StdC/float.h	11;"	d
FLT_MANT_DIG	hdr/CPU32/Headers/StdC/float.h	12;"	d
FLT_MAX	hdr/CPU32/Headers/StdC/float.h	13;"	d
FLT_MAX_10_EXP	hdr/CPU32/Headers/StdC/float.h	15;"	d
FLT_MAX_EXP	hdr/CPU32/Headers/StdC/float.h	14;"	d
FLT_MIN	hdr/CPU32/Headers/StdC/float.h	16;"	d
FLT_MIN_10_EXP	hdr/CPU32/Headers/StdC/float.h	18;"	d
FLT_MIN_EXP	hdr/CPU32/Headers/StdC/float.h	17;"	d
FLT_RADIX	hdr/CPU32/Headers/StdC/float.h	8;"	d
FLT_ROUNDS	hdr/CPU32/Headers/StdC/float.h	9;"	d
FOPEN_MAX	hdr/CPU32/Headers/StdC/stdio.h	12;"	d
FP_INFINITE	hdr/CFX/Headers/math.mx.h	59;"	d
FP_NAN	hdr/CFX/Headers/math.mx.h	58;"	d
FP_NORMAL	hdr/CFX/Headers/math.mx.h	61;"	d
FP_SUBNORMAL	hdr/CFX/Headers/math.mx.h	62;"	d
FP_ZERO	hdr/CFX/Headers/math.mx.h	60;"	d
FQM	hdr/CFX/Headers/_cfx_tpu.h	/^	FQM		= 0x9,	\/\/		22	Frequency Measurement$/;"	e	enum:__anon88
FSI_Free_Count	hdr/CFX/Headers/dosdrive.h	/^	ulong	FSI_Free_Count;			\/\/ last known free cluster count$/;"	m	struct:FSInfo
FSI_LEADSIG	hdr/CFX/Headers/dosdrive.h	98;"	d
FSI_LEADSIG_BIG_END	hdr/CFX/Headers/dosdrive.h	99;"	d
FSI_LeadSig	hdr/CFX/Headers/dosdrive.h	/^	ulong	FSI_LeadSig;			\/\/ Value 0x41615252.$/;"	m	struct:FSInfo
FSI_Nxt_Free	hdr/CFX/Headers/dosdrive.h	/^	ulong	FSI_Nxt_Free;			\/\/ hint for the FAT driver$/;"	m	struct:FSInfo
FSI_Reserved1	hdr/CFX/Headers/dosdrive.h	/^	uchar	FSI_Reserved1[480];		\/\/ zeros$/;"	m	struct:FSInfo
FSI_Reserved2	hdr/CFX/Headers/dosdrive.h	/^	uchar	FSI_Reserved2[12];		\/\/ zeros$/;"	m	struct:FSInfo
FSI_STRUCSIG	hdr/CFX/Headers/dosdrive.h	100;"	d
FSI_STRUCSIG_BIG_END	hdr/CFX/Headers/dosdrive.h	101;"	d
FSI_StrucSig	hdr/CFX/Headers/dosdrive.h	/^	ulong	FSI_StrucSig;			\/\/ Value 0x61417272.$/;"	m	struct:FSInfo
FSI_TRAILSIG	hdr/CFX/Headers/dosdrive.h	102;"	d
FSI_TRAILSIG_BIG_END	hdr/CFX/Headers/dosdrive.h	103;"	d
FSI_TrailSig	hdr/CFX/Headers/dosdrive.h	/^	ulong	FSI_TrailSig;			\/\/ Value 0xAA550000.$/;"	m	struct:FSInfo
FSInfo	hdr/CFX/Headers/dosdrive.h	/^	}	FSInfo;$/;"	t	typeref:struct:FSInfo
FSInfo	hdr/CFX/Headers/dosdrive.h	/^typedef struct FSInfo$/;"	s
FastStop	hdr/CFX/Headers/_cfx_power.h	/^enum { 	FullStop = 0x00,	FastStop = 0x02,		CPUStop = 0x42 };$/;"	e	enum:__anon76
FdiskCmd	hdr/CFX/Headers/_cfx_console.h	/^char	*FdiskCmd(CmdInfoPtr cip)					PICO_CALL(FdiskCmd);$/;"	v
FlashErrCode	hdr/CFX/Headers/_cfx_globals.h	/^	short	FlashErrCode;		\/\/ global flash error code$/;"	m	struct:__anon49
ForCmd	hdr/CFX/Headers/_cfx_console.h	/^char	*ForCmd(CmdInfoPtr cip)						PICO_CALL(ForCmd);$/;"	v
FormatCmd	hdr/CFX/Headers/_cfx_console.h	/^char	*FormatCmd(CmdInfoPtr cip)					PICO_CALL(FormatCmd);$/;"	v
Freef	hdr/CFX/Headers/_cfx_types.h	/^typedef void	Freef(void *ptr);$/;"	t
FullStop	hdr/CFX/Headers/_cfx_power.h	/^enum { 	FullStop = 0x00,	FastStop = 0x02,		CPUStop = 0x42 };$/;"	e	enum:__anon76
GoCmd	hdr/CFX/Headers/_cfx_console.h	/^char	*GoCmd(CmdInfoPtr cip)						PICO_CALL(GoCmd);$/;"	v
GotoCmd	hdr/CFX/Headers/_cfx_console.h	/^char	*GotoCmd(CmdInfoPtr cip)					PICO_CALL(GotoCmd);$/;"	v
HSQR0	hdr/CFX/Headers/mc68332.h	/^	vushort		HSQR0;		\/\/ $YFFE14	Host Sequence Register 0 [S\/U]$/;"	m	struct:__anon18
HSQR0_15to8_W	hdr/CFX/Headers/tpu68332.h	60;"	d
HSQR1	hdr/CFX/Headers/mc68332.h	/^	vushort		HSQR1;		\/\/ $YFFE16	Host Sequence Register 1 [S\/U]$/;"	m	struct:__anon18
HSQR1_7to0_W	hdr/CFX/Headers/tpu68332.h	70;"	d
HSQR_0	hdr/CFX/Headers/tpu68332.h	78;"	d
HSQR_1	hdr/CFX/Headers/tpu68332.h	77;"	d
HSQR_10	hdr/CFX/Headers/tpu68332.h	66;"	d
HSQR_11	hdr/CFX/Headers/tpu68332.h	65;"	d
HSQR_12	hdr/CFX/Headers/tpu68332.h	64;"	d
HSQR_13	hdr/CFX/Headers/tpu68332.h	63;"	d
HSQR_14	hdr/CFX/Headers/tpu68332.h	62;"	d
HSQR_15	hdr/CFX/Headers/tpu68332.h	61;"	d
HSQR_2	hdr/CFX/Headers/tpu68332.h	76;"	d
HSQR_3	hdr/CFX/Headers/tpu68332.h	75;"	d
HSQR_4	hdr/CFX/Headers/tpu68332.h	74;"	d
HSQR_5	hdr/CFX/Headers/tpu68332.h	73;"	d
HSQR_6	hdr/CFX/Headers/tpu68332.h	72;"	d
HSQR_7	hdr/CFX/Headers/tpu68332.h	71;"	d
HSQR_8	hdr/CFX/Headers/tpu68332.h	68;"	d
HSQR_9	hdr/CFX/Headers/tpu68332.h	67;"	d
HSRR0	hdr/CFX/Headers/mc68332.h	/^	vushort		HSRR0;		\/\/ $YFFE18	Host Service Request Register 0 [S\/U]$/;"	m	struct:__anon18
HSRR0_15to8_W	hdr/CFX/Headers/tpu68332.h	80;"	d
HSRR1	hdr/CFX/Headers/mc68332.h	/^	vushort		HSRR1;		\/\/ $YFFE1A	Host Service Request Register 1 [S\/U]$/;"	m	struct:__anon18
HSRR1_7to0_W	hdr/CFX/Headers/tpu68332.h	90;"	d
HSRR_0	hdr/CFX/Headers/tpu68332.h	98;"	d
HSRR_1	hdr/CFX/Headers/tpu68332.h	97;"	d
HSRR_10	hdr/CFX/Headers/tpu68332.h	86;"	d
HSRR_11	hdr/CFX/Headers/tpu68332.h	85;"	d
HSRR_12	hdr/CFX/Headers/tpu68332.h	84;"	d
HSRR_13	hdr/CFX/Headers/tpu68332.h	83;"	d
HSRR_14	hdr/CFX/Headers/tpu68332.h	82;"	d
HSRR_15	hdr/CFX/Headers/tpu68332.h	81;"	d
HSRR_2	hdr/CFX/Headers/tpu68332.h	96;"	d
HSRR_3	hdr/CFX/Headers/tpu68332.h	95;"	d
HSRR_4	hdr/CFX/Headers/tpu68332.h	94;"	d
HSRR_5	hdr/CFX/Headers/tpu68332.h	93;"	d
HSRR_6	hdr/CFX/Headers/tpu68332.h	92;"	d
HSRR_7	hdr/CFX/Headers/tpu68332.h	91;"	d
HSRR_8	hdr/CFX/Headers/tpu68332.h	88;"	d
HSRR_9	hdr/CFX/Headers/tpu68332.h	87;"	d
HUGE_VAL	hdr/CFX/Headers/math.mx.h	54;"	d
HUGE_VAL	hdr/CPU32/Headers/StdC/math.h	8;"	d
HaltMonEnable	hdr/CFX/Headers/_cfx_internals.h	/^enum	{	HaltMonEnable = 0x08,		BusMonEnable = 0x04,	$/;"	e	enum:ResetResume::__anon51
HelpCmd	hdr/CFX/Headers/_cfx_console.h	/^char	*HelpCmd(CmdInfoPtr cip)					PICO_CALL(HelpCmd);$/;"	v
IEVCWrapper	hdr/CFX/Headers/_cfx_cpulevel.h	/^typedef struct { short entry[5]; ievfptr cf; short exit[3]; } IEVCWrapper;$/;"	t	typeref:struct:__anon40
IEVDisableAll	hdr/CFX/Headers/_cfx_cpulevel.h	91;"	d
IEVEnableAll	hdr/CFX/Headers/_cfx_cpulevel.h	90;"	d
IEVInsertAsmFunct	hdr/CFX/Headers/_cfx_cpulevel.h	/^												BIOS_CALL(IEVInsertAsmFunct);$/;"	v
IEVInsertCFunct	hdr/CFX/Headers/_cfx_cpulevel.h	/^												BIOS_CALL(IEVInsertCFunct);$/;"	v
IEVRestoreSavedSR	hdr/CFX/Headers/_cfx_cpulevel.h	89;"	d
IEVStack	hdr/CFX/Headers/_cfx_cpulevel.h	/^typedef struct { ushort sr; ulong pc; ushort fmtvct; ulong info[4]; } IEVStack;$/;"	t	typeref:struct:__anon39
IEV_C_FUNCT	hdr/CFX/Headers/_cfx_cpulevel.h	109;"	d
IEV_C_PROTO	hdr/CFX/Headers/_cfx_cpulevel.h	105;"	d
IEV_DISABLE	hdr/CFX/Headers/_cfx_internals.h	432;"	d
IEV_ENABLE	hdr/CFX/Headers/_cfx_internals.h	434;"	d
IEV_MASK_ALL	hdr/CFX/Headers/_cfx_internals.h	431;"	d
IEV_MASK_NONE	hdr/CFX/Headers/_cfx_internals.h	433;"	d
INFINITY	hdr/CFX/Headers/math.mx.h	55;"	d
INT_MAX	hdr/CPU32/Headers/StdC/limits.h	12;"	d
INT_MIN	hdr/CPU32/Headers/StdC/limits.h	13;"	d
IRQ2	hdr/CFX/Headers/_cf1_pins.h	/^	, IRQ2	= 41	\/\/ Interrupt Request 2	I\/O		GPIO\/IRQ	10K	IB+	I+$/;"	e	enum:__anon33
IRQ2	hdr/CFX/Headers/_cfx_pins.h	/^	, IRQ2	= 41	\/\/ Interrupt Request 2	I\/O		GPIO\/IRQ	10K	IB+	I+$/;"	e	enum:__anon75
IRQ3RXX	hdr/CFX/Headers/_cf1_pins.h	/^	, IRQ3RXX= 50	\/\/ IRQ\/CMOS RXX	Sense	in		GPIO\/UART		IB?	I+$/;"	e	enum:__anon33
IRQ3RXX	hdr/CFX/Headers/_cfx_pins.h	/^	, IRQ3RXX= 50	\/\/ IRQ\/CMOS RXX	Sense	in		GPIO\/UART		IB?	I+$/;"	e	enum:__anon75
IRQ4RXD	hdr/CFX/Headers/_cf1_pins.h	/^	, IRQ4RXD= 45	\/\/ IRQ\/CMOS RxD	Sense	In		GPIO\/UART		IB?	I+$/;"	e	enum:__anon33
IRQ4RXD	hdr/CFX/Headers/_cfx_pins.h	/^	, IRQ4RXD= 45	\/\/ IRQ\/CMOS RxD	Sense	In		GPIO\/UART		IB?	I+$/;"	e	enum:__anon75
IRQ5	hdr/CFX/Headers/_cf1_pins.h	/^	, IRQ5	= 39	\/\/ Interrupt Request 5	I\/O		GPIO\/IRQ	10K	IB+	I+$/;"	e	enum:__anon33
IRQ5	hdr/CFX/Headers/_cfx_pins.h	/^	, IRQ5	= 39	\/\/ Interrupt Request 5	I\/O		GPIO\/IRQ	10K	IB+	I+$/;"	e	enum:__anon75
IRQ7	hdr/CFX/Headers/_cf1_pins.h	/^	, IRQ7	= 40	\/\/ Interrupt Request 7	I\/O		GPIO\/IRQ	10K	IB+	I+$/;"	e	enum:__anon33
IRQ7	hdr/CFX/Headers/_cfx_pins.h	/^	, IRQ7	= 40	\/\/ Interrupt Request 7	I\/O		GPIO\/IRQ	10K	IB+	I+$/;"	e	enum:__anon75
IfCmd	hdr/CFX/Headers/_cfx_console.h	/^char	*IfCmd(CmdInfoPtr cip)						PICO_CALL(IfCmd);$/;"	v
Initflog	hdr/CFX/Headers/_cfx_console.h	/^void	Initflog(char *fname, uchar flags)			PICO_CALL(Initflog);$/;"	v
InstallCmd	hdr/CFX/Headers/_cfx_console.h	/^char	*InstallCmd(CmdInfoPtr cip)					PICO_CALL(InstallCmd);$/;"	v
KB_SIZE	hdr/CFX/Headers/dosdrive.h	88;"	d
LAST_CLUSTER_12_BIT	hdr/CFX/Headers/dosdrive.h	109;"	d
LAST_CLUSTER_16_BIT	hdr/CFX/Headers/dosdrive.h	110;"	d
LAST_CLUSTER_32_BIT	hdr/CFX/Headers/dosdrive.h	111;"	d
LBP	hdr/CFX/Headers/_cfx_internals.h	/^	} LongBit, *LBP;$/;"	t	struct:ResetResume	typeref:struct:ResetResume::__anon63
LC_ALL	hdr/CPU32/Headers/StdC/locale.h	17;"	d
LC_COLLATE	hdr/CPU32/Headers/StdC/locale.h	11;"	d
LC_CTYPE	hdr/CPU32/Headers/StdC/locale.h	12;"	d
LC_FIRST	hdr/CPU32/Headers/StdC/locale.h	10;"	d
LC_LAST	hdr/CPU32/Headers/StdC/locale.h	18;"	d
LC_MESSAGES	hdr/CPU32/Headers/StdC/locale.h	16;"	d
LC_MONETARY	hdr/CPU32/Headers/StdC/locale.h	13;"	d
LC_NUMERIC	hdr/CPU32/Headers/StdC/locale.h	14;"	d
LC_TIME	hdr/CPU32/Headers/StdC/locale.h	15;"	d
LEDGetState	hdr/CFX/Headers/_cfx_util.h	/^ushort	LEDGetState(ushort lrLED)					BIOS_CALL(LEDGetState);$/;"	v
LEDInit	hdr/CFX/Headers/_cfx_util.h	/^void	LEDInit(void)								BIOS_CALL(LEDInit);$/;"	v
LEDOrbit	hdr/CFX/Headers/_cfx_util.h	/^void	LEDOrbit(bool ccw)							BIOS_CALL(LEDOrbit);$/;"	v
LEDSetState	hdr/CFX/Headers/_cfx_util.h	/^void	LEDSetState(ushort lrLED, ushort state)		BIOS_CALL(LEDSetState);$/;"	v
LEDToggleRG	hdr/CFX/Headers/_cfx_util.h	/^void	LEDToggleRG(ushort lrLED)					BIOS_CALL(LEDToggleRG);$/;"	v
LEDToggleRGOff	hdr/CFX/Headers/_cfx_util.h	/^void	LEDToggleRGOff(ushort lrLED)				BIOS_CALL(LEDToggleRGOff);$/;"	v
LEDbus	hdr/CFX/Headers/_cfx_util.h	/^enum { LEDoff, LEDred, LEDgreen, LEDbus };$/;"	e	enum:__anon101
LEDgreen	hdr/CFX/Headers/_cfx_util.h	/^enum { LEDoff, LEDred, LEDgreen, LEDbus };$/;"	e	enum:__anon101
LEDleft	hdr/CFX/Headers/_cfx_util.h	/^enum { LEDleft, LEDright };$/;"	e	enum:__anon100
LEDoff	hdr/CFX/Headers/_cfx_util.h	/^enum { LEDoff, LEDred, LEDgreen, LEDbus };$/;"	e	enum:__anon101
LEDred	hdr/CFX/Headers/_cfx_util.h	/^enum { LEDoff, LEDred, LEDgreen, LEDbus };$/;"	e	enum:__anon101
LEDright	hdr/CFX/Headers/_cfx_util.h	/^enum { LEDleft, LEDright };$/;"	e	enum:__anon100
LONG_MAX	hdr/CPU32/Headers/StdC/limits.h	14;"	d
LONG_MIN	hdr/CPU32/Headers/StdC/limits.h	15;"	d
LPStop	hdr/CFX/Headers/_cfx_power.h	/^void	LPStop(void)								PICO_CALL(LPStop);$/;"	v
LPStopCSE	hdr/CFX/Headers/_cfx_power.h	/^void	LPStopCSE(uchar csebits)					PICO_CALL(LPStopCSE);$/;"	v
LR	hdr/CFX/Headers/mc68332.h	/^	vushort		LR;			\/\/ $YFFE22	Link Register [S]$/;"	m	struct:__anon18
LRTC	hdr/CFX/Headers/_cfx_tpu.h	/^	LRTC	= 0xF	\/\/*REQ*	39	Local Real Time Clock for CF2$/;"	e	enum:__anon88
L_tmpnam	hdr/CPU32/Headers/StdC/stdio.h	14;"	d
LabelCmd	hdr/CFX/Headers/_cfx_console.h	/^char	*LabelCmd(CmdInfoPtr cip)					PICO_CALL(LabelCmd);$/;"	v
LoadMxCmd	hdr/CFX/Headers/_cfx_console.h	/^char	*LoadMxCmd(CmdInfoPtr cip)					PICO_CALL(LoadMxCmd);$/;"	v
LoadSRecCmd	hdr/CFX/Headers/_cfx_console.h	/^char	*LoadSRecCmd(CmdInfoPtr cip)				PICO_CALL(LoadSRecCmd);$/;"	v
LogPicoCommands	hdr/CFX/Headers/_cfx_console.h	/^enum { EchoToConsole = 0x01, LogPicoCommands = 0x10, DontOpenClose = 0x20 };$/;"	e	enum:__anon35
LongBit	hdr/CFX/Headers/_cfx_internals.h	/^	} LongBit, *LBP;$/;"	t	struct:ResetResume	typeref:struct:ResetResume::__anon63
LongBitClear	hdr/CFX/Headers/_cfx_internals.h	835;"	d
LongBitSet	hdr/CFX/Headers/_cfx_internals.h	834;"	d
LongBitTest	hdr/CFX/Headers/_cfx_internals.h	836;"	d
LoopDelay100us	hdr/CFX/Headers/_cfx_globals.h	/^	short	LoopDelay100us;		\/\/ plug into LoopModeDelay() for 100 us delay$/;"	m	struct:__anon49
LoopDelay10us	hdr/CFX/Headers/_cfx_globals.h	/^	short	LoopDelay10us;		\/\/ plug into LoopModeDelay() for 10 us delay$/;"	m	struct:__anon49
LoopDelay1ms	hdr/CFX/Headers/_cfx_globals.h	/^	short	LoopDelay1ms;		\/\/ plug into LoopModeDelay() for 1 ms delay$/;"	m	struct:__anon49
LoopDelay1us	hdr/CFX/Headers/_cfx_globals.h	/^	short	LoopDelay1us;		\/\/ plug into LoopModeDelay() for 1 us delay$/;"	m	struct:__anon49
LoopDelay200us	hdr/CFX/Headers/_cfx_globals.h	/^	short	LoopDelay200us;		\/\/ plug into LoopModeDelay() for 200 us delay$/;"	m	struct:__anon49
LoopDelay20us	hdr/CFX/Headers/_cfx_globals.h	/^	short	LoopDelay20us;		\/\/ plug into LoopModeDelay() for 20 us delay$/;"	m	struct:__anon49
LoopDelay2us	hdr/CFX/Headers/_cfx_globals.h	/^	short	LoopDelay2us;		\/\/ plug into LoopModeDelay() for 2 us delay$/;"	m	struct:__anon49
LoopDelay500us	hdr/CFX/Headers/_cfx_globals.h	/^	short	LoopDelay500us;		\/\/ plug into LoopModeDelay() for 500 us delay$/;"	m	struct:__anon49
LoopDelay50us	hdr/CFX/Headers/_cfx_globals.h	/^	short	LoopDelay50us;		\/\/ plug into LoopModeDelay() for 50 us delay$/;"	m	struct:__anon49
LoopDelay5us	hdr/CFX/Headers/_cfx_globals.h	/^	short	LoopDelay5us;		\/\/ plug into LoopModeDelay() for 5 us delay$/;"	m	struct:__anon49
MAGIC_RESUME_KEY_HI	hdr/CFX/Headers/_cfx_internals.h	53;"	d
MAGIC_RESUME_KEY_LO	hdr/CFX/Headers/_cfx_internals.h	54;"	d
MAGIC_RESUME_RAM	hdr/CFX/Headers/_cfx_memmap.h	69;"	d
MAGIC_RESUME_RAM	hdr/CFX/Headers/_cfx_memmap.h	89;"	d
MAX3222	hdr/CFX/Headers/_cfx_internals.h	/^enum { MAX3222 = 1, MAX3223 };			\/\/ driver chip ID$/;"	e	enum:ResetResume::__anon58
MAX3223	hdr/CFX/Headers/_cfx_internals.h	/^enum { MAX3222 = 1, MAX3223 };			\/\/ driver chip ID$/;"	e	enum:ResetResume::__anon58
MAX_CLUSTER_COUNT_12_BIT	hdr/CFX/Headers/dosdrive.h	112;"	d
MAX_CLUSTER_COUNT_16_BIT	hdr/CFX/Headers/dosdrive.h	113;"	d
MAX_PIT_CHORES	hdr/CFX/Headers/_cfx_time.h	66;"	d
MAX_PWR_CHG_CHORES	hdr/CFX/Headers/_cfx_power.h	78;"	d
MAX_RTC_CHORES	hdr/CFX/Headers/_cfx_internals.h	603;"	d
MAX_TMG_CHG_CHORES	hdr/CFX/Headers/_cfx_internals.h	777;"	d
MBR_ERRORS	hdr/CFX/Headers/_cfx_errors.h	140;"	d
MBR_MAX_PARTS	hdr/CFX/Headers/_cfx_internals.h	1207;"	d
MB_CUR_MAX	hdr/CPU32/Headers/StdC/stdlib.h	14;"	d
MB_LEN_MAX	hdr/CPU32/Headers/StdC/limits.h	25;"	d
MB_SIZE	hdr/CFX/Headers/dosdrive.h	89;"	d
MC2CNT	hdr/CFX/Headers/mc68338.h	/^	vushort		MC2CNT;		\/\/ $YFF412 MCSM2 Counter Register $/;"	m	struct:__anon22
MC2ML	hdr/CFX/Headers/mc68338.h	/^	vushort		MC2ML;		\/\/ $YFF414 MCSM2 Modulus Latch $/;"	m	struct:__anon22
MC2SICR	hdr/CFX/Headers/mc68338.h	/^	vushort		MC2SICR;	\/\/ $YFF410 MCSM2 Status\/Interrupt\/Control Register $/;"	m	struct:__anon22
MC30CNT	hdr/CFX/Headers/mc68338.h	/^	vushort		MC30CNT;	\/\/ $YFF4F2 MCSM30 Counter Register $/;"	m	struct:__anon22
MC30ML	hdr/CFX/Headers/mc68338.h	/^	vushort		MC30ML;		\/\/ $YFF4F4 MCSM30 Modulus Latch $/;"	m	struct:__anon22
MC30SIC	hdr/CFX/Headers/mc68338.h	/^	vushort		MC30SIC;	\/\/ $YFF4F0 MCSM30 Status\/Interrupt\/Control Register $/;"	m	struct:__anon22
MC31CNT	hdr/CFX/Headers/mc68338.h	/^	vushort		MC31CNT;	\/\/ $YFF4FA MCSM31 Counter Register $/;"	m	struct:__anon22
MC31ML	hdr/CFX/Headers/mc68338.h	/^	vushort		MC31ML;		\/\/ $YFF4FC MCSM31 Modulus Latch $/;"	m	struct:__anon22
MC31SIC	hdr/CFX/Headers/mc68338.h	/^	vushort		MC31SIC;	\/\/ $YFF4F8 MCSM31 Status\/Interrupt\/Control Register $/;"	m	struct:__anon22
MCSM2	hdr/CFX/Headers/_cf1_ctm.h	/^typedef enum 	{ MCSM2 = 2, MCSM30 = 30, MCSM31 = 31 } ModulusCounterID;$/;"	e	enum:__anon23
MCSM30	hdr/CFX/Headers/_cf1_ctm.h	/^typedef enum 	{ MCSM2 = 2, MCSM30 = 30, MCSM31 = 31 } ModulusCounterID;$/;"	e	enum:__anon23
MCSM31	hdr/CFX/Headers/_cf1_ctm.h	/^typedef enum 	{ MCSM2 = 2, MCSM30 = 30, MCSM31 = 31 } ModulusCounterID;$/;"	e	enum:__anon23
MISO	hdr/CFX/Headers/_cf1_pins.h	/^	, MISO	= 20	\/\/ SPI Master Data In	I\/O		GPIO\/QSPI	1M	I+	I+$/;"	e	enum:__anon33
MISO	hdr/CFX/Headers/_cfx_pins.h	/^	, MISO	= 20	\/\/ SPI Master Data In	I\/O		GPIO\/QSPI	1M	I+	I+$/;"	e	enum:__anon75
MKDirCmd	hdr/CFX/Headers/_cfx_console.h	/^char	*MKDirCmd(CmdInfoPtr cip)					PICO_CALL(MKDirCmd);$/;"	v
MMDDYY	hdr/CFX/Headers/_cfx_types.h	/^		  				, MMDDYY	\/\/ US$/;"	e	enum:__anon97
MODCLK	hdr/CFX/Headers/_cf1_pins.h	/^	, MODCLK= 42	\/\/ Clk Source Sel\/GPIO	I\/O		GPIO\/CLK	10K	I+	I+$/;"	e	enum:__anon33
MODCLK	hdr/CFX/Headers/_cfx_pins.h	/^	, MODCLK= 42	\/\/ Clk Source Sel\/GPIO	I\/O		GPIO\/CLK	10K	I+	I+$/;"	e	enum:__anon75
MOSI	hdr/CFX/Headers/_cf1_pins.h	/^	, MOSI	= 18	\/\/ SPI Master Data Out	I\/O		GPIO\/QSPI		I?	O+$/;"	e	enum:__anon33
MOSI	hdr/CFX/Headers/_cfx_pins.h	/^	, MOSI	= 18	\/\/ SPI Master Data Out	I\/O		GPIO\/QSPI		I?	O+$/;"	e	enum:__anon75
Max146BIP	hdr/CFX/Headers/Drivers/Max146.h	/^enum { 	Max146UNI		= 0x08, 		Max146BIP		= 0x00	};$/;"	e	enum:__anon13
Max146CMD	hdr/CFX/Headers/Drivers/Max146.h	/^enum { 	Max146CMD		= 0x80	};$/;"	e	enum:__anon11
Max146DIF	hdr/CFX/Headers/Drivers/Max146.h	/^enum { 	Max146SGL		= 0x04, 		Max146DIF		= 0x00	};$/;"	e	enum:__anon12
Max146EXT	hdr/CFX/Headers/Drivers/Max146.h	/^		Max146INT		= 0x02, 		Max146EXT		= 0x03	};$/;"	e	enum:__anon14
Max146INT	hdr/CFX/Headers/Drivers/Max146.h	/^		Max146INT		= 0x02, 		Max146EXT		= 0x03	};$/;"	e	enum:__anon14
Max146PDFast	hdr/CFX/Headers/Drivers/Max146.h	/^enum { 	Max146PDFull	= 0x00,			Max146PDFast	= 0x01,$/;"	e	enum:__anon14
Max146PDFull	hdr/CFX/Headers/Drivers/Max146.h	/^enum { 	Max146PDFull	= 0x00,			Max146PDFast	= 0x01,$/;"	e	enum:__anon14
Max146SGL	hdr/CFX/Headers/Drivers/Max146.h	/^enum { 	Max146SGL		= 0x04, 		Max146DIF		= 0x00	};$/;"	e	enum:__anon12
Max146UNI	hdr/CFX/Headers/Drivers/Max146.h	/^enum { 	Max146UNI		= 0x08, 		Max146BIP		= 0x00	};$/;"	e	enum:__anon13
MemCmd	hdr/CFX/Headers/_cfx_console.h	/^char	*MemCmd(CmdInfoPtr cip)						PICO_CALL(MemCmd);$/;"	v
MemDispCmd	hdr/CFX/Headers/_cfx_console.h	/^char	*MemDispCmd(CmdInfoPtr cip)					PICO_CALL(MemDispCmd);$/;"	v
MemListCmd	hdr/CFX/Headers/_cfx_console.h	/^char	*MemListCmd(CmdInfoPtr cip)					PICO_CALL(MemListCmd);$/;"	v
MemModCmd	hdr/CFX/Headers/_cfx_console.h	/^char	*MemModCmd(CmdInfoPtr cip)					PICO_CALL(MemModCmd);$/;"	v
MemReadCmd	hdr/CFX/Headers/_cfx_console.h	/^char	*MemReadCmd(CmdInfoPtr cip)					PICO_CALL(MemReadCmd);$/;"	v
MemWriteCmd	hdr/CFX/Headers/_cfx_console.h	/^char	*MemWriteCmd(CmdInfoPtr cip)				PICO_CALL(MemWriteCmd);$/;"	v
ModeCmd	hdr/CFX/Headers/_cfx_console.h	/^char	*ModeCmd(CmdInfoPtr cip)					PICO_CALL(ModeCmd);$/;"	v
ModulusCounterID	hdr/CFX/Headers/_cf1_ctm.h	/^typedef enum 	{ MCSM2 = 2, MCSM30 = 30, MCSM31 = 31 } ModulusCounterID;$/;"	t	typeref:enum:__anon23
MountCmd	hdr/CFX/Headers/_cfx_console.h	/^char	*MountCmd(CmdInfoPtr cip)					PICO_CALL(MountCmd);$/;"	v
NAN	hdr/CFX/Headers/math.mx.h	56;"	d
NITC	hdr/CFX/Headers/_cfx_tpu.h	/^	NITC	= 0xA,	\/\/*REQ*	35	New Input Capture\/Input Transition Counter$/;"	e	enum:__anon88
NMPCS0	hdr/CFX/Headers/_cfx_expand.h	/^enum { NMPCS0 = 14, NMPCS1 = 13, NMPCS2 = 11, NMPCS3 = 7 };$/;"	e	enum:__anon48
NMPCS1	hdr/CFX/Headers/_cfx_expand.h	/^enum { NMPCS0 = 14, NMPCS1 = 13, NMPCS2 = 11, NMPCS3 = 7 };$/;"	e	enum:__anon48
NMPCS2	hdr/CFX/Headers/_cfx_expand.h	/^enum { NMPCS0 = 14, NMPCS1 = 13, NMPCS2 = 11, NMPCS3 = 7 };$/;"	e	enum:__anon48
NMPCS3	hdr/CFX/Headers/_cfx_expand.h	/^enum { NMPCS0 = 14, NMPCS1 = 13, NMPCS2 = 11, NMPCS3 = 7 };$/;"	e	enum:__anon48
NULL	hdr/CPU32/Headers/StdC/null.h	9;"	d
NULL	hdr/CPU32/Headers/StdC/wchar.h	10;"	d
NoEraXlateFlag	hdr/CFX/Headers/_cfx_internals.h	/^enum { NoEraXlateFlag = 0x01, CFMaster = 0x10 };	\/\/ > 228r1$/;"	e	enum:ResetResume::__anon54
NothingCmd	hdr/CFX/Headers/_cfx_console.h	/^char	*NothingCmd(CmdInfoPtr cip)					PICO_CALL(NothingCmd);$/;"	v
OC	hdr/CFX/Headers/_cfx_tpu.h	/^	OC		= 0x7,	\/\/		39	Output Compare$/;"	e	enum:__anon88
OEMData	hdr/CFX/Headers/_cfx_internals.h	/^typedef enum	{ PIIData, OEMData } SCSDataSrc;$/;"	e	enum:ResetResume::__anon71
OPEN_MAX	hdr/CFX/Headers/_cfx_files.h	54;"	d
O_APPEND	hdr/CFX/Headers/fcntl.h	/^	O_APPEND	=	0x0800,$/;"	e	enum:__anon15
O_BINARY	hdr/CFX/Headers/fcntl.h	/^	O_BINARY	=	0x8000$/;"	e	enum:__anon15
O_CREAT	hdr/CFX/Headers/fcntl.h	/^	O_CREAT		=	0x0100,$/;"	e	enum:__anon15
O_RDONLY	hdr/CFX/Headers/fcntl.h	/^	O_RDONLY	=	0,$/;"	e	enum:__anon15
O_RDWR	hdr/CFX/Headers/fcntl.h	/^	O_RDWR		=	2,$/;"	e	enum:__anon15
O_TRUNC	hdr/CFX/Headers/fcntl.h	/^	O_TRUNC		=	0x0200,$/;"	e	enum:__anon15
O_WRONLY	hdr/CFX/Headers/fcntl.h	/^	O_WRONLY	=	1,$/;"	e	enum:__anon15
OptCmd	hdr/CFX/Headers/_cfx_console.h	/^char	*OptCmd(CmdInfoPtr cip)						PICO_CALL(OptCmd);$/;"	v
P6Div128	hdr/CFX/Headers/_cf1_ctm.h	/^	P6Div64,		P6Div128,	P6Div256,	P6Div512,	\/\/ div by 2$/;"	e	enum:__anon28
P6Div192	hdr/CFX/Headers/_cf1_ctm.h	/^	P6Div96 = 0,	P6Div192,	P6Div384,	P6Div768	\/\/ div by 3$/;"	e	enum:__anon28
P6Div256	hdr/CFX/Headers/_cf1_ctm.h	/^	P6Div64,		P6Div128,	P6Div256,	P6Div512,	\/\/ div by 2$/;"	e	enum:__anon28
P6Div384	hdr/CFX/Headers/_cf1_ctm.h	/^	P6Div96 = 0,	P6Div192,	P6Div384,	P6Div768	\/\/ div by 3$/;"	e	enum:__anon28
P6Div512	hdr/CFX/Headers/_cf1_ctm.h	/^	P6Div64,		P6Div128,	P6Div256,	P6Div512,	\/\/ div by 2$/;"	e	enum:__anon28
P6Div64	hdr/CFX/Headers/_cf1_ctm.h	/^	P6Div64,		P6Div128,	P6Div256,	P6Div512,	\/\/ div by 2$/;"	e	enum:__anon28
P6Div768	hdr/CFX/Headers/_cf1_ctm.h	/^	P6Div96 = 0,	P6Div192,	P6Div384,	P6Div768	\/\/ div by 3$/;"	e	enum:__anon28
P6Div96	hdr/CFX/Headers/_cf1_ctm.h	/^	P6Div96 = 0,	P6Div192,	P6Div384,	P6Div768	\/\/ div by 3$/;"	e	enum:__anon28
P6DivisorID	hdr/CFX/Headers/_cf1_ctm.h	/^	} P6DivisorID;$/;"	t	typeref:enum:__anon28
PACChanControl	hdr/CFX/Headers/_cfx_tpu.h	/^enum	PACChanControl			\/\/ Input:			\/\/ Output:$/;"	g
PCLK1	hdr/CFX/Headers/_cf1_ctm.h	/^	PCLK1, PCLK2, PCLK3, PCLK4, PCLK5, PCLK6,$/;"	e	enum:__anon24
PCLK2	hdr/CFX/Headers/_cf1_ctm.h	/^	PCLK1, PCLK2, PCLK3, PCLK4, PCLK5, PCLK6,$/;"	e	enum:__anon24
PCLK3	hdr/CFX/Headers/_cf1_ctm.h	/^	PCLK1, PCLK2, PCLK3, PCLK4, PCLK5, PCLK6,$/;"	e	enum:__anon24
PCLK4	hdr/CFX/Headers/_cf1_ctm.h	/^	PCLK1, PCLK2, PCLK3, PCLK4, PCLK5, PCLK6,$/;"	e	enum:__anon24
PCLK5	hdr/CFX/Headers/_cf1_ctm.h	/^	PCLK1, PCLK2, PCLK3, PCLK4, PCLK5, PCLK6,$/;"	e	enum:__anon24
PCLK6	hdr/CFX/Headers/_cf1_ctm.h	/^	PCLK1, PCLK2, PCLK3, PCLK4, PCLK5, PCLK6,$/;"	e	enum:__anon24
PCMCIAHardDiskChkBsyDelay	hdr/CFX/Headers/_cfx_internals.h	/^	, PCMCIAHardDiskChkBsyDelay		= 1000000	\/\/ 1 second$/;"	e	enum:ResetResume::__anon65
PCMCIAHardDiskStartupTimeout	hdr/CFX/Headers/_cfx_internals.h	/^	, PCMCIAHardDiskStartupTimeout	= 5000000	\/\/ 5 seconds (typ. <1s)$/;"	e	enum:ResetResume::__anon65
PCS0	hdr/CFX/Headers/_cf1_pins.h	/^	, PCS0	= 21	\/\/ SPI Chip Select 0	I\/O		GPIO\/QSPI		I?	O+$/;"	e	enum:__anon33
PCS0	hdr/CFX/Headers/_cfx_pins.h	/^	, PCS0	= 21	\/\/ SPI Chip Select 0	I\/O		GPIO\/QSPI		I?	O+$/;"	e	enum:__anon75
PCS1	hdr/CFX/Headers/_cf1_pins.h	/^	, PCS1	= 19	\/\/ SPI Chip Select 1	I\/O		GPIO\/QSPI		I?	O+$/;"	e	enum:__anon33
PCS1	hdr/CFX/Headers/_cfx_pins.h	/^	, PCS1	= 19	\/\/ SPI Chip Select 1	I\/O		GPIO\/QSPI		I?	O+$/;"	e	enum:__anon75
PCS2	hdr/CFX/Headers/_cf1_pins.h	/^	, PCS2	= 15	\/\/ SPI Chip Select 2	I\/O		GPIO\/QSPI		I?	O+$/;"	e	enum:__anon33
PCS2	hdr/CFX/Headers/_cfx_pins.h	/^	, PCS2	= 15	\/\/ SPI Chip Select 2	I\/O		GPIO\/QSPI		I?	O+$/;"	e	enum:__anon75
PCS3	hdr/CFX/Headers/_cf1_pins.h	/^	, PCS3	= 17	\/\/ SPI Chip Select 3	I\/O		GPIO\/QSPI		I?	O+$/;"	e	enum:__anon33
PCS3	hdr/CFX/Headers/_cfx_pins.h	/^	, PCS3	= 17	\/\/ SPI Chip Select 3	I\/O		GPIO\/QSPI		I?	O+$/;"	e	enum:__anon75
PDCLINK	hdr/CFX/Headers/_cfx_console.h	332;"	d
PDX_ERRORS	hdr/CFX/Headers/_cfx_errors.h	52;"	d
PDX_OPEN_MAX	hdr/CFX/Headers/_cfx_files.h	55;"	d
PEPAR	hdr/CFX/Headers/mc68332.h	/^	vuchar		PEPAR;		\/\/ $YFFA17 Port E Pin Assignment [S]$/;"	m	struct:__anon16
PEPAR	hdr/CFX/Headers/mc68338.h	/^	vuchar		PEPAR;		\/\/ $YFFA17 Port E Pin Assignment [S]$/;"	m	struct:__anon20
PFPAR	hdr/CFX/Headers/mc68332.h	/^	vuchar		PFPAR;		\/\/ $YFFA1F Port F Pin Assignment [S]$/;"	m	struct:__anon16
PFPAR	hdr/CFX/Headers/mc68338.h	/^	vuchar		PFPAR;		\/\/ $YFFA1F Port F Pin Assignment [S]$/;"	m	struct:__anon20
PICOHandlerAddress	hdr/CFX/Headers/_cfx_cpulevel.h	55;"	d
PICOLIST	hdr/CFX/Headers/_cfx_linktab.h	367;"	d
PICOPatchInsert	hdr/CFX/Headers/_cfx_cpulevel.h	54;"	d
PICORelease	hdr/CFX/Headers/_cfx_globals.h	/^	uchar	PICORelease;		\/\/ fixes, no table changes REL*10+SUB$/;"	m	struct:__anon49
PICORelease	hdr/CFX/Headers/cfxpatch.h	/^	uchar	PICORelease;		\/\/ 0 for PicoDOS x.00, 25 for x.25, etc.$/;"	m	struct:__anon2
PICOVersion	hdr/CFX/Headers/_cfx_globals.h	/^	uchar	PICOVersion;		\/\/ must be equal or greater than your app$/;"	m	struct:__anon49
PICOVersion	hdr/CFX/Headers/cfxpatch.h	/^	uchar	PICOVersion;		\/\/ 1 for PicoDOS 1.xx, 2 for 2.xx, etc.$/;"	m	struct:__anon2
PICO_BETA	hdr/CFX/Headers/cfxpico.h	53;"	d
PICO_BUILD	hdr/CFX/Headers/cfxpico.h	50;"	d
PICO_CALL	hdr/CFX/Headers/_cfx_linktab.h	370;"	d
PICO_DEF_BEGIN	hdr/CFX/Headers/_cfx_linktab.h	363;"	d
PICO_DEF_END	hdr/CFX/Headers/_cfx_linktab.h	364;"	d
PICO_FDTAB_BASE	hdr/CFX/Headers/_cfx_memmap.h	94;"	d
PICO_FDTAB_SIZE	hdr/CFX/Headers/_cfx_memmap.h	95;"	d
PICO_NAME_MAX	hdr/CFX/Headers/_cfx_drives.h	126;"	d
PICO_PATCH	hdr/CFX/Headers/cfxpico.h	51;"	d
PICO_PATH_MAX	hdr/CFX/Headers/_cfx_drives.h	125;"	d
PICO_RELEASE	hdr/CFX/Headers/cfxpico.h	47;"	d
PICO_SECTOR_SIZE	hdr/CFX/Headers/_cfx_internals.h	879;"	d
PICO_SUBREL	hdr/CFX/Headers/cfxpico.h	48;"	d
PICO_VERSION	hdr/CFX/Headers/cfxpico.h	46;"	d
PICR	hdr/CFX/Headers/mc68332.h	/^	vushort		PICR;		\/\/ $YFFA22 Periodic Interrupt Control Register [S]$/;"	m	struct:__anon16
PICR	hdr/CFX/Headers/mc68338.h	/^	vushort		PICR;		\/\/ $YFFA22 Periodic Interrupt Control Register [S]$/;"	m	struct:__anon20
PIIData	hdr/CFX/Headers/_cfx_internals.h	/^typedef enum	{ PIIData, OEMData } SCSDataSrc;$/;"	e	enum:ResetResume::__anon71
PIO17A	hdr/CFX/Headers/mc68338.h	/^	vushort		PIO17A;		\/\/ $YFF488 PIOSM17A I\/O Port Register $/;"	m	struct:__anon22
PIOBusFunct	hdr/CFX/Headers/_cfx_pins.h	/^short	PIOBusFunct(short pin)						BIOS_CALL(PIOBusFunct);$/;"	v
PIOClear	hdr/CFX/Headers/_cfx_pins.h	/^short	PIOClear(short pin)							BIOS_CALL(PIOClear);$/;"	v
PIOMirror	hdr/CFX/Headers/_cfx_pins.h	/^short	PIOMirror(short pin)						BIOS_CALL(PIOMirror);$/;"	v
PIOMirrorList	hdr/CFX/Headers/_cfx_pins.h	/^void	PIOMirrorList(uchar *pinlist)				BIOS_CALL(PIOMirrorList);$/;"	v
PIORead	hdr/CFX/Headers/_cfx_pins.h	/^short	PIORead(short pin)							BIOS_CALL(PIORead);$/;"	v
PIOSet	hdr/CFX/Headers/_cfx_pins.h	/^short	PIOSet(short pin)							BIOS_CALL(PIOSet);$/;"	v
PIOTestAssertClear	hdr/CFX/Headers/_cfx_pins.h	/^short	PIOTestAssertClear(short pin)				BIOS_CALL(PIOTestAssertClear);$/;"	v
PIOTestAssertSet	hdr/CFX/Headers/_cfx_pins.h	/^short	PIOTestAssertSet(short pin)					BIOS_CALL(PIOTestAssertSet);$/;"	v
PIOTestIsItBus	hdr/CFX/Headers/_cfx_pins.h	/^short	PIOTestIsItBus(short pin)					BIOS_CALL(PIOTestIsItBus);$/;"	v
PIOToggle	hdr/CFX/Headers/_cfx_pins.h	/^short	PIOToggle(short pin)						BIOS_CALL(PIOToggle);$/;"	v
PIOWrite	hdr/CFX/Headers/_cfx_pins.h	/^short	PIOWrite(short pin, short value)			BIOS_CALL(PIOWrite);$/;"	v
PIO_ERR_TRAP	hdr/CFX/Headers/_cfx_errors.h	/^	, PIO_ERR_TRAP				\/\/ 1 for detecting invalid PIO requests$/;"	e	enum:__anon44
PIO_MAX_PIN	hdr/CFX/Headers/_cfx_internals.h	481;"	d
PIT100Hz	hdr/CFX/Headers/_cfx_time.h	/^			PIT125Hz = 80,	PIT100Hz = 100,	PIT50Hz = 200,	PIT40Hz = 250,$/;"	e	enum:__anon85
PIT10kHz	hdr/CFX/Headers/_cfx_time.h	/^enum	{	PIT10kHz = 1,	PIT5kHz = 2,	PIT2500Hz = 4,	PIT2kHz = 5,$/;"	e	enum:__anon85
PIT125Hz	hdr/CFX/Headers/_cfx_time.h	/^			PIT125Hz = 80,	PIT100Hz = 100,	PIT50Hz = 200,	PIT40Hz = 250,$/;"	e	enum:__anon85
PIT1kHz	hdr/CFX/Headers/_cfx_time.h	/^			PIT1kHz = 10,	PIT500Hz = 20,	PIT250Hz = 40,	PIT200Hz = 50,$/;"	e	enum:__anon85
PIT200Hz	hdr/CFX/Headers/_cfx_time.h	/^			PIT1kHz = 10,	PIT500Hz = 20,	PIT250Hz = 40,	PIT200Hz = 50,$/;"	e	enum:__anon85
PIT2500Hz	hdr/CFX/Headers/_cfx_time.h	/^enum	{	PIT10kHz = 1,	PIT5kHz = 2,	PIT2500Hz = 4,	PIT2kHz = 5,$/;"	e	enum:__anon85
PIT250Hz	hdr/CFX/Headers/_cfx_time.h	/^			PIT1kHz = 10,	PIT500Hz = 20,	PIT250Hz = 40,	PIT200Hz = 50,$/;"	e	enum:__anon85
PIT2kHz	hdr/CFX/Headers/_cfx_time.h	/^enum	{	PIT10kHz = 1,	PIT5kHz = 2,	PIT2500Hz = 4,	PIT2kHz = 5,$/;"	e	enum:__anon85
PIT40Hz	hdr/CFX/Headers/_cfx_time.h	/^			PIT125Hz = 80,	PIT100Hz = 100,	PIT50Hz = 200,	PIT40Hz = 250,$/;"	e	enum:__anon85
PIT500Hz	hdr/CFX/Headers/_cfx_time.h	/^			PIT1kHz = 10,	PIT500Hz = 20,	PIT250Hz = 40,	PIT200Hz = 50,$/;"	e	enum:__anon85
PIT50Hz	hdr/CFX/Headers/_cfx_time.h	/^			PIT125Hz = 80,	PIT100Hz = 100,	PIT50Hz = 200,	PIT40Hz = 250,$/;"	e	enum:__anon85
PIT5kHz	hdr/CFX/Headers/_cfx_time.h	/^enum	{	PIT10kHz = 1,	PIT5kHz = 2,	PIT2500Hz = 4,	PIT2kHz = 5,$/;"	e	enum:__anon85
PITAddChore	hdr/CFX/Headers/_cfx_time.h	/^bool	PITAddChore(vfptr chore, ushort intReqLevel)BIOS_CALL(PITAddChore);$/;"	v
PITInit	hdr/CFX/Headers/_cfx_time.h	/^void	PITInit(ushort intReqLevel)					BIOS_CALL(PITInit);$/;"	v
PITLPOff	hdr/CFX/Headers/_cfx_time.h	/^			PITOff = 0, PITLPOff = 0x100	};$/;"	e	enum:__anon85
PITLsCount	hdr/CFX/Headers/_cfx_globals.h	/^	ulong	PITLsCount;			\/\/ periodic interrupt count least sig lword	$/;"	m	struct:__anon49
PITMsCount	hdr/CFX/Headers/_cfx_globals.h	/^	ulong	PITMsCount;			\/\/ periodic interrupt count most sig lword	$/;"	m	struct:__anon49
PITOff	hdr/CFX/Headers/_cfx_time.h	/^			PITOff = 0, PITLPOff = 0x100	};$/;"	e	enum:__anon85
PITPeriod	hdr/CFX/Headers/_cfx_time.h	/^ulong	PITPeriod(void)								BIOS_CALL(PITPeriod);$/;"	v
PITR	hdr/CFX/Headers/mc68332.h	/^	vushort		PITR;		\/\/ $YFFA24 Periodic Interrupt Timer Register [S]$/;"	m	struct:__anon16
PITR	hdr/CFX/Headers/mc68338.h	/^	vushort		PITR;		\/\/ $YFFA24 Periodic Interrupt Timer Register [S]$/;"	m	struct:__anon20
PITRemoveChore	hdr/CFX/Headers/_cfx_time.h	/^bool	PITRemoveChore(vfptr chore)					BIOS_CALL(PITRemoveChore);$/;"	v
PITSet100usPeriod	hdr/CFX/Headers/_cfx_time.h	/^void	PITSet100usPeriod(uchar per100us)			BIOS_CALL(PITSet100usPeriod);$/;"	v
PITSet51msPeriod	hdr/CFX/Headers/_cfx_time.h	/^void	PITSet51msPeriod(uchar per51ms)				BIOS_CALL(PITSet51msPeriod);$/;"	v
PIT_ERR_TRAP	hdr/CFX/Headers/_cfx_errors.h	/^	, PIT_ERR_TRAP				\/\/ 5 for detecting PIT chore errors$/;"	e	enum:__anon44
PITchrCompat	hdr/CFX/Headers/_cfx_time.h	/^	PITchrCompat =	0,		\/\/ default is compatible with all pre-2.2.2.6 releases$/;"	e	enum:__anon86
PITchrRunLowHi	hdr/CFX/Headers/_cfx_time.h	/^	PITchrRunLowHi =0x40,	\/\/ Run all chores at PIT intReqLevel (default: 6)$/;"	e	enum:__anon86
PITchrSkipLow	hdr/CFX/Headers/_cfx_time.h	/^	PITchrSkipLow =	0x20,	\/\/ Only run chores >= current mask level$/;"	e	enum:__anon86
PITchrTrapInv	hdr/CFX/Headers/_cfx_time.h	/^	PITchrTrapInv =	0x80	\/\/ Take TRAP 5 on possible priority inversions$/;"	e	enum:__anon86
PORTC	hdr/CFX/Headers/mc68332.h	/^	vuchar		PORTC;		\/\/ $YFFA41 Port C Data [S\/U]$/;"	m	struct:__anon16
PORTC	hdr/CFX/Headers/mc68338.h	/^	vuchar		PORTC;		\/\/ $YFFA41 Port C Data [S\/U]$/;"	m	struct:__anon20
PORTE	hdr/CFX/Headers/mc68332.h	/^	vuchar		PORTE;		\/\/ $YFFA13 Port E Data [S\/U]$/;"	m	struct:__anon16
PORTE	hdr/CFX/Headers/mc68338.h	/^	vuchar		PORTE;		\/\/ $YFFA13 Port E Data [S\/U]$/;"	m	struct:__anon20
PORTE0	hdr/CFX/Headers/mc68332.h	/^	vuchar		PORTE0;		\/\/ $YFFA11 Port E Data [S\/U]$/;"	m	struct:__anon16
PORTE0	hdr/CFX/Headers/mc68338.h	/^	vuchar		PORTE0;		\/\/ $YFFA11 Port E Data [S\/U]$/;"	m	struct:__anon20
PORTF	hdr/CFX/Headers/mc68332.h	/^	vuchar		PORTF;		\/\/ $YFFA1B Port F Data [S\/U]$/;"	m	struct:__anon16
PORTF	hdr/CFX/Headers/mc68338.h	/^	vuchar		PORTF;		\/\/ $YFFA1B Port F Data [S\/U]$/;"	m	struct:__anon20
PORTF0	hdr/CFX/Headers/mc68332.h	/^	vuchar		PORTF0;		\/\/ $YFFA19 Port F Data [S\/U]$/;"	m	struct:__anon16
PORTF0	hdr/CFX/Headers/mc68338.h	/^	vuchar		PORTF0;		\/\/ $YFFA19 Port F Data [S\/U]$/;"	m	struct:__anon20
PORTQS	hdr/CFX/Headers/mc68332.h	/^	vuchar		PORTQS;		\/\/ $YFFC15 PQS Data Register [S\/U]$/;"	m	struct:__anon17
PORTQS	hdr/CFX/Headers/mc68338.h	/^	vuchar		PORTQS;		\/\/ $YFFC15 PQS Data Register [S\/U]$/;"	m	struct:__anon21
PPBCheckRdAvail	hdr/CFX/Headers/_cfx_util.h	/^long	PPBCheckRdAvail(void *ppb)					PICO_CALL(PPBCheckRdAvail);$/;"	v
PPBCheckWrFree	hdr/CFX/Headers/_cfx_util.h	/^long	PPBCheckWrFree(void *ppb)					PICO_CALL(PPBCheckWrFree);$/;"	v
PPBClose	hdr/CFX/Headers/_cfx_util.h	/^void	PPBClose(void *ppb)							PICO_CALL(PPBClose);$/;"	v
PPBErrorCode	hdr/CFX/Headers/_cfx_util.h	/^short	PPBErrorCode(void *ppb, bool clear)			PICO_CALL(PPBErrorCode);$/;"	v
PPBFlush	hdr/CFX/Headers/_cfx_util.h	/^short	PPBFlush(void *ppb)							PICO_CALL(PPBFlush);$/;"	v
PPBGetMemBuf	hdr/CFX/Headers/_cfx_util.h	/^			bool flush)								PICO_CALL(PPBGetMemBuf);$/;"	v
PPBOpen	hdr/CFX/Headers/_cfx_util.h	/^			vfptr ppnotify)							PICO_CALL(PPBOpen);$/;"	v
PPBPutByte	hdr/CFX/Headers/_cfx_util.h	/^short	PPBPutByte(void *ppb, uchar byte)			PICO_CALL(PPBPutByte);$/;"	v
PPBPutWord	hdr/CFX/Headers/_cfx_util.h	/^short	PPBPutWord(void *ppb, ushort word)			PICO_CALL(PPBPutWord);$/;"	v
PPBRdf	hdr/CFX/Headers/_cfx_util.h	/^typedef long 	PPBRdf(void *buf, void *rdp, ulong rdofs, ulong n);$/;"	t
PPBRead	hdr/CFX/Headers/_cfx_util.h	/^long	PPBRead(void *ppb, void *buf, long nbyte)	PICO_CALL(PPBRead);$/;"	v
PPBWrf	hdr/CFX/Headers/_cfx_util.h	/^typedef long 	PPBWrf(void *buf, void *wrp, ulong wrofs, ulong n);$/;"	t
PPBWrite	hdr/CFX/Headers/_cfx_util.h	/^long	PPBWrite(void *ppb, void *buf, long nbyte)	PICO_CALL(PPBWrite);$/;"	v
PPB_ERRORS	hdr/CFX/Headers/_cfx_errors.h	153;"	d
PQSPAR	hdr/CFX/Headers/mc68332.h	/^	vuchar		PQSPAR;		\/\/ $YFFC16 PQS Pin Assignment Register [S\/U]$/;"	m	struct:__anon17
PQSPAR	hdr/CFX/Headers/mc68338.h	/^	vuchar		PQSPAR;		\/\/ $YFFC16 PQS Pin Assignment Register [S\/U]$/;"	m	struct:__anon21
PRAM	hdr/CFX/Headers/mc68332.h	/^	vushort		PRAM[16][8];\/\/ $YFFF00-$YFFFFF	Channel Parameter RAM$/;"	m	struct:__anon18
PRAM_W	hdr/CFX/Headers/tpu68332.h	55;"	d
PRCHCTL	hdr/CFX/Headers/tpu68332.h	56;"	d
PRES	hdr/CFX/Headers/_cfx_tpu.h	/^	PRES	= 0xE,	\/\/*REQ*	8	Reserved for PicoDOS (8 to 40 long words)$/;"	e	enum:__anon88
PRMATCH	hdr/CFX/Headers/tpu68332.h	58;"	d
PRPINLV	hdr/CFX/Headers/tpu68332.h	57;"	d
PSCChanControl	hdr/CFX/Headers/_cfx_tpu.h	/^enum	PSCChanControl$/;"	g
PTA	hdr/CFX/Headers/_cfx_tpu.h	/^	PTA		= 0xC,	\/\/*REQ*	63	Programmable Time Accumulator$/;"	e	enum:__anon88
PVer	hdr/CFX/Headers/_cfx_globals.h	/^	}	PVer;	\/\/ use explicit (PVer *) typecast with picodosver()$/;"	t	typeref:struct:__anon50
PWDPeriodMax	hdr/CFX/Headers/_cfx_globals.h	/^	short	PWDPeriodMax;		\/\/ PIC watchdog maximum period in milliseconds$/;"	m	struct:__anon49
PWDPeriodMin	hdr/CFX/Headers/_cfx_globals.h	/^	short	PWDPeriodMin;		\/\/ PIC watchdog minimum period in milliseconds$/;"	m	struct:__anon49
PWM	hdr/CFX/Headers/_cfx_tpu.h	/^	PWM		= 0xB,	\/\/*REQ*	40	Pulse Width Modulation$/;"	e	enum:__anon88
PWRLPStopSetup	hdr/CFX/Headers/_cfx_power.h	/^			bool stext)								PICO_CALL(PWRLPStopSetup);$/;"	v
PWRPostChgAddChore	hdr/CFX/Headers/_cfx_power.h	/^			ushort priority)						PICO_CALL(PWRPostChgAddChore);$/;"	v
PWRPostChgRemoveChore	hdr/CFX/Headers/_cfx_power.h	/^bool	PWRPostChgRemoveChore(vfptr chore)			PICO_CALL(PWRPostChgRemoveChore);$/;"	v
PWRPreChgAddChore	hdr/CFX/Headers/_cfx_power.h	/^			ushort priority)						PICO_CALL(PWRPreChgAddChore);$/;"	v
PWRPreChgRemoveChore	hdr/CFX/Headers/_cfx_power.h	/^bool	PWRPreChgRemoveChore(vfptr chore)			PICO_CALL(PWRPreChgRemoveChore);$/;"	v
PWRSuspendSecs	hdr/CFX/Headers/_cfx_power.h	/^			bool resume, short WhatWakesSuspend)	PICO_CALL(PWRSuspendSecs);$/;"	v
PWRSuspendTicks	hdr/CFX/Headers/_cfx_power.h	/^			bool resume, short WhatWakesSuspend)	PICO_CALL(PWRSuspendTicks);$/;"	v
PWRSuspendUntil	hdr/CFX/Headers/_cfx_power.h	/^			bool resume, short WhatWakesSuspend)	PICO_CALL(PWRSuspendUntil);$/;"	v
PZCacheFlush	hdr/CFX/Headers/_cfx_drives.h	/^bool	PZCacheFlush(short logdrv)					PICO_CALL(PZCacheFlush);$/;"	v
PZCacheRelease	hdr/CFX/Headers/_cfx_drives.h	/^bool	PZCacheRelease(short logdrv)				PICO_CALL(PZCacheRelease);$/;"	v
PZCacheSetup	hdr/CFX/Headers/_cfx_drives.h	/^			Callocf *callocf, Freef *freef)			PICO_CALL(PZCacheSetup);$/;"	v
PatchInfo	hdr/CFX/Headers/cfxpatch.h	/^	}	PatchInfo;$/;"	t	typeref:struct:__anon2
PatchRelease	hdr/CFX/Headers/cfxpatch.h	/^	uchar	PatchRelease;		\/\/ 0 for patch.lib x.00, 25 for x.25, etc.$/;"	m	struct:__anon2
PatchVersion	hdr/CFX/Headers/cfxpatch.h	/^	uchar	PatchVersion;		\/\/ 1 for patch.lib 1.xx, 2 for 2.xx, etc.$/;"	m	struct:__anon2
PathCmd	hdr/CFX/Headers/_cfx_console.h	/^char	*PathCmd(CmdInfoPtr cip)					PICO_CALL(PathCmd);$/;"	v
PauseCmd	hdr/CFX/Headers/_cfx_console.h	/^char	*PauseCmd(CmdInfoPtr cip)					PICO_CALL(PauseCmd);$/;"	v
PicoZoomCmd	hdr/CFX/Headers/_cfx_console.h	/^char	*PicoZoomCmd(CmdInfoPtr cip)				PICO_CALL(PicoZoomCmd);$/;"	v
PinBus	hdr/CFX/Headers/_cfx_internals.h	500;"	d
PinClear	hdr/CFX/Headers/_cfx_internals.h	503;"	d
PinFunctDefEFQ	hdr/CFX/Headers/_cfx_internals.h	514;"	d
PinID	hdr/CFX/Headers/_cf1_pins.h	/^	} PinID;$/;"	t	typeref:enum:__anon33
PinID	hdr/CFX/Headers/_cfx_pins.h	/^	} PinID;$/;"	t	typeref:enum:__anon75
PinIO	hdr/CFX/Headers/_cfx_internals.h	501;"	d
PinIODefineCTM	hdr/CFX/Headers/_cfx_internals.h	526;"	d
PinIODefineEFQ	hdr/CFX/Headers/_cfx_internals.h	519;"	d
PinIODefineTPU	hdr/CFX/Headers/_cfx_internals.h	532;"	d
PinMirror	hdr/CFX/Headers/_cfx_internals.h	508;"	d
PinRead	hdr/CFX/Headers/_cfx_internals.h	505;"	d
PinSet	hdr/CFX/Headers/_cfx_internals.h	502;"	d
PinTest	hdr/CFX/Headers/_cfx_internals.h	504;"	d
PinTestAssertClear	hdr/CFX/Headers/_cfx_internals.h	512;"	d
PinTestAssertSet	hdr/CFX/Headers/_cfx_internals.h	511;"	d
PinTestIsItBus	hdr/CFX/Headers/_cfx_internals.h	509;"	d
PinToggle	hdr/CFX/Headers/_cfx_internals.h	507;"	d
PinWrite	hdr/CFX/Headers/_cfx_internals.h	506;"	d
PortClrCmd	hdr/CFX/Headers/_cfx_console.h	/^char	*PortClrCmd(CmdInfoPtr cip)					PICO_CALL(PortClrCmd);$/;"	v
PortMirrorCmd	hdr/CFX/Headers/_cfx_console.h	/^char	*PortMirrorCmd(CmdInfoPtr cip)				PICO_CALL(PortMirrorCmd);$/;"	v
PortReadCmd	hdr/CFX/Headers/_cfx_console.h	/^char	*PortReadCmd(CmdInfoPtr cip)				PICO_CALL(PortReadCmd);$/;"	v
PortSetCmd	hdr/CFX/Headers/_cfx_console.h	/^char	*PortSetCmd(CmdInfoPtr cip)					PICO_CALL(PortSetCmd);$/;"	v
PortToggleCmd	hdr/CFX/Headers/_cfx_console.h	/^char	*PortToggleCmd(CmdInfoPtr cip)				PICO_CALL(PortToggleCmd);$/;"	v
PromptCmd	hdr/CFX/Headers/_cfx_console.h	/^char	*PromptCmd(CmdInfoPtr cip)					PICO_CALL(PromptCmd);$/;"	v
Protected	hdr/CFX/Headers/_cfx_internals.h	/^typedef enum	{ Visible, Protected } SCSDataType;$/;"	e	enum:ResetResume::__anon72
QCR	hdr/CFX/Headers/mc68332.h	/^	vuchar		QCR[16];	\/\/ $YFFD40 Ð $YFFD4F Command RAM [S\/U]$/;"	m	struct:__anon17
QCR	hdr/CFX/Headers/mc68338.h	/^	vuchar		QCR[16];	\/\/ $YFFD40 Ð $YFFD4F Command RAM [S\/U]$/;"	m	struct:__anon21
QILR	hdr/CFX/Headers/mc68332.h	/^	vuchar		QILR;		\/\/ $YFFC04 QSM Interrupt Level Register [S]$/;"	m	struct:__anon17
QILR	hdr/CFX/Headers/mc68338.h	/^	vuchar		QILR;		\/\/ $YFFC04 QSM Interrupt Level Register [S]$/;"	m	struct:__anon21
QIVR	hdr/CFX/Headers/mc68332.h	/^	vuchar		QIVR;		\/\/ $YFFC05 QSM Interrupt Vector Register [S]$/;"	m	struct:__anon17
QIVR	hdr/CFX/Headers/mc68338.h	/^	vuchar		QIVR;		\/\/ $YFFC05 QSM Interrupt Vector Register [S]$/;"	m	struct:__anon21
QOM	hdr/CFX/Headers/_cfx_tpu.h	/^	QOM		= 0x8,	\/\/		49	Queued Output Match$/;"	e	enum:__anon88
QPB	hdr/CFX/Headers/_cfx_expand.h	/^	} QPB;$/;"	t	typeref:struct:__anon47
QPBClearBusy	hdr/CFX/Headers/_cfx_expand.h	/^void	*QPBClearBusy(void)							PICO_CALL(QPBClearBusy);$/;"	v
QPBDev	hdr/CFX/Headers/_cfx_expand.h	/^	} QPBDev;$/;"	t	typeref:struct:qpbDev
QPBFreeSlot	hdr/CFX/Headers/_cfx_expand.h	/^void	QPBFreeSlot(QPBDev *qbpd)					PICO_CALL(QPBFreeSlot);$/;"	v
QPBInitSlot	hdr/CFX/Headers/_cfx_expand.h	/^QPB		*QPBInitSlot(QPBDev *qbpd)					PICO_CALL(QPBInitSlot);$/;"	v
QPBLockSlot	hdr/CFX/Headers/_cfx_expand.h	/^bool	QPBLockSlot(QPB *qpb)						PICO_CALL(QPBLockSlot);$/;"	v
QPBSLOTS	hdr/CFX/Headers/_cfx_expand.h	84;"	d
QPBSetup	hdr/CFX/Headers/_cfx_expand.h	/^			ushort count, ushort *spidata)			PICO_CALL(QPBSetup);$/;"	v
QPBTestBusy	hdr/CFX/Headers/_cfx_expand.h	/^QPB		*QPBTestBusy(void)							PICO_CALL(QPBTestBusy);$/;"	v
QPBTestLocked	hdr/CFX/Headers/_cfx_expand.h	/^QPB		*QPBTestLocked(void)						PICO_CALL(QPBTestLocked);$/;"	v
QPBTransact	hdr/CFX/Headers/_cfx_expand.h	/^			ushort count, ushort *spidata)			PICO_CALL(QPBTransact);$/;"	v
QPBUnlockSlot	hdr/CFX/Headers/_cfx_expand.h	/^bool	QPBUnlockSlot(QPB *qpb)						PICO_CALL(QPBUnlockSlot);$/;"	v
QRR	hdr/CFX/Headers/mc68332.h	/^	vushort		QRR[16];	\/\/ $YFFD00 Ð $YFFD1F Receive RAM [S\/U]$/;"	m	struct:__anon17
QRR	hdr/CFX/Headers/mc68338.h	/^	vushort		QRR[16];	\/\/ $YFFD00 Ð $YFFD1F Receive RAM [S\/U]$/;"	m	struct:__anon21
QR_BUF_LEN	hdr/CFX/Headers/_cfx_console.h	114;"	d
QRchar	hdr/CFX/Headers/_cfx_console.h	/^			char *reply, char *instr, bool uc)		PICO_CALL(QRchar);$/;"	v
QRconfirm	hdr/CFX/Headers/_cfx_console.h	/^			bool crok)								PICO_CALL(QRconfirm);$/;"	v
QRdate	hdr/CFX/Headers/_cfx_console.h	/^			bool y4d, bool crok, struct tm *tp)		PICO_CALL(QRdate);$/;"	v
QRdatetime	hdr/CFX/Headers/_cfx_console.h	/^			bool y4d, bool crok, struct tm *tp)		PICO_CALL(QRdatetime);$/;"	v
QRdouble	hdr/CFX/Headers/_cfx_console.h	/^			double *value, double min, double max)	PICO_CALL(QRdouble);$/;"	v
QRfloat	hdr/CFX/Headers/_cfx_console.h	/^			float *value, float min, float max)		PICO_CALL(QRfloat);$/;"	v
QRlong	hdr/CFX/Headers/_cfx_console.h	/^			long *value, long min, long max)		PICO_CALL(QRlong);$/;"	v
QRshort	hdr/CFX/Headers/_cfx_console.h	/^			short *value, short min, short max)		PICO_CALL(QRshort);$/;"	v
QRstring	hdr/CFX/Headers/_cfx_console.h	/^			char *strbuf, short len)				PICO_CALL(QRstring);$/;"	v
QRtime	hdr/CFX/Headers/_cfx_console.h	/^			bool crok, struct tm *tp)				PICO_CALL(QRtime);$/;"	v
QRulong	hdr/CFX/Headers/_cfx_console.h	/^			ulong *value, ulong min, ulong max)		PICO_CALL(QRulong);$/;"	v
QRushort	hdr/CFX/Headers/_cfx_console.h	/^			ushort *value, ushort min, ushort max)	PICO_CALL(QRushort);$/;"	v
QSLOT_SPV	hdr/CFX/Headers/_cfx_expand.h	87;"	d
QSMCR	hdr/CFX/Headers/mc68332.h	/^	vushort		QSMCR;		\/\/ $YFFC00 QSM Module Configuration Register [S]$/;"	m	struct:__anon17
QSMCR	hdr/CFX/Headers/mc68338.h	/^	vushort		QSMCR;		\/\/ $YFFC00 QSM Module Configuration Register [S]$/;"	m	struct:__anon21
QSM_BASE_ADDR	hdr/CFX/Headers/mc68332.h	52;"	d
QSM_BASE_ADDR	hdr/CFX/Headers/mc68338.h	54;"	d
QSM_DEF_IARB	hdr/CFX/Headers/_cfx_internals.h	443;"	d
QSM_SCI_DEF_IPL	hdr/CFX/Headers/_cfx_internals.h	445;"	d
QSM_SPI_DEF_IPL	hdr/CFX/Headers/_cfx_internals.h	444;"	d
QTEST	hdr/CFX/Headers/mc68332.h	/^	vushort		QTEST;		\/\/ $YFFC02 QSM Test Register [S]$/;"	m	struct:__anon17
QTEST	hdr/CFX/Headers/mc68338.h	/^	vushort		QTEST;		\/\/ $YFFC02 QSM Test Register [S]$/;"	m	struct:__anon21
QTR	hdr/CFX/Headers/mc68332.h	/^	vushort		QTR[16];	\/\/ $YFFD20 Ð $YFFD3F Transmit RAM [S\/U]$/;"	m	struct:__anon17
QTR	hdr/CFX/Headers/mc68338.h	/^	vushort		QTR[16];	\/\/ $YFFD20 Ð $YFFD3F Transmit RAM [S\/U]$/;"	m	struct:__anon21
R16FRCH	hdr/CFX/Headers/mc68338.h	/^	vushort		R16FRCH;	\/\/ $YFF484 RTCSM16 32-Bit Free-Running Counter High $/;"	m	struct:__anon22
R16FRCL	hdr/CFX/Headers/mc68338.h	/^	vushort		R16FRCL;	\/\/ $YFF486 RTCSM16 32-Bit Free-Running Counter Low $/;"	m	struct:__anon22
R16PRR	hdr/CFX/Headers/mc68338.h	/^	vushort		R16PRR;		\/\/ $YFF482 RTCSM16 Prescaler Register $/;"	m	struct:__anon22
RAM32	hdr/CFX/Headers/mc68338.h	/^	vuchar		RAM32[32];	\/\/ $YFF500 Ð $YFF51E RAMSM32 $/;"	m	struct:__anon22
RAM36	hdr/CFX/Headers/mc68338.h	/^	vuchar		RAM36[32];	\/\/ $YFF520 Ð $YFF53E RAMSM36 $/;"	m	struct:__anon22
RAMSM_BASE_ADDR	hdr/CFX/Headers/_cfx_memmap.h	66;"	d
RAMSize	hdr/CFX/Headers/_cfx_globals.h	/^	ulong	RAMSize;			\/\/ size of SRAM in bytes$/;"	m	struct:__anon49
RAM_BASE_ADDR	hdr/CFX/Headers/_cfx_memmap.h	51;"	d
RAM_END_ADDR	hdr/CFX/Headers/_cfx_memmap.h	108;"	d
RAND_MAX	hdr/CPU32/Headers/StdC/stdlib.h	15;"	d
RMDirCmd	hdr/CFX/Headers/_cfx_console.h	/^char	*RMDirCmd(CmdInfoPtr cip)					PICO_CALL(RMDirCmd);$/;"	v
RSR	hdr/CFX/Headers/mc68332.h	/^	vuchar		RSR;		\/\/ $YFFA07 Reset Status Register [S]$/;"	m	struct:__anon16
RSR	hdr/CFX/Headers/mc68338.h	/^	vuchar		RSR;		\/\/ $YFFA07 Reset Status Register [S]$/;"	m	struct:__anon20
RTC16SIC	hdr/CFX/Headers/mc68338.h	/^	vushort		RTC16SIC;	\/\/ $YFF480 RTCSM16 Status\/Interrupt\/Control Register $/;"	m	struct:__anon22
RTCDelayMicroSeconds	hdr/CFX/Headers/_cfx_time.h	/^bool	RTCDelayMicroSeconds(ulong us)				BIOS_CALL(RTCDelayMicroSeconds);$/;"	v
RTCElapsedTime	hdr/CFX/Headers/_cfx_time.h	/^ulong	RTCElapsedTime(RTCTimer *rt)				BIOS_CALL(RTCElapsedTime);$/;"	v
RTCElapsedTimerSetup	hdr/CFX/Headers/_cfx_time.h	/^void	RTCElapsedTimerSetup(RTCTimer *rt)			BIOS_CALL(RTCElapsedTimerSetup);$/;"	v
RTCGetTime	hdr/CFX/Headers/_cfx_time.h	/^ulong	RTCGetTime(ulong *secs, ushort *ticks)		BIOS_CALL(RTCGetTime);$/;"	v
RTCSetTime	hdr/CFX/Headers/_cfx_time.h	/^void	RTCSetTime(ulong secs, ushort ticks)		BIOS_CALL(RTCSetTime);$/;"	v
RTCTickfCount	hdr/CFX/Headers/_cfx_globals.h	/^	ulong	RTCTickfCount;		\/\/ one second tick flag from RTC$/;"	m	struct:__anon49
RTCTimer	hdr/CFX/Headers/_cfx_time.h	/^	}	RTCTimer;$/;"	t	typeref:struct:__anon87
RTC_ERR_TRAP	hdr/CFX/Headers/_cfx_errors.h	/^	, RTC_ERR_TRAP				\/\/ 3 for detecting RTC off lockup errors$/;"	e	enum:__anon44
RTCchrCompat	hdr/CFX/Headers/_cfx_internals.h	/^	RTCchrCompat =	0,		\/\/ default is compatible with all pre-2.2.2.6 releases$/;"	e	enum:ResetResume::__anon59
RTCchrRunLowHi	hdr/CFX/Headers/_cfx_internals.h	/^	RTCchrRunLowHi =0x40,	\/\/ Run all chores at PIT intReqLevel (default: 6)$/;"	e	enum:ResetResume::__anon59
RTCchrSkipLow	hdr/CFX/Headers/_cfx_internals.h	/^	RTCchrSkipLow =	0x20,	\/\/ Only run chores >= current mask level$/;"	e	enum:ResetResume::__anon59
RTCchrTrapInv	hdr/CFX/Headers/_cfx_internals.h	/^	RTCchrTrapInv =	0x80	\/\/ Take TRAP 5 on possible priority inversions$/;"	e	enum:ResetResume::__anon59
RTCtm	hdr/CFX/Headers/_cfx_internals.h	/^	} RTCtm;	\/\/ same as ANSI struct tm$/;"	t	struct:ResetResume	typeref:struct:ResetResume::rtctm
RemCmd	hdr/CFX/Headers/_cfx_console.h	/^char	*RemCmd(CmdInfoPtr cip)						PICO_CALL(RemCmd);$/;"	v
RenCmd	hdr/CFX/Headers/_cfx_console.h	/^char	*RenCmd(CmdInfoPtr cip)						PICO_CALL(RenCmd);$/;"	v
ResResData	hdr/CFX/Headers/_cfx_internals.h	/^struct ResResData$/;"	s
ResetCmd	hdr/CFX/Headers/_cfx_console.h	/^char	*ResetCmd(CmdInfoPtr cip)					PICO_CALL(ResetCmd);$/;"	v
ResetResume	hdr/CFX/Headers/_cfx_internals.h	/^	}	ResetResume	:	MAGIC_RESUME_RAM;	\/\/ BIOS->PBM communications$/;"	s
ResetToPBMCmd	hdr/CFX/Headers/_cfx_console.h	/^char	*ResetToPBMCmd(CmdInfoPtr cip)				PICO_CALL(ResetToPBMCmd);$/;"	v
RxD_BOV_MASK	hdr/CFX/Headers/_cfx_sercomm.h	/^	, RxD_BOV_MASK	= 0x0800	\/\/ Buffer Overflow Flag$/;"	e	enum:__anon83
RxD_DATA7_MASK	hdr/CFX/Headers/_cfx_sercomm.h	/^	, RxD_DATA7_MASK= 0x007F	\/\/ Receive Data, 7 Bits (ASCII)$/;"	e	enum:__anon83
RxD_DATA8_MASK	hdr/CFX/Headers/_cfx_sercomm.h	/^	, RxD_DATA8_MASK= 0x00FF	\/\/ Receive Data, 8 Bits$/;"	e	enum:__anon83
RxD_DATA9_MASK	hdr/CFX/Headers/_cfx_sercomm.h	/^	, RxD_DATA9_MASK= 0x01FF	\/\/ Receive Data, 9 Bits$/;"	e	enum:__anon83
RxD_ERR_MASK	hdr/CFX/Headers/_cfx_sercomm.h	/^	, RxD_ERR_MASK	= 0xF800	\/\/ Any Error Flags$/;"	e	enum:__anon83
RxD_FE_MASK	hdr/CFX/Headers/_cfx_sercomm.h	/^	, RxD_FE_MASK	= 0x2000	\/\/ Framing Error Flag$/;"	e	enum:__anon83
RxD_NF_MASK	hdr/CFX/Headers/_cfx_sercomm.h	/^	, RxD_NF_MASK	= 0x4000	\/\/ Noise Error Flag$/;"	e	enum:__anon83
RxD_NO_DATA	hdr/CFX/Headers/_cfx_sercomm.h	/^	, RxD_NO_DATA	= 0xFCFF	\/\/ Return value for no data available$/;"	e	enum:__anon83
RxD_OR_MASK	hdr/CFX/Headers/_cfx_sercomm.h	/^	  RxD_OR_MASK	= 0x8000	\/\/ Overrun Error Flag$/;"	e	enum:__anon83
RxD_PF_MASK	hdr/CFX/Headers/_cfx_sercomm.h	/^	, RxD_PF_MASK	= 0x1000	\/\/ Parity Error Flag$/;"	e	enum:__anon83
RxD_RPTF_MASK	hdr/CFX/Headers/_cfx_sercomm.h	/^	, RxD_RPTF_MASK	= 0x0200	\/\/ Repeat Filter Call Flag (user sets)$/;"	e	enum:__anon83
RxD_TOSS_MASK	hdr/CFX/Headers/_cfx_sercomm.h	/^	, RxD_TOSS_MASK	= 0x0400	\/\/ Throw Away Flag (user sets)$/;"	e	enum:__anon83
S12DATA	hdr/CFX/Headers/mc68338.h	/^	vushort		S12DATA;	\/\/ $YFF462 SASM12 Data Register A $/;"	m	struct:__anon22
S12DATB	hdr/CFX/Headers/mc68338.h	/^	vushort		S12DATB;	\/\/ $YFF466 SASM12 Data Register B $/;"	m	struct:__anon22
S14DATA	hdr/CFX/Headers/mc68338.h	/^	vushort		S14DATA;	\/\/ $YFF472 SASM14 Data Register A $/;"	m	struct:__anon22
S14DATB	hdr/CFX/Headers/mc68338.h	/^	vushort		S14DATB;	\/\/ $YFF476 SASM14 Data Register B $/;"	m	struct:__anon22
S18DATA	hdr/CFX/Headers/mc68338.h	/^	vushort		S18DATA;	\/\/ $YFF492 SASM18 Data Register A $/;"	m	struct:__anon22
S18DATB	hdr/CFX/Headers/mc68338.h	/^	vushort		S18DATB;	\/\/ $YFF496 SASM18 Data Register B $/;"	m	struct:__anon22
S24DATA	hdr/CFX/Headers/mc68338.h	/^	vushort		S24DATA;	\/\/ $YFF4C2 SASM24 Data Register A $/;"	m	struct:__anon22
S24DATB	hdr/CFX/Headers/mc68338.h	/^	vushort		S24DATB;	\/\/ $YFF4C6 SASM24 Data Register B $/;"	m	struct:__anon22
SASM12A	hdr/CFX/Headers/_cf1_ctm.h	/^	SASM12A = 12,	SASM12B = 13,	SASM14A = 14,	SASM14B = 15,$/;"	e	enum:__anon25
SASM12B	hdr/CFX/Headers/_cf1_ctm.h	/^	SASM12A = 12,	SASM12B = 13,	SASM14A = 14,	SASM14B = 15,$/;"	e	enum:__anon25
SASM14A	hdr/CFX/Headers/_cf1_ctm.h	/^	SASM12A = 12,	SASM12B = 13,	SASM14A = 14,	SASM14B = 15,$/;"	e	enum:__anon25
SASM14B	hdr/CFX/Headers/_cf1_ctm.h	/^	SASM12A = 12,	SASM12B = 13,	SASM14A = 14,	SASM14B = 15,$/;"	e	enum:__anon25
SASM18A	hdr/CFX/Headers/_cf1_ctm.h	/^	SASM18A = 18,	SASM18B = 19,	SASM24A = 24,	SASM24B = 25$/;"	e	enum:__anon25
SASM18B	hdr/CFX/Headers/_cf1_ctm.h	/^	SASM18A = 18,	SASM18B = 19,	SASM24A = 24,	SASM24B = 25$/;"	e	enum:__anon25
SASM24A	hdr/CFX/Headers/_cf1_ctm.h	/^	SASM18A = 18,	SASM18B = 19,	SASM24A = 24,	SASM24B = 25$/;"	e	enum:__anon25
SASM24B	hdr/CFX/Headers/_cf1_ctm.h	/^	SASM18A = 18,	SASM18B = 19,	SASM24A = 24,	SASM24B = 25$/;"	e	enum:__anon25
SASMInCap	hdr/CFX/Headers/_cf1_ctm.h	/^	SASMInCap, SASMOutPort, SASMOutCompare,	SASMOutTogg } SASMModeID;$/;"	e	enum:__anon30
SASMModeID	hdr/CFX/Headers/_cf1_ctm.h	/^	SASMInCap, SASMOutPort, SASMOutCompare,	SASMOutTogg } SASMModeID;$/;"	t	typeref:enum:__anon30
SASMOutCompare	hdr/CFX/Headers/_cf1_ctm.h	/^	SASMInCap, SASMOutPort, SASMOutCompare,	SASMOutTogg } SASMModeID;$/;"	e	enum:__anon30
SASMOutPort	hdr/CFX/Headers/_cf1_ctm.h	/^	SASMInCap, SASMOutPort, SASMOutCompare,	SASMOutTogg } SASMModeID;$/;"	e	enum:__anon30
SASMOutTogg	hdr/CFX/Headers/_cf1_ctm.h	/^	SASMInCap, SASMOutPort, SASMOutCompare,	SASMOutTogg } SASMModeID;$/;"	e	enum:__anon30
SBRAM_BASE_ADDR	hdr/CFX/Headers/mc68332.h	51;"	d
SCB_DEFAULTS	hdr/CFX/Headers/_cfx_internals.h	1255;"	d
SCB_DEFAULT_CS10_ADDR	hdr/CFX/Headers/_cfx_internals.h	1257;"	d
SCB_DEFAULT_CS8_ADDR	hdr/CFX/Headers/_cfx_internals.h	1256;"	d
SCB_DEFAULT_NS_ACCESS	hdr/CFX/Headers/_cfx_internals.h	1258;"	d
SCB_MAX_CARDS	hdr/CFX/Headers/_cfx_internals.h	1259;"	d
SCB_MAX_HALF	hdr/CFX/Headers/_cfx_internals.h	1260;"	d
SCCR0	hdr/CFX/Headers/mc68332.h	/^	vushort		SCCR0;		\/\/ $YFFC08 SCI Control 0 Register [S\/U]$/;"	m	struct:__anon17
SCCR0	hdr/CFX/Headers/mc68338.h	/^	vushort		SCCR0;		\/\/ $YFFC08 SCI Control 0 Register [S\/U]$/;"	m	struct:__anon21
SCCR1	hdr/CFX/Headers/mc68332.h	/^	vushort		SCCR1;		\/\/ $YFFC0A SCI Control 1 Register [S\/U]$/;"	m	struct:__anon17
SCCR1	hdr/CFX/Headers/mc68338.h	/^	vushort		SCCR1;		\/\/ $YFFC0A SCI Control 1 Register [S\/U]$/;"	m	struct:__anon21
SCDR	hdr/CFX/Headers/mc68332.h	/^	vushort		SCDR;		\/\/ $YFFC0E SCI Data Register [S\/U]$/;"	m	struct:__anon17
SCDR	hdr/CFX/Headers/mc68338.h	/^	vushort		SCDR;		\/\/ $YFFC0E SCI Data Register [S\/U]$/;"	m	struct:__anon21
SCHAR_MAX	hdr/CPU32/Headers/StdC/limits.h	16;"	d
SCHAR_MIN	hdr/CPU32/Headers/StdC/limits.h	17;"	d
SCIConfigure	hdr/CFX/Headers/_cfx_sercomm.h	/^				bool autoTiming)					BIOS_CALL(SCIConfigure);$/;"	v
SCIGetConfig	hdr/CFX/Headers/_cfx_sercomm.h	/^long	SCIGetConfig(long *baudPtr, char *parityPtr)BIOS_CALL(SCIGetConfig);$/;"	v
SCIRxBreak	hdr/CFX/Headers/_cfx_sercomm.h	/^bool	SCIRxBreak(short millisecs)					BIOS_CALL(SCIRxBreak);$/;"	v
SCIRxFlush	hdr/CFX/Headers/_cfx_sercomm.h	/^void	SCIRxFlush(void)							BIOS_CALL(SCIRxFlush);$/;"	v
SCIRxGetByte	hdr/CFX/Headers/_cfx_sercomm.h	/^ushort	SCIRxGetByte(bool block)					BIOS_CALL(SCIRxGetByte);$/;"	v
SCIRxGetChar	hdr/CFX/Headers/_cfx_sercomm.h	/^short	SCIRxGetChar(void)							BIOS_CALL(SCIRxGetChar);$/;"	v
SCIRxGetCharWithTimeout	hdr/CFX/Headers/_cfx_sercomm.h	/^short	SCIRxGetCharWithTimeout(short millisecs)	BIOS_CALL(SCIRxGetCharWithTimeout);$/;"	v
SCIRxHandshake	hdr/CFX/Headers/_cfx_sercomm.h	/^			char xon, char xoff)					BIOS_CALL(SCIRxHandshake);$/;"	v
SCIRxQueuedCount	hdr/CFX/Headers/_cfx_sercomm.h	/^short	SCIRxQueuedCount(void)						BIOS_CALL(SCIRxQueuedCount);$/;"	v
SCIRxSetBuffered	hdr/CFX/Headers/_cfx_sercomm.h	/^void	SCIRxSetBuffered(bool buffered)				BIOS_CALL(SCIRxSetBuffered);$/;"	v
SCIRxTxIdle	hdr/CFX/Headers/_cfx_sercomm.h	/^bool	SCIRxTxIdle(void)							BIOS_CALL(SCIRxTxIdle);$/;"	v
SCITxBreak	hdr/CFX/Headers/_cfx_sercomm.h	/^void	SCITxBreak(short millisecs)					BIOS_CALL(SCITxBreak);$/;"	v
SCITxFlush	hdr/CFX/Headers/_cfx_sercomm.h	/^void	SCITxFlush(void)							BIOS_CALL(SCITxFlush);$/;"	v
SCITxHandshake	hdr/CFX/Headers/_cfx_sercomm.h	/^			char xon, char xoff)					BIOS_CALL(SCITxHandshake);$/;"	v
SCITxPutByte	hdr/CFX/Headers/_cfx_sercomm.h	/^bool	SCITxPutByte(ushort data, bool block)		BIOS_CALL(SCITxPutByte);$/;"	v
SCITxPutChar	hdr/CFX/Headers/_cfx_sercomm.h	/^void	SCITxPutChar(ushort data)					BIOS_CALL(SCITxPutChar);$/;"	v
SCITxQueuedCount	hdr/CFX/Headers/_cfx_sercomm.h	/^short	SCITxQueuedCount(void)						BIOS_CALL(SCITxQueuedCount);$/;"	v
SCITxSetBuffered	hdr/CFX/Headers/_cfx_sercomm.h	/^void	SCITxSetBuffered(bool buffered)				BIOS_CALL(SCITxSetBuffered);$/;"	v
SCITxWaitCompletion	hdr/CFX/Headers/_cfx_sercomm.h	/^void	SCITxWaitCompletion(void)					BIOS_CALL(SCITxWaitCompletion);$/;"	v
SCI_ERR_TRAP	hdr/CFX/Headers/_cfx_errors.h	/^	, SCI_ERR_TRAP				\/\/ 2 for detecting SCI receive errors$/;"	e	enum:__anon44
SCK	hdr/CFX/Headers/_cf1_pins.h	/^	, SCK	= 16	\/\/ SPI Serial Clock		I\/O		GPIO\/QSPI		I?	O+$/;"	e	enum:__anon33
SCK	hdr/CFX/Headers/_cfx_pins.h	/^	, SCK	= 16	\/\/ SPI Serial Clock		I\/O		GPIO\/QSPI		I?	O+$/;"	e	enum:__anon75
SCSAD2	hdr/CFX/Headers/_cfx_internals.h	/^typedef enum { SCSAD2, SCSAD3 } SCS_ADCH;	\/\/ for A-D reads$/;"	e	enum:ResetResume::__anon73
SCSAD3	hdr/CFX/Headers/_cfx_internals.h	/^typedef enum { SCSAD2, SCSAD3 } SCS_ADCH;	\/\/ for A-D reads$/;"	e	enum:ResetResume::__anon73
SCSDataSrc	hdr/CFX/Headers/_cfx_internals.h	/^typedef enum	{ PIIData, OEMData } SCSDataSrc;$/;"	t	struct:ResetResume	typeref:enum:ResetResume::__anon71
SCSDataType	hdr/CFX/Headers/_cfx_internals.h	/^typedef enum	{ Visible, Protected } SCSDataType;$/;"	t	struct:ResetResume	typeref:enum:ResetResume::__anon72
SCSDevice	hdr/CFX/Headers/_cfx_internals.h	/^typedef	union { struct { ushort id; ushort sn; }; ulong ref; } SCSDevice;$/;"	t	struct:ResetResume	typeref:union:ResetResume::__anon68
SCSR	hdr/CFX/Headers/mc68332.h	/^	vushort		SCSR;		\/\/ $YFFC0C SCI Status Register [S\/U]$/;"	m	struct:__anon17
SCSR	hdr/CFX/Headers/mc68338.h	/^	vushort		SCSR;		\/\/ $YFFC0C SCI Status Register [S\/U]$/;"	m	struct:__anon21
SCS_ADCH	hdr/CFX/Headers/_cfx_internals.h	/^typedef enum { SCSAD2, SCSAD3 } SCS_ADCH;	\/\/ for A-D reads$/;"	t	struct:ResetResume	typeref:enum:ResetResume::__anon73
SCS_ALL_DEVS_REF	hdr/CFX/Headers/_cfx_internals.h	1294;"	d
SCS_STD_MAX_DEVS	hdr/CFX/Headers/_cfx_internals.h	1291;"	d
SEEK_CUR	hdr/CPU32/Headers/StdC/stdio.h	16;"	d
SEEK_END	hdr/CPU32/Headers/StdC/stdio.h	17;"	d
SEEK_SET	hdr/CPU32/Headers/StdC/stdio.h	18;"	d
SGLR	hdr/CFX/Headers/mc68332.h	/^	vushort		SGLR;		\/\/ $YFFE24	Service Grant Latch Register [S]$/;"	m	struct:__anon18
SHRT_MAX	hdr/CPU32/Headers/StdC/limits.h	18;"	d
SHRT_MIN	hdr/CPU32/Headers/StdC/limits.h	19;"	d
SIC12A	hdr/CFX/Headers/mc68338.h	/^	vushort		SIC12A;		\/\/ $YFF460 SASM12 Status\/Interrupt\/Control Register A $/;"	m	struct:__anon22
SIC12B	hdr/CFX/Headers/mc68338.h	/^	vushort		SIC12B;		\/\/ $YFF464 SASM12 Status\/Interrupt\/Control Register B $/;"	m	struct:__anon22
SIC14A	hdr/CFX/Headers/mc68338.h	/^	vushort		SIC14A;		\/\/ $YFF470 SASM14 Status\/Interrupt\/Control Register A $/;"	m	struct:__anon22
SIC14B	hdr/CFX/Headers/mc68338.h	/^	vushort		SIC14B;		\/\/ $YFF474 SASM14 Status\/Interrupt\/Control Register B $/;"	m	struct:__anon22
SIC18A	hdr/CFX/Headers/mc68338.h	/^	vushort		SIC18A;		\/\/ $YFF490 SASM18 Status\/Interrupt\/Control Register A $/;"	m	struct:__anon22
SIC18B	hdr/CFX/Headers/mc68338.h	/^	vushort		SIC18B;		\/\/ $YFF494 SASM18 Status\/Interrupt\/Control Register B $/;"	m	struct:__anon22
SIC24A	hdr/CFX/Headers/mc68338.h	/^	vushort		SIC24A;		\/\/ $YFF4C0 SASM24 Status\/Interrupt\/Control Register A $/;"	m	struct:__anon22
SIC24B	hdr/CFX/Headers/mc68338.h	/^	vushort		SIC24B;		\/\/ $YFF4C4 SASM24 Status\/Interrupt\/Control Register B $/;"	m	struct:__anon22
SIGABRT	hdr/CPU32/Headers/StdC/signal.h	8;"	d
SIGBREAK	hdr/CPU32/Headers/StdC/signal.h	14;"	d
SIGFPE	hdr/CPU32/Headers/StdC/signal.h	9;"	d
SIGILL	hdr/CPU32/Headers/StdC/signal.h	10;"	d
SIGINT	hdr/CPU32/Headers/StdC/signal.h	11;"	d
SIGSEGV	hdr/CPU32/Headers/StdC/signal.h	12;"	d
SIGTERM	hdr/CPU32/Headers/StdC/signal.h	13;"	d
SIG_DFL	hdr/CPU32/Headers/StdC/signal.h	16;"	d
SIG_ERR	hdr/CPU32/Headers/StdC/signal.h	17;"	d
SIG_IGN	hdr/CPU32/Headers/StdC/signal.h	18;"	d
SIMLCR	hdr/CFX/Headers/mc68332.h	/^	vushort		SIMLCR;		\/\/ $YFFA00 SIML Module Configuration Register [S]$/;"	m	struct:__anon16
SIMLCR	hdr/CFX/Headers/mc68338.h	/^	vushort		SIMLCR;		\/\/ $YFFA00 SIML Module Configuration Register [S]$/;"	m	struct:__anon20
SIMLTR	hdr/CFX/Headers/mc68332.h	/^	vushort		SIMLTR;		\/\/ $YFFA02 SIML Test Register [S]$/;"	m	struct:__anon16
SIMLTR	hdr/CFX/Headers/mc68338.h	/^	vushort		SIMLTR;		\/\/ $YFFA02 SIML Test Register [S]$/;"	m	struct:__anon20
SIMLTRE	hdr/CFX/Headers/mc68332.h	/^	vushort		SIMLTRE;	\/\/ $YFFA08 SIML Test Register E [S]$/;"	m	struct:__anon16
SIMLTRE	hdr/CFX/Headers/mc68338.h	/^	vushort		SIMLTRE;	\/\/ $YFFA08 SIML Test Register E [S]$/;"	m	struct:__anon20
SIML_BASE_ADDR	hdr/CFX/Headers/mc68332.h	50;"	d
SIML_BASE_ADDR	hdr/CFX/Headers/mc68338.h	53;"	d
SIM_DEF_IARB	hdr/CFX/Headers/_cfx_internals.h	438;"	d
SIM_PITR_DEF_IPL	hdr/CFX/Headers/_cfx_internals.h	439;"	d
SIOP	hdr/CFX/Headers/_cfx_tpu.h	/^	SIOP	= 0x5,	\/\/		39	2\/3-Wire Synchronous Serial Input\/Output Port$/;"	e	enum:__anon88
SPCR0	hdr/CFX/Headers/mc68332.h	/^	vushort		SPCR0;		\/\/ $YFFC18 SPI Control Register 0 [S\/U]$/;"	m	struct:__anon17
SPCR0	hdr/CFX/Headers/mc68338.h	/^	vushort		SPCR0;		\/\/ $YFFC18 SPI Control Register 0 [S\/U]$/;"	m	struct:__anon21
SPCR1	hdr/CFX/Headers/mc68332.h	/^	vushort		SPCR1;		\/\/ $YFFC1A SPI Control Register 1 [S\/U]$/;"	m	struct:__anon17
SPCR1	hdr/CFX/Headers/mc68338.h	/^	vushort		SPCR1;		\/\/ $YFFC1A SPI Control Register 1 [S\/U]$/;"	m	struct:__anon21
SPCR2	hdr/CFX/Headers/mc68332.h	/^	vushort		SPCR2;		\/\/ $YFFC1C SPI Control Register 2 [S\/U]$/;"	m	struct:__anon17
SPCR2	hdr/CFX/Headers/mc68338.h	/^	vushort		SPCR2;		\/\/ $YFFC1C SPI Control Register 2 [S\/U]$/;"	m	struct:__anon21
SPCR3	hdr/CFX/Headers/mc68332.h	/^	vuchar		SPCR3;		\/\/ $YFFC1E SPI Control Register 3 [S\/U]$/;"	m	struct:__anon17
SPCR3	hdr/CFX/Headers/mc68338.h	/^	vuchar		SPCR3;		\/\/ $YFFC1E SPI Control Register 3 [S\/U]$/;"	m	struct:__anon21
SPSR	hdr/CFX/Headers/mc68332.h	/^	vuchar		SPSR;		\/\/ $YFFC1F SPI Status Register [S\/U]$/;"	m	struct:__anon17
SPSR	hdr/CFX/Headers/mc68338.h	/^	vuchar		SPSR;		\/\/ $YFFC1F SPI Status Register [S\/U]$/;"	m	struct:__anon21
SPVCardFlag	hdr/CFX/Headers/_cfx_internals.h	/^	} SPVCardFlag; $/;"	t	struct:ResetResume	typeref:enum:ResetResume::__anon74
SSDCapf	hdr/CFX/Headers/_cfx_internals.h	/^typedef short 	SSDCapf(void *iodvr, ulong *sectors, ushort *spt,$/;"	t	struct:ResetResume
SSDChkf	hdr/CFX/Headers/_cfx_internals.h	/^typedef bool 	SSDChkf(void *iodvr);$/;"	t	struct:ResetResume
SSDRdf	hdr/CFX/Headers/_cfx_internals.h	/^typedef short 	SSDRdf(void *iodvr, ulong sector, void *buffer, short count);$/;"	t	struct:ResetResume
SSDWrf	hdr/CFX/Headers/_cfx_internals.h	/^typedef short 	SSDWrf(void *iodvr, ulong sector, void *buffer, short count);$/;"	t	struct:ResetResume
SSD_FIRST_DEV	hdr/CFX/Headers/_cfx_internals.h	1351;"	d
SSD_LAST_DEV	hdr/CFX/Headers/_cfx_internals.h	1352;"	d
SSD_MAX_DEVNAME	hdr/CFX/Headers/_cfx_internals.h	1354;"	d
SSD_MAX_DEVS	hdr/CFX/Headers/_cfx_internals.h	1353;"	d
SWSR	hdr/CFX/Headers/mc68332.h	/^	vuchar		SWSR;		\/\/ $YFFA27 Software Service [S]$/;"	m	struct:__anon16
SWSR	hdr/CFX/Headers/mc68338.h	/^	vuchar		SWSR;		\/\/ $YFFA27 Software Service [S]$/;"	m	struct:__anon20
SYNCR	hdr/CFX/Headers/mc68332.h	/^	vushort		SYNCR;		\/\/ $YFFA04 Clock Synthesizer Control Register [S]$/;"	m	struct:__anon16
SYNCR	hdr/CFX/Headers/mc68338.h	/^	vushort		SYNCR;		\/\/ $YFFA04 Clock Synthesizer Control Register [S]$/;"	m	struct:__anon20
SYPCR	hdr/CFX/Headers/mc68332.h	/^	vuchar		SYPCR;		\/\/ $YFFA21 System Protection Control [S]$/;"	m	struct:__anon16
SYPCR	hdr/CFX/Headers/mc68338.h	/^	vuchar		SYPCR;		\/\/ $YFFA21 System Protection Control [S]$/;"	m	struct:__anon20
SYPDEFAULT	hdr/CFX/Headers/_cfx_internals.h	/^enum { SYPDEFAULT = (WDTOff | HaltMonEnable | BusMonEnable | BMT32) };$/;"	e	enum:ResetResume::__anon52
SYS_AUTOEXEC_DEFAULT	hdr/CFX/Headers/_cfx_globals.h	130;"	d
SYS_AUTOEXEC_VEENAME	hdr/CFX/Headers/_cfx_globals.h	129;"	d
SYS_BAUD_DEFAULT	hdr/CFX/Headers/_cfx_globals.h	111;"	d
SYS_BAUD_VEENAME	hdr/CFX/Headers/_cfx_globals.h	110;"	d
SYS_CFEXTRA_DEFAULT	hdr/CFX/Headers/_cfx_globals.h	147;"	d
SYS_CFEXTRA_MICRODRIVE	hdr/CFX/Headers/_cfx_globals.h	149;"	d
SYS_CFEXTRA_NOCARD	hdr/CFX/Headers/_cfx_globals.h	150;"	d
SYS_CFEXTRA_PCCARD	hdr/CFX/Headers/_cfx_globals.h	148;"	d
SYS_CFEXTRA_VEENAME	hdr/CFX/Headers/_cfx_globals.h	146;"	d
SYS_CLOCK_DEFAULT	hdr/CFX/Headers/_cfx_globals.h	114;"	d
SYS_CLOCK_VEENAME	hdr/CFX/Headers/_cfx_globals.h	113;"	d
SYS_DFO_DEFAULT	hdr/CFX/Headers/_cfx_globals.h	123;"	d
SYS_DFO_VEENAME	hdr/CFX/Headers/_cfx_globals.h	122;"	d
SYS_F32MBCUT_DEFAULT	hdr/CFX/Headers/_cfx_globals.h	156;"	d
SYS_F32MBCUT_VEENAME	hdr/CFX/Headers/_cfx_globals.h	155;"	d
SYS_F32TRUST_DEFAULT	hdr/CFX/Headers/_cfx_globals.h	159;"	d
SYS_F32TRUST_VEENAME	hdr/CFX/Headers/_cfx_globals.h	158;"	d
SYS_HOUR_DEFAULT	hdr/CFX/Headers/_cfx_globals.h	127;"	d
SYS_HOUR_VEENAME	hdr/CFX/Headers/_cfx_globals.h	126;"	d
SYS_PATCHSTR_DEFAULT	hdr/CFX/Headers/_cfx_globals.h	162;"	d
SYS_PATCHSTR_VEENAME	hdr/CFX/Headers/_cfx_globals.h	161;"	d
SYS_PATH_DEFAULT	hdr/CFX/Headers/_cfx_globals.h	120;"	d
SYS_PATH_VEENAME	hdr/CFX/Headers/_cfx_globals.h	119;"	d
SYS_PROMPT_DEFAULT	hdr/CFX/Headers/_cfx_globals.h	117;"	d
SYS_PROMPT_VEENAME	hdr/CFX/Headers/_cfx_globals.h	116;"	d
SYS_PSCK_DEFAULT	hdr/CFX/Headers/_cfx_globals.h	141;"	d
SYS_PSCK_VEENAME	hdr/CFX/Headers/_cfx_globals.h	140;"	d
SYS_QPBCS_DEFAULT	hdr/CFX/Headers/_cfx_globals.h	144;"	d
SYS_QPBCS_VEENAME	hdr/CFX/Headers/_cfx_globals.h	143;"	d
SYS_SPVX_DEFAULT	hdr/CFX/Headers/_cfx_globals.h	153;"	d
SYS_SPVX_VEENAME	hdr/CFX/Headers/_cfx_globals.h	152;"	d
SYS_TCR1P_DEFAULT	hdr/CFX/Headers/_cfx_globals.h	138;"	d
SYS_TCR1P_VEENAME	hdr/CFX/Headers/_cfx_globals.h	137;"	d
SYS_TPUIMG_DEFAULT	hdr/CFX/Headers/_cfx_globals.h	135;"	d
SYS_TPUIMG_VEENAME	hdr/CFX/Headers/_cfx_globals.h	134;"	d
S_IEXEC	hdr/CFX/Headers/sys/stat.h	89;"	d
S_IFBLK	hdr/CFX/Headers/sys/stat.h	84;"	d
S_IFCHR	hdr/CFX/Headers/sys/stat.h	85;"	d
S_IFDIR	hdr/CFX/Headers/sys/stat.h	83;"	d
S_IFIFO	hdr/CFX/Headers/sys/stat.h	86;"	d
S_IFMT	hdr/CFX/Headers/sys/stat.h	81;"	d
S_IFREG	hdr/CFX/Headers/sys/stat.h	82;"	d
S_IREAD	hdr/CFX/Headers/sys/stat.h	87;"	d
S_ISBLK	hdr/CFX/Headers/sys/stat.h	93;"	d
S_ISCHR	hdr/CFX/Headers/sys/stat.h	94;"	d
S_ISDIR	hdr/CFX/Headers/sys/stat.h	91;"	d
S_ISFIFO	hdr/CFX/Headers/sys/stat.h	95;"	d
S_ISREG	hdr/CFX/Headers/sys/stat.h	92;"	d
S_IWRITE	hdr/CFX/Headers/sys/stat.h	88;"	d
SaveCmd	hdr/CFX/Headers/_cfx_console.h	/^char	*SaveCmd(CmdInfoPtr cip)					PICO_CALL(SaveCmd);$/;"	v
SectorDumpCmd	hdr/CFX/Headers/_cfx_console.h	/^char	*SectorDumpCmd(CmdInfoPtr cip)				PICO_CALL(SectorDumpCmd);$/;"	v
SetCmd	hdr/CFX/Headers/_cfx_console.h	/^char	*SetCmd(CmdInfoPtr cip)						PICO_CALL(SetCmd);$/;"	v
ShiftCmd	hdr/CFX/Headers/_cfx_console.h	/^char	*ShiftCmd(CmdInfoPtr cip)					PICO_CALL(ShiftCmd);$/;"	v
SingleActionID	hdr/CFX/Headers/_cf1_ctm.h	/^	} SingleActionID;$/;"	t	typeref:enum:__anon25
SmallHardDiskChkBsyDelay	hdr/CFX/Headers/_cfx_internals.h	/^	, SmallHardDiskChkBsyDelay		= 1000000	\/\/ 1 second$/;"	e	enum:ResetResume::__anon65
SmallHardDiskStartupTimeout	hdr/CFX/Headers/_cfx_internals.h	/^	, SmallHardDiskStartupTimeout	= 10000000	\/\/ 10 seconds (typ. 4s)$/;"	e	enum:ResetResume::__anon65
SysFreqHz	hdr/CFX/Headers/_cfx_globals.h	/^	long	SysFreqHz;			\/\/ current system clock in Hz$/;"	m	struct:__anon49
SysFreqkHz	hdr/CFX/Headers/_cfx_globals.h	/^	ushort	SysFreqkHz;			\/\/ current system clock in kHz$/;"	m	struct:__anon49
SysFreqkHzChg	hdr/CFX/Headers/_cfx_globals.h	/^	ushort	SysFreqkHzChg;		\/\/ system clock changing to (pre) or from (post)$/;"	m	struct:__anon49
SysPer_ps	hdr/CFX/Headers/_cfx_globals.h	/^	long	SysPer_ps;			\/\/ current system clock period in picoseconds$/;"	m	struct:__anon49
T2CLK	hdr/CFX/Headers/_cfx_pins.h	/^	, T2CLK = 36	\/\/ Timer Load\/Clock		In		GPIO\/TMR	1M	I+	I+$/;"	e	enum:__anon75
TBB1	hdr/CFX/Headers/_cf1_ctm.h	/^typedef enum	{	TBB1,	TBB2,	TBB3,	TBB4	} TimeBaseBusID;$/;"	e	enum:__anon29
TBB2	hdr/CFX/Headers/_cf1_ctm.h	/^typedef enum	{	TBB1,	TBB2,	TBB3,	TBB4	} TimeBaseBusID;$/;"	e	enum:__anon29
TBB3	hdr/CFX/Headers/_cf1_ctm.h	/^typedef enum	{	TBB1,	TBB2,	TBB3,	TBB4	} TimeBaseBusID;$/;"	e	enum:__anon29
TBB4	hdr/CFX/Headers/_cf1_ctm.h	/^typedef enum	{	TBB1,	TBB2,	TBB3,	TBB4	} TimeBaseBusID;$/;"	e	enum:__anon29
TBSChanControl	hdr/CFX/Headers/_cfx_tpu.h	/^enum	TBSChanControl$/;"	g
TCHSetup	hdr/CFX/Headers/_cfx_tpu.h	/^	}	TCHSetup;$/;"	t	typeref:struct:TCHSetup
TCHSetup	hdr/CFX/Headers/_cfx_tpu.h	/^typedef struct TCHSetup$/;"	s
TCR	hdr/CFX/Headers/mc68332.h	/^	vushort		TCR;		\/\/ $YFFE02	Test Configuration Register [S]$/;"	m	struct:__anon18
TICR	hdr/CFX/Headers/mc68332.h	/^	vushort		TICR;		\/\/ $YFFE08	TPU Interrupt Configuration Register [S]$/;"	m	struct:__anon18
TLCGetTCR	hdr/CFX/Headers/_cfx_tpu.h	/^ushort	TLCGetTCR(short tcr)						BIOS_CALL(TLCGetTCR);$/;"	v
TLCGetTime	hdr/CFX/Headers/_cfx_tpu.h	/^ulong	TLCGetTime(vulong *secs, vushort *ticks)	BIOS_CALL(TLCGetTime);$/;"	v
TLCGetVerRelInfo	hdr/CFX/Headers/_cfx_tpu.h	/^TLCInfo *TLCGetVerRelInfo(void)						BIOS_CALL(TLCGetVerRelInfo);$/;"	v
TLCIE	hdr/CFX/Headers/_cfx_tpu.h	/^typedef enum { tlcNoChangeI = -1, tlcDI, tlcEI } TLCIE;$/;"	t	typeref:enum:__anon93
TLCInfo	hdr/CFX/Headers/_cfx_tpu.h	/^	}	TLCInfo;$/;"	t	typeref:struct:__anon95
TLCInit	hdr/CFX/Headers/_cfx_tpu.h	/^			ushort links, vfptr handler, TLCIE ei)	BIOS_CALL(TLCInit);$/;"	v
TLCSetTime	hdr/CFX/Headers/_cfx_tpu.h	/^void	TLCSetTime(ulong secs, ushort ticks)		BIOS_CALL(TLCSetTime);$/;"	v
TLCStatus	hdr/CFX/Headers/_cfx_tpu.h	/^bool	TLCStatus(TLCSync action)					BIOS_CALL(TLCStatus);$/;"	v
TLCSync	hdr/CFX/Headers/_cfx_tpu.h	/^	}	TLCSync;$/;"	t	typeref:enum:__anon94
TLCUpdate	hdr/CFX/Headers/_cfx_tpu.h	/^void	TLCUpdate(void)								BIOS_CALL(TLCUpdate);$/;"	v
TLCchannel	hdr/CFX/Headers/_cfx_tpu.h	/^enum { TLCchannel = 0 };	\/\/ Channel fixed by CF2 hardware configuration$/;"	e	enum:__anon92
TMCR	hdr/CFX/Headers/mc68332.h	/^	vushort		TMCR;		\/\/ $YFFE00	Module Configuration Register [S]$/;"	m	struct:__anon18
TMGGetSpeed	hdr/CFX/Headers/_cfx_power.h	/^ushort	TMGGetSpeed(void)							BIOS_CALL(TMGGetSpeed);$/;"	v
TMGSetSpeed	hdr/CFX/Headers/_cfx_power.h	/^ushort	TMGSetSpeed(ushort kHz)						BIOS_CALL(TMGSetSpeed);$/;"	v
TMGSetupCLKOUTPin	hdr/CFX/Headers/_cfx_power.h	/^			bool onLPSTOP)							BIOS_CALL(TMGSetupCLKOUTPin);$/;"	v
TMP_MAX	hdr/CPU32/Headers/StdC/stdio.h	15;"	d
TPU1	hdr/CFX/Headers/_cfx_pins.h	/^	, TPU1	= 22	\/\/ Time Processor Pin	I\/O		GPIO\/TMR		I?	I?$/;"	e	enum:__anon75
TPU10	hdr/CFX/Headers/_cfx_pins.h	/^	, TPU10	= 31	\/\/ Time Processor Pin	I\/O		GPIO\/TMR		I?	I?$/;"	e	enum:__anon75
TPU11	hdr/CFX/Headers/_cfx_pins.h	/^	, TPU11	= 32	\/\/ Time Processor Pin	I\/O		GPIO\/TMR		I?	I?$/;"	e	enum:__anon75
TPU12	hdr/CFX/Headers/_cfx_pins.h	/^	, TPU12	= 33	\/\/ Time Processor Pin	I\/O		GPIO\/TMR		I?	I?$/;"	e	enum:__anon75
TPU13	hdr/CFX/Headers/_cfx_pins.h	/^	, TPU13	= 34	\/\/ Time Processor Pin	I\/O		GPIO\/TMR		I?	I?$/;"	e	enum:__anon75
TPU14	hdr/CFX/Headers/_cfx_pins.h	/^	, TPU14	= 35	\/\/ Time Processor Pin	I\/O		GPIO\/TMR		I?	I?$/;"	e	enum:__anon75
TPU15	hdr/CFX/Headers/_cfx_pins.h	/^	, TPU15	= 37	\/\/ Time Processor Pin	I\/O		GPIO\/TMR		I?	I?$/;"	e	enum:__anon75
TPU2	hdr/CFX/Headers/_cfx_pins.h	/^	, TPU2	= 23	\/\/ Time Processor Pin	I\/O		GPIO\/TMR		I?	I?$/;"	e	enum:__anon75
TPU3	hdr/CFX/Headers/_cfx_pins.h	/^	, TPU3	= 24	\/\/ Time Processor Pin	I\/O		GPIO\/TMR		I?	I?$/;"	e	enum:__anon75
TPU4	hdr/CFX/Headers/_cfx_pins.h	/^	, TPU4	= 25	\/\/ Time Processor Pin	I\/O		GPIO\/TMR		I?	I?$/;"	e	enum:__anon75
TPU5	hdr/CFX/Headers/_cfx_pins.h	/^	, TPU5	= 26	\/\/ Time Processor Pin	I\/O		GPIO\/TMR		I?	I?$/;"	e	enum:__anon75
TPU6	hdr/CFX/Headers/_cfx_pins.h	/^	, TPU6	= 27	\/\/ Time Processor Pin	I\/O		GPIO\/TMR		I?	I?$/;"	e	enum:__anon75
TPU7	hdr/CFX/Headers/_cfx_pins.h	/^	, TPU7	= 28	\/\/ Time Processor Pin	I\/O		GPIO\/TMR		I?	I?$/;"	e	enum:__anon75
TPU8	hdr/CFX/Headers/_cfx_pins.h	/^	, TPU8	= 29	\/\/ Time Processor Pin	I\/O		GPIO\/TMR		I?	I?$/;"	e	enum:__anon75
TPU9	hdr/CFX/Headers/_cfx_pins.h	/^	, TPU9	= 30	\/\/ Time Processor Pin	I\/O		GPIO\/TMR		I?	I?$/;"	e	enum:__anon75
TPUCF2Function	hdr/CFX/Headers/_cfx_tpu.h	/^	}	TPUCF2Function;$/;"	t	typeref:enum:__anon88
TPUChanFromPin	hdr/CFX/Headers/_cfx_tpu.h	168;"	d
TPUChannelFunctionSelect	hdr/CFX/Headers/_cfx_tpu.h	/^			ushort function)						BIOS_CALL(TPUChannelFunctionSelect);$/;"	v
TPUChannelPrioritySelect	hdr/CFX/Headers/_cfx_tpu.h	/^			TPUPriority priority)					BIOS_CALL(TPUChannelPrioritySelect);$/;"	v
TPUChannelSetup	hdr/CFX/Headers/_cfx_tpu.h	/^void	*TPUChannelSetup(TCHSetup *setup)			BIOS_CALL(TPUChannelSetup);$/;"	v
TPUGetTCR1Clock	hdr/CFX/Headers/_cfx_tpu.h	/^long	TPUGetTCR1Clock(void)						BIOS_CALL(TPUGetTCR1Clock);$/;"	v
TPUHostSequenceSelect	hdr/CFX/Headers/_cfx_tpu.h	/^			short hsq)								BIOS_CALL(TPUHostSequenceSelect);$/;"	v
TPUHostServiceCheckComplete	hdr/CFX/Headers/_cfx_tpu.h	/^			bool waitstatus)						BIOS_CALL(TPUHostServiceCheckComplete);$/;"	v
TPUHostServiceSession	hdr/CFX/Headers/_cfx_tpu.h	/^			TPUPriority priority, bool waitstatus)	BIOS_CALL(TPUHostServiceSession);$/;"	v
TPUInit	hdr/CFX/Headers/_cfx_tpu.h	/^void	TPUInit(void)								BIOS_CALL(TPUInit);$/;"	v
TPUPinFromChan	hdr/CFX/Headers/_cfx_tpu.h	167;"	d
TPUPrescale	hdr/CFX/Headers/_cfx_tpu.h	/^typedef enum	{ tpuDiv1, tpuDiv2, tpuDiv4, tpuDiv8 } TPUPrescale;$/;"	t	typeref:enum:__anon90
TPUPriority	hdr/CFX/Headers/_cfx_tpu.h	/^	} TPUPriority;$/;"	t	typeref:enum:__anon89
TPURun	hdr/CFX/Headers/_cfx_tpu.h	/^void	TPURun(bool run)							BIOS_CALL(TPURun);$/;"	v
TPUState	hdr/CFX/Headers/_cfx_globals.h	/^	void	*TPUState;			\/\/ Points to private TPU state information$/;"	m	struct:__anon49
TPU_BASE_ADDR	hdr/CFX/Headers/mc68332.h	53;"	d
TPU_DEF_IARB	hdr/CFX/Headers/_cfx_internals.h	458;"	d
TPU_DEF_IPL	hdr/CFX/Headers/_cfx_internals.h	459;"	d
TPU_RAM_ADDR	hdr/CFX/Headers/mc68332.h	49;"	d
TRAMBAR	hdr/CFX/Headers/mc68332.h	/^	vushort		TRAMBAR;		\/\/ $YFFB04     TPURAM Base Address and Status Register [S]$/;"	m	struct:__anon19
TRAMMCR	hdr/CFX/Headers/mc68332.h	/^	vushort		TRAMMCR;		\/\/ $YFFB00     TPURAM Module Configuration Register [S]$/;"	m	struct:__anon19
TRAMTST	hdr/CFX/Headers/mc68332.h	/^	vushort		TRAMTST;		\/\/ $YFFB02     TPURAM Test Register [S] $/;"	m	struct:__anon19
TSTMSRA	hdr/CFX/Headers/mc68332.h	/^	vushort		TSTMSRA;	\/\/ $YFFA30 Test Module Master Shift A [S]$/;"	m	struct:__anon16
TSTMSRA	hdr/CFX/Headers/mc68338.h	/^	vushort		TSTMSRA;	\/\/ $YFFA30 Test Module Master Shift A [S]$/;"	m	struct:__anon20
TSTMSRB	hdr/CFX/Headers/mc68332.h	/^	vushort		TSTMSRB;	\/\/ $YFFA32 Test Module Master Shift B [S]$/;"	m	struct:__anon16
TSTMSRB	hdr/CFX/Headers/mc68338.h	/^	vushort		TSTMSRB;	\/\/ $YFFA32 Test Module Master Shift B [S]$/;"	m	struct:__anon20
TSTRC	hdr/CFX/Headers/mc68332.h	/^	vushort		TSTRC;		\/\/ $YFFA36 Test Module Repetition Counter [S]$/;"	m	struct:__anon16
TSTRC	hdr/CFX/Headers/mc68338.h	/^	vushort		TSTRC;		\/\/ $YFFA36 Test Module Repetition Counter [S]$/;"	m	struct:__anon20
TSTSC	hdr/CFX/Headers/mc68332.h	/^	vushort		TSTSC;		\/\/ $YFFA34 Test Module Shift Count [S]$/;"	m	struct:__anon16
TSTSC	hdr/CFX/Headers/mc68338.h	/^	vushort		TSTSC;		\/\/ $YFFA34 Test Module Shift Count [S]$/;"	m	struct:__anon20
TUBlockDuration	hdr/CFX/Headers/_cfx_sercomm.h	/^long	TUBlockDuration(TUPort *tup, long bytes)	PICO_CALL(TUBlockDuration);$/;"	v
TUChParams	hdr/CFX/Headers/_cfx_sercomm.h	/^	} TUChParams;$/;"	t	typeref:struct:__anon84
TUChanLookup	hdr/CFX/Headers/_cfx_sercomm.h	/^void	*TUChanLookup(short tuch)					PICO_CALL(TUChanLookup);$/;"	v
TUClose	hdr/CFX/Headers/_cfx_sercomm.h	/^void	TUClose(TUPort *tup)						PICO_CALL(TUClose);$/;"	v
TUConfigure	hdr/CFX/Headers/_cfx_sercomm.h	/^void	TUConfigure(TUPort *tup, TUChParams *tp)	PICO_CALL(TUConfigure);$/;"	v
TUGetCurrentParams	hdr/CFX/Headers/_cfx_sercomm.h	/^				TUChParams *cp)						PICO_CALL(TUGetCurrentParams);$/;"	v
TUGetDefaultParams	hdr/CFX/Headers/_cfx_sercomm.h	/^TUChParams	*TUGetDefaultParams(void)				PICO_CALL(TUGetDefaultParams);$/;"	v
TUInit	hdr/CFX/Headers/_cfx_sercomm.h	/^void	TUInit(Callocf *callocf, Freef *freef)		PICO_CALL(TUInit);$/;"	v
TUNotifyPostClockChange	hdr/CFX/Headers/_cfx_sercomm.h	/^void	TUNotifyPostClockChange(TUPort *tup)		PICO_CALL(TUNotifyPostClockChange);$/;"	v
TUNotifyPreClockChange	hdr/CFX/Headers/_cfx_sercomm.h	/^void	TUNotifyPreClockChange(TUPort *tup)			PICO_CALL(TUNotifyPreClockChange);$/;"	v
TUOpen	hdr/CFX/Headers/_cfx_sercomm.h	/^			TUChParams *tp)							PICO_CALL(TUOpen);$/;"	v
TUPort	hdr/CFX/Headers/_cfx_sercomm.h	155;"	d
TURelease	hdr/CFX/Headers/_cfx_sercomm.h	/^void	TURelease(void)								PICO_CALL(TURelease);$/;"	v
TURxFlush	hdr/CFX/Headers/_cfx_sercomm.h	/^void	TURxFlush(TUPort *tup)						PICO_CALL(TURxFlush);$/;"	v
TURxGetBlock	hdr/CFX/Headers/_cfx_sercomm.h	/^			long bytes, short millisecs)			PICO_CALL(TURxGetBlock);$/;"	v
TURxGetByte	hdr/CFX/Headers/_cfx_sercomm.h	/^short	TURxGetByte(TUPort *tup, bool block)		PICO_CALL(TURxGetByte);$/;"	v
TURxGetByteWithTimeout	hdr/CFX/Headers/_cfx_sercomm.h	/^			short millisecs)						PICO_CALL(TURxGetByteWithTimeout);$/;"	v
TURxPeekByte	hdr/CFX/Headers/_cfx_sercomm.h	/^short	TURxPeekByte(TUPort *tup, short index)		PICO_CALL(TURxPeekByte);$/;"	v
TURxQueuedCount	hdr/CFX/Headers/_cfx_sercomm.h	/^short	TURxQueuedCount(TUPort *tup)				PICO_CALL(TURxQueuedCount);$/;"	v
TUSetDefaultParams	hdr/CFX/Headers/_cfx_sercomm.h	/^void	TUSetDefaultParams(TUChParams *rp)			PICO_CALL(TUSetDefaultParams);$/;"	v
TUTxBreak	hdr/CFX/Headers/_cfx_sercomm.h	/^void	TUTxBreak(TUPort *tup, short millisecs)		PICO_CALL(TUTxBreak);$/;"	v
TUTxFlush	hdr/CFX/Headers/_cfx_sercomm.h	/^void	TUTxFlush(TUPort *tup)						PICO_CALL(TUTxFlush);$/;"	v
TUTxPrintf	hdr/CFX/Headers/_cfx_sercomm.h	/^short	TUTxPrintf(TUPort *tup, char * str, ...)	PICO_CALL(TUTxPrintf);$/;"	v
TUTxPutBlock	hdr/CFX/Headers/_cfx_sercomm.h	/^			long bytes, short millisecs)			PICO_CALL(TUTxPutBlock);$/;"	v
TUTxPutByte	hdr/CFX/Headers/_cfx_sercomm.h	/^			bool block)								PICO_CALL(TUTxPutByte);$/;"	v
TUTxQueuedCount	hdr/CFX/Headers/_cfx_sercomm.h	/^short	TUTxQueuedCount(TUPort *tup)				PICO_CALL(TUTxQueuedCount);$/;"	v
TUTxWaitCompletion	hdr/CFX/Headers/_cfx_sercomm.h	/^void	TUTxWaitCompletion(TUPort *tup)				PICO_CALL(TUTxWaitCompletion);$/;"	v
TU_RX_STD_PRIORITY	hdr/CFX/Headers/_cfx_sercomm.h	175;"	d
TU_RX_STD_QSIZE	hdr/CFX/Headers/_cfx_sercomm.h	173;"	d
TU_TX_STD_PRIORITY	hdr/CFX/Headers/_cfx_sercomm.h	176;"	d
TU_TX_STD_QSIZE	hdr/CFX/Headers/_cfx_sercomm.h	174;"	d
TXD	hdr/CFX/Headers/_cf1_pins.h	/^	, TXD	= 46	\/\/ CMOS Serial TxD		Out		GPIO\/UART		I?	OB$/;"	e	enum:__anon33
TXD	hdr/CFX/Headers/_cfx_pins.h	/^	, TXD	= 46	\/\/ CMOS Serial TxD		Out		GPIO\/UART		I?	OB$/;"	e	enum:__anon75
TXX	hdr/CFX/Headers/_cf1_pins.h	/^	, TXX	= 48	\/\/ CMOS TXX\/RXX Output	Out		GPIO\/UART	1M	I+	O+	$/;"	e	enum:__anon33
TXX	hdr/CFX/Headers/_cfx_pins.h	/^	, TXX	= 48	\/\/ CMOS TXX\/RXX Output	Out		GPIO\/UART	1M	I+	O+	$/;"	e	enum:__anon75
TimeBaseBusID	hdr/CFX/Headers/_cf1_ctm.h	/^typedef enum	{	TBB1,	TBB2,	TBB3,	TBB4	} TimeBaseBusID;$/;"	t	typeref:enum:__anon29
TimeCmd	hdr/CFX/Headers/_cfx_console.h	/^char	*TimeCmd(CmdInfoPtr cip)					PICO_CALL(TimeCmd);$/;"	v
TypeCmd	hdr/CFX/Headers/_cfx_console.h	/^char	*TypeCmd(CmdInfoPtr cip)					PICO_CALL(TypeCmd);$/;"	v
UART	hdr/CFX/Headers/_cfx_tpu.h	/^	UART	= 0x6,	\/\/		67	Asynchronous Serial Interface$/;"	e	enum:__anon88
UCHAR	hdr/CFX/Headers/_cfx_types.h	55;"	d
UCHAR_MAX	hdr/CPU32/Headers/StdC/limits.h	20;"	d
UINT_MAX	hdr/CPU32/Headers/StdC/limits.h	21;"	d
ULONG	hdr/CFX/Headers/_cfx_types.h	69;"	d
ULONG_MAX	hdr/CPU32/Headers/StdC/limits.h	23;"	d
USHORT	hdr/CFX/Headers/_cfx_types.h	62;"	d
USHRT_MAX	hdr/CPU32/Headers/StdC/limits.h	22;"	d
VEEClear	hdr/CFX/Headers/_cfx_util.h	/^bool	VEEClear(void)								PICO_CALL(VEEClear);$/;"	v
VEEData	hdr/CFX/Headers/_cfx_util.h	/^typedef union { void *bin; char *str; long lng; float flt; } VEEData;$/;"	t	typeref:union:__anon103
VEEDelete	hdr/CFX/Headers/_cfx_util.h	/^bool	VEEDelete(char *name)						PICO_CALL(VEEDelete);$/;"	v
VEEFetchData	hdr/CFX/Headers/_cfx_util.h	/^VEEData	VEEFetchData(char *name)					PICO_CALL(VEEFetchData);$/;"	v
VEEFetchFloat	hdr/CFX/Headers/_cfx_util.h	/^float	VEEFetchFloat(char *name, float fallback)	PICO_CALL(VEEFetchFloat);$/;"	v
VEEFetchLong	hdr/CFX/Headers/_cfx_util.h	/^long	VEEFetchLong(char *name, long fallback)		PICO_CALL(VEEFetchLong);$/;"	v
VEEFetchStr	hdr/CFX/Headers/_cfx_util.h	/^char	*VEEFetchStr(char *name, char *fallback)	PICO_CALL(VEEFetchStr);$/;"	v
VEELock	hdr/CFX/Headers/_cfx_util.h	/^void	VEELock(void)								PICO_CALL(VEELock);$/;"	v
VEEStoreBin	hdr/CFX/Headers/_cfx_util.h	/^			void *data)								PICO_CALL(VEEStoreBin);$/;"	v
VEEStoreFloat	hdr/CFX/Headers/_cfx_util.h	/^bool	VEEStoreFloat(char *name, float fvalue)		PICO_CALL(VEEStoreFloat);$/;"	v
VEEStoreLong	hdr/CFX/Headers/_cfx_util.h	/^bool	VEEStoreLong(char *name, long lvalue)		PICO_CALL(VEEStoreLong);$/;"	v
VEEStoreStr	hdr/CFX/Headers/_cfx_util.h	/^bool	VEEStoreStr(char *name, char *str)			PICO_CALL(VEEStoreStr);$/;"	v
VEEType	hdr/CFX/Headers/_cfx_util.h	/^typedef enum { vbin, vstr, vlong, vfloat } VEEType;$/;"	t	typeref:enum:__anon102
VEEVar	hdr/CFX/Headers/_cfx_internals.h	/^	}	VEEVar;$/;"	t	struct:ResetResume	typeref:struct:ResetResume::__anon64
VEE_ERRORS	hdr/CFX/Headers/_cfx_errors.h	163;"	d
VEE_MAX_DATA_LEN	hdr/CFX/Headers/_cfx_internals.h	919;"	d
VEE_MAX_DATA_LEN	hdr/CFX/Headers/_cfx_util.h	178;"	d
VEE_MAX_NAME_LEN	hdr/CFX/Headers/_cfx_internals.h	918;"	d
VEE_MAX_NAME_LEN	hdr/CFX/Headers/_cfx_util.h	177;"	d
VPTR	hdr/CFX/Headers/_cfx_types.h	76;"	d
VU_TYPES	hdr/CFX/Headers/_cfx_types.h	89;"	d
VerCmd	hdr/CFX/Headers/_cfx_console.h	/^char	*VerCmd(CmdInfoPtr cip)						PICO_CALL(VerCmd);$/;"	v
Visible	hdr/CFX/Headers/_cfx_internals.h	/^typedef enum	{ Visible, Protected } SCSDataType;$/;"	e	enum:ResetResume::__anon72
VolCmd	hdr/CFX/Headers/_cfx_console.h	/^char	*VolCmd(CmdInfoPtr cip)						PICO_CALL(VolCmd);$/;"	v
WBP	hdr/CFX/Headers/_cfx_internals.h	/^	} WordBit, *WBP;$/;"	t	struct:ResetResume	typeref:struct:ResetResume::__anon62
WCHAR_MAX	hdr/CPU32/Headers/StdC/wchar.h	11;"	d
WCHAR_MIN	hdr/CPU32/Headers/StdC/wchar.h	12;"	d
WDT105s	hdr/CFX/Headers/_cfx_internals.h	/^			WDT105s = 0xE0,		WDT419s = 0xF0,		WDTOff = 0x00 };$/;"	m	struct:ResetResume
WDT13ms	hdr/CFX/Headers/_cfx_internals.h	/^enum	{	WDT13ms = 0x80,		WDT51ms = 0x90,		WDT205ms = 0xA0,$/;"	m	struct:ResetResume
WDT205ms	hdr/CFX/Headers/_cfx_internals.h	/^enum	{	WDT13ms = 0x80,		WDT51ms = 0x90,		WDT205ms = 0xA0,$/;"	m	struct:ResetResume
WDT26s	hdr/CFX/Headers/_cfx_internals.h	/^			WDT819ms = 0xB0,	WDT6_5s = 0xC0,		WDT26s = 0xD0,$/;"	m	struct:ResetResume
WDT419s	hdr/CFX/Headers/_cfx_internals.h	/^			WDT105s = 0xE0,		WDT419s = 0xF0,		WDTOff = 0x00 };$/;"	m	struct:ResetResume
WDT51ms	hdr/CFX/Headers/_cfx_internals.h	/^enum	{	WDT13ms = 0x80,		WDT51ms = 0x90,		WDT205ms = 0xA0,$/;"	m	struct:ResetResume
WDT6_5s	hdr/CFX/Headers/_cfx_internals.h	/^			WDT819ms = 0xB0,	WDT6_5s = 0xC0,		WDT26s = 0xD0,$/;"	m	struct:ResetResume
WDT819ms	hdr/CFX/Headers/_cfx_internals.h	/^			WDT819ms = 0xB0,	WDT6_5s = 0xC0,		WDT26s = 0xD0,$/;"	m	struct:ResetResume
WDTOff	hdr/CFX/Headers/_cfx_internals.h	/^			WDT105s = 0xE0,		WDT419s = 0xF0,		WDTOff = 0x00 };$/;"	m	struct:ResetResume
WEOF	hdr/CPU32/Headers/StdC/wchar.h	13;"	d
WakeCFChg	hdr/CFX/Headers/_cfx_power.h	/^enum	{ WakeTimeout,  WakePinChg,  WakeCFChg, WakePinOrCF };	\/\/ older designators$/;"	e	enum:__anon77
WakeOnTimeout	hdr/CFX/Headers/_cfx_power.h	/^	WakeOnTimeout,  	WakeTmtOrWAKEFall,$/;"	e	enum:__anon78
WakePinChg	hdr/CFX/Headers/_cfx_power.h	/^enum	{ WakeTimeout,  WakePinChg,  WakeCFChg, WakePinOrCF };	\/\/ older designators$/;"	e	enum:__anon77
WakePinOrCF	hdr/CFX/Headers/_cfx_power.h	/^enum	{ WakeTimeout,  WakePinChg,  WakeCFChg, WakePinOrCF };	\/\/ older designators$/;"	e	enum:__anon77
WakeTimeout	hdr/CFX/Headers/_cfx_power.h	/^enum	{ WakeTimeout,  WakePinChg,  WakeCFChg, WakePinOrCF };	\/\/ older designators$/;"	e	enum:__anon77
WakeTmtOrCFChg	hdr/CFX/Headers/_cfx_power.h	/^	WakeTmtOrCFChg,		WakeTmtWAKECFChg	}	WhatWakesSuspend;$/;"	e	enum:__anon78
WakeTmtOrWAKEFall	hdr/CFX/Headers/_cfx_power.h	/^	WakeOnTimeout,  	WakeTmtOrWAKEFall,$/;"	e	enum:__anon78
WakeTmtWAKECFChg	hdr/CFX/Headers/_cfx_power.h	/^	WakeTmtOrCFChg,		WakeTmtWAKECFChg	}	WhatWakesSuspend;$/;"	e	enum:__anon78
WhatWakesSuspend	hdr/CFX/Headers/_cfx_power.h	/^	WakeTmtOrCFChg,		WakeTmtWAKECFChg	}	WhatWakesSuspend;$/;"	t	typeref:enum:__anon78
WhatWokeSuspend	hdr/CFX/Headers/_cfx_power.h	/^	WokeTmtWAKECFChg,	WokeNeverSuspending }	WhatWokeSuspend;$/;"	t	typeref:enum:__anon79
Wint_t	hdr/CPU32/Headers/StdC/wchar_t.h	/^typedef unsigned short	Wint_t;$/;"	t
WokeFromTimeout	hdr/CFX/Headers/_cfx_power.h	/^	WokeFromTimeout,  	WokeTmtOrWAKEFall,		WokeTmtOrCFChg,$/;"	e	enum:__anon79
WokeNeverSuspending	hdr/CFX/Headers/_cfx_power.h	/^	WokeTmtWAKECFChg,	WokeNeverSuspending }	WhatWokeSuspend;$/;"	e	enum:__anon79
WokeTmtOrCFChg	hdr/CFX/Headers/_cfx_power.h	/^	WokeFromTimeout,  	WokeTmtOrWAKEFall,		WokeTmtOrCFChg,$/;"	e	enum:__anon79
WokeTmtOrWAKEFall	hdr/CFX/Headers/_cfx_power.h	/^	WokeFromTimeout,  	WokeTmtOrWAKEFall,		WokeTmtOrCFChg,$/;"	e	enum:__anon79
WokeTmtWAKECFChg	hdr/CFX/Headers/_cfx_power.h	/^	WokeTmtWAKECFChg,	WokeNeverSuspending }	WhatWokeSuspend;$/;"	e	enum:__anon79
Word2Swap	hdr/CFX/Headers/_cfx_util.h	54;"	d
WordBit	hdr/CFX/Headers/_cfx_internals.h	/^	} WordBit, *WBP;$/;"	t	struct:ResetResume	typeref:struct:ResetResume::__anon62
WordBitClear	hdr/CFX/Headers/_cfx_internals.h	825;"	d
WordBitSet	hdr/CFX/Headers/_cfx_internals.h	824;"	d
WordBitTest	hdr/CFX/Headers/_cfx_internals.h	826;"	d
XRCmd	hdr/CFX/Headers/_cfx_console.h	/^char	*XRCmd(CmdInfoPtr cip)						PICO_CALL(XRCmd);$/;"	v
XSCmd	hdr/CFX/Headers/_cfx_console.h	/^char	*XSCmd(CmdInfoPtr cip)						PICO_CALL(XSCmd);$/;"	v
YRCmd	hdr/CFX/Headers/_cfx_console.h	/^char	*YRCmd(CmdInfoPtr cip)						PICO_CALL(YRCmd);$/;"	v
YSCmd	hdr/CFX/Headers/_cfx_console.h	/^char	*YSCmd(CmdInfoPtr cip)						PICO_CALL(YSCmd);$/;"	v
YYMMDD	hdr/CFX/Headers/_cfx_types.h	/^typedef enum {		 	  YYMMDD	\/\/ ISO$/;"	e	enum:__anon97
_A_ANY	hdr/CFX/Headers/dirent.h	84;"	d
_A_ARCH	hdr/CFX/Headers/dirent.h	82;"	d
_A_HIDDEN	hdr/CFX/Headers/dirent.h	78;"	d
_A_LONGNM	hdr/CFX/Headers/dirent.h	83;"	d
_A_NORMAL	hdr/CFX/Headers/dirent.h	76;"	d
_A_RDONLY	hdr/CFX/Headers/dirent.h	77;"	d
_A_SUBDIR	hdr/CFX/Headers/dirent.h	81;"	d
_A_SYSTEM	hdr/CFX/Headers/dirent.h	79;"	d
_A_VOLID	hdr/CFX/Headers/dirent.h	80;"	d
_CTM6	hdr/CFX/Headers/mc68338.h	/^	}	_CTM6;$/;"	t	typeref:struct:__anon22
_DDRE_	hdr/CFX/Headers/mc68332.h	/^	vuchar		_DDRE_;		\/\/ $YFFA14 Not Used [S\/U]$/;"	m	struct:__anon16
_DDRE_	hdr/CFX/Headers/mc68338.h	/^	vuchar		_DDRE_;		\/\/ $YFFA14 Not Used [S\/U]$/;"	m	struct:__anon20
_DDRF_	hdr/CFX/Headers/mc68332.h	/^	vuchar		_DDRF_;		\/\/ $YFFA1C Not Used [S\/U]$/;"	m	struct:__anon16
_DDRF_	hdr/CFX/Headers/mc68338.h	/^	vuchar		_DDRF_;		\/\/ $YFFA1C Not Used [S\/U]$/;"	m	struct:__anon20
_DIRENT_H	hdr/CFX/Headers/dirent.h	46;"	d
_DOSDRIVE_H	hdr/CFX/Headers/dosdrive.h	46;"	d
_FCNTL_H	hdr/CFX/Headers/fcntl.h	46;"	d
_FILE	hdr/CPU32/Headers/StdC/stdio.h	/^	struct _FILE { long handle; char pad[72]; };$/;"	s
_IOFBF	hdr/CPU32/Headers/StdC/stdio.h	22;"	d
_IOLBF	hdr/CPU32/Headers/StdC/stdio.h	21;"	d
_IONBF	hdr/CPU32/Headers/StdC/stdio.h	20;"	d
_MSL_CLOCKS_PER_SEC	hdr/CFX/Headers/mxcfxstd.h	89;"	d
_MSL_CLOCKS_PER_SEC	hdr/CFX/Headers/mxcfxstd.h	92;"	d
_PEPAR_	hdr/CFX/Headers/mc68332.h	/^	vuchar		_PEPAR_;	\/\/ $YFFA16 Not Used [S]$/;"	m	struct:__anon16
_PEPAR_	hdr/CFX/Headers/mc68338.h	/^	vuchar		_PEPAR_;	\/\/ $YFFA16 Not Used [S]$/;"	m	struct:__anon20
_PFPAR_	hdr/CFX/Headers/mc68332.h	/^	vuchar		_PFPAR_;	\/\/ $YFFA1E Not Used [S]$/;"	m	struct:__anon16
_PFPAR_	hdr/CFX/Headers/mc68338.h	/^	vuchar		_PFPAR_;	\/\/ $YFFA1E Not Used [S]$/;"	m	struct:__anon20
_PORTC_	hdr/CFX/Headers/mc68332.h	/^	vuchar		_PORTC_;	\/\/ $YFFA40 Not Used [S\/U]$/;"	m	struct:__anon16
_PORTC_	hdr/CFX/Headers/mc68338.h	/^	vuchar		_PORTC_;	\/\/ $YFFA40 Not Used [S\/U]$/;"	m	struct:__anon20
_PORTE0	hdr/CFX/Headers/mc68332.h	/^	vuchar		_PORTE0;	\/\/ $YFFA10 Not Used[S\/U]$/;"	m	struct:__anon16
_PORTE0	hdr/CFX/Headers/mc68338.h	/^	vuchar		_PORTE0;	\/\/ $YFFA10 Not Used[S\/U]$/;"	m	struct:__anon20
_PORTE1_	hdr/CFX/Headers/mc68332.h	/^	vuchar		_PORTE1_;	\/\/ $YFFA12 Not Used[S\/U]$/;"	m	struct:__anon16
_PORTE1_	hdr/CFX/Headers/mc68338.h	/^	vuchar		_PORTE1_;	\/\/ $YFFA12 Not Used[S\/U]$/;"	m	struct:__anon20
_PORTF0_	hdr/CFX/Headers/mc68332.h	/^	vuchar		_PORTF0_;	\/\/ $YFFA18 Not Used [S\/U]$/;"	m	struct:__anon16
_PORTF0_	hdr/CFX/Headers/mc68338.h	/^	vuchar		_PORTF0_;	\/\/ $YFFA18 Not Used [S\/U]$/;"	m	struct:__anon20
_PORTF1_	hdr/CFX/Headers/mc68332.h	/^	vuchar		_PORTF1_;	\/\/ $YFFA1A Not Used [S\/U]$/;"	m	struct:__anon16
_PORTF1_	hdr/CFX/Headers/mc68338.h	/^	vuchar		_PORTF1_;	\/\/ $YFFA1A Not Used [S\/U]$/;"	m	struct:__anon20
_PORTQS_	hdr/CFX/Headers/mc68332.h	/^	vuchar		_PORTQS_;	\/\/ $YFFC14 Not Used [S\/U]$/;"	m	struct:__anon17
_PORTQS_	hdr/CFX/Headers/mc68338.h	/^	vuchar		_PORTQS_;	\/\/ $YFFC14 Not Used [S\/U]$/;"	m	struct:__anon21
_QSM	hdr/CFX/Headers/mc68332.h	/^	}	_QSM;$/;"	t	typeref:struct:__anon17
_QSM	hdr/CFX/Headers/mc68338.h	/^	}	_QSM;$/;"	t	typeref:struct:__anon21
_RSR_	hdr/CFX/Headers/mc68332.h	/^	vuchar		_RSR_;		\/\/ $YFFA06 Not Used [S]$/;"	m	struct:__anon16
_RSR_	hdr/CFX/Headers/mc68338.h	/^	vuchar		_RSR_;		\/\/ $YFFA06 Not Used [S]$/;"	m	struct:__anon20
_SBRAM	hdr/CFX/Headers/mc68332.h	/^	}	_SBRAM;$/;"	t	typeref:struct:__anon19
_SIML	hdr/CFX/Headers/mc68332.h	/^	}	_SIML;$/;"	t	typeref:struct:__anon16
_SIML	hdr/CFX/Headers/mc68338.h	/^	}	_SIML;$/;"	t	typeref:struct:__anon20
_STAT_H	hdr/CFX/Headers/sys/stat.h	46;"	d
_SWSR_	hdr/CFX/Headers/mc68332.h	/^	vuchar		_SWSR_;		\/\/ $YFFA26 Not Used [S]$/;"	m	struct:__anon16
_SWSR_	hdr/CFX/Headers/mc68338.h	/^	vuchar		_SWSR_;		\/\/ $YFFA26 Not Used [S]$/;"	m	struct:__anon20
_SYPCR_	hdr/CFX/Headers/mc68332.h	/^	vuchar		_SYPCR_;	\/\/ $YFFA20 Not Used [S]$/;"	m	struct:__anon16
_SYPCR_	hdr/CFX/Headers/mc68338.h	/^	vuchar		_SYPCR_;	\/\/ $YFFA20 Not Used [S]$/;"	m	struct:__anon20
_TERMIOS_H	hdr/CFX/Headers/termios.h	46;"	d
_TPU	hdr/CFX/Headers/mc68332.h	/^	}	_TPU;$/;"	t	typeref:struct:__anon18
_UNISTD_H	hdr/CFX/Headers/unistd.h	46;"	d
_UTIME_H	hdr/CFX/Headers/utime.h	46;"	d
__ADS8344_H	hdr/CFX/Headers/Drivers/ADS8344.h	46;"	d
__ASSERT_H	hdr/CPU32/Headers/StdC/assert.h	6;"	d
__CTYPE_H	hdr/CPU32/Headers/StdC/ctype.h	6;"	d
__ERRNO_H	hdr/CPU32/Headers/StdC/errno.h	6;"	d
__FLOAT_H	hdr/CPU32/Headers/StdC/float.h	6;"	d
__ISO646_H	hdr/CPU32/Headers/StdC/iso646.h	6;"	d
__LIMITS_H	hdr/CPU32/Headers/StdC/limits.h	6;"	d
__LOCALE_H	hdr/CPU32/Headers/StdC/locale.h	6;"	d
__MATH_H	hdr/CPU32/Headers/StdC/math.h	6;"	d
__Max146_H	hdr/CFX/Headers/Drivers/Max146.h	46;"	d
__NULL_H	hdr/CPU32/Headers/StdC/null.h	6;"	d
__SETJMP_H	hdr/CPU32/Headers/StdC/setjmp.h	6;"	d
__SIGNAL_H	hdr/CPU32/Headers/StdC/signal.h	6;"	d
__SIZE_T_H	hdr/CPU32/Headers/StdC/size_t.h	6;"	d
__STDARG_H	hdr/CPU32/Headers/StdC/stdarg.h	6;"	d
__STDDEF_H	hdr/CPU32/Headers/StdC/stddef.h	6;"	d
__STDIO_H	hdr/CPU32/Headers/StdC/stdio.h	6;"	d
__STDLIB_H	hdr/CPU32/Headers/StdC/stdlib.h	6;"	d
__STRING_H	hdr/CPU32/Headers/StdC/string.h	6;"	d
__TIME_H	hdr/CPU32/Headers/StdC/time.h	6;"	d
__VA_LIST_H	hdr/CPU32/Headers/StdC/va_list.h	6;"	d
__WCHAR_H	hdr/CPU32/Headers/StdC/wchar.h	6;"	d
__WCHAR_T_H	hdr/CPU32/Headers/StdC/wchar_t.h	6;"	d
__WCTYPE_H	hdr/CPU32/Headers/StdC/wctype.h	6;"	d
___cf1_ctm_h	hdr/CFX/Headers/_cf1_ctm.h	45;"	d
___cf1_pins_h	hdr/CFX/Headers/_cf1_pins.h	45;"	d
___cfx_console_h	hdr/CFX/Headers/_cfx_console.h	45;"	d
___cfx_cpulevel_h	hdr/CFX/Headers/_cfx_cpulevel.h	45;"	d
___cfx_drives_h	hdr/CFX/Headers/_cfx_drives.h	45;"	d
___cfx_errors_h	hdr/CFX/Headers/_cfx_errors.h	45;"	d
___cfx_expand_h	hdr/CFX/Headers/_cfx_expand.h	45;"	d
___cfx_files_h	hdr/CFX/Headers/_cfx_files.h	45;"	d
___cfx_globals_h	hdr/CFX/Headers/_cfx_globals.h	45;"	d
___cfx_internals_h	hdr/CFX/Headers/_cfx_internals.h	45;"	d
___cfx_memmap_h	hdr/CFX/Headers/_cfx_memmap.h	45;"	d
___cfx_pins_h	hdr/CFX/Headers/_cfx_pins.h	45;"	d
___cfx_power_h	hdr/CFX/Headers/_cfx_power.h	45;"	d
___cfx_sercomm_h	hdr/CFX/Headers/_cfx_sercomm.h	45;"	d
___cfx_time_h	hdr/CFX/Headers/_cfx_time.h	45;"	d
___cfx_tpu_h	hdr/CFX/Headers/_cfx_tpu.h	45;"	d
___cfx_types_h	hdr/CFX/Headers/_cfx_types.h	45;"	d
___cfx_util_h	hdr/CFX/Headers/_cfx_util.h	45;"	d
__cfxad_H	hdr/CFX/Headers/cfxad.h	46;"	d
__cfxpatch_h	hdr/CFX/Headers/cfxpatch.h	46;"	d
__cfxpico_h	hdr/CFX/Headers/cfxpico.h	43;"	d
__cmath__	hdr/CFX/Headers/math.mx.h	45;"	d
__d0	hdr/CFX/Headers/_cfx_cpulevel.h	/^void	CPUWriteInterruptMask(ushort ipmask:__d0) = { 0xE148, 0x0040, 0x2000, 0x46C0 };$/;"	v
__d0	hdr/CFX/Headers/_cfx_cpulevel.h	/^void	CPUWriteStackReg(ulong a7:__d0) = { 0x2E40 };$/;"	v
__d0	hdr/CFX/Headers/_cfx_cpulevel.h	/^void	CPUWriteStatusReg(ushort sr:__d0) = { 0x46C0 };$/;"	v
__d0	hdr/CFX/Headers/_cfx_power.h	/^void	LPStopCSE(uchar csebits:__d0) = {	0x0238, 0x00BC, 0xFA05, 0x8138,$/;"	v
__d0	hdr/CFX/Headers/_cfx_tpu.h	/^void	TPUClearInterrupt(ushort tch:__d0) = \\$/;"	v
__d0	hdr/CFX/Headers/_cfx_tpu.h	/^void	TPUDisableInterrupt(ushort tch:__d0) = \\$/;"	v
__d0	hdr/CFX/Headers/_cfx_tpu.h	/^void	TPUEnableInterrupt(ushort tch:__d0) = \\$/;"	v
__double_huge	hdr/CFX/Headers/math.mx.h	/^long __double_huge[];$/;"	v
__extended_huge	hdr/CFX/Headers/math.mx.h	/^long __extended_huge[];$/;"	v
__float_huge	hdr/CFX/Headers/math.mx.h	/^long __float_huge[];$/;"	v
__float_nan	hdr/CFX/Headers/math.mx.h	/^long __float_nan[];$/;"	v
__isfinite	hdr/CFX/Headers/math.mx.h	69;"	d
__mc68332_h	hdr/CFX/Headers/mc68332.h	43;"	d
__mc68338_h	hdr/CFX/Headers/mc68338.h	46;"	d
__mxcfxstd_h	hdr/CFX/Headers/mxcfxstd.h	81;"	d
__mxcfxwdt_h	hdr/CFX/Headers/mxcfx4i.h	46;"	d
__mxcfxwdt_h	hdr/CFX/Headers/mxcfxwdt.h	46;"	d
__mxcpu32_h	hdr/CPU32/Headers/mxcpu32.h	43;"	d
__tpu68332_h	hdr/CFX/Headers/tpu68332.h	46;"	d
__va_start	hdr/CPU32/Headers/StdC/stdarg.h	11;"	d
_resvC00_	hdr/CFX/Headers/mc68338.h	/^	vushort		_resvC00_;	\/\/ $YFF406 $/;"	m	struct:__anon22
_resvC01_	hdr/CFX/Headers/mc68338.h	/^	vushort		_resvC01_;	\/\/ $YFF416 $/;"	m	struct:__anon22
_resvC02_	hdr/CFX/Headers/mc68338.h	/^	vushort		_resvC02_;	\/\/ $YFF426 $/;"	m	struct:__anon22
_resvC03_	hdr/CFX/Headers/mc68338.h	/^	vushort		_resvC03_;	\/\/ $YFF42E $/;"	m	struct:__anon22
_resvC04_	hdr/CFX/Headers/mc68338.h	/^	vushort		_resvC04_;	\/\/ $YFF436 $/;"	m	struct:__anon22
_resvC05_	hdr/CFX/Headers/mc68338.h	/^	vushort		_resvC05_;	\/\/ $YFF43E $/;"	m	struct:__anon22
_resvC06_	hdr/CFX/Headers/mc68338.h	/^	vushort		_resvC06_;	\/\/ $YFF446 $/;"	m	struct:__anon22
_resvC07_	hdr/CFX/Headers/mc68338.h	/^	vushort		_resvC07_;	\/\/ $YFF44E $/;"	m	struct:__anon22
_resvC08_	hdr/CFX/Headers/mc68338.h	/^	vushort		_resvC08_;	\/\/ $YFF4D6 $/;"	m	struct:__anon22
_resvC09_	hdr/CFX/Headers/mc68338.h	/^	vushort		_resvC09_;	\/\/ $YFF4DE $/;"	m	struct:__anon22
_resvC10_	hdr/CFX/Headers/mc68338.h	/^	vushort		_resvC10_;	\/\/ $YFF4E6 $/;"	m	struct:__anon22
_resvC11_	hdr/CFX/Headers/mc68338.h	/^	vushort		_resvC11_;	\/\/ $YFF4EE $/;"	m	struct:__anon22
_resvC12_	hdr/CFX/Headers/mc68338.h	/^	vushort		_resvC12_;	\/\/ $YFF4F6 $/;"	m	struct:__anon22
_resvC13_	hdr/CFX/Headers/mc68338.h	/^	vushort		_resvC13_;	\/\/ $YFF4FE $/;"	m	struct:__anon22
_resvQ00_	hdr/CFX/Headers/mc68332.h	/^	vushort		_resvQ00_;	\/\/ $YFFC06 [S\/U]$/;"	m	struct:__anon17
_resvQ00_	hdr/CFX/Headers/mc68338.h	/^	vushort		_resvQ00_;	\/\/ $YFFC06 [S\/U]$/;"	m	struct:__anon21
_resvQ01_	hdr/CFX/Headers/mc68332.h	/^	vushort		_resvQ01_;	\/\/ $YFFC10 [S\/U]$/;"	m	struct:__anon17
_resvQ01_	hdr/CFX/Headers/mc68338.h	/^	vushort		_resvQ01_;	\/\/ $YFFC10 [S\/U]$/;"	m	struct:__anon21
_resvQ02_	hdr/CFX/Headers/mc68332.h	/^	vushort		_resvQ02_;	\/\/ $YFFC12 [S\/U]$/;"	m	struct:__anon17
_resvQ02_	hdr/CFX/Headers/mc68338.h	/^	vushort		_resvQ02_;	\/\/ $YFFC12 [S\/U]$/;"	m	struct:__anon21
_resvS00_	hdr/CFX/Headers/mc68332.h	/^	vushort		_resvS00_;	\/\/ $YFFA0A $/;"	m	struct:__anon16
_resvS00_	hdr/CFX/Headers/mc68338.h	/^	vushort		_resvS00_;	\/\/ $YFFA0A $/;"	m	struct:__anon20
_resvS01_	hdr/CFX/Headers/mc68332.h	/^	vushort		_resvS01_;	\/\/ $YFFA0C $/;"	m	struct:__anon16
_resvS01_	hdr/CFX/Headers/mc68338.h	/^	vushort		_resvS01_;	\/\/ $YFFA0C $/;"	m	struct:__anon20
_resvS02_	hdr/CFX/Headers/mc68332.h	/^	vushort		_resvS02_;	\/\/ $YFFA0E $/;"	m	struct:__anon16
_resvS02_	hdr/CFX/Headers/mc68338.h	/^	vushort		_resvS02_;	\/\/ $YFFA0E $/;"	m	struct:__anon20
_resvS03_	hdr/CFX/Headers/mc68332.h	/^	vushort		_resvS03_;	\/\/ $YFFA28 [S]$/;"	m	struct:__anon16
_resvS03_	hdr/CFX/Headers/mc68338.h	/^	vushort		_resvS03_;	\/\/ $YFFA28 [S]$/;"	m	struct:__anon20
_resvS04_	hdr/CFX/Headers/mc68332.h	/^	vushort		_resvS04_;	\/\/ $YFFA2A [S]$/;"	m	struct:__anon16
_resvS04_	hdr/CFX/Headers/mc68338.h	/^	vushort		_resvS04_;	\/\/ $YFFA2A [S]$/;"	m	struct:__anon20
_resvS05_	hdr/CFX/Headers/mc68332.h	/^	vushort		_resvS05_;	\/\/ $YFFA2C [S]$/;"	m	struct:__anon16
_resvS05_	hdr/CFX/Headers/mc68338.h	/^	vushort		_resvS05_;	\/\/ $YFFA2C [S]$/;"	m	struct:__anon20
_resvS06_	hdr/CFX/Headers/mc68332.h	/^	vushort		_resvS06_;	\/\/ $YFFA2E [S]$/;"	m	struct:__anon16
_resvS06_	hdr/CFX/Headers/mc68338.h	/^	vushort		_resvS06_;	\/\/ $YFFA2E [S]$/;"	m	struct:__anon20
_resvS07_	hdr/CFX/Headers/mc68332.h	/^	vushort		_resvS07_;	\/\/ $YFFA3C $/;"	m	struct:__anon16
_resvS07_	hdr/CFX/Headers/mc68338.h	/^	vushort		_resvS07_;	\/\/ $YFFA3C $/;"	m	struct:__anon20
_resvS08_	hdr/CFX/Headers/mc68332.h	/^	vushort		_resvS08_;	\/\/ $YFFA3E $/;"	m	struct:__anon16
_resvS08_	hdr/CFX/Headers/mc68338.h	/^	vushort		_resvS08_;	\/\/ $YFFA3E $/;"	m	struct:__anon20
_resvS09_	hdr/CFX/Headers/mc68332.h	/^	vushort		_resvS09_;	\/\/ $YFFA42 $/;"	m	struct:__anon16
_resvS09_	hdr/CFX/Headers/mc68338.h	/^	vushort		_resvS09_;	\/\/ $YFFA42 $/;"	m	struct:__anon20
_resvS10_	hdr/CFX/Headers/mc68332.h	/^	vushort		_resvS10_;	\/\/ $YFFA78 $/;"	m	struct:__anon16
_resvS10_	hdr/CFX/Headers/mc68338.h	/^	vushort		_resvS10_;	\/\/ $YFFA78 $/;"	m	struct:__anon20
_resvS11_	hdr/CFX/Headers/mc68332.h	/^	vushort		_resvS11_;	\/\/ $YFFA7A $/;"	m	struct:__anon16
_resvS11_	hdr/CFX/Headers/mc68338.h	/^	vushort		_resvS11_;	\/\/ $YFFA7A $/;"	m	struct:__anon20
_resvS12_	hdr/CFX/Headers/mc68332.h	/^	vushort		_resvS12_;	\/\/ $YFFA7C $/;"	m	struct:__anon16
_resvS12_	hdr/CFX/Headers/mc68338.h	/^	vushort		_resvS12_;	\/\/ $YFFA7C $/;"	m	struct:__anon20
_resvS13_	hdr/CFX/Headers/mc68332.h	/^	vushort		_resvS13_;	\/\/ $YFFA7E $/;"	m	struct:__anon16
_resvS13_	hdr/CFX/Headers/mc68338.h	/^	vushort		_resvS13_;	\/\/ $YFFA7E $/;"	m	struct:__anon20
_rvC0_	hdr/CFX/Headers/mc68338.h	/^	vushort		_rvC0_[2];	\/\/ $YFF40C Ð$YFF40E $/;"	m	struct:__anon22
_rvC1_	hdr/CFX/Headers/mc68338.h	/^	vushort		_rvC1_[2];	\/\/ $YFF41C Ð $YFF41E $/;"	m	struct:__anon22
_rvC2_	hdr/CFX/Headers/mc68338.h	/^	vushort		_rvC2_[5];	\/\/ $YFF456 Ð $YFF45E $/;"	m	struct:__anon22
_rvC3_	hdr/CFX/Headers/mc68338.h	/^	vushort		_rvC3_[4];	\/\/ $YFF468 Ð $YFF46E $/;"	m	struct:__anon22
_rvC4_	hdr/CFX/Headers/mc68338.h	/^	vushort		_rvC4_[4];	\/\/ $YFF478 Ð $YFF47E $/;"	m	struct:__anon22
_rvC5_	hdr/CFX/Headers/mc68338.h	/^	vushort		_rvC5_[3];	\/\/ $YFF48A Ð $YFF48E $/;"	m	struct:__anon22
_rvC6_	hdr/CFX/Headers/mc68338.h	/^	vushort		_rvC6_[20];	\/\/ $YFF498 Ð$YFF4BE $/;"	m	struct:__anon22
_rvC7_	hdr/CFX/Headers/mc68338.h	/^	vushort		_rvC7_[4];	\/\/ $YFF4C8 Ð $YFF4CE $/;"	m	struct:__anon22
_rvQ_	hdr/CFX/Headers/mc68332.h	/^	vushort		_rvQ_[112];	\/\/ $YFFC20 Ð $YFFCFF [S\/U]$/;"	m	struct:__anon17
_rvQ_	hdr/CFX/Headers/mc68338.h	/^	vushort		_rvQ_[112];	\/\/ $YFFC20 Ð $YFFCFF [S\/U]$/;"	m	struct:__anon21
_vTR_	hdr/CFX/Headers/mc68332.h	/^	vushort		_vTR_[58];		\/\/ $YFFB06-$YFFB3F     Not Used$/;"	m	struct:__anon19
_vT_	hdr/CFX/Headers/mc68332.h	/^	vushort		_vT_[108];	\/\/ $YFFE28-$YFFEFF     Not Used$/;"	m	struct:__anon18
abbrevOk	hdr/CFX/Headers/_cfx_console.h	/^	char		abbrevOk;				\/\/ accept abbreviated commands (default)$/;"	m	struct:CmdInfo
actime	hdr/CFX/Headers/utime.h	/^	time_t		actime;					\/\/ access time$/;"	m	struct:utimbuf
adif	hdr/CFX/Headers/cfxad.h	/^typedef bool (*adif)(ushort qslot, void *ad);$/;"	t
adtype	hdr/CFX/Headers/cfxad.h	/^	ushort	adtype;$/;"	m	struct:__anon1
altctp	hdr/CFX/Headers/_cfx_console.h	/^	CmdTablePtr	altctp;					\/\/ secondary command table$/;"	m	struct:CmdInfo
altgets	hdr/CFX/Headers/_cfx_console.h	/^	getsfptr	altgets; 				\/\/ zero defaults to stdlib gets()$/;"	m	struct:CmdInfo
and	hdr/CPU32/Headers/StdC/iso646.h	8;"	d
and_eq	hdr/CPU32/Headers/StdC/iso646.h	9;"	d
argc	hdr/CFX/Headers/_cfx_console.h	/^	short		argc;					\/\/ number of arguments (incl. command$/;"	m	struct:CmdInfo
argv	hdr/CFX/Headers/_cfx_console.h	/^	CmdParam	argv[CMDMAXARGS];		\/\/ arguments$/;"	m	struct:CmdInfo
assert	hdr/CPU32/Headers/StdC/assert.h	11;"	d
assert	hdr/CPU32/Headers/StdC/assert.h	14;"	d
assert	hdr/CPU32/Headers/StdC/assert.h	16;"	d
assert	hdr/CPU32/Headers/StdC/assert.h	8;"	d
autoTiming	hdr/CFX/Headers/_cfx_expand.h	/^	bool			autoTiming;		\/\/ Auto adjust timing to clock flag$/;"	m	struct:qpbDev
autobaud	hdr/CFX/Headers/_cfx_sercomm.h	/^	short	autobaud;	\/\/ auto-adjust baud on clock change (not implemented!)$/;"	m	struct:__anon84
b	hdr/CFX/Headers/_cfx_util.h	/^typedef	union	{ uchar	b[2]; ushort w; } BW, *pBW;$/;"	m	union:__anon98
b	hdr/CFX/Headers/_cfx_util.h	/^typedef	union	{ uchar	b[4]; ushort w[2]; ulong l; } BWL, *pBWL;$/;"	m	union:__anon99
b0	hdr/CFX/Headers/_cfx_internals.h	/^			b7:1,	b6:1,	b5:1,	b4:1,	b3:1,	b2:1,	b1:1,	b0:1;$/;"	m	struct:ResetResume::__anon62
b0	hdr/CFX/Headers/_cfx_internals.h	/^			b7:1,	b6:1,	b5:1,	b4:1,	b3:1,	b2:1,	b1:1,	b0:1;$/;"	m	struct:ResetResume::__anon63
b0	hdr/CFX/Headers/_cfx_internals.h	/^	uchar	b7:1,	b6:1,	b5:1,	b4:1,	b3:1,	b2:1,	b1:1,	b0:1;$/;"	m	struct:ResetResume::__anon61
b1	hdr/CFX/Headers/_cfx_internals.h	/^			b7:1,	b6:1,	b5:1,	b4:1,	b3:1,	b2:1,	b1:1,	b0:1;$/;"	m	struct:ResetResume::__anon62
b1	hdr/CFX/Headers/_cfx_internals.h	/^			b7:1,	b6:1,	b5:1,	b4:1,	b3:1,	b2:1,	b1:1,	b0:1;$/;"	m	struct:ResetResume::__anon63
b1	hdr/CFX/Headers/_cfx_internals.h	/^	uchar	b7:1,	b6:1,	b5:1,	b4:1,	b3:1,	b2:1,	b1:1,	b0:1;$/;"	m	struct:ResetResume::__anon61
b10	hdr/CFX/Headers/_cfx_internals.h	/^			b15:1,	b14:1,	b13:1,	b12:1,	b11:1,	b10:1,	b9:1,	b8:1,$/;"	m	struct:ResetResume::__anon63
b10	hdr/CFX/Headers/_cfx_internals.h	/^	ushort	b15:1,	b14:1,	b13:1,	b12:1,	b11:1,	b10:1,	b9:1,	b8:1,$/;"	m	struct:ResetResume::__anon62
b11	hdr/CFX/Headers/_cfx_internals.h	/^			b15:1,	b14:1,	b13:1,	b12:1,	b11:1,	b10:1,	b9:1,	b8:1,$/;"	m	struct:ResetResume::__anon63
b11	hdr/CFX/Headers/_cfx_internals.h	/^	ushort	b15:1,	b14:1,	b13:1,	b12:1,	b11:1,	b10:1,	b9:1,	b8:1,$/;"	m	struct:ResetResume::__anon62
b12	hdr/CFX/Headers/_cfx_internals.h	/^			b15:1,	b14:1,	b13:1,	b12:1,	b11:1,	b10:1,	b9:1,	b8:1,$/;"	m	struct:ResetResume::__anon63
b12	hdr/CFX/Headers/_cfx_internals.h	/^	ushort	b15:1,	b14:1,	b13:1,	b12:1,	b11:1,	b10:1,	b9:1,	b8:1,$/;"	m	struct:ResetResume::__anon62
b13	hdr/CFX/Headers/_cfx_internals.h	/^			b15:1,	b14:1,	b13:1,	b12:1,	b11:1,	b10:1,	b9:1,	b8:1,$/;"	m	struct:ResetResume::__anon63
b13	hdr/CFX/Headers/_cfx_internals.h	/^	ushort	b15:1,	b14:1,	b13:1,	b12:1,	b11:1,	b10:1,	b9:1,	b8:1,$/;"	m	struct:ResetResume::__anon62
b14	hdr/CFX/Headers/_cfx_internals.h	/^			b15:1,	b14:1,	b13:1,	b12:1,	b11:1,	b10:1,	b9:1,	b8:1,$/;"	m	struct:ResetResume::__anon63
b14	hdr/CFX/Headers/_cfx_internals.h	/^	ushort	b15:1,	b14:1,	b13:1,	b12:1,	b11:1,	b10:1,	b9:1,	b8:1,$/;"	m	struct:ResetResume::__anon62
b15	hdr/CFX/Headers/_cfx_internals.h	/^			b15:1,	b14:1,	b13:1,	b12:1,	b11:1,	b10:1,	b9:1,	b8:1,$/;"	m	struct:ResetResume::__anon63
b15	hdr/CFX/Headers/_cfx_internals.h	/^	ushort	b15:1,	b14:1,	b13:1,	b12:1,	b11:1,	b10:1,	b9:1,	b8:1,$/;"	m	struct:ResetResume::__anon62
b16	hdr/CFX/Headers/_cfx_internals.h	/^			b23:1,	b22:1,	b21:1,	b20:1,	b19:1,	b18:1,	b17:1,	b16:1,$/;"	m	struct:ResetResume::__anon63
b17	hdr/CFX/Headers/_cfx_internals.h	/^			b23:1,	b22:1,	b21:1,	b20:1,	b19:1,	b18:1,	b17:1,	b16:1,$/;"	m	struct:ResetResume::__anon63
b18	hdr/CFX/Headers/_cfx_internals.h	/^			b23:1,	b22:1,	b21:1,	b20:1,	b19:1,	b18:1,	b17:1,	b16:1,$/;"	m	struct:ResetResume::__anon63
b19	hdr/CFX/Headers/_cfx_internals.h	/^			b23:1,	b22:1,	b21:1,	b20:1,	b19:1,	b18:1,	b17:1,	b16:1,$/;"	m	struct:ResetResume::__anon63
b2	hdr/CFX/Headers/_cfx_internals.h	/^			b7:1,	b6:1,	b5:1,	b4:1,	b3:1,	b2:1,	b1:1,	b0:1;$/;"	m	struct:ResetResume::__anon62
b2	hdr/CFX/Headers/_cfx_internals.h	/^			b7:1,	b6:1,	b5:1,	b4:1,	b3:1,	b2:1,	b1:1,	b0:1;$/;"	m	struct:ResetResume::__anon63
b2	hdr/CFX/Headers/_cfx_internals.h	/^	uchar	b7:1,	b6:1,	b5:1,	b4:1,	b3:1,	b2:1,	b1:1,	b0:1;$/;"	m	struct:ResetResume::__anon61
b20	hdr/CFX/Headers/_cfx_internals.h	/^			b23:1,	b22:1,	b21:1,	b20:1,	b19:1,	b18:1,	b17:1,	b16:1,$/;"	m	struct:ResetResume::__anon63
b21	hdr/CFX/Headers/_cfx_internals.h	/^			b23:1,	b22:1,	b21:1,	b20:1,	b19:1,	b18:1,	b17:1,	b16:1,$/;"	m	struct:ResetResume::__anon63
b22	hdr/CFX/Headers/_cfx_internals.h	/^			b23:1,	b22:1,	b21:1,	b20:1,	b19:1,	b18:1,	b17:1,	b16:1,$/;"	m	struct:ResetResume::__anon63
b23	hdr/CFX/Headers/_cfx_internals.h	/^			b23:1,	b22:1,	b21:1,	b20:1,	b19:1,	b18:1,	b17:1,	b16:1,$/;"	m	struct:ResetResume::__anon63
b24	hdr/CFX/Headers/_cfx_internals.h	/^	ulong	b31:1,	b30:1,	b29:1,	b28:1,	b27:1,	b26:1,	b25:1,	b24:1,$/;"	m	struct:ResetResume::__anon63
b25	hdr/CFX/Headers/_cfx_internals.h	/^	ulong	b31:1,	b30:1,	b29:1,	b28:1,	b27:1,	b26:1,	b25:1,	b24:1,$/;"	m	struct:ResetResume::__anon63
b26	hdr/CFX/Headers/_cfx_internals.h	/^	ulong	b31:1,	b30:1,	b29:1,	b28:1,	b27:1,	b26:1,	b25:1,	b24:1,$/;"	m	struct:ResetResume::__anon63
b27	hdr/CFX/Headers/_cfx_internals.h	/^	ulong	b31:1,	b30:1,	b29:1,	b28:1,	b27:1,	b26:1,	b25:1,	b24:1,$/;"	m	struct:ResetResume::__anon63
b28	hdr/CFX/Headers/_cfx_internals.h	/^	ulong	b31:1,	b30:1,	b29:1,	b28:1,	b27:1,	b26:1,	b25:1,	b24:1,$/;"	m	struct:ResetResume::__anon63
b29	hdr/CFX/Headers/_cfx_internals.h	/^	ulong	b31:1,	b30:1,	b29:1,	b28:1,	b27:1,	b26:1,	b25:1,	b24:1,$/;"	m	struct:ResetResume::__anon63
b3	hdr/CFX/Headers/_cfx_internals.h	/^			b7:1,	b6:1,	b5:1,	b4:1,	b3:1,	b2:1,	b1:1,	b0:1;$/;"	m	struct:ResetResume::__anon62
b3	hdr/CFX/Headers/_cfx_internals.h	/^			b7:1,	b6:1,	b5:1,	b4:1,	b3:1,	b2:1,	b1:1,	b0:1;$/;"	m	struct:ResetResume::__anon63
b3	hdr/CFX/Headers/_cfx_internals.h	/^	uchar	b7:1,	b6:1,	b5:1,	b4:1,	b3:1,	b2:1,	b1:1,	b0:1;$/;"	m	struct:ResetResume::__anon61
b30	hdr/CFX/Headers/_cfx_internals.h	/^	ulong	b31:1,	b30:1,	b29:1,	b28:1,	b27:1,	b26:1,	b25:1,	b24:1,$/;"	m	struct:ResetResume::__anon63
b31	hdr/CFX/Headers/_cfx_internals.h	/^	ulong	b31:1,	b30:1,	b29:1,	b28:1,	b27:1,	b26:1,	b25:1,	b24:1,$/;"	m	struct:ResetResume::__anon63
b4	hdr/CFX/Headers/_cfx_internals.h	/^			b7:1,	b6:1,	b5:1,	b4:1,	b3:1,	b2:1,	b1:1,	b0:1;$/;"	m	struct:ResetResume::__anon62
b4	hdr/CFX/Headers/_cfx_internals.h	/^			b7:1,	b6:1,	b5:1,	b4:1,	b3:1,	b2:1,	b1:1,	b0:1;$/;"	m	struct:ResetResume::__anon63
b4	hdr/CFX/Headers/_cfx_internals.h	/^	uchar	b7:1,	b6:1,	b5:1,	b4:1,	b3:1,	b2:1,	b1:1,	b0:1;$/;"	m	struct:ResetResume::__anon61
b5	hdr/CFX/Headers/_cfx_internals.h	/^			b7:1,	b6:1,	b5:1,	b4:1,	b3:1,	b2:1,	b1:1,	b0:1;$/;"	m	struct:ResetResume::__anon62
b5	hdr/CFX/Headers/_cfx_internals.h	/^			b7:1,	b6:1,	b5:1,	b4:1,	b3:1,	b2:1,	b1:1,	b0:1;$/;"	m	struct:ResetResume::__anon63
b5	hdr/CFX/Headers/_cfx_internals.h	/^	uchar	b7:1,	b6:1,	b5:1,	b4:1,	b3:1,	b2:1,	b1:1,	b0:1;$/;"	m	struct:ResetResume::__anon61
b6	hdr/CFX/Headers/_cfx_internals.h	/^			b7:1,	b6:1,	b5:1,	b4:1,	b3:1,	b2:1,	b1:1,	b0:1;$/;"	m	struct:ResetResume::__anon62
b6	hdr/CFX/Headers/_cfx_internals.h	/^			b7:1,	b6:1,	b5:1,	b4:1,	b3:1,	b2:1,	b1:1,	b0:1;$/;"	m	struct:ResetResume::__anon63
b6	hdr/CFX/Headers/_cfx_internals.h	/^	uchar	b7:1,	b6:1,	b5:1,	b4:1,	b3:1,	b2:1,	b1:1,	b0:1;$/;"	m	struct:ResetResume::__anon61
b7	hdr/CFX/Headers/_cfx_internals.h	/^			b7:1,	b6:1,	b5:1,	b4:1,	b3:1,	b2:1,	b1:1,	b0:1;$/;"	m	struct:ResetResume::__anon62
b7	hdr/CFX/Headers/_cfx_internals.h	/^			b7:1,	b6:1,	b5:1,	b4:1,	b3:1,	b2:1,	b1:1,	b0:1;$/;"	m	struct:ResetResume::__anon63
b7	hdr/CFX/Headers/_cfx_internals.h	/^	uchar	b7:1,	b6:1,	b5:1,	b4:1,	b3:1,	b2:1,	b1:1,	b0:1;$/;"	m	struct:ResetResume::__anon61
b8	hdr/CFX/Headers/_cfx_internals.h	/^			b15:1,	b14:1,	b13:1,	b12:1,	b11:1,	b10:1,	b9:1,	b8:1,$/;"	m	struct:ResetResume::__anon63
b8	hdr/CFX/Headers/_cfx_internals.h	/^	ushort	b15:1,	b14:1,	b13:1,	b12:1,	b11:1,	b10:1,	b9:1,	b8:1,$/;"	m	struct:ResetResume::__anon62
b9	hdr/CFX/Headers/_cfx_internals.h	/^			b15:1,	b14:1,	b13:1,	b12:1,	b11:1,	b10:1,	b9:1,	b8:1,$/;"	m	struct:ResetResume::__anon63
b9	hdr/CFX/Headers/_cfx_internals.h	/^	ushort	b15:1,	b14:1,	b13:1,	b12:1,	b11:1,	b10:1,	b9:1,	b8:1,$/;"	m	struct:ResetResume::__anon62
baud	hdr/CFX/Headers/_cfx_sercomm.h	/^	long	baud;		\/\/ baud rate$/;"	m	struct:__anon84
biaBoard	hdr/CFX/Headers/_cfx_internals.h	/^	uchar		biaBoard;		\/\/ BigIDEA board version$/;"	m	struct:ResetResume::__anon67
biaBuild	hdr/CFX/Headers/_cfx_internals.h	/^	uchar		biaBuild;		\/\/ BigIDEA board changes$/;"	m	struct:ResetResume::__anon67
biaCantAllocSSD	hdr/CFX/Headers/_cfx_errors.h	/^	, biaCantAllocSSD			\/\/ failed trying to allocate the SSD$/;"	e	enum:__anon43
biaDev	hdr/CFX/Headers/_cfx_internals.h	/^	BIADEV		biaDev;			\/\/ device type: BIAHD, BIA25, BIA18, ...$/;"	m	struct:ResetResume::__anon67
biaErrorStart	hdr/CFX/Headers/_cfx_errors.h	/^	, biaErrorStart = BIA_ERRORS$/;"	e	enum:__anon43
biaIndex	hdr/CFX/Headers/_cfx_internals.h	/^	char		biaIndex;		\/\/ BigIDEA SCS index number$/;"	m	struct:ResetResume::__anon67
biaInvalidIDEport	hdr/CFX/Headers/_cfx_errors.h	/^	, biaInvalidIDEport			\/\/ BIA driver references invalid IDE port$/;"	e	enum:__anon43
biaPowerUpFailed	hdr/CFX/Headers/_cfx_errors.h	/^	, biaPowerUpFailed			\/\/ failed waiting for ready at power up$/;"	e	enum:__anon43
biaSCBInitFailed	hdr/CFX/Headers/_cfx_errors.h	/^	, biaSCBInitFailed			\/\/ failed trying to initialize the SCB$/;"	e	enum:__anon43
biaSCSIndexConfFailed	hdr/CFX/Headers/_cfx_errors.h	/^	, biaSCSIndexConfFailed		\/\/ failed to confirm the requested the SCS index$/;"	e	enum:__anon43
biaSCSIndexReadyFailed	hdr/CFX/Headers/_cfx_errors.h	/^	, biaSCSIndexReadyFailed	\/\/ failed acknowledge the requested the SCS index$/;"	e	enum:__anon43
biaSCSIndexReqUnknown	hdr/CFX/Headers/_cfx_errors.h	/^	, biaSCSIndexReqUnknown		\/\/ failed to find the requested the SCS index$/;"	e	enum:__anon43
biaSCSInitFailed	hdr/CFX/Headers/_cfx_errors.h	/^	, biaSCSInitFailed			\/\/ failed trying to initialize the SCS$/;"	e	enum:__anon43
biaSCSUnknownVersion	hdr/CFX/Headers/_cfx_errors.h	/^	, biaSCSUnknownVersion		\/\/ SCS driver is incompatible with this BIA$/;"	e	enum:__anon43
biaUnknownDevice	hdr/CFX/Headers/_cfx_errors.h	/^	, biaUnknownDevice			\/\/ unknown BIADEV requested$/;"	e	enum:__anon43
biaUnknownHardwareVersion	hdr/CFX/Headers/_cfx_errors.h	/^	, biaUnknownHardwareVersion	\/\/ BIA driver doesn't recognize this BIA$/;"	e	enum:__anon43
bin	hdr/CFX/Headers/_cfx_util.h	/^typedef union { void *bin; char *str; long lng; float flt; } VEEData;$/;"	m	union:__anon103
biosver	hdr/CFX/Headers/_cfx_globals.h	/^	char	biosver[10];		\/\/ "2.28r1  ", "2.28r1p1"$/;"	m	struct:__anon50
bitand	hdr/CPU32/Headers/StdC/iso646.h	10;"	d
bitor	hdr/CPU32/Headers/StdC/iso646.h	11;"	d
bits	hdr/CFX/Headers/_cfx_expand.h	/^	ushort			bits;			\/\/ Bits Per Transfer$/;"	m	struct:qpbDev
bits	hdr/CFX/Headers/_cfx_sercomm.h	/^	short	bits;		\/\/ data bits exclusive of start, stop, parity$/;"	m	struct:__anon84
bool	hdr/CFX/Headers/_cfx_types.h	/^		typedef char bool;$/;"	t
bool	hdr/CFX/Headers/_cfx_types.h	85;"	d
bool	hdr/CFX/Headers/mxcfxstd.h	/^	typedef char bool;$/;"	t
bool	hdr/CFX/Headers/mxcfxstd.h	103;"	d
bps	hdr/CFX/Headers/dosdrive.h	61;"	d
bs	hdr/CFX/Headers/dosdrive.h	/^  	BS			bs;						\/\/ FAT32 or FAT16 fields$/;"	m	struct:DWBPB
bs16	hdr/CFX/Headers/dosdrive.h	/^	  	BS16	bs16;					\/\/ FAT12\/16 fields$/;"	m	union:DPBPB::__anon4
bs32	hdr/CFX/Headers/dosdrive.h	/^	  	BS32	bs32;					\/\/ FAT32 fields$/;"	m	union:DPBPB::__anon4
bsdErrorStart	hdr/CFX/Headers/_cfx_errors.h	/^	, bsdErrorStart = BSD_ERRORS$/;"	e	enum:__anon43
bsdIdOfRange	hdr/CFX/Headers/_cfx_errors.h	/^	, bsdIdOfRange				\/\/ invalid ID requested$/;"	e	enum:__anon43
bsdNotRegistered	hdr/CFX/Headers/_cfx_errors.h	/^	, bsdNotRegistered			\/\/ no BSD registered to this ID$/;"	e	enum:__anon43
bsdSectorRangeError	hdr/CFX/Headers/_cfx_errors.h	/^	, bsdSectorRangeError		\/\/ read\/write requested sector out of range$/;"	e	enum:__anon43
bufferSize	hdr/CFX/Headers/dosdrive.h	/^	ushort		bufferSize;				\/\/ buffer size in 512 byte segments$/;"	m	struct:ATADriveID
bufferType	hdr/CFX/Headers/dosdrive.h	/^	ushort		bufferType;				\/\/ buffer type$/;"	m	struct:ATADriveID
busy	hdr/CFX/Headers/_cfx_internals.h	/^	bool	busy		: 1;	\/\/ drive currently busy (online must be valid)$/;"	m	struct:ResetResume::__anon66
captFall	hdr/CFX/Headers/_cfx_expand.h	/^typedef enum { captLead, captFall } cpha;$/;"	e	enum:__anon46
captLead	hdr/CFX/Headers/_cfx_expand.h	/^typedef enum { captLead, captFall } cpha;$/;"	e	enum:__anon46
carraydump	hdr/CFX/Headers/_cfx_console.h	70;"	d
cdrain	hdr/CFX/Headers/_cfx_console.h	58;"	d
cf	hdr/CFX/Headers/_cfx_cpulevel.h	/^typedef struct { short entry[5]; ievfptr cf; short exit[3]; } IEVCWrapper;$/;"	m	struct:__anon40
cfErrorStart	hdr/CFX/Headers/_cfx_errors.h	/^	, cfErrorStart = CF_ERRORS$/;"	e	enum:__anon43
cfInvalidIDEport	hdr/CFX/Headers/_cfx_errors.h	/^	, cfInvalidIDEport			\/\/ CF driver references invalid IDE port$/;"	e	enum:__anon43
cfgetispeed	hdr/CFX/Headers/termios.h	57;"	d
cfgetospeed	hdr/CFX/Headers/termios.h	56;"	d
cfsetispeed	hdr/CFX/Headers/termios.h	59;"	d
cfsetospeed	hdr/CFX/Headers/termios.h	58;"	d
cgetc	hdr/CFX/Headers/_cfx_console.h	55;"	d
cgetclp	hdr/CFX/Headers/_cfx_console.h	60;"	d
cgetq	hdr/CFX/Headers/_cfx_console.h	54;"	d
cgets	hdr/CFX/Headers/_cfx_console.h	62;"	d
chexdump	hdr/CFX/Headers/_cfx_console.h	67;"	d
chore	hdr/CFX/Headers/_cfx_internals.h	/^	vfptr	chore;		\/\/ what we'll do$/;"	m	struct:ResetResume::__anon53
ciflush	hdr/CFX/Headers/_cfx_console.h	56;"	d
clockPhase	hdr/CFX/Headers/_cfx_expand.h	/^	cpha			clockPhase;		\/\/ SPI Clock Phase$/;"	m	struct:qpbDev
clockPolar	hdr/CFX/Headers/_cfx_expand.h	/^	cpol			clockPolar;		\/\/ SPI Clock Polarity$/;"	m	struct:qpbDev
clock_t	hdr/CPU32/Headers/StdC/time.h	/^typedef unsigned long	clock_t;$/;"	t
cmdApplicationError	hdr/CFX/Headers/_cfx_errors.h	/^	, cmdApplicationError		\/\/ "Application error"$/;"	e	enum:__anon43
cmdCancelled	hdr/CFX/Headers/_cfx_errors.h	/^	, cmdCancelled				\/\/ "Cancelled" -- always last item$/;"	e	enum:__anon43
cmdCantWithPicoZOOM	hdr/CFX/Headers/_cfx_errors.h	/^	, cmdCantWithPicoZOOM		\/\/ "Can't with PicoZOOM active"$/;"	e	enum:__anon43
cmdDriveError	hdr/CFX/Headers/_cfx_errors.h	/^	, cmdDriveError				\/\/ "Drive error"$/;"	e	enum:__anon43
cmdDriveNotFound	hdr/CFX/Headers/_cfx_errors.h	/^	, cmdDriveNotFound			\/\/ "Drive not found"$/;"	e	enum:__anon43
cmdErrGeneralFailure	hdr/CFX/Headers/_cfx_errors.h	/^	, cmdErrGeneralFailure		\/\/ "Failed"$/;"	e	enum:__anon43
cmdErrInvalidParam	hdr/CFX/Headers/_cfx_errors.h	/^	, cmdErrInvalidParam		\/\/ "Invalid argument(s)"$/;"	e	enum:__anon43
cmdErrPrivViolation	hdr/CFX/Headers/_cfx_errors.h	/^	, cmdErrPrivViolation		\/\/ "Privilege violation"$/;"	e	enum:__anon43
cmdErrUnknownCommand	hdr/CFX/Headers/_cfx_errors.h	/^	, cmdErrUnknownCommand		\/\/ "Bad command or file name"$/;"	e	enum:__anon43
cmdErrWrongParmCount	hdr/CFX/Headers/_cfx_errors.h	/^	, cmdErrWrongParmCount		\/\/ "Wrong number of arguments"$/;"	e	enum:__anon43
cmdErrorStart	hdr/CFX/Headers/_cfx_errors.h	/^	, cmdErrorStart = CMD_ERRORS$/;"	e	enum:__anon43
cmdExpectingValue	hdr/CFX/Headers/_cfx_errors.h	/^	, cmdExpectingValue			\/\/ "ExpectingValue"$/;"	e	enum:__anon43
cmdFileNotFound	hdr/CFX/Headers/_cfx_errors.h	/^	, cmdFileNotFound			\/\/ "File not found"$/;"	e	enum:__anon43
cmdInvalidAddress	hdr/CFX/Headers/_cfx_errors.h	/^	, cmdInvalidAddress			\/\/ "Invalid Address"$/;"	e	enum:__anon43
cmdInvalidRange	hdr/CFX/Headers/_cfx_errors.h	/^	, cmdInvalidRange			\/\/ "Invalid Range"$/;"	e	enum:__anon43
cmdNoErr	hdr/CFX/Headers/_cfx_errors.h	/^	, cmdNoErr = 0$/;"	e	enum:__anon43
cmdNotEnoughMemory	hdr/CFX/Headers/_cfx_errors.h	/^	, cmdNotEnoughMemory		\/\/ "Not enough memory"$/;"	e	enum:__anon43
cmdOddAddress	hdr/CFX/Headers/_cfx_errors.h	/^	, cmdOddAddress				\/\/ "Odd Address"$/;"	e	enum:__anon43
cmdReqParamMissing	hdr/CFX/Headers/_cfx_errors.h	/^	, cmdReqParamMissing		\/\/ "Required parameter missing"$/;"	e	enum:__anon43
cmdWrongParmType	hdr/CFX/Headers/_cfx_errors.h	/^	, cmdWrongParmType			\/\/ "Wrong Parameter Type"$/;"	e	enum:__anon43
cmdchrs	hdr/CFX/Headers/_cfx_console.h	/^	char		*cmdchrs;				\/\/ command token to match$/;"	m	struct:CmdTable
cmdf	hdr/CFX/Headers/_cfx_console.h	/^typedef char 	*cmdf(struct CmdInfo *);	\/\/ standard CMD handler$/;"	t
cmdfptr	hdr/CFX/Headers/_cfx_console.h	/^typedef cmdf	*cmdfptr;					\/\/ pointer to same$/;"	t
coflush	hdr/CFX/Headers/_cfx_console.h	59;"	d
compl	hdr/CPU32/Headers/StdC/iso646.h	12;"	d
config	hdr/CFX/Headers/dosdrive.h	/^	ushort		config;					\/\/ general configuration bits$/;"	m	struct:ATADriveID
contCSMulti	hdr/CFX/Headers/_cfx_expand.h	/^	bool			contCSMulti;	\/\/ Continue CS assert between mult xfrs$/;"	m	struct:qpbDev
cpha	hdr/CFX/Headers/_cfx_expand.h	/^typedef enum { captLead, captFall } cpha;$/;"	t	typeref:enum:__anon46
cpol	hdr/CFX/Headers/_cfx_expand.h	/^typedef enum { iaLowSCK, iaHighSCK } cpol;$/;"	t	typeref:enum:__anon45
cprintf	hdr/CFX/Headers/_cfx_console.h	64;"	d
cputc	hdr/CFX/Headers/_cfx_console.h	57;"	d
cputs	hdr/CFX/Headers/_cfx_console.h	63;"	d
crRepOk	hdr/CFX/Headers/_cfx_console.h	/^	char		crRepOk;				\/\/ repeat on just CR$/;"	m	struct:CmdTable
crc	hdr/CFX/Headers/_cfx_internals.h	/^	ushort	crc;			\/\/ of all fields$/;"	m	struct:ResetResume::__anon64
csprintf	hdr/CFX/Headers/_cfx_console.h	65;"	d
cstructdump	hdr/CFX/Headers/_cfx_console.h	69;"	d
ctp	hdr/CFX/Headers/_cfx_console.h	/^	CmdTablePtr	ctp;					\/\/ working command table$/;"	m	struct:CmdInfo
curNumCyl	hdr/CFX/Headers/dosdrive.h	/^	ushort		curNumCyl;				\/\/ apparent number of cylinders$/;"	m	struct:ATADriveID
curNumHeads	hdr/CFX/Headers/dosdrive.h	/^	ushort		curNumHeads;			\/\/ apparent number of heads$/;"	m	struct:ATADriveID
curSectorCapacity	hdr/CFX/Headers/dosdrive.h	/^	ulong		curSectorCapacity;		\/\/ apparent capacity in sectors$/;"	m	struct:ATADriveID
curSectorsPerTrack	hdr/CFX/Headers/dosdrive.h	/^	ushort		curSectorsPerTrack;		\/\/ apparent sectors per track$/;"	m	struct:ATADriveID
currency_symbol	hdr/CPU32/Headers/StdC/locale.h	/^	char	*currency_symbol;$/;"	m	struct:lconv
cvprintf	hdr/CFX/Headers/_cfx_console.h	66;"	d
cycleTimingDMA	hdr/CFX/Headers/dosdrive.h	/^	ushort		cycleTimingDMA;			\/\/ timing mode for DMA$/;"	m	struct:ATADriveID
cycleTimingPIO	hdr/CFX/Headers/dosdrive.h	/^	ushort		cycleTimingPIO;			\/\/ timing mode for PIO$/;"	m	struct:ATADriveID
d_attr	hdr/CFX/Headers/dirent.h	/^	uchar		d_attr;					\/\/ file\/entry attributes$/;"	m	struct:dirent
d_clustsize	hdr/CFX/Headers/dirent.h	/^	ulong		d_clustsize;			\/\/ cluster size in bytes$/;"	m	struct:dirent
d_clustsizeF16	hdr/CFX/Headers/dirent.h	/^	ushort		d_clustsizeF16;			\/\/ 2.2x legacy support$/;"	m	struct:dirent
d_link	hdr/CFX/Headers/dirent.h	/^	void		*d_link;				\/\/ used internally$/;"	m	struct:dirent
d_name	hdr/CFX/Headers/dirent.h	/^	uchar		d_name[13];				\/\/ expanded file name$/;"	m	struct:dirent
d_size	hdr/CFX/Headers/dirent.h	/^	ulong		d_size;					\/\/ actual size in bytes$/;"	m	struct:dirent
d_stcluster	hdr/CFX/Headers/dirent.h	/^	ulong		d_stcluster;			\/\/ starting cluster$/;"	m	struct:dirent
d_stclusterF16	hdr/CFX/Headers/dirent.h	/^	ushort		d_stclusterF16;			\/\/ 2.2x legacy support$/;"	m	struct:dirent
d_tm	hdr/CFX/Headers/dirent.h	/^	struct tm	d_tm;					\/\/ modification date in C time$/;"	m	struct:dirent	typeref:struct:dirent::tm
dasm10Vector	hdr/CFX/Headers/_cf1_ctm.h	/^	, dasm10Vector						\/\/  74	CTD10 (Pin22) IC\/OC Event Interrupt$/;"	e	enum:__anon32
dasm26Vector	hdr/CFX/Headers/_cf1_ctm.h	/^	, dasm26Vector				= 90	\/\/  90	CTD26 (Pin37) IC\/OC Event Interrupt$/;"	e	enum:__anon32
dasm27Vector	hdr/CFX/Headers/_cf1_ctm.h	/^	, dasm27Vector						\/\/  91	CTD27 (Pin35) IC\/OC Event Interrupt$/;"	e	enum:__anon32
dasm28Vector	hdr/CFX/Headers/_cf1_ctm.h	/^	, dasm28Vector						\/\/  92	CTD28 (Pin34) IC\/OC Event Interrupt$/;"	e	enum:__anon32
dasm29Vector	hdr/CFX/Headers/_cf1_ctm.h	/^	, dasm29Vector						\/\/  93	CTD29 (Pin33) IC\/OC Event Interrupt$/;"	e	enum:__anon32
dasm4Vector	hdr/CFX/Headers/_cf1_ctm.h	/^	, dasm4Vector						\/\/  68	CTD4 (Pin29) IC\/OC Event Interrupt$/;"	e	enum:__anon32
dasm5Vector	hdr/CFX/Headers/_cf1_ctm.h	/^	, dasm5Vector						\/\/  69	CTD5 (Pin27) IC\/OC Event Interrupt$/;"	e	enum:__anon32
dasm6Vector	hdr/CFX/Headers/_cf1_ctm.h	/^	, dasm6Vector						\/\/  70	CTD6 (Pin26) IC\/OC Event Interrupt$/;"	e	enum:__anon32
dasm7Vector	hdr/CFX/Headers/_cf1_ctm.h	/^	, dasm7Vector						\/\/  71	CTD7 (Pin24) IC\/OC Event Interrupt$/;"	e	enum:__anon32
dasm8Vector	hdr/CFX/Headers/_cf1_ctm.h	/^	, dasm8Vector						\/\/  72	CTD8 (Pin25) IC\/OC Event Interrupt$/;"	e	enum:__anon32
dasm9Vector	hdr/CFX/Headers/_cf1_ctm.h	/^	, dasm9Vector						\/\/  73	CTD9 (Pin23) IC\/OC Event Interrupt$/;"	e	enum:__anon32
data	hdr/CFX/Headers/_cfx_internals.h	/^	uchar	data[];	\/\/ name, 1 or 2 zeros, data, 0 or 1 zeros$/;"	m	struct:ResetResume::__anon64
dbrErrorStart	hdr/CFX/Headers/_cfx_errors.h	/^	, dbrErrorStart = DBR_ERRORS$/;"	e	enum:__anon43
dbrInvalidDOSBootRecord	hdr/CFX/Headers/_cfx_errors.h	/^	, dbrInvalidDOSBootRecord	\/\/ not a valid DOS boot sector$/;"	e	enum:__anon43
dbrV2BinsCantFAT32	hdr/CFX/Headers/_cfx_errors.h	/^	, dbrV2BinsCantFAT32		\/\/ PicoDOS version 2.xx binaries can't use FAT32$/;"	e	enum:__anon43
dd_buf	hdr/CFX/Headers/dirent.h	/^	void		*dd_buf;$/;"	m	struct:dir
dd_dirname	hdr/CFX/Headers/dirent.h	/^	uchar		dd_dirname[13];$/;"	m	struct:dir
dd_fd	hdr/CFX/Headers/dirent.h	/^	int			dd_fd;$/;"	m	struct:dir
dd_loc	hdr/CFX/Headers/dirent.h	/^	ulong		dd_loc;$/;"	m	struct:dir
dd_size	hdr/CFX/Headers/dirent.h	/^	ulong		dd_size;$/;"	m	struct:dir
decimal_point	hdr/CPU32/Headers/StdC/locale.h	/^	char	*decimal_point;$/;"	m	struct:lconv
defAbbrv	hdr/CFX/Headers/_cfx_console.h	188;"	d
defCROk	hdr/CFX/Headers/_cfx_console.h	186;"	d
defNumCyl	hdr/CFX/Headers/dosdrive.h	/^	ushort		defNumCyl;				\/\/ default number of cylinders$/;"	m	struct:ATADriveID
defNumHeads	hdr/CFX/Headers/dosdrive.h	/^	ushort		defNumHeads;			\/\/ default number of heads$/;"	m	struct:ATADriveID
defNumSectorsPerTrack	hdr/CFX/Headers/dosdrive.h	/^	ushort		defNumSectorsPerTrack;	\/\/ default number of sectors per track$/;"	m	struct:ATADriveID
defPriv	hdr/CFX/Headers/_cfx_console.h	184;"	d
dev	hdr/CFX/Headers/_cfx_expand.h	/^	QPBDev			*dev;			\/\/ device description$/;"	m	struct:__anon47
devName	hdr/CFX/Headers/_cfx_expand.h	/^	char			devName[16];	\/\/ C string with device name (15 max)$/;"	m	struct:qpbDev
dev_t	hdr/CFX/Headers/sys/stat.h	/^typedef unsigned long	dev_t;$/;"	t
dir	hdr/CFX/Headers/dirent.h	/^typedef struct dir$/;"	s
dirErrorStart	hdr/CFX/Headers/_cfx_errors.h	/^	, dirErrorStart = DIR_ERRORS$/;"	e	enum:__anon43
dirent	hdr/CFX/Headers/dirent.h	/^typedef struct dirent$/;"	s
div_t	hdr/CPU32/Headers/StdC/stdlib.h	/^	}	div_t;$/;"	t	typeref:struct:__anon104
dlen	hdr/CFX/Headers/_cfx_internals.h	/^	ushort	dlen	: 10;	\/\/ data length (1024 max)$/;"	m	struct:ResetResume::__anon64
dos33ext	hdr/CFX/Headers/dosdrive.h	/^	dos33ext		= 0x05,				\/\/ FAT12\/16 DOS extended 0MB-2GB$/;"	e	enum:__anon5
dos40pri	hdr/CFX/Headers/dosdrive.h	/^	dos40pri		= 0x06,				\/\/ FAT16 DOS primary 32MB-2GB$/;"	e	enum:__anon5
doubleWordSupport	hdr/CFX/Headers/dosdrive.h	/^	ushort		doubleWordSupport;		\/\/ supports double word I\/O$/;"	m	struct:ATADriveID
double_t	hdr/CFX/Headers/math.mx.h	/^typedef double double_t;$/;"	t
dsdDriveOutOfRange	hdr/CFX/Headers/_cfx_errors.h	/^	, dsdDriveOutOfRange		\/\/ requested drive number is out of range$/;"	e	enum:__anon43
dsdDriveReqAlreadyMounted	hdr/CFX/Headers/_cfx_errors.h	/^	, dsdDriveReqAlreadyMounted	\/\/ attempted to mount to drive letter in use$/;"	e	enum:__anon43
dsdDriveReqNotMounted	hdr/CFX/Headers/_cfx_errors.h	/^	, dsdDriveReqNotMounted		\/\/ requested logical drive is not mounted$/;"	e	enum:__anon43
dsdEndOfDir	hdr/CFX/Headers/_cfx_errors.h	/^	, dsdEndOfDir				\/\/ reached the end of the directory$/;"	e	enum:__anon43
dsdErrorStart	hdr/CFX/Headers/_cfx_errors.h	/^	, dsdErrorStart = DSD_ERRORS$/;"	e	enum:__anon43
dsdInvalidWorkingDirectory	hdr/CFX/Headers/_cfx_errors.h	/^	, dsdInvalidWorkingDirectory\/\/ tried to set invalid working directory$/;"	e	enum:__anon43
dsdMountCantFindSSD	hdr/CFX/Headers/_cfx_errors.h	/^	, dsdMountCantFindSSD		\/\/ failed to find SSD attempting mount$/;"	e	enum:__anon43
dsdNoMedia	hdr/CFX/Headers/_cfx_errors.h	/^	, dsdNoMedia				\/\/ no media found$/;"	e	enum:__anon43
dsdNotEnoughMemory	hdr/CFX/Headers/_cfx_errors.h	/^	, dsdNotEnoughMemory		\/\/ not enough memory to mount the drive$/;"	e	enum:__anon43
emumfg	hdr/CFX/Headers/_cfx_tpu.h	/^	char	emumfg;		\/\/ author identifier character, 'P' for Persistor$/;"	m	struct:__anon95
emuset	hdr/CFX/Headers/_cfx_tpu.h	/^	uchar	emuset;		\/\/ emulation set, 1 identifies PII standard$/;"	m	struct:__anon95
enabints	hdr/CFX/Headers/_cfx_tpu.h	/^	bool		enabints;		\/\/ true to enable interrupts$/;"	m	struct:TCHSetup
endHead	hdr/CFX/Headers/dosdrive.h	/^	uchar		endHead;$/;"	m	struct:__anon6
endSC	hdr/CFX/Headers/dosdrive.h	/^	ushort		endSC;$/;"	m	struct:__anon6
entry	hdr/CFX/Headers/_cfx_cpulevel.h	/^typedef struct { short entry[5]; ievfptr cf; short exit[3]; } IEVCWrapper;$/;"	m	struct:__anon40
errmes	hdr/CFX/Headers/_cfx_console.h	/^	char		*errmes;				\/\/ last error message$/;"	m	struct:CmdInfo
ets	hdr/CFX/Headers/dosdrive.h	72;"	d
execstr	hdr/CFX/Headers/_cfx_console.h	/^short	execstr(char *cmdstr, ...)					PICO_CALL(execstr);$/;"	v
execstrGeneralFailure	hdr/CFX/Headers/_cfx_console.h	/^enum {	execstrNoError = 0, execstrGeneralFailure = 1 };$/;"	e	enum:__anon34
execstrNoError	hdr/CFX/Headers/_cfx_console.h	/^enum {	execstrNoError = 0, execstrGeneralFailure = 1 };$/;"	e	enum:__anon34
exit	hdr/CFX/Headers/_cfx_cpulevel.h	/^typedef struct { short entry[5]; ievfptr cf; short exit[3]; } IEVCWrapper;$/;"	m	struct:__anon40
false	hdr/CFX/Headers/_cfx_types.h	82;"	d
false	hdr/CFX/Headers/mxcfxstd.h	100;"	d
fat12pri	hdr/CFX/Headers/dosdrive.h	/^	fat12pri		= 0x01,				\/\/ FAT12 DOS primary <16MB$/;"	e	enum:__anon5
fat16LBA	hdr/CFX/Headers/dosdrive.h	/^	fat16LBA		= 0x0E,				\/\/ FAT16 WIN95 primary LBA 32MB-2GB$/;"	e	enum:__anon5
fat16extLBA	hdr/CFX/Headers/dosdrive.h	/^	fat16extLBA		= 0x0F				\/\/ FAT16 WIN95 extended LBA 0MB-2GB$/;"	e	enum:__anon5
fat16pri	hdr/CFX/Headers/dosdrive.h	/^	fat16pri		= 0x04,				\/\/ FAT16 DOS primary 16MB-32MB$/;"	e	enum:__anon5
fat32LBA	hdr/CFX/Headers/dosdrive.h	/^	fat32LBA		= 0x0C,				\/\/ FAT32 WIN95 primary LBA 512MB-2TB$/;"	e	enum:__anon5
fat32pri	hdr/CFX/Headers/dosdrive.h	/^	fat32pri 		= 0x0B,				\/\/ FAT32 WIN95 primary 512MB-2TB$/;"	e	enum:__anon5
fatid	hdr/CFX/Headers/dosdrive.h	75;"	d
fats	hdr/CFX/Headers/dosdrive.h	64;"	d
fcsm3Vector	hdr/CFX/Headers/_cf1_ctm.h	/^	, fcsm3Vector						\/\/  67	FCSM Counter Overflow Interrupt$/;"	e	enum:__anon32
fdat	hdr/CFX/Headers/dosdrive.h	80;"	d
fdir	hdr/CFX/Headers/dosdrive.h	79;"	d
ffat	hdr/CFX/Headers/dosdrive.h	78;"	d
firmwareRevText	hdr/CFX/Headers/dosdrive.h	/^	char		firmwareRevText[8];		\/\/ firmware revision text (Rev M.ms)$/;"	m	struct:ATADriveID
flashAddrOutOfRange	hdr/CFX/Headers/_cfx_errors.h	/^	, flashAddrOutOfRange		\/\/ bad flash address specified$/;"	e	enum:__anon43
flashCantBurnZeroToOne	hdr/CFX/Headers/_cfx_errors.h	/^	, flashCantBurnZeroToOne	\/\/ attempting to write a one to a zeroed location$/;"	e	enum:__anon43
flashEraseDisabled	hdr/CFX/Headers/_cfx_errors.h	/^	, flashEraseDisabled		\/\/ request denied, erasing has been disabled$/;"	e	enum:__anon43
flashEraseFailed	hdr/CFX/Headers/_cfx_errors.h	/^	, flashEraseFailed			\/\/ failed attempting to erase flash$/;"	e	enum:__anon43
flashErrorStart	hdr/CFX/Headers/_cfx_errors.h	/^	, flashErrorStart = FLASH_ERRORS$/;"	e	enum:__anon43
flashInitFailedRepeat	hdr/CFX/Headers/_cfx_errors.h	/^	, flashInitFailedRepeat		\/\/ flash initialization failed, contact factory$/;"	e	enum:__anon43
flashInitFailedWrReMap	hdr/CFX/Headers/_cfx_errors.h	/^	, flashInitFailedWrReMap	\/\/ flash initialization failed, contact factory$/;"	e	enum:__anon43
flashNoError	hdr/CFX/Headers/_cfx_errors.h	/^	, flashNoError = 0$/;"	e	enum:__anon43
flashPrgEraConcludeFailed	hdr/CFX/Headers/_cfx_errors.h	/^	, flashPrgEraConcludeFailed	\/\/ failed concluding write or erase operation$/;"	e	enum:__anon43
flashProgramFailed	hdr/CFX/Headers/_cfx_errors.h	/^	, flashProgramFailed		\/\/ failed attempting to burn flash$/;"	e	enum:__anon43
flashRequestInvalidSector	hdr/CFX/Headers/_cfx_errors.h	/^	, flashRequestInvalidSector	\/\/ requested operation to invalid sector$/;"	e	enum:__anon43
flashRequestReservedSector	hdr/CFX/Headers/_cfx_errors.h	/^	, flashRequestReservedSector\/\/ request denied, can't write to reserved sectors$/;"	e	enum:__anon43
flashWordWriteToOddAddr	hdr/CFX/Headers/_cfx_errors.h	/^	, flashWordWriteToOddAddr	\/\/ attempting word write to odd flash address$/;"	e	enum:__anon43
flashWriteDisabled	hdr/CFX/Headers/_cfx_errors.h	/^	, flashWriteDisabled		\/\/ request denied, write has been disabled$/;"	e	enum:__anon43
flashWriteReqFromFlash	hdr/CFX/Headers/_cfx_errors.h	/^	, flashWriteReqFromFlash	\/\/ attempting flash write op from flash itself$/;"	e	enum:__anon43
float_t	hdr/CFX/Headers/math.mx.h	/^typedef float float_t;$/;"	t
flogf	hdr/CFX/Headers/_cfx_console.h	/^short	flogf(char *format, ...)					PICO_CALL(flogf);$/;"	v
flt	hdr/CFX/Headers/_cfx_util.h	/^typedef union { void *bin; char *str; long lng; float flt; } VEEData;$/;"	m	union:__anon103
fmtvct	hdr/CFX/Headers/_cfx_cpulevel.h	/^typedef struct { ushort sr; ulong pc; ushort fmtvct; ulong info[4]; } IEVStack;$/;"	m	struct:__anon39
fpos_t	hdr/CPU32/Headers/StdC/stdio.h	/^typedef unsigned long	fpos_t;$/;"	t
frac_digits	hdr/CPU32/Headers/StdC/locale.h	/^	char	frac_digits;$/;"	m	struct:lconv
function	hdr/CFX/Headers/_cfx_tpu.h	/^	ushort		function;		\/\/ from the function list$/;"	m	struct:TCHSetup
fv	hdr/CFX/Headers/_cfx_console.h	/^		float	fv;			\/\/ float (32 bit) value$/;"	m	union:__anon37::__anon38
getch	hdr/CFX/Headers/_cfx_console.h	76;"	d
getsfptr	hdr/CFX/Headers/_cfx_console.h	/^typedef short	(*getsfptr)(char *, short);	\/\/ line input function$/;"	t
getstr	hdr/CFX/Headers/_cfx_console.h	79;"	d
gid_t	hdr/CFX/Headers/sys/stat.h	/^typedef unsigned long	gid_t;$/;"	t
grouping	hdr/CPU32/Headers/StdC/locale.h	/^	char	*grouping;$/;"	m	struct:lconv
handle	hdr/CPU32/Headers/StdC/stdio.h	/^	struct _FILE { long handle; char pad[72]; };$/;"	m	struct:_FILE
handler	hdr/CFX/Headers/_cfx_console.h	/^	cmdfptr		handler;				\/\/ its associated C function$/;"	m	struct:CmdTable
hasv	hdr/CFX/Headers/_cfx_console.h	/^	bool		hasv;		\/\/ value was found and successfully sscanf'd$/;"	m	struct:__anon37
help	hdr/CFX/Headers/_cfx_console.h	/^	char		*help;					\/\/ help text for menus$/;"	m	struct:CmdTable
hexdump	hdr/CFX/Headers/_cfx_console.h	82;"	d
hs	hdr/CFX/Headers/dosdrive.h	71;"	d
hshkCtsRts	hdr/CFX/Headers/_cfx_sercomm.h	/^enum { hshkOff, hshkXonXoff, hshkCtsRts };$/;"	e	enum:__anon81
hshkOff	hdr/CFX/Headers/_cfx_sercomm.h	/^enum { hshkOff, hshkXonXoff, hshkCtsRts };$/;"	e	enum:__anon81
hshkXonXoff	hdr/CFX/Headers/_cfx_sercomm.h	/^enum { hshkOff, hshkXonXoff, hshkCtsRts };$/;"	e	enum:__anon81
iaHighSCK	hdr/CFX/Headers/_cfx_expand.h	/^typedef enum { iaLowSCK, iaHighSCK } cpol;$/;"	e	enum:__anon45
iaLowSCK	hdr/CFX/Headers/_cfx_expand.h	/^typedef enum { iaLowSCK, iaHighSCK } cpol;$/;"	e	enum:__anon45
id	hdr/CFX/Headers/_cfx_internals.h	/^typedef	union { struct { ushort id; ushort sn; }; ulong ref; } SCSDevice;$/;"	m	struct:ResetResume::__anon68::__anon69
idch	hdr/CFX/Headers/_cfx_console.h	/^	const char	idch;		\/\/ switch ID character ('V' for \/V)$/;"	m	struct:__anon37
idePort	hdr/CFX/Headers/_cfx_internals.h	/^	vusptr	 	idePort;		\/\/ base address for IDE registers$/;"	m	struct:ResetResume::__anon67
ievfptr	hdr/CFX/Headers/_cfx_cpulevel.h	/^typedef	void (*ievfptr)(IEVStack *ievstack:__a0);$/;"	t
info	hdr/CFX/Headers/_cfx_cpulevel.h	/^typedef struct { ushort sr; ulong pc; ushort fmtvct; ulong info[4]; } IEVStack;$/;"	m	struct:__anon39
init	hdr/CFX/Headers/cfxad.h	/^	bool	(*init)(ushort qslot, void *ad);$/;"	m	struct:__anon1
ino_t	hdr/CFX/Headers/sys/stat.h	/^typedef unsigned long	ino_t;$/;"	t
int_curr_symbol	hdr/CPU32/Headers/StdC/locale.h	/^	char	*int_curr_symbol;$/;"	m	struct:lconv
int_frac_digits	hdr/CPU32/Headers/StdC/locale.h	/^	char	int_frac_digits;$/;"	m	struct:lconv
isPowered	hdr/CFX/Headers/_cfx_internals.h	/^	bool		isPowered;		\/\/ true when known to be on$/;"	m	struct:ResetResume::__anon67
isnan	hdr/CFX/Headers/math.mx.h	65;"	d
isnormal	hdr/CFX/Headers/math.mx.h	64;"	d
isrange	hdr/CFX/Headers/_cfx_console.h	/^	int			isrange		: 1;		\/\/ args form range of values$/;"	m	struct:__anon36
isval	hdr/CFX/Headers/_cfx_console.h	/^	int			isval		: 1;		\/\/ arg contains value$/;"	m	struct:__anon36
jmp_buf	hdr/CPU32/Headers/StdC/setjmp.h	/^  typedef long *jmp_buf[13];$/;"	t
justCR	hdr/CFX/Headers/_cfx_console.h	/^	char		justCR;					\/\/ last was just carriage return$/;"	m	struct:CmdInfo
justCROk	hdr/CFX/Headers/_cfx_console.h	/^	char		justCROk;				\/\/ accept CR repeat commands (default)$/;"	m	struct:CmdInfo
kbflush	hdr/CFX/Headers/_cfx_console.h	75;"	d
kbhit	hdr/CFX/Headers/_cfx_console.h	74;"	d
l	hdr/CFX/Headers/_cfx_util.h	/^typedef	union	{ uchar	b[4]; ushort w[2]; ulong l; } BWL, *pBWL;$/;"	m	union:__anon99
lconv	hdr/CPU32/Headers/StdC/locale.h	/^struct lconv$/;"	s
ldat	hdr/CFX/Headers/dosdrive.h	81;"	d
ldiv_t	hdr/CPU32/Headers/StdC/stdlib.h	/^	}	ldiv_t;$/;"	t	typeref:struct:__anon105
level2InterruptAutovector	hdr/CFX/Headers/_cfx_cpulevel.h	/^	, level2InterruptAutovector	= 26	\/\/  26	IRQ2 (Pin41) Interrupt$/;"	e	enum:__anon41
level3InterruptAutovector	hdr/CFX/Headers/_cfx_cpulevel.h	/^	, level3InterruptAutovector	= 27	\/\/  27	IRQ3 (RXX\/RSRXX) Interrupt$/;"	e	enum:__anon41
level4InterruptAutovector	hdr/CFX/Headers/_cfx_cpulevel.h	/^	, level4InterruptAutovector	= 28	\/\/  28	IRQ4 (RSRxD) Interrupt$/;"	e	enum:__anon41
level5InterruptAutovector	hdr/CFX/Headers/_cfx_cpulevel.h	/^	, level5InterruptAutovector	= 29	\/\/  29	IRQ5 (Pin39) Interrupt$/;"	e	enum:__anon41
level7InterruptAutovector	hdr/CFX/Headers/_cfx_cpulevel.h	/^	, level7InterruptAutovector	= 31	\/\/  31	IRQ7 (Pin40) Interrupt$/;"	e	enum:__anon41
line	hdr/CFX/Headers/_cfx_console.h	/^	char		line[CMDLINELEN];		\/\/ working command line buffer$/;"	m	struct:CmdInfo
lineDelim	hdr/CFX/Headers/_cfx_console.h	/^	char		lineDelim;				\/\/ default as #defined$/;"	m	struct:CmdInfo
lng	hdr/CFX/Headers/_cfx_util.h	/^typedef union { void *bin; char *str; long lng; float flt; } VEEData;$/;"	m	union:__anon103
lock	hdr/CFX/Headers/cfxad.h	/^	bool	(*lock)(ushort qslot);$/;"	m	struct:__anon1
lv	hdr/CFX/Headers/_cfx_console.h	/^		long	lv;			\/\/ long or unsigned long value$/;"	m	union:__anon37::__anon38
maxBaud	hdr/CFX/Headers/_cfx_expand.h	/^	ulong			maxBaud;		\/\/ maximum baud rate in Hz for device$/;"	m	struct:qpbDev
maxcl	hdr/CFX/Headers/dosdrive.h	82;"	d
mbrErrorStart	hdr/CFX/Headers/_cfx_errors.h	/^	, mbrErrorStart = MBR_ERRORS$/;"	e	enum:__anon43
mbrInvalidDOSPrtnSector	hdr/CFX/Headers/_cfx_errors.h	/^	, mbrInvalidDOSPrtnSector	\/\/ MBR is not valid DOS partition sector$/;"	e	enum:__anon43
mbrInvalidPartitionIndex	hdr/CFX/Headers/_cfx_errors.h	/^	, mbrInvalidPartitionIndex	\/\/ invalid partition index (>= MBR_MAX_PARTS)$/;"	e	enum:__anon43
mbrNoKnownPartitionType	hdr/CFX/Headers/_cfx_errors.h	/^	, mbrNoKnownPartitionType	\/\/ no recognizable partitions found$/;"	e	enum:__anon43
mbrNotDOSPartitionType	hdr/CFX/Headers/_cfx_errors.h	/^	, mbrNotDOSPartitionType	\/\/ partition is not valid DOS type$/;"	e	enum:__anon43
mbstate_t	hdr/CPU32/Headers/StdC/wchar_t.h	/^typedef int 			mbstate_t;$/;"	t
mcsm2Vector	hdr/CFX/Headers/_cf1_ctm.h	/^	, mcsm2Vector				= 66	\/\/  66	MCSM2 Counter Overflow Interrupt$/;"	e	enum:__anon32
mcsm30Vector	hdr/CFX/Headers/_cf1_ctm.h	/^	, mcsm30Vector						\/\/  94	MCSM30 Counter Overflow Interrupt$/;"	e	enum:__anon32
mcsm31Vector	hdr/CFX/Headers/_cf1_ctm.h	/^	, mcsm31Vector						\/\/  95	MCSM31 Counter Overflow Interrupt$/;"	e	enum:__anon32
md	hdr/CFX/Headers/dosdrive.h	67;"	d
minParmCount	hdr/CFX/Headers/_cfx_console.h	/^ 	char		minParmCount;			\/\/ minimum required parameters$/;"	m	struct:CmdTable
mode_t	hdr/CFX/Headers/sys/stat.h	/^typedef unsigned long	mode_t;$/;"	t
model	hdr/CFX/Headers/_cfx_globals.h	/^	char	model[8];			\/\/ "CF1", "CF2"$/;"	m	struct:__anon50
modelNumText	hdr/CFX/Headers/dosdrive.h	/^	char		modelNumText[40];		\/\/ model number in right justified text$/;"	m	struct:ATADriveID
modtime	hdr/CFX/Headers/utime.h	/^	time_t		modtime;				\/\/ modification time$/;"	m	struct:utimbuf
mon_decimal_point	hdr/CPU32/Headers/StdC/locale.h	/^	char	*mon_decimal_point;$/;"	m	struct:lconv
mon_grouping	hdr/CPU32/Headers/StdC/locale.h	/^	char	*mon_grouping;$/;"	m	struct:lconv
mon_thousands_sep	hdr/CPU32/Headers/StdC/locale.h	/^	char	*mon_thousands_sep;$/;"	m	struct:lconv
multSectorsPerInt	hdr/CFX/Headers/dosdrive.h	/^	ushort		multSectorsPerInt;		\/\/ multiple sectors per interrupt$/;"	m	struct:ATADriveID
n_cs_precedes	hdr/CPU32/Headers/StdC/locale.h	/^	char	n_cs_precedes;$/;"	m	struct:lconv
n_sep_by_space	hdr/CPU32/Headers/StdC/locale.h	/^	char	n_sep_by_space;$/;"	m	struct:lconv
n_sign_posn	hdr/CPU32/Headers/StdC/locale.h	/^	char	n_sign_posn;$/;"	m	struct:lconv
name	hdr/CFX/Headers/dosdrive.h	74;"	d
negative_sign	hdr/CPU32/Headers/StdC/locale.h	/^	char	*negative_sign;$/;"	m	struct:lconv
nh	hdr/CFX/Headers/dosdrive.h	70;"	d
nlen	hdr/CFX/Headers/_cfx_internals.h	/^	ushort	nlen	: 4;	\/\/ name length (15+'\\0' max)$/;"	m	struct:ResetResume::__anon64
nlink_t	hdr/CFX/Headers/sys/stat.h	/^typedef short			nlink_t;$/;"	t
nondos	hdr/CFX/Headers/dosdrive.h	/^	nondos			= 0x00,				\/\/ unused partition table entry$/;"	e	enum:__anon5
not	hdr/CPU32/Headers/StdC/iso646.h	13;"	d
not_eq	hdr/CPU32/Headers/StdC/iso646.h	14;"	d
nottarg	hdr/CFX/Headers/_cfx_internals.h	/^	ulong	nottarg;		\/\/ logical compliment of target address above$/;"	m	struct:ResResData
nsCFStd	hdr/CFX/Headers/_cfx_internals.h	/^  enum { nsFlashStd = 100, nsRAMStd = 70, nsCFStd = 190 };$/;"	e	enum:ResetResume::__anon55
nsCFStd	hdr/CFX/Headers/_cfx_internals.h	/^  enum { nsFlashStd = 90, nsRAMStd = 70, nsCFStd = 190 };$/;"	e	enum:ResetResume::__anon56
nsFlashStd	hdr/CFX/Headers/_cfx_internals.h	/^  enum { nsFlashStd = 100, nsRAMStd = 70, nsCFStd = 190 };$/;"	e	enum:ResetResume::__anon55
nsFlashStd	hdr/CFX/Headers/_cfx_internals.h	/^  enum { nsFlashStd = 90, nsRAMStd = 70, nsCFStd = 190 };$/;"	e	enum:ResetResume::__anon56
nsMotoSpecAdj	hdr/CFX/Headers/_cfx_internals.h	/^enum { nsMotoSpecAdj = 0, nsStdSmallBusAdj = -10 };$/;"	e	enum:ResetResume::__anon57
nsRAMStd	hdr/CFX/Headers/_cfx_internals.h	/^  enum { nsFlashStd = 100, nsRAMStd = 70, nsCFStd = 190 };$/;"	e	enum:ResetResume::__anon55
nsRAMStd	hdr/CFX/Headers/_cfx_internals.h	/^  enum { nsFlashStd = 90, nsRAMStd = 70, nsCFStd = 190 };$/;"	e	enum:ResetResume::__anon56
nsStdSmallBusAdj	hdr/CFX/Headers/_cfx_internals.h	/^enum { nsMotoSpecAdj = 0, nsStdSmallBusAdj = -10 };$/;"	e	enum:ResetResume::__anon57
numBase	hdr/CFX/Headers/_cfx_console.h	/^	char		numBase;				\/\/ default for scan (0 is no scan)$/;"	m	struct:CmdTable
numECCBytes	hdr/CFX/Headers/dosdrive.h	/^	ushort		numECCBytes;			\/\/ number of ECC bytes passed on long commands$/;"	m	struct:ATADriveID
numSectors	hdr/CFX/Headers/dosdrive.h	/^	ulong		numSectors;				\/\/ number of sectors in partition$/;"	m	struct:__anon6
off_t	hdr/CFX/Headers/sys/stat.h	/^typedef long			off_t;$/;"	t
offsetof	hdr/CPU32/Headers/StdC/stddef.h	12;"	d
online	hdr/CFX/Headers/_cfx_internals.h	/^	bool	online		: 1;	\/\/ drive should be up and running$/;"	m	struct:ResetResume::__anon66
optDelims	hdr/CFX/Headers/_cfx_console.h	/^	char		optDelims;				\/\/ default as #defined$/;"	m	struct:CmdInfo
options	hdr/CFX/Headers/_cfx_console.h	/^	char		*options;				\/\/ command modifiers (typ. CMD.mmm)$/;"	m	struct:CmdInfo
or	hdr/CPU32/Headers/StdC/iso646.h	15;"	d
or_eq	hdr/CPU32/Headers/StdC/iso646.h	16;"	d
pBW	hdr/CFX/Headers/_cfx_util.h	/^typedef	union	{ uchar	b[2]; ushort w; } BW, *pBW;$/;"	t	typeref:union:__anon98
pBWL	hdr/CFX/Headers/_cfx_util.h	/^typedef	union	{ uchar	b[4]; ushort w[2]; ulong l; } BWL, *pBWL;$/;"	t	typeref:union:__anon99
p_cs_precedes	hdr/CPU32/Headers/StdC/locale.h	/^	char	p_cs_precedes;$/;"	m	struct:lconv
p_sep_by_space	hdr/CPU32/Headers/StdC/locale.h	/^	char	p_sep_by_space;$/;"	m	struct:lconv
p_sign_posn	hdr/CPU32/Headers/StdC/locale.h	/^	char	p_sign_posn;$/;"	m	struct:lconv
pacDetectEither	hdr/CFX/Headers/_cfx_tpu.h	/^	pacDetectEither =	0x0C,	\/\/ Detect Either Edge$/;"	e	enum:PACChanControl
pacDetectFalling	hdr/CFX/Headers/_cfx_tpu.h	/^	pacDetectFalling =	0x08,	\/\/ Detect Falling Edge$/;"	e	enum:PACChanControl
pacDetectRising	hdr/CFX/Headers/_cfx_tpu.h	/^	pacDetectRising =	0x04,	\/\/ Detect Rising Edge$/;"	e	enum:PACChanControl
pacDoNotChange	hdr/CFX/Headers/_cfx_tpu.h	/^	pacDoNotChange =	0x10	\/\/ Do Not Change PAC$/;"	e	enum:PACChanControl
pacDoNotChangePin	hdr/CFX/Headers/_cfx_tpu.h	/^	pacDoNotChangePin =	0x00,						\/\/ Do Not Change Pin State$/;"	e	enum:PACChanControl
pacDoNotDetect	hdr/CFX/Headers/_cfx_tpu.h	/^	pacDoNotDetect = 	0x00,	\/\/ Do Not Detect Transition$/;"	e	enum:PACChanControl
pacHighOnMatch	hdr/CFX/Headers/_cfx_tpu.h	/^	pacHighOnMatch =	0x04,						\/\/ High on Match$/;"	e	enum:PACChanControl
pacLowOnMatch	hdr/CFX/Headers/_cfx_tpu.h	/^	pacLowOnMatch =		0x08,						\/\/ Low on Match$/;"	e	enum:PACChanControl
pacToggleOnMatch	hdr/CFX/Headers/_cfx_tpu.h	/^	pacToggleOnMatch =	0x0C,						\/\/ Toggle on Match$/;"	e	enum:PACChanControl
pad	hdr/CPU32/Headers/StdC/stdio.h	/^	struct _FILE { long handle; char pad[72]; };$/;"	m	struct:_FILE
parity	hdr/CFX/Headers/_cfx_sercomm.h	/^	short	parity;		\/\/ parity: 'o','O','e','E', all else is none$/;"	m	struct:__anon84
pbmver	hdr/CFX/Headers/_cfx_globals.h	/^	char	pbmver[8];			\/\/ "2.28  ", "2.28p1"$/;"	m	struct:__anon50
pc	hdr/CFX/Headers/_cfx_cpulevel.h	/^typedef struct { ushort sr; ulong pc; ushort fmtvct; ulong info[4]; } IEVStack;$/;"	m	struct:__anon39
pdxErrorStart	hdr/CFX/Headers/_cfx_errors.h	/^	, pdxErrorStart = PDX_ERRORS$/;"	e	enum:__anon43
pdx_attrib	hdr/CFX/Headers/_cfx_files.h	/^			uchar setatrb, uchar clratrb)				PICO_CALL(pdx_attrib);$/;"	v
pdx_chdir	hdr/CFX/Headers/_cfx_files.h	/^short	pdx_chdir(const char *path)						PICO_CALL(pdx_chdir);$/;"	v
pdx_close	hdr/CFX/Headers/_cfx_files.h	/^short	pdx_close(short fd)								PICO_CALL(pdx_close);$/;"	v
pdx_copy	hdr/CFX/Headers/_cfx_files.h	/^			short vflag, void *buf, ulong bufsize)		PICO_CALL(pdx_copy);$/;"	v
pdx_creat	hdr/CFX/Headers/_cfx_files.h	/^short	pdx_creat(const char *path, long mode)			PICO_CALL(pdx_creat);$/;"	v
pdx_errno	hdr/CFX/Headers/_cfx_files.h	/^short	pdx_errno(bool clear)							PICO_CALL(pdx_errno);$/;"	v
pdx_fcntl	hdr/CFX/Headers/_cfx_files.h	/^short	pdx_fcntl(short fildes, short cmd, ...)			PICO_CALL(pdx_fcntl);$/;"	v
pdx_fstat	hdr/CFX/Headers/_cfx_files.h	/^short	pdx_fstat(int fd, struct stat *buf)				PICO_CALL(pdx_fstat);$/;"	v
pdx_getcwd	hdr/CFX/Headers/_cfx_files.h	/^char	*pdx_getcwd(char *buf, ulong size)				PICO_CALL(pdx_getcwd);$/;"	v
pdx_isatty	hdr/CFX/Headers/_cfx_files.h	/^short	pdx_isatty(short fd)							PICO_CALL(pdx_isatty);$/;"	v
pdx_lseek	hdr/CFX/Headers/_cfx_files.h	/^long	pdx_lseek(short fd, long offset, short whence)	PICO_CALL(pdx_lseek);$/;"	v
pdx_mkdir	hdr/CFX/Headers/_cfx_files.h	/^short	pdx_mkdir(const char *path, short mode)			PICO_CALL(pdx_mkdir);$/;"	v
pdx_open	hdr/CFX/Headers/_cfx_files.h	/^short	pdx_open(const char *path, short oflag)			PICO_CALL(pdx_open);$/;"	v
pdx_read	hdr/CFX/Headers/_cfx_files.h	/^ulong	pdx_read(short fd, void *buf, ulong nbyte)		PICO_CALL(pdx_read);$/;"	v
pdx_rename	hdr/CFX/Headers/_cfx_files.h	/^short	pdx_rename(const char *old, const char *nwn)	PICO_CALL(pdx_rename);$/;"	v
pdx_rmdir	hdr/CFX/Headers/_cfx_files.h	/^short	pdx_rmdir(const char *path)						PICO_CALL(pdx_rmdir);$/;"	v
pdx_stat	hdr/CFX/Headers/_cfx_files.h	/^short	pdx_stat(const char *path, struct stat *buf)	PICO_CALL(pdx_stat);$/;"	v
pdx_unlink	hdr/CFX/Headers/_cfx_files.h	/^short	pdx_unlink(const char *path)					PICO_CALL(pdx_unlink);$/;"	v
pdx_utime	hdr/CFX/Headers/_cfx_files.h	/^			const struct utimbuf *tm)					PICO_CALL(pdx_utime);$/;"	v
pdx_write	hdr/CFX/Headers/_cfx_files.h	/^ulong	pdx_write(short fd, void *buf, ulong nbyte)		PICO_CALL(pdx_write);$/;"	v
picodos	hdr/CFX/Headers/_cfx_globals.h	/^	char	picodos[10];		\/\/ "2.28r1  ", "2.28r1p1"$/;"	m	struct:__anon50
picodosver	hdr/CFX/Headers/_cfx_globals.h	/^char	*picodosver(void)							PICO_CALL(picodosver);$/;"	v
pitVector	hdr/CFX/Headers/_cf1_ctm.h	/^	, pitVector					= 87	\/\/  87	Periodic Timer Interrupt$/;"	e	enum:__anon32
pitVector	hdr/CFX/Headers/_cfx_tpu.h	/^	, pitVector							\/\/  82	Periodic Timer Interrupt$/;"	e	enum:__anon96
pos	hdr/CFX/Headers/_cfx_console.h	/^	char		pos;		\/\/ was specified, at argv position n$/;"	m	struct:__anon37
positive_sign	hdr/CPU32/Headers/StdC/locale.h	/^	char	*positive_sign;$/;"	m	struct:lconv
powerDown	hdr/CFX/Headers/cfxad.h	/^	void	(*powerDown)(ushort qslot, bool pdfull);$/;"	m	struct:__anon1
poweron	hdr/CFX/Headers/_cfx_internals.h	/^	bool	poweron		: 1;	\/\/ we're applying power to the drive$/;"	m	struct:ResetResume::__anon66
ppbErrorStart	hdr/CFX/Headers/_cfx_errors.h	/^	, ppbErrorStart = PPB_ERRORS$/;"	e	enum:__anon43
ppbMemAccReqToNonMem	hdr/CFX/Headers/_cfx_errors.h	/^	, ppbMemAccReqToNonMem		\/\/ attempted direct memory access to non-memory device$/;"	e	enum:__anon43
ppbRdBufferEmpty	hdr/CFX/Headers/_cfx_errors.h	/^	, ppbRdBufferEmpty			\/\/ no data in read buffer$/;"	e	enum:__anon43
ppbRdIOFailed	hdr/CFX/Headers/_cfx_errors.h	/^	, ppbRdIOFailed				\/\/ low level read failed$/;"	e	enum:__anon43
ppbWrIOFailed	hdr/CFX/Headers/_cfx_errors.h	/^	, ppbWrIOFailed				\/\/ low level write failed$/;"	e	enum:__anon43
ppbWrOvflBufNotEmpty	hdr/CFX/Headers/_cfx_errors.h	/^	, ppbWrOvflBufNotEmpty		\/\/ write operation would overflow to non-empty buffer$/;"	e	enum:__anon43
pppRdBufferInUse	hdr/CFX/Headers/_cfx_errors.h	/^	, pppRdBufferInUse			\/\/ attempted to read while another read was in process$/;"	e	enum:__anon43
pppWrBufferInUse	hdr/CFX/Headers/_cfx_errors.h	/^	, pppWrBufferInUse			\/\/ attempted to write while another write was in process$/;"	e	enum:__anon43
pram	hdr/CFX/Headers/_cfx_tpu.h	/^	ushort		pram[8];		\/\/ initial parameter ram$/;"	m	struct:TCHSetup
priority	hdr/CFX/Headers/_cfx_internals.h	/^	ushort	priority;	\/\/ when we'll get around to it$/;"	m	struct:ResetResume::__anon53
priority	hdr/CFX/Headers/_cfx_tpu.h	/^	TPUPriority priority;		\/\/ initial priority$/;"	m	struct:TCHSetup
priv	hdr/CFX/Headers/_cfx_expand.h	/^	struct QPBPriv	*priv;			\/\/ parameters used internally$/;"	m	struct:__anon47	typeref:struct:__anon47::QPBPriv
privLevel	hdr/CFX/Headers/_cfx_console.h	/^	char		privLevel;				\/\/ privilege level to invoke$/;"	m	struct:CmdTable
privLevel	hdr/CFX/Headers/_cfx_console.h	/^	char		privLevel;				\/\/ working privilege level$/;"	m	struct:CmdInfo
psDelaySCK	hdr/CFX/Headers/_cfx_expand.h	/^	ulong			psDelaySCK;		\/\/ Min. Delay Before SCK (pico secs)$/;"	m	struct:qpbDev
psDelayTXFR	hdr/CFX/Headers/_cfx_expand.h	/^	ulong			psDelayTXFR;	\/\/ Min. Delay After Transfer (pico secs)$/;"	m	struct:qpbDev
pscDoNotForce	hdr/CFX/Headers/_cfx_tpu.h	/^	pscDoNotForce =		0x03	\/\/ Do Not Force Any State$/;"	e	enum:PSCChanControl
pscForcePinHigh	hdr/CFX/Headers/_cfx_tpu.h	/^	pscForcePinHigh =	0x01,	\/\/ Force Pin High$/;"	e	enum:PSCChanControl
pscForcePinLow	hdr/CFX/Headers/_cfx_tpu.h	/^	pscForcePinLow =	0x02,	\/\/ Force Pin Low$/;"	e	enum:PSCChanControl
pscForcePinPAC	hdr/CFX/Headers/_cfx_tpu.h	/^	pscForcePinPAC =	0x00,	\/\/ Force Pin as Specified by PAC Latches$/;"	e	enum:PSCChanControl
ptrdiff_t	hdr/CPU32/Headers/StdC/stddef.h	/^typedef long			ptrdiff_t;$/;"	t
ptype	hdr/CFX/Headers/dosdrive.h	/^	uchar		ptype;					\/\/ from enum list above (+ others)$/;"	m	struct:__anon6
putch	hdr/CFX/Headers/_cfx_console.h	77;"	d
putflush	hdr/CFX/Headers/_cfx_console.h	78;"	d
putstr	hdr/CFX/Headers/_cfx_console.h	80;"	d
qpbDev	hdr/CFX/Headers/_cfx_expand.h	/^typedef struct qpbDev$/;"	s
qslot	hdr/CFX/Headers/_cfx_expand.h	/^	ushort			qslot;			\/\/ QSPI slot associated with this device$/;"	m	struct:qpbDev
qslot	hdr/CFX/Headers/cfxad.h	/^	ushort	qslot;$/;"	m	struct:__anon1
qspiVector	hdr/CFX/Headers/_cf1_ctm.h	/^	, qspiVector						\/\/  65	Queued Serial Peripheral Interface$/;"	e	enum:__anon32
qspiVector	hdr/CFX/Headers/_cfx_tpu.h	/^	, qspiVector						\/\/  81	Queued Serial Peripheral Interface$/;"	e	enum:__anon96
queueToArray	hdr/CFX/Headers/cfxad.h	/^	ushort *(*queueToArray)(void *qp, ushort count);$/;"	m	struct:__anon1
quiet	hdr/CFX/Headers/_cfx_console.h	/^	char		quiet;					\/\/ no messages or prompts$/;"	m	struct:CmdInfo
quot	hdr/CPU32/Headers/StdC/stdlib.h	/^	int		quot;$/;"	m	struct:__anon104
quot	hdr/CPU32/Headers/StdC/stdlib.h	/^	long	quot;$/;"	m	struct:__anon105
rangeChar	hdr/CFX/Headers/_cfx_console.h	/^	char		rangeChar;				\/\/ default as #defined$/;"	m	struct:CmdInfo
rawToVolts	hdr/CFX/Headers/cfxad.h	/^	float	(*rawToVolts)(ushort raw, float vref, bool uni);$/;"	m	struct:__anon1
rcvData	hdr/CFX/Headers/_cfx_expand.h	/^	ushort			*rcvData;		\/\/ pointer to received data buffer$/;"	m	struct:qpbDev
rde	hdr/CFX/Headers/dosdrive.h	65;"	d
ready	hdr/CFX/Headers/_cfx_internals.h	/^	bool	ready		: 1;	\/\/ online and not busy$/;"	m	struct:ResetResume::__anon66
ref	hdr/CFX/Headers/_cfx_internals.h	/^typedef	union { struct { ushort id; ushort sn; }; ulong ref; } SCSDevice;$/;"	m	union:ResetResume::__anon68
rem	hdr/CPU32/Headers/StdC/stdlib.h	/^	int		rem;$/;"	m	struct:__anon104
rem	hdr/CPU32/Headers/StdC/stdlib.h	/^	long	rem;$/;"	m	struct:__anon105
repChar	hdr/CFX/Headers/_cfx_console.h	/^	char		repChar;				\/\/ default as #defined$/;"	m	struct:CmdInfo
repeat	hdr/CFX/Headers/_cfx_console.h	/^	long		repeat;					\/\/ number of times to execute$/;"	m	struct:CmdInfo
repeat	hdr/CFX/Headers/cfxad.h	/^	ushort	(*repeat)(ushort qslot);$/;"	m	struct:__anon1
request	hdr/CFX/Headers/_cfx_tpu.h	/^	ushort		request;		\/\/ host service request$/;"	m	struct:TCHSetup
res1	hdr/CFX/Headers/dosdrive.h	/^	ushort		res1;					\/\/ reserved$/;"	m	struct:ATADriveID
res2	hdr/CFX/Headers/dosdrive.h	/^	ushort		res2;					\/\/ reserved$/;"	m	struct:ATADriveID
res3	hdr/CFX/Headers/dosdrive.h	/^	ushort		res3;					\/\/ reserved$/;"	m	struct:ATADriveID
res4	hdr/CFX/Headers/dosdrive.h	/^	ushort		res4;					\/\/ reserved$/;"	m	struct:ATADriveID
res5	hdr/CFX/Headers/dosdrive.h	/^	char		res5[388];				\/\/ reserved$/;"	m	struct:ATADriveID
resumeData	hdr/CFX/Headers/_cfx_internals.h	/^	ulong	resumeData[4];	\/\/ application defined data$/;"	m	struct:ResResData
resumeKeyHi	hdr/CFX/Headers/_cfx_internals.h	/^	ulong	resumeKeyHi;	\/\/ must be MAGIC_RESUME_KEY1 to be considered valid$/;"	m	struct:ResResData
resumeKeyLo	hdr/CFX/Headers/_cfx_internals.h	/^	ulong	resumeKeyLo;	\/\/ must be MAGIC_RESUME_KEY2 to be considered valid$/;"	m	struct:ResResData
rs	hdr/CFX/Headers/dosdrive.h	63;"	d
rtcsm16Vector	hdr/CFX/Headers/_cf1_ctm.h	/^	, rtcsm16Vector						\/\/  80	RTC TICKF Interrupt$/;"	e	enum:__anon32
rtctm	hdr/CFX/Headers/_cfx_internals.h	/^typedef struct rtctm$/;"	s	struct:ResetResume
rxFE	hdr/CFX/Headers/_cfx_sercomm.h	/^enum { rxOR = 0x8000, rxNF = 0x4000, rxFE = 0x2000, rxPF = 0x1000 };$/;"	e	enum:__anon80
rxNF	hdr/CFX/Headers/_cfx_sercomm.h	/^enum { rxOR = 0x8000, rxNF = 0x4000, rxFE = 0x2000, rxPF = 0x1000 };$/;"	e	enum:__anon80
rxOR	hdr/CFX/Headers/_cfx_sercomm.h	/^enum { rxOR = 0x8000, rxNF = 0x4000, rxFE = 0x2000, rxPF = 0x1000 };$/;"	e	enum:__anon80
rxPF	hdr/CFX/Headers/_cfx_sercomm.h	/^enum { rxOR = 0x8000, rxNF = 0x4000, rxFE = 0x2000, rxPF = 0x1000 };$/;"	e	enum:__anon80
rxpri	hdr/CFX/Headers/_cfx_sercomm.h	/^	short	rxpri;		\/\/ receive channel TPUPriority$/;"	m	struct:__anon84
rxqsz	hdr/CFX/Headers/_cfx_sercomm.h	/^	short	rxqsz;		\/\/ receive channel queue buffer size$/;"	m	struct:__anon84
sample	hdr/CFX/Headers/cfxad.h	/^	ushort	(*sample)(ushort qslot, ushort chan, bool uni, bool sgl, bool pd);$/;"	m	struct:__anon1
sampleBlock	hdr/CFX/Headers/cfxad.h	/^	ushort	*(*sampleBlock)(ushort qslot, ushort first, ushort count,  $/;"	m	struct:__anon1
sasm12AVector	hdr/CFX/Headers/_cf1_ctm.h	/^	, sasm12AVector				= 76	\/\/  76	CTS12A IC\/OC Event Interrupt$/;"	e	enum:__anon32
sasm12BVector	hdr/CFX/Headers/_cf1_ctm.h	/^	, sasm12BVector						\/\/  77	CTS12B IC\/OC Event Interrupt$/;"	e	enum:__anon32
sasm14AVector	hdr/CFX/Headers/_cf1_ctm.h	/^	, sasm14AVector						\/\/  78	CTS14A (Pin30) IC\/OC Event Interrupt$/;"	e	enum:__anon32
sasm14BVector	hdr/CFX/Headers/_cf1_ctm.h	/^	, sasm14BVector						\/\/  79	CTS12B (Pin28) IC\/OC Event Interrupt$/;"	e	enum:__anon32
sasm18AVector	hdr/CFX/Headers/_cf1_ctm.h	/^	, sasm18AVector				= 82	\/\/  82	CTS18A (Pin31) IC\/OC Event Interrupt$/;"	e	enum:__anon32
sasm18BVector	hdr/CFX/Headers/_cf1_ctm.h	/^	, sasm18BVector						\/\/  83	CTS18B (Pin32) IC\/OC Event Interrupt$/;"	e	enum:__anon32
scfmt	hdr/CFX/Headers/_cfx_console.h	/^	const char	*scfmt;		\/\/ nonzero sscanf format to attempt scan$/;"	m	struct:__anon37
sciVector	hdr/CFX/Headers/_cf1_ctm.h	/^	  sciVector					= 64	\/\/  64	Serial Controller Interface (UART)$/;"	e	enum:__anon32
sciVector	hdr/CFX/Headers/_cfx_tpu.h	/^	, sciVector					= 80	\/\/  80	Serial Controller Interface (UART)$/;"	e	enum:__anon96
scsDev	hdr/CFX/Headers/_cfx_internals.h	/^	ulong		scsDev;			\/\/ SCS reference number id:sn (actually SCSDevice)$/;"	m	struct:ResetResume::__anon67
scsIsReady	hdr/CFX/Headers/_cfx_internals.h	/^	, scsIsReady	= 0x00000000	\/\/ from SCSReady()$/;"	e	enum:ResetResume::__anon70
scsNoErr	hdr/CFX/Headers/_cfx_internals.h	/^	  scsNoErr		= 0x00000000$/;"	e	enum:ResetResume::__anon70
scsNoResponse	hdr/CFX/Headers/_cfx_internals.h	/^	, scsNoResponse	= 0xFFFFFFFE	\/\/ did not receive proper response$/;"	e	enum:ResetResume::__anon70
scsNotReady	hdr/CFX/Headers/_cfx_internals.h	/^	, scsNotReady	= 0xFFFFFFFF	\/\/ not ready, and told not to wait$/;"	e	enum:ResetResume::__anon70
scsPatch	hdr/CFX/Headers/_cfx_internals.h	/^	uchar		scsPatch;		\/\/ SCS firmware patch$/;"	m	struct:ResetResume::__anon67
scsRel	hdr/CFX/Headers/_cfx_internals.h	/^	uchar		scsRel;			\/\/ SCS firmware release$/;"	m	struct:ResetResume::__anon67
scsVer	hdr/CFX/Headers/_cfx_internals.h	/^	uchar		scsVer;			\/\/ SCS firmware version$/;"	m	struct:ResetResume::__anon67
secsRef	hdr/CFX/Headers/_cfx_time.h	/^	ulong		secsRef;	\/\/ timeout secs$/;"	m	struct:__anon87
sectOffset	hdr/CFX/Headers/dosdrive.h	/^	ulong		sectOffset;				\/\/ offset to start sector$/;"	m	struct:__anon6
sequence	hdr/CFX/Headers/_cfx_tpu.h	/^	ushort		sequence;		\/\/ host sequence $/;"	m	struct:TCHSetup
serialNumberText	hdr/CFX/Headers/dosdrive.h	/^	char		serialNumberText[20];	\/\/ serial number in right justified text$/;"	m	struct:ATADriveID
sernum	hdr/CFX/Headers/_cfx_globals.h	/^	char	sernum[6];			\/\/ "00001" to "99999"$/;"	m	struct:__anon50
shortVerStr	hdr/CFX/Headers/_cfx_globals.h	/^	char	shortVerStr[12];	\/\/ "12345-2.28"  serial number - version$/;"	m	struct:__anon50
sid	hdr/CFX/Headers/dosdrive.h	73;"	d
sig_atomic_t	hdr/CPU32/Headers/StdC/signal.h	/^typedef int sig_atomic_t;$/;"	t
singSectorsPerInt	hdr/CFX/Headers/dosdrive.h	/^	ushort		singSectorsPerInt;		\/\/ single sectors per interrupt$/;"	m	struct:ATADriveID
size_t	hdr/CPU32/Headers/StdC/size_t.h	/^	typedef unsigned long	size_t;$/;"	t
skpspc	hdr/CFX/Headers/_cfx_console.h	/^	const bool	skpspc;		\/\/ accept space between switch and argument$/;"	m	struct:__anon37
sn	hdr/CFX/Headers/_cfx_internals.h	/^typedef	union { struct { ushort id; ushort sn; }; ulong ref; } SCSDevice;$/;"	m	struct:ResetResume::__anon68::__anon69
sp	hdr/CFX/Headers/_cfx_console.h	/^		char	*sp;		\/\/ >2.30, save string pointer for "%s" scfmt$/;"	m	union:__anon37::__anon38
spc	hdr/CFX/Headers/dosdrive.h	62;"	d
spf	hdr/CFX/Headers/dosdrive.h	68;"	d
spinup	hdr/CFX/Headers/_cfx_internals.h	/^	bool	spinup		: 1;	\/\/ drive is spinning up and not yet online$/;"	m	struct:ResetResume::__anon66
spt	hdr/CFX/Headers/dosdrive.h	69;"	d
spuriousInterrupt	hdr/CFX/Headers/_cfx_cpulevel.h	/^	, spuriousInterrupt			= 24	\/\/  24	Spurious Interrupt$/;"	e	enum:__anon41
spvCardDetError	hdr/CFX/Headers/_cfx_internals.h	/^	spvCardDetError = -1,	\/\/ SPV not available to look$/;"	e	enum:ResetResume::__anon74
spvCardDetected	hdr/CFX/Headers/_cfx_internals.h	/^	spvCardDetected			\/\/ card found$/;"	e	enum:ResetResume::__anon74
spvCardNotDetected	hdr/CFX/Headers/_cfx_internals.h	/^	spvCardNotDetected,		\/\/ no card found$/;"	e	enum:ResetResume::__anon74
spvver	hdr/CFX/Headers/_cfx_globals.h	/^	char	spvver[6];			\/\/ "3.07 "$/;"	m	struct:__anon50
sr	hdr/CFX/Headers/_cfx_cpulevel.h	/^typedef struct { ushort sr; ulong pc; ushort fmtvct; ulong info[4]; } IEVStack;$/;"	m	struct:__anon39
sscandate	hdr/CFX/Headers/_cfx_console.h	/^			DateFieldOrder dfo)						PICO_CALL(sscandate);$/;"	v
sscantime	hdr/CFX/Headers/_cfx_console.h	/^short	sscantime(const char *str, struct tm *ptm)	PICO_CALL(sscantime);$/;"	v
st_atime	hdr/CFX/Headers/sys/stat.h	/^	time_t	st_atime;	\/\/ time of last access$/;"	m	struct:stat
st_attr	hdr/CFX/Headers/sys/stat.h	/^	ushort	st_attr;	\/\/ DOS attributes$/;"	m	struct:stat
st_ctime	hdr/CFX/Headers/sys/stat.h	/^	time_t	st_ctime;	\/\/ time of change to status$/;"	m	struct:stat
st_dev	hdr/CFX/Headers/sys/stat.h	/^	dev_t	st_dev;		\/\/ device ID$/;"	m	struct:stat
st_dfi	hdr/CFX/Headers/sys/stat.h	/^	struct dirent *st_dfi;	\/\/ directory entry block$/;"	m	struct:stat	typeref:struct:stat::dirent
st_dwbpb	hdr/CFX/Headers/sys/stat.h	/^	struct DWBPB *st_dwbpb;	\/\/ working boot param block$/;"	m	struct:stat	typeref:struct:stat::DWBPB
st_gid	hdr/CFX/Headers/sys/stat.h	/^	gid_t	st_gid;		\/\/ group ID$/;"	m	struct:stat
st_ino	hdr/CFX/Headers/sys/stat.h	/^	ino_t	st_ino;		\/\/ file serial number$/;"	m	struct:stat
st_mode	hdr/CFX/Headers/sys/stat.h	/^	mode_t	st_mode;	\/\/ file mode$/;"	m	struct:stat
st_mtime	hdr/CFX/Headers/sys/stat.h	/^	time_t	st_mtime;	\/\/ time of last modification$/;"	m	struct:stat
st_nlink	hdr/CFX/Headers/sys/stat.h	/^	nlink_t	st_nlink;	\/\/ number of links$/;"	m	struct:stat
st_path	hdr/CFX/Headers/sys/stat.h	/^	char	*st_path;	\/\/ full path$/;"	m	struct:stat
st_pdcb	hdr/CFX/Headers/sys/stat.h	/^	void	*st_pdcb;	\/\/ PicoDOS control block$/;"	m	struct:stat
st_size	hdr/CFX/Headers/sys/stat.h	/^	off_t	st_size;	\/\/ file size in bytes$/;"	m	struct:stat
st_uid	hdr/CFX/Headers/sys/stat.h	/^	uid_t	st_uid;		\/\/ user ID$/;"	m	struct:stat
startHead	hdr/CFX/Headers/dosdrive.h	/^	uchar		startHead;$/;"	m	struct:__anon6
startSC	hdr/CFX/Headers/dosdrive.h	/^	ushort		startSC;				\/\/ complicated, see technical manuals$/;"	m	struct:__anon6
stat	hdr/CFX/Headers/sys/stat.h	/^struct stat$/;"	s
status	hdr/CFX/Headers/dosdrive.h	/^	uchar		status;					\/\/ 0x80 = boot, 0x00 = non boot$/;"	m	struct:__anon6
stderr	hdr/CPU32/Headers/StdC/stdio.h	30;"	d
stdin	hdr/CPU32/Headers/StdC/stdio.h	28;"	d
stdout	hdr/CPU32/Headers/StdC/stdio.h	29;"	d
str	hdr/CFX/Headers/_cfx_console.h	/^	char		*str;					\/\/ pointer to argument text$/;"	m	struct:__anon36
str	hdr/CFX/Headers/_cfx_util.h	/^typedef union { void *bin; char *str; long lng; float flt; } VEEData;$/;"	m	union:__anon103
style	hdr/CFX/Headers/_cfx_globals.h	/^	char	style[4];			\/\/ "C", "I5", "1M"$/;"	m	struct:__anon50
swset	hdr/CFX/Headers/_cfx_console.h	/^	const char	*swset;		\/\/ standard is "\/"$/;"	m	struct:__anon37
target	hdr/CFX/Headers/_cfx_internals.h	/^	vfptr	target;			\/\/ address of function to handle resumption$/;"	m	struct:ResResData
tbsCap1Match1	hdr/CFX/Headers/_cfx_tpu.h	/^	  tbsCap1Match1 =	0x00,	\/\/ Capture TCR1, Match TCR1$/;"	e	enum:TBSChanControl
tbsCap1Match2	hdr/CFX/Headers/_cfx_tpu.h	/^	  tbsCap1Match2 =	0x20,	\/\/ Capture TCR1, Match TCR2$/;"	e	enum:TBSChanControl
tbsCap2Match1	hdr/CFX/Headers/_cfx_tpu.h	/^	  tbsCap2Match1 =	0x40,	\/\/ Capture TCR2, Match TCR1$/;"	e	enum:TBSChanControl
tbsCap2Match2	hdr/CFX/Headers/_cfx_tpu.h	/^	  tbsCap2Match2 =	0x60,	\/\/ Capture TCR2, Match TCR2$/;"	e	enum:TBSChanControl
tbsDoNotChange	hdr/CFX/Headers/_cfx_tpu.h	/^	tbsDoNotChange =	0x100	\/\/ Do Not Change TBS$/;"	e	enum:TBSChanControl
tbsInput	hdr/CFX/Headers/_cfx_tpu.h	/^	tbsInput =			0x00,	\/\/ Input Channel$/;"	e	enum:TBSChanControl
tbsOutput	hdr/CFX/Headers/_cfx_tpu.h	/^	tbsOutput =			0x80,	\/\/ Output Channel$/;"	e	enum:TBSChanControl
tcdrain	hdr/CFX/Headers/termios.h	54;"	d
tclose	hdr/CFX/Headers/_cfx_sercomm.h	160;"	d
tconfig	hdr/CFX/Headers/_cfx_sercomm.h	161;"	d
tdrain	hdr/CFX/Headers/_cfx_sercomm.h	168;"	d
text	hdr/CFX/Headers/_cfx_console.h	/^	char		text[CMDLINELEN];		\/\/ verbatim copy of input line$/;"	m	struct:CmdInfo
tgetc	hdr/CFX/Headers/_cfx_sercomm.h	162;"	d
tgetq	hdr/CFX/Headers/_cfx_sercomm.h	163;"	d
thousands_sep	hdr/CPU32/Headers/StdC/locale.h	/^	char	*thousands_sep;$/;"	m	struct:lconv
ticks	hdr/CFX/Headers/_cfx_time.h	/^	ulong		ticks;		\/\/ ticks$/;"	m	struct:__anon87
ticksRef	hdr/CFX/Headers/_cfx_time.h	/^	ushort		ticksRef;	\/\/ timeout ticks$/;"	m	struct:__anon87
tiflush	hdr/CFX/Headers/_cfx_sercomm.h	165;"	d
time_t	hdr/CPU32/Headers/StdC/time.h	/^typedef unsigned long	time_t;$/;"	t
timedout	hdr/CFX/Headers/_cfx_internals.h	/^	bool	timedout	: 1;	\/\/ spinup timed out waiting for not busy$/;"	m	struct:ResetResume::__anon66
tinit	hdr/CFX/Headers/_cfx_sercomm.h	158;"	d
tinitch	hdr/CFX/Headers/_cfx_tpu.h	/^	tinitf		*tinitch;		\/\/ reinit for resume from suspend$/;"	m	struct:TCHSetup
tinitf	hdr/CFX/Headers/_cfx_tpu.h	/^typedef void	*tinitf(struct TCHSetup *setup);$/;"	t
tlcCheckSync	hdr/CFX/Headers/_cfx_tpu.h	/^	tlcCheckSync = -1,$/;"	e	enum:__anon94
tlcDI	hdr/CFX/Headers/_cfx_tpu.h	/^typedef enum { tlcNoChangeI = -1, tlcDI, tlcEI } TLCIE;$/;"	e	enum:__anon93
tlcEI	hdr/CFX/Headers/_cfx_tpu.h	/^typedef enum { tlcNoChangeI = -1, tlcDI, tlcEI } TLCIE;$/;"	e	enum:__anon93
tlcInSync	hdr/CFX/Headers/_cfx_tpu.h	/^	tlcInSync$/;"	e	enum:__anon94
tlcNoChangeI	hdr/CFX/Headers/_cfx_tpu.h	/^typedef enum { tlcNoChangeI = -1, tlcDI, tlcEI } TLCIE;$/;"	e	enum:__anon93
tlcNotSync	hdr/CFX/Headers/_cfx_tpu.h	/^	tlcNotSync,$/;"	e	enum:__anon94
tlcver	hdr/CFX/Headers/_cfx_globals.h	/^	char	tlcver[6];			\/\/ "1.02 "$/;"	m	struct:__anon50
tm	hdr/CPU32/Headers/StdC/time.h	/^struct tm$/;"	s
tm_hour	hdr/CFX/Headers/_cfx_internals.h	/^	int tm_hour;	\/\/ hours since midnight (0,23)$/;"	m	struct:ResetResume::rtctm
tm_hour	hdr/CPU32/Headers/StdC/time.h	/^	int	tm_hour;$/;"	m	struct:tm
tm_isdst	hdr/CFX/Headers/_cfx_internals.h	/^	int tm_isdst;	\/\/ Daylight Savings Time flag$/;"	m	struct:ResetResume::rtctm
tm_isdst	hdr/CPU32/Headers/StdC/time.h	/^	int	tm_isdst;$/;"	m	struct:tm
tm_mday	hdr/CFX/Headers/_cfx_internals.h	/^	int tm_mday;	\/\/ day of the month (1,31)$/;"	m	struct:ResetResume::rtctm
tm_mday	hdr/CPU32/Headers/StdC/time.h	/^	int	tm_mday;$/;"	m	struct:tm
tm_min	hdr/CFX/Headers/_cfx_internals.h	/^	int tm_min;		\/\/ minutes after the hour (0,59)$/;"	m	struct:ResetResume::rtctm
tm_min	hdr/CPU32/Headers/StdC/time.h	/^	int	tm_min;$/;"	m	struct:tm
tm_mon	hdr/CFX/Headers/_cfx_internals.h	/^	int tm_mon;		\/\/ months since January (0,11)$/;"	m	struct:ResetResume::rtctm
tm_mon	hdr/CPU32/Headers/StdC/time.h	/^	int	tm_mon;$/;"	m	struct:tm
tm_sec	hdr/CFX/Headers/_cfx_internals.h	/^	int tm_sec;		\/\/ seconds after the minute (0,60)$/;"	m	struct:ResetResume::rtctm
tm_sec	hdr/CPU32/Headers/StdC/time.h	/^	int	tm_sec;$/;"	m	struct:tm
tm_wday	hdr/CFX/Headers/_cfx_internals.h	/^	int tm_wday;	\/\/ days since Sunday (0,6)$/;"	m	struct:ResetResume::rtctm
tm_wday	hdr/CPU32/Headers/StdC/time.h	/^	int	tm_wday;$/;"	m	struct:tm
tm_yday	hdr/CFX/Headers/_cfx_internals.h	/^	int tm_yday;	\/\/ days since January 1 (0,365)$/;"	m	struct:ResetResume::rtctm
tm_yday	hdr/CPU32/Headers/StdC/time.h	/^	int	tm_yday;$/;"	m	struct:tm
tm_year	hdr/CFX/Headers/_cfx_internals.h	/^	int tm_year;	\/\/ years since 1900$/;"	m	struct:ResetResume::rtctm
tm_year	hdr/CPU32/Headers/StdC/time.h	/^	int	tm_year;$/;"	m	struct:tm
tmgChgFirstActions	hdr/CFX/Headers/_cfx_internals.h	/^enum { tmgChgLastActions, tmgChgMidActions, tmgChgFirstActions };$/;"	e	enum:ResetResume::__anon60
tmgChgLastActions	hdr/CFX/Headers/_cfx_internals.h	/^enum { tmgChgLastActions, tmgChgMidActions, tmgChgFirstActions };$/;"	e	enum:ResetResume::__anon60
tmgChgMidActions	hdr/CFX/Headers/_cfx_internals.h	/^enum { tmgChgLastActions, tmgChgMidActions, tmgChgFirstActions };$/;"	e	enum:ResetResume::__anon60
toflush	hdr/CFX/Headers/_cfx_sercomm.h	169;"	d
topen	hdr/CFX/Headers/_cfx_sercomm.h	159;"	d
totalSectorsLBA	hdr/CFX/Headers/dosdrive.h	/^	ulong		totalSectorsLBA;		\/\/ total addressabel sectors in LBA mode$/;"	m	struct:ATADriveID
tpeekc	hdr/CFX/Headers/_cfx_sercomm.h	164;"	d
tpfbsz	hdr/CFX/Headers/_cfx_sercomm.h	/^	short	tpfbsz;		\/\/ transmit channel printf buffer size$/;"	m	struct:__anon84
tprintf	hdr/CFX/Headers/_cfx_sercomm.h	167;"	d
tpuAutoPriority	hdr/CFX/Headers/_cfx_sercomm.h	171;"	d
tpuDisabled	hdr/CFX/Headers/_cfx_tpu.h	/^	tpuDisabled,$/;"	e	enum:__anon89
tpuDiv1	hdr/CFX/Headers/_cfx_tpu.h	/^typedef enum	{ tpuDiv1, tpuDiv2, tpuDiv4, tpuDiv8 } TPUPrescale;$/;"	e	enum:__anon90
tpuDiv2	hdr/CFX/Headers/_cfx_tpu.h	/^typedef enum	{ tpuDiv1, tpuDiv2, tpuDiv4, tpuDiv8 } TPUPrescale;$/;"	e	enum:__anon90
tpuDiv4	hdr/CFX/Headers/_cfx_tpu.h	/^typedef enum	{ tpuDiv1, tpuDiv2, tpuDiv4, tpuDiv8 } TPUPrescale;$/;"	e	enum:__anon90
tpuDiv8	hdr/CFX/Headers/_cfx_tpu.h	/^typedef enum	{ tpuDiv1, tpuDiv2, tpuDiv4, tpuDiv8 } TPUPrescale;$/;"	e	enum:__anon90
tpuDontChangeHostSequence	hdr/CFX/Headers/_cfx_tpu.h	/^enum	{ tpuDontChangeHostSequence = -1 };$/;"	e	enum:__anon91
tpuDontChangePriority	hdr/CFX/Headers/_cfx_tpu.h	/^	tpuDontChangePriority = -1,$/;"	e	enum:__anon89
tpuHighPriority	hdr/CFX/Headers/_cfx_tpu.h	/^	tpuHighPriority$/;"	e	enum:__anon89
tpuLowPriority	hdr/CFX/Headers/_cfx_tpu.h	/^	tpuLowPriority,$/;"	e	enum:__anon89
tpuMiddlePriority	hdr/CFX/Headers/_cfx_tpu.h	/^	tpuMiddlePriority,$/;"	e	enum:__anon89
tpuV0	hdr/CFX/Headers/_cfx_tpu.h	/^	, tpuV0					= tpuVector	\/\/	64	TPU0 (internal use)$/;"	e	enum:__anon96
tpuV1	hdr/CFX/Headers/_cfx_tpu.h	/^	, tpuV1								\/\/	65	TPU1 (Pin22)$/;"	e	enum:__anon96
tpuV10	hdr/CFX/Headers/_cfx_tpu.h	/^	, tpuV10							\/\/	74	TPU10 (Pin31)$/;"	e	enum:__anon96
tpuV11	hdr/CFX/Headers/_cfx_tpu.h	/^	, tpuV11							\/\/	75	TPU11 (Pin32)$/;"	e	enum:__anon96
tpuV12	hdr/CFX/Headers/_cfx_tpu.h	/^	, tpuV12							\/\/	76	TPU12 (Pin33)$/;"	e	enum:__anon96
tpuV13	hdr/CFX/Headers/_cfx_tpu.h	/^	, tpuV13							\/\/	77	TPU13 (Pin34)$/;"	e	enum:__anon96
tpuV14	hdr/CFX/Headers/_cfx_tpu.h	/^	, tpuV14							\/\/	78	TPU14 (Pin35)$/;"	e	enum:__anon96
tpuV15	hdr/CFX/Headers/_cfx_tpu.h	/^	, tpuV15							\/\/	79	TPU15 (Pin37)$/;"	e	enum:__anon96
tpuV2	hdr/CFX/Headers/_cfx_tpu.h	/^	, tpuV2								\/\/	66	TPU2 (Pin23)$/;"	e	enum:__anon96
tpuV3	hdr/CFX/Headers/_cfx_tpu.h	/^	, tpuV3								\/\/	67	TPU3 (Pin24)$/;"	e	enum:__anon96
tpuV4	hdr/CFX/Headers/_cfx_tpu.h	/^	, tpuV4								\/\/	68	TPU4 (Pin25)$/;"	e	enum:__anon96
tpuV5	hdr/CFX/Headers/_cfx_tpu.h	/^	, tpuV5								\/\/	69	TPU5 (Pin26)$/;"	e	enum:__anon96
tpuV6	hdr/CFX/Headers/_cfx_tpu.h	/^	, tpuV6								\/\/	70	TPU6 (Pin27)$/;"	e	enum:__anon96
tpuV7	hdr/CFX/Headers/_cfx_tpu.h	/^	, tpuV7								\/\/	71	TPU7 (Pin28)$/;"	e	enum:__anon96
tpuV8	hdr/CFX/Headers/_cfx_tpu.h	/^	, tpuV8								\/\/	72	TPU8 (Pin29)$/;"	e	enum:__anon96
tpuV9	hdr/CFX/Headers/_cfx_tpu.h	/^	, tpuV9								\/\/	73	TPU9 (Pin30)$/;"	e	enum:__anon96
tpuVector	hdr/CFX/Headers/_cfx_tpu.h	/^	  tpuVector					= 64	\/\/  64	Time Processor Unit (TPU) ALIGN 16!$/;"	e	enum:__anon96
tpuch	hdr/CFX/Headers/_cfx_tpu.h	/^	ushort		tpuch;			\/\/ TPU channel 0 to 15$/;"	m	struct:TCHSetup
tputc	hdr/CFX/Headers/_cfx_sercomm.h	166;"	d
tpuver	hdr/CFX/Headers/_cfx_globals.h	/^	char	tpuver[6];			\/\/ "P.01 "$/;"	m	struct:__anon50
tpux0	hdr/CFX/Headers/_cfx_tpu.h	/^	tpux0	= 0x0,	\/\/		0	unassigned and NOT AVAILABLE FOR USE!$/;"	e	enum:__anon88
tpux1	hdr/CFX/Headers/_cfx_tpu.h	/^	tpux1	= 0x1,	\/\/		0	unassigned$/;"	e	enum:__anon88
tpux2	hdr/CFX/Headers/_cfx_tpu.h	/^	tpux2	= 0x2,	\/\/		0	unassigned$/;"	e	enum:__anon88
tpux3	hdr/CFX/Headers/_cfx_tpu.h	/^	tpux3	= 0x3,	\/\/		0	unassigned$/;"	e	enum:__anon88
tpux4	hdr/CFX/Headers/_cfx_tpu.h	/^	tpux4	= 0x4,	\/\/		0	unassigned$/;"	e	enum:__anon88
trtcrel	hdr/CFX/Headers/_cfx_tpu.h	/^	uchar	trtcrel;	\/\/ release number for TRCT function$/;"	m	struct:__anon95
trtcver	hdr/CFX/Headers/_cfx_tpu.h	/^	uchar	trtcver;	\/\/ version number for TRCT function$/;"	m	struct:__anon95
true	hdr/CFX/Headers/_cfx_types.h	83;"	d
true	hdr/CFX/Headers/mxcfxstd.h	101;"	d
ts	hdr/CFX/Headers/dosdrive.h	66;"	d
twoCols	hdr/CFX/Headers/_cfx_console.h	182;"	d
txpri	hdr/CFX/Headers/_cfx_sercomm.h	/^	short	txpri;		\/\/ transmit channel TPUPriority$/;"	m	struct:__anon84
txqsz	hdr/CFX/Headers/_cfx_sercomm.h	/^	short	txqsz;		\/\/ transmit channel queue buffer size$/;"	m	struct:__anon84
type	hdr/CFX/Headers/_cfx_internals.h	/^	ushort	type	: 2;	\/\/ string, long, float, binary $/;"	m	struct:ResetResume::__anon64
uchar	hdr/CFX/Headers/_cfx_types.h	/^	  typedef unsigned char		uchar;			\/\/ 8 bits, 0..255$/;"	t
uid_t	hdr/CFX/Headers/sys/stat.h	/^typedef unsigned long	uid_t;$/;"	t
ulong	hdr/CFX/Headers/_cfx_types.h	/^	  typedef unsigned long		ulong;			\/\/ 32 bits, 0..4,294,967,296$/;"	t
unfNumBytesPerSector	hdr/CFX/Headers/dosdrive.h	/^	ushort		unfNumBytesPerSector;	\/\/ number of unformatted bytes per sector$/;"	m	struct:ATADriveID
unfNumBytesPerTrack	hdr/CFX/Headers/dosdrive.h	/^	ushort		unfNumBytesPerTrack;	\/\/ number of unformatted bytes per track$/;"	m	struct:ATADriveID
unlock	hdr/CFX/Headers/cfxad.h	/^	bool	(*unlock)(ushort qslot);$/;"	m	struct:__anon1
uprintf	hdr/CFX/Headers/_cfx_console.h	81;"	d
us	hdr/CFX/Headers/_cfx_time.h	/^	ulong		us;			\/\/ microseconds$/;"	m	struct:__anon87
ushort	hdr/CFX/Headers/_cfx_types.h	/^	  typedef unsigned short	ushort;			\/\/ 16 bits, 0..65535$/;"	t
utimbuf	hdr/CFX/Headers/utime.h	/^	} utimbuf;$/;"	t	typeref:struct:utimbuf
utimbuf	hdr/CFX/Headers/utime.h	/^typedef struct utimbuf$/;"	s
va_arg	hdr/CPU32/Headers/StdC/stdarg.h	12;"	d
va_copy	hdr/CPU32/Headers/StdC/stdarg.h	15;"	d
va_end	hdr/CPU32/Headers/StdC/stdarg.h	13;"	d
va_list	hdr/CPU32/Headers/StdC/va_list.h	/^typedef char *va_list;$/;"	t
va_start	hdr/CPU32/Headers/StdC/stdarg.h	10;"	d
value	hdr/CFX/Headers/_cfx_console.h	/^	long		value;					\/\/ value if numeric (isval true)$/;"	m	struct:__anon36
vbin	hdr/CFX/Headers/_cfx_util.h	/^typedef enum { vbin, vstr, vlong, vfloat } VEEType;$/;"	e	enum:__anon102
veeBadCRC	hdr/CFX/Headers/_cfx_errors.h	/^	, veeBadCRC					\/\/ Entry has bad CRC$/;"	e	enum:__anon43
veeBadDataSize	hdr/CFX/Headers/_cfx_errors.h	/^	, veeBadDataSize			\/\/ Entry has bad data size$/;"	e	enum:__anon43
veeBadEntry	hdr/CFX/Headers/_cfx_errors.h	/^	, veeBadEntry				\/\/ Invalid entry in VEE table$/;"	e	enum:__anon43
veeBadNameSize	hdr/CFX/Headers/_cfx_errors.h	/^	, veeBadNameSize			\/\/ Entry has bad name size$/;"	e	enum:__anon43
veeClearFailed	hdr/CFX/Headers/_cfx_errors.h	/^	, veeClearFailed			\/\/ Clear command failed$/;"	e	enum:__anon43
veeErrorStart	hdr/CFX/Headers/_cfx_errors.h	/^	, veeErrorStart = VEE_ERRORS$/;"	e	enum:__anon43
veeLocked	hdr/CFX/Headers/_cfx_errors.h	/^	, veeLocked					\/\/ Locked, can't write or erase$/;"	e	enum:__anon43
veeNotInited	hdr/CFX/Headers/_cfx_errors.h	/^	, veeNotInited				\/\/ Hasn't been initialized$/;"	e	enum:__anon43
veeSpareNotErased	hdr/CFX/Headers/_cfx_errors.h	/^	, veeSpareNotErased			\/\/ Flash invalid, must be cleared to use$/;"	e	enum:__anon43
veeStoreFailed	hdr/CFX/Headers/_cfx_errors.h	/^	, veeStoreFailed			\/\/ Store command failed$/;"	e	enum:__anon43
veeTableFull	hdr/CFX/Headers/_cfx_errors.h	/^	, veeTableFull				\/\/ No more room in VEE$/;"	e	enum:__anon43
veeTooManyErases	hdr/CFX/Headers/_cfx_errors.h	/^	, veeTooManyErases			\/\/ Too many erases (limited to 4)$/;"	e	enum:__anon43
vendorSpec1	hdr/CFX/Headers/dosdrive.h	/^	ulong		vendorSpec1;			\/\/ vendor specific$/;"	m	struct:ATADriveID
vfloat	hdr/CFX/Headers/_cfx_util.h	/^typedef enum { vbin, vstr, vlong, vfloat } VEEType;$/;"	e	enum:__anon102
vfptr	hdr/CFX/Headers/_cfx_types.h	/^	  typedef	vfv				*vfptr;			\/\/ "" "" pointer to$/;"	t
vfv	hdr/CFX/Headers/_cfx_types.h	/^	  typedef void 				vfv(void);		\/\/ generic void function$/;"	t
vlong	hdr/CFX/Headers/_cfx_util.h	/^typedef enum { vbin, vstr, vlong, vfloat } VEEType;$/;"	e	enum:__anon102
vptr	hdr/CFX/Headers/_cfx_types.h	/^	  typedef void 				*vptr;			\/\/ generic pointer$/;"	t
vstr	hdr/CFX/Headers/_cfx_util.h	/^typedef enum { vbin, vstr, vlong, vfloat } VEEType;$/;"	e	enum:__anon102
vuchar	hdr/CFX/Headers/_cfx_types.h	/^	  typedef volatile uchar	vuchar;			\/\/ "" "" volatile $/;"	t
vucptr	hdr/CFX/Headers/_cfx_types.h	/^	  typedef vuchar			*vucptr;		\/\/ "" "" pointer to$/;"	t
vulong	hdr/CFX/Headers/_cfx_types.h	/^	  typedef volatile ulong	vulong;			\/\/ "" "" volatile$/;"	t
vulptr	hdr/CFX/Headers/_cfx_types.h	/^	  typedef vulong			*vulptr;		\/\/ "" "" pointer to$/;"	t
vushort	hdr/CFX/Headers/_cfx_types.h	/^	  typedef volatile ushort	vushort;		\/\/ "" "" volatile $/;"	t
vusptr	hdr/CFX/Headers/_cfx_types.h	/^	  typedef vushort			*vusptr;		\/\/ "" "" pointer to$/;"	t
w	hdr/CFX/Headers/_cfx_util.h	/^typedef	union	{ uchar	b[2]; ushort w; } BW, *pBW;$/;"	m	union:__anon98
w	hdr/CFX/Headers/_cfx_util.h	/^typedef	union	{ uchar	b[4]; ushort w[2]; ulong l; } BWL, *pBWL;$/;"	m	union:__anon99
wctrans_t	hdr/CPU32/Headers/StdC/wchar_t.h	/^typedef unsigned short	wctrans_t;$/;"	t
wctype_t	hdr/CPU32/Headers/StdC/wchar_t.h	/^typedef unsigned short	wctype_t;$/;"	t
wds	hdr/CFX/Headers/dosdrive.h	77;"	d
wint_t	hdr/CPU32/Headers/StdC/wchar_t.h	/^typedef unsigned short	wint_t;$/;"	t
wts	hdr/CFX/Headers/dosdrive.h	76;"	d
x100	hdr/CFX/Headers/mc68332.h	/^vushort	TPRAM[16][8] : TPU_BASE_ADDR + 0x100;	\/\/ $YFFF00-$YFFFFF	Parameter RAM$/;"	v
xFFFFFD00	hdr/CFX/Headers/_cfx_expand.h	/^vushort		QRR[16] : 0xFFFFFD00;	\/\/ $YFFD00 Ð $YFFD1F Receive RAM [S\/U]$/;"	v
xFFFFFD20	hdr/CFX/Headers/_cfx_expand.h	/^vushort		QTR[16] : 0xFFFFFD20;	\/\/ $YFFD20 Ð $YFFD3F Transmit RAM [S\/U]$/;"	v
xFFFFFD40	hdr/CFX/Headers/_cfx_expand.h	/^vuchar		QCR[16] : 0xFFFFFD40;	\/\/ $YFFD40 Ð $YFFD4F Command RAM [S\/U]$/;"	v
xfrCount	hdr/CFX/Headers/_cfx_expand.h	/^	ushort			xfrCount;		\/\/ words transferred$/;"	m	struct:qpbDev
xoffStd	hdr/CFX/Headers/_cfx_sercomm.h	/^enum { xonStd = 0x11, xoffStd = 0x13 };		\/\/ ctl-Q, ctl-S$/;"	e	enum:__anon82
xonStd	hdr/CFX/Headers/_cfx_sercomm.h	/^enum { xonStd = 0x11, xoffStd = 0x13 };		\/\/ ctl-Q, ctl-S$/;"	e	enum:__anon82
xor	hdr/CPU32/Headers/StdC/iso646.h	17;"	d
xor_eq	hdr/CPU32/Headers/StdC/iso646.h	18;"	d
zeroDivision	hdr/CFX/Headers/_cfx_cpulevel.h	/^	  zeroDivision				= 5		\/\/   5	Division by Zero Interrupt$/;"	e	enum:__anon41
