void PreRun(void);
void SetupHardware(void);

/*Time:
        @ulong pointer to point at number of seconds since 1970
        @return string of date/time
        */
char *Time(ulong *);
char *TimeDate(ulong *);

/*MakeDirectory:
        @Takes string of new directory name to be created
        */
void Make_Directory(char *);

/*DOSCom:
        @Param1: Takes Command String "copy, del, ren"
        @Param2: Takes 8 digit file number
        @Param3: With Extension .xxx
        @Param4: For renaming with different extension .yyy
        */
void DOS_Com(char *, long, char *, char *);

/*System_Timer:
        @Return: Returns 1 upon Det Timer Alarm; 2 upon data alarm; 0 upon n/a
        */
int System_Timer();
// void System_Timer();
/*Check_Timers:
        @Param1: Takes boolean pointer of Recorder status
        @return: Returns the system power logging interval time. 51.2 seconds if
   BITSHIFT is defined as 11 in ADS.h
        */
float Check_Timers(ushort);

/*AppendFiles:
        @Param1: File Pointer to destination file
        @Param2: String name of file to be appended
        @Param3: Boolean true to delete appeneded file
        @Param4: MaxNumber of bytes to upload, 0 to upload all
        @Return: Boolean true of successful appendage
        */
bool Append_Files(int, const char *, bool, long);

// print ascii, non-ascii
void printsafe (long, uchar *);

/*Delay_AD_Log:
        @Param1: Short value, number of seconds for delay while watching Power
   Logging & Tickling Watch Dog Timer
        */
void Delay_AD_Log(short);

/*Free_Disk_Space:
        @Return: long value of kilobytes of freespace
        */
long Free_Disk_Space();

/*CreateLogfile:
        @Param1: long pointer to return current filenumber
        */

char *GetFileName(bool, bool, long *, const char *);

/*GetFileName:
        @Param1: boolean. True to search for lowest file number.
        @Param2: boolean to increment index number
        @Param3: long pointer to return next filenumber
        @Param4: const char pointer. 3 characters of file ending type "dat"
        @Return: char pointer of filename
        */
bool SaveParams(const char *);

/*ParseStartupParams:
        @Param1: Boolean: True if we use default.cfg as the main startup script.
                                                        False if we use the new
   system.cfg file received from iridium.


        */
void ParseStartupParams(bool);

/*VEEStoreShort:
        @Param1:string veeprom name
        @Param2: short value to be saved
        */
void VEEStoreShort(char *, short);

/*Sleep:
   Interrupts: WISPR, CONSOLE, Power Logger
   */
void Sleep();

/*CTDSleep:
   Interrupts: CTD/SeaGlider, CONSOLE, POWER LOGGER
   */
void CTDSleep();

/*GetSettings:
   Grabs all relevant VEEPROM Settings for use with Platform
   */
void GetSettings();

/*Check_Vitals:
   Looks through system health for reasons for a full shutdown/hibernation.
   @Param1: floating point value of most recent voltage measurement
   @Return: short value of reason for shutdown, if any
   */
short Check_Vitals();

void print_clock_cycle_count(clock_t, clock_t, char *);

int fdprint(int, const char *, ...);

void GetSEAGLIDERSettings();
