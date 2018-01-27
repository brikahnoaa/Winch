// dat.c
#include <com.h>
#include <dat.h>

// gain num detMax detNum dutycycl port
DataInfo data = {
  2, 4, 10, 0, 360, NULL
};

bool SendDATAGPS = false;
static int DATAGPSSends;
bool DATA_On;
float DATAFreeSpace = 0.0;
int TotalDetections;
int dtxrqst;

// char* DATAString;
// short DATAStringLength=64;
// PAM TUPORT Setup
Serial PAMPort;
short PAM_RX, PAM_TX;

char *GetDATAInput(float *);
void AppendDetections(char *, int);

static char datafile[] = "c:DATAFRS.DAT";
static char DataString[64];

bool DATA_Status(void) { return DATA_On; }

/*
 * Power is applied to the data boards
 */
void DATAPower(bool power) {
  DBG0("DATAPower()")
  // must call dataInit first
  if (power) {
    flogf("\n%s|DATA: ON", Time(NULL));
    PIOSet(DATA_PWR_ON);
    delayms(10);
    PIOClear(DATA_PWR_ON);
    data.on = true;
  } else {
    flogf("\n%s|DATA: OFF", Time(NULL));
    PIOSet(DATA_PWR_OFF);
    delayms(10);
    PIOClear(DATA_PWR_OFF);
    data.on = false;
    delayms(1000);        //??
  }
} // DATAInit

/*
 * void DATA_Data()
 * Incoming DATA ASCII Communication. Looks for certain commands starting with
 * '$' and ending with '*'
 * Return values:
 * 1: GPS
 * 2: DFP
 * 3: DXN
 * 4: DTX
 * 5: NGN
 * 6: FIN
 * 0: NULL
 * -1: No Match
 * -2: <MIN FREE SPACE
 */
short DATA_Data(void) {
  float returndouble;
  int i;
  int DATAFile;
  short returnvalue = 0;
  char DTXFilename[] = "c:00000000.DTX";

  // memset(DATAString, 0, DATAStringLength*(sizeof DATAString[0]));

  // DataString =
  GetDATAInput(
      &returndouble); // changed to 1.25 second wait time to receive the '$'

  if (strncmp(DataString, "$GPS", 4) == 0) {
    DATAGPS(124.5, 45);
    return 1;
  }
  /*   else if(strncmp(DataString, "$TFP", 4)==0){

        flogf("\n\t|DATA%d: %0.2f%% Total Space", DATA.NUM, returndouble);
        if(returndouble ==0.0){
           DBG1("\t|Bad TFP return... tryaing again")
           DATATFP();
           return -3;
           }
        //Calculate what minimum free space should be for given detint.
        }
 */
  else if (strncmp(DataString, "$DFP", 4) == 0) {

    flogf("\n\t|DATA%d: %.2f%% Free Space", DATA.NUM, returndouble);

    if (returndouble == 0.0) {
      DBG1("\t|Bad DFP return...trying again")
      DATADFP();
      return -2;
    }

    DATAFreeSpace = returndouble;

#ifndef SEAGLIDER
    UpdateDATAFRS();
#endif

    if (returndouble < MIN_FREESPACE) {

      flogf("\n\t|DATA%d FreeSpace below Minimum", DATA.NUM);
      if (DATA.NUM < DATANUMBER) {
        flogf("\n\t|Incrementing DATANumber");
        ChangeDATA(DATA.NUM + 1);
        // Only location when to permanently increment data number: When free
        // space runs out. or from start script.
        VEEStoreShort(DATANUM_NAME, DATA.NUM);
      }

      else {
        flogf("\n\t|Exceeded DATANumber");
        DATA.DUTYCYCL = 0;
        VEEStoreShort(DUTYCYCLE_NAME, 0);
        flogf("\n\t|DATA Shutdown");
        // DATASafeShutdown(); //This creates a never ending loop.
        if (!DATAExit())
          if (!DATAExit()) {
            flogf(": Forcing Off");
            // ?? fetch storm warning
            DATAPower(false);
          }
        Check_Timers(Return_ADSTIME());
        return 2;
      }

      return 2;
    }

    if (!SendDATAGPS) {
      DATAGPS(124.5, 45);
      delayms(150);
      TUTxFlush(PAMPort);
      TURxFlush(PAMPort);
      DATAGain(-1);
    }

    return 2;
  }

  else if (strncmp(DataString, "$DXN", 4) == 0) {

    TotalDetections += (int)returndouble;
    flogf("\n\t|Total Detections: %d", TotalDetections);
#ifdef SEAGLIDER
    sprintf(&DTXFilename[2], "%08d.dtx", SEAG.DIVENUM);
#else
    sprintf(&DTXFilename[2], "%08ld.dtx", MPC.FILENUM);
#endif

    DBG1("\t|DTX file: %s", DTXFilename)
    DATAFile = open(DTXFilename, O_APPEND | O_RDWR | O_CREAT);
    delayms(25);
    if (DATAFile <= 0)
      flogf("\nERROR  |DATA_Data() %s open errno: %d", DTXFilename, errno);
    DBG(else flogf("\n\t|DATA_Data() %s opened", DTXFilename);)

    if (DATAFile <= 0) {
      flogf("\nERROR|DATA_Data() open errno: %d", errno);
      return -3;
    }

    if ((int)returndouble < dtxrqst)
      dtxrqst = (int)returndouble;
    if ((int)returndouble == 0) {
      flogf("\n\t|Writing %d detections to %s", dtxrqst, DTXFilename);
      if (close(DATAFile) < 0)
        flogf("\nERROR |DATA_Data() %s close errno %d", DTXFilename, errno);
      DBG(else flogf("\n\t|DATA_Data() %s closed", DTXFilename);)
      return 3;
    }

    else if (dtxrqst > 0) {
      DBG1("%s", DataString)
      AppendDetections(DataString, DATAFile);
      if (dtxrqst > 0) {
        for (i = 0; i < dtxrqst; i++) {
          TickleSWSR();
          // memset(DataString, 0, 64*(sizeof DataString[0]));
          GetDATAInput(&returndouble);
          if ((int)returndouble == -1) {
            flogf("\nERROR|Bad Return from GetDATAInput()");
            break;
          }
          AppendDetections(DataString, DATAFile);
        }
        TURxFlush(PAMPort);
#ifdef ACOUSTICMODEM // This if defined statement should be based on whether an
                     // AMODEM is implemented but rather a real-time detection
                     // turn around.
        if (DATA.DETNUM > 0)
          if (returndouble >= DATA.DETNUM)
            flogf("\n\t|Detection Number surpassed");
#endif
      }
    }
    close(DATAFile);

    return 3;
  }

  else if (strncmp(DataString, "$DTX", 4) == 0) {
    DBG1("%s", DataString)
    return 4;
  }

  else if (strncmp(DataString, "$NGN", 4) == 0) {
    if (!SendDATAGPS) {
      DATAGPS();
      delayms(50);
    }

    DATAGain(-1);

    return 5;
  }

  else if (strncmp(DataString, "$FIN", 4) == 0) {
    flogf(": Found Exit");
    delayms(2000);     // Gives a little bit of time to DATA to umount /mnt
    DATAPower(false); // Powers off Data
    return 6;
  } else if (strcmp(DataString, NULL) == 0) {
    return 0;
  } else
    return -1;

  return 0;

} // DATA_Data
/*
 * int DATADet()
This is where we inquire about the total number of detections every so often.
This function will call to the data board with max_detections. It first
repsonds with a $DXN...* com line then lists each actual detection
afterward with a $DTX...* com line. Each are up to ~60 bytes per line

 */
void DATADet(int dtx) {

  if (dtx < 0)
    return;                   // if negative number then don't call detections
  else if (dtx > DATA.DETMAX) // if received dtx reqeust > Maximum user set.
    dtx = DATA.DETMAX;

  dtxrqst = dtx;

  delayms(10);
  TUTxFlush(PAMPort);
  TURxFlush(PAMPort);
  TUTxPrintf(PAMPort, "$DX?,%d*\n", dtx);
  TUTxWaitCompletion(PAMPort);

  delayms(500);

} // DATADet
/*
 * int DATAGPS()
This is where we send the GPS Time and Location to DATA Board at startup
 */
void DATAGPS(void) {

  float minutes;
  float decimal;
  float LAT, LONG;
  char LATITUDE[17];
  char LONGITUDE[17];

  flogf("\n%s|DATAGPS(%d):", Time(NULL), DATA.NUM);
  strcpy(LATITUDE, MPC.LAT);
  strcpy(LONGITUDE, MPC.LONG);
  flogf("\n\t|LAT: %s, LONG: %s", LATITUDE, LONGITUDE);
  LONG = atof(strtok(LONGITUDE, ":")); // Get Degree
  minutes = atof(strtok(NULL, " NS")); // Get decimal minutes
  decimal = minutes / 60.0;
  LONG += decimal;
  LAT = atof(strtok(LATITUDE, ":"));
  minutes = atof(strtok(NULL, " WE"));
  decimal = minutes / 60.0;
  LAT += decimal;

  flogf("\n\t|Sending GPS: %.4f %.4f", LONG, LAT);
  TUTxFlush(PAMPort);
  TUTxPrintf(PAMPort, "$GPS,%ld,%.4f,%.4f*\n", RTCGetTime(NULL, NULL), LONG,
             LAT);
  TUTxWaitCompletion(PAMPort);

  SendDATAGPS = true;

} // DATAGPS
/*
 * int DATAGain()
We can update the gain parameters for the DATA Board.
Might want to do this at the start up of DATA Program when DATA Requests
for gain values.

 */
void DATAGain(short c) {

  if (c < 0 || c > 3)
    c = DATA.GAIN;

  flogf(" & Gain: %d", c);
  TUTxFlush(PAMPort);
  TUTxPrintf(PAMPort, "$NGN,%d*\n", c);
  TUTxWaitCompletion(PAMPort);
  delayms(2);

} // DATAGain
/*
 * int DATADFP()
 */
void DATADFP(void) {

  DBG0("\t|DATADFP(%d)", DATA.NUM)
  TURxFlush(PAMPort);
  TUTxFlush(PAMPort);
  TUTxPrintf(PAMPort, "$DFP*\n");
  TUTxWaitCompletion(PAMPort);
  delayms(250);

} // DATADFP
/*
 * int DATATFP()
 */
void DATATFP(void) {

  DBG0("\t|DATATFP(%d)", DATA.NUM)
  TURxFlush(PAMPort);
  TUTxFlush(PAMPort);
  TUTxPrintf(PAMPort, "$TFP*\n");
  TUTxWaitCompletion(PAMPort);
  delayms(250);

} // DATADFP
/*
 * int DATAExit()
 */
bool DATAExit(void) {

  flogf("\n\t|DATAExit()");

  TUTxFlush(PAMPort);
  TURxFlush(PAMPort);
  TUTxPrintf(PAMPort, "$EXI*\n");
  TUTxWaitCompletion(PAMPort);
  Delay_AD_Log(150);  delayms(200);

  DATA_Data();

  if (!DATA_On) {

    return true;
  } else
    return false;

} // DATAExit
/*
 * char* GetDATAInput()
1. Gets incoming serial data from ActivePAM on MPC
2. If it is -1 it breaks and exits, once it sees a '*' it stops taking serial
3. Look for the appropriate DATA Command (DFP, EXI, DXN, DTX)
4. returns numchars if we are looking for DFP
5. will return number of detections if we get DXN. for which we will run a for
loop to get each detection in this same function
6. Should return char* of DTX if we need to get each detection.

 */
char *GetDATAInput(float *numchars) {
  // global DataString;
  const char *asterisk;
  int stringlength;
  bool good = false;
  int i = 0, k;
  char in;
  char r[65];
  char inchar;
  short count = 0;

  memset(DataString, 0, 64 * sizeof(char));

  inchar = TURxGetByteWithTimeout(PAMPort, 250);
  for (k = 0; k < 100; k++) {
    if (inchar != '$')
      inchar = TURxGetByteWithTimeout(PAMPort, 25);
    else
      break;
  }

  if (inchar != '$')
    return NULL;

  r[0] = '$';
  r[1] = TURxGetByteWithTimeout(PAMPort, 100);
  if (r[1] == -1) {
    DBG1("The first input character is negative one")
    return NULL;
  }
  for (i = 2; i < 64; i++) { // Up to 59 characters
    in = TURxGetByteWithTimeout(PAMPort, 100);
    if (in == '*') {
      r[i] = in; // TURxFlush(PAMPort);
      break;
    } // if we see an * we call that the last of this DATA Input
    else if (in == -1) {
      TURxFlush(PAMPort);
      return NULL;
    } else
      r[i] = in;
  }

  // Looks for end of DATAInput, gets length, appends to returnline
  asterisk = strchr(r, '*');
  if (asterisk == NULL) {
    *numchars = -1;
    return NULL;
  }
  stringlength = asterisk - r + 1; // length from '$' to '*'
  strncat(DataString, r, stringlength);
  strcat(DataString, "\n\0");

  // If calling for number of detections return detections.
  if (strncmp("DXN", r + 1, 3) == 0) {

    *numchars = atoi(DataString + 5);
  } else if (strncmp("DTX", r + 1, 3) == 0) {
    *numchars = 0;
  }

  // Sending Free Space, return value between , and *
  else if (strncmp("DFP", r + 1, 3) == 0) {
    *numchars = atof(strtok(DataString + 5, "*"));
    // if(numchars==0.00) return false?
  }
  /*
  else if(strncmp("TFP", r+1, 3)==0){
     *numchars = atof(strtok(DataString+5, "*"));

     }
 */

  return DataString;

} // GetIRIDInput
/*
 ** void ChangeDATA()
 */
void ChangeDATA(short wnum) {

  // Shut off DATA
  if (!DATAExit()) {
    if (!DATAExit()) {
      flogf(": Forcing Off");
      DATAPower(false);
    }
  }

  dataInit(false);
  delayms(100);
  DATA.NUM = wnum;
  dataInit(true);
  DATAPower(true);
}
/*
 * void GetDATASettings()
 */
void GetDATASettings(void) {
  char *p;
  p = VEEFetchData(DETECTIONMAX_NAME).str;
  DATA.DETMAX = atoi(p ? p : DETECTIONMAX_DEFAULT);
  DBG1("DETMAX=%u (%s)", DATA.DETMAX, p ? "vee" : "def")
  if (DATA.DETMAX > MAX_DETECTIONS) {
    DATA.DETMAX = MAX_DETECTIONS;
    VEEStoreShort(DETECTIONMAX_NAME, DATA.DETMAX);
  }

  //"g" 0-3 gain values
  p = VEEFetchData(DATAGAIN_NAME).str;
  DATA.GAIN = atoi(p ? p : DATAGAIN_DEFAULT);
  DBG1("DATAGAIN=%u (%s)", DATA.GAIN, p ? "vee" : "def")
  if (DATA.GAIN > 3) {
    DATA.GAIN = 3;
    VEEStoreShort(DATAGAIN_NAME, DATA.GAIN);
  } else if (DATA.GAIN < 0) {
    DATA.GAIN = 0;
    VEEStoreShort(DATAGAIN_NAME, DATA.GAIN);
  }

  // "N" Number of detections per DETINT to trigger AModem
  p = VEEFetchData(DETECTIONNUM_NAME).str;
  DATA.DETNUM = atoi(p ? p : DETECTIONNUM_DEFAULT);
  DBG1("DETECTNUM=%u (%s)", DATA.DETNUM, p ? "vee" : "def")

  //"C" dutycycle
  p = VEEFetchData(DUTYCYCLE_NAME).str;
  DATA.DUTYCYCL = atoi(p ? p : DUTYCYCLE_DEFAULT);
  DBG1("DUTYCYCLE=%d (%s)", DATA.DUTYCYCL, p ? "vee" : "def")
  if (DATA.DUTYCYCL > MAX_DUTYCYCLE) {
    DATA.DUTYCYCL = MAX_DUTYCYCLE;
    VEEStoreShort(DUTYCYCLE_NAME, DATA.DUTYCYCL);
  } else if (DATA.DUTYCYCL < MIN_DUTYCYCLE) {
    DATA.DUTYCYCL = MIN_DUTYCYCLE;
    VEEStoreShort(DUTYCYCLE_NAME, DATA.DUTYCYCL);
  }

  // "x" datanum
  p = VEEFetchData(DATANUM_NAME).str;
  DATA.NUM = atoi(p ? p : DATANUM_DEFAULT);
  DBG1("DATANUM=%d (%s)", DATA.NUM, p ? "vee" : "def")
  if (DATA.NUM < 1) {
    DATA.NUM = 1;
    VEEStoreShort(DATANUM_NAME, DATA.NUM);
  } else if (DATA.NUM > DATANUMBER) {
    DATA.NUM = DATANUMBER;
    VEEStoreShort(DATANUM_NAME, DATA.NUM);
  }

} // GetDATASettings
/*
 * void DATASafeShutdown()
 */
void DATASafeShutdown(void) {

  int i;

  if (DATA_On) {
    DATADFP();
    i = DATA_Data();
    if (i != 2) {
      DATADFP();
      i = DATA_Data();
    }

    // Shut off DATA for Data Transmission
    if (!DATAExit()) {
      if (!DATAExit()) {
        flogf(": Forcing Off");
        DATAPower(false);
      }
    }

  } else
    return;

} // DATASafeShutdown
/*
 * Void DATAWriteFile()
 */
void DATAWriteFile(int uploadfilehandle) {
  char detfname[] = "c:00000000.dtx";
  int byteswritten;

  memset(WriteBuffer, 0, BUFSZ);

  flogf("\n\t|DATAWriteFile()");

  if (DATA.DUTYCYCL > 0) {
    sprintf(WriteBuffer, "\n---DATA%d---\nGPS Sends: %d\nGain:%d\nFree "
                         "Space:%4.2f%%\nDuty Cycle:%d%%\nMax "
                         "Detections:%02d\nTotal Detections:%d\n\0",
            DATA.NUM, DATAGPSSends, DATA.GAIN, DATAFreeSpace, DATA.DUTYCYCL,
            DATA.DETMAX, TotalDetections);

    DBG1("%s", WriteBuffer)
    /*
            #ifdef REALTIME
               if(DATA.DETNUM>0){
               sprintf(stringadd, "\nCall upon %04d Detections\n", DATA.DETNUM);
               strncat(buf, stringadd, 27*sizeof(char));
               }
            #endif
 */

  } else {
    sprintf(WriteBuffer, "\nDATA-OFF\n\0");
  }

  byteswritten = write(uploadfilehandle, WriteBuffer, strlen(WriteBuffer));
  DBG1("DATAWrite File: Number of Bytes written: %d", byteswritten)

  // If more than one data, add total free space of all DATAs to Write File.
  if (DATANUMBER > 1)
    Append_Files(uploadfilehandle, "C:DATAFRS.DAT", false, 0);
  if (TotalDetections > 0) {
#ifdef SEAGLIDER
    sprintf(&detfname[2], "%08d.dtx", SEAG.DIVENUM);
#else
    sprintf(&detfname[2], "%08ld.dtx", MPC.FILENUM);
#endif
    DBG1("\t|Append File: %s", detfname)
    Append_Files(uploadfilehandle, detfname, true, 0);
  } else {
#ifdef SEAGLIDER
    DOS_Com("del", (long)SEAG.DIVENUM, "dtx", NULL);
#else
    DOS_Com("del", MPC.FILENUM, "dtx", NULL);
#endif
  }

  TotalDetections = 0;

} // DATAWriteFile
/*
 * float GetDATAFreeSpace
 * Read "datafs.bat" file. return free space of current DATA? ||  return free
space of last data
 */
float GetDATAFreeSpace(void) {

  return DATAFreeSpace;

} // GetDATAFreeSpace

void AppendDetections(char *DTXString, int FileDescriptor) {
  int i;

  DBG0("AppendDetections() dtxstring length: %lu", strlen(DTXString))
  i = write(FileDescriptor, DTXString, strlen(DTXString));
}

void create_dtx_file(long fnum) {
  int filehandle;
  char SourceFileName[] = "c:00000000.dtx";

  sprintf(&SourceFileName[2], "%08ld.dtx", fnum);
  filehandle = creat(SourceFileName, 0);
  delayms(500);
  if (filehandle < 0) {
    flogf("\nERROR  |Create_DTX_File() errno: %d", errno);
  }
  DBG(else flogf("\n\t|create_dtx_file() %s opened", SourceFileName);)

  if (close(filehandle) < 0)
    flogf("\nERROR  |create_dtx_file(): %s close errno: %d", SourceFileName,
          errno);
  DBG(else flogf("\n\t|create_dtx_file(): %s closed", SourceFileName);)

  delayms(10);
}
/*
 * GatherDATAFreeSpace()
 * Only should come here upon MPC.STARTUPS==0 && DATANUMBER>1
 */
void GatherDATAFreeSpace(void) {
  short wret = 0, i, count = 0, wnum = 1;
  int byteswritten = 0;
  int writenum = 0;
  bool gain = false, dfp = false;
  int datafilehandle;
  char databuff[16];
  static char *filename = "C:DATAFRS.DAT";

  if (DATA_On) {
    DATAExit();
    delayms(2500);
  }
  DATAPower(true);

  datafilehandle = open(filename, O_CREAT | O_TRUNC | O_RDWR);
  if (datafilehandle <= 0) {
    flogf("file handle: %d", datafilehandle);
    flogf("errno: %d", errno);
    return;
  }
  for (i = 1; i <= DATANUMBER; i++) {
    sprintf(databuff, "W%d:00.00%,", i); // sprintf adds trailing \0
    byteswritten =
        write(datafilehandle, databuff, strlen(databuff) * sizeof(char));
    flogf("\n\t|Bytes written: %d", byteswritten);
  }
  close(datafilehandle);

  while (wnum <= DATANUMBER) {
    while (!dfp && count <= 3) {

      Sleep();

      if (AD_Check()) {
        count++;
        if (gain || count == 2) {
          DBG1("\t|GWFS: DFP2")
          DATADFP();
          delayms(150);
          if (DATA_Data() == 2)
            dfp = true;
        }
      }

      if (tgetq(PAMPort)) {
        wret = DATA_Data();
        if (wret == 1) {
          delayms(150);
          wret = DATA_Data();
          if (wret == 5) {
            gain = true;
            delayms(150);
            DBG1("\t|GWFS: DFP1")
            DATADFP();
            delayms(150);
            if (DATA_Data() == 2)
              dfp = true;
          }
        }
      }
    }

    TURxFlush(PAMPort);
    TUTxFlush(PAMPort);
    if (!dfp)
      DATAFreeSpace = 00.00;

    dfp = false;
    gain = false;
    count = 0;
    wnum++;
    if (wnum > DATANUMBER)
      break;
    //      if(DATAFreeSpace>=MIN_FREESPACE)
    ChangeDATA(wnum);
  }
  DBG1("\t|wnum: %d, DATA.NUM: %d", wnum, DATA.NUM)
  wnum = 1;

  // Shut off DATA && Close TUPort
  if (!DATAExit()) {
    if (!DATAExit()) {
      flogf(": Forcing Off");
      DATAPower(false);
    }
  }

  dataInit(false);
  delayms(100);
  DATA.NUM = wnum;
  VEEStoreShort(DATANUM_NAME, DATA.NUM);

} //GatherDATAFreeSpace
/*
 * void UpdateDATAFRS()
 * 
 * Write to file: DATAFRS.DAT with the current data's free space
 */
void UpdateDATAFRS(void) {
  // global *DataString;
  int datafilehandle;
  struct stat fileinfo;
  char *datanum = "W0:";
  char *p;
  int length;
  int bytes;
  long filesize;

  flogf("\n%s|Update %s ", Time(NULL), datafile);
  delayms(10);
  // sprintf(&datafile[2], "DATAFRS.DAT");
  delayms(20);
  if (stat(datafile, &fileinfo) != 0) {
    flogf("%s file does not exist. making file...", datafile);
    GatherDATAFreeSpace();
    stat(datafile, &fileinfo);
  }
  filesize = fileinfo.st_size;

  DBG1("\t|File size: %ld", filesize)

  datafilehandle = open(datafile, O_RDWR);
  delayms(25);
  if (datafilehandle <= 0)
    flogf("\nERROR  |UpdateDATAFRS(): file open errno: %d", errno);
  DBG(else flogf("\n\t|UpdateDATAFRS() %s opened", datafile);)

  read(datafilehandle, DataString, fileinfo.st_size);
  // flogf("\n\t|%s", DataString);
  if (DATA.NUM > DATANUMBER)
    DATA.NUM = DATANUMBER;
  sprintf(datanum, "W%d:", DATA.NUM);

  p = strstr(DataString, datanum);
  if (p == NULL)
    flogf("\nERROR  |No String search found for: %s", datanum);
  length = p - DataString;
  if (length > 30) {
    flogf("\nERROR |Returning due to bad filde position for %s", datafile);
    close(datafilehandle);
    return;
  }

  p = strtok(p + 3, ",");

  DBG1("Updating DATAFRS at position %d from %5.2f to %5.2f", length, atof(p), DATAFreeSpace)

  sprintf(datanum, "%5.2f", DATAFreeSpace);
  lseek(datafilehandle, length + 3, SEEK_SET);
  bytes = write(datafilehandle, datanum, 5);
  DBG1("Bytes written: %d", bytes)
  lseek(datafilehandle, 0, SEEK_SET);
  read(datafilehandle, DataString, fileinfo.st_size);
  flogf("%s", DataString);

  close(datafilehandle);

} // UpdateDATAFRS
/*
 * void dataInit()
 */
void dataInit(bool on) {
  // global DataStr
  int DataNum;

  DataNum = DATA.NUM;
  flogf("\n\t|%s DATA%d TUPort", on ? "Open" : "Close", DataNum);
  if (on) {
    PAM_RX = TPUChanFromPin(28);
    PAM_TX = TPUChanFromPin(27);
    PAMPort = TUOpen(PAM_RX, PAM_TX, DATABAUD, 0);
  } else if (!on) {
    TUTxFlush(PAMPort);
    TURxFlush(PAMPort);
    TUClose(PAMPort);
    delayms(1000);
  }

  PIOClear(DATA_PWR_ON);
  PIOClear(DATA_PWR_OFF);

  // PAM 1
  if (DataNum == 1) {
    if (on) {
      PIOSet(DATAONE);
      PIOClear(DATATWO);
    } else {
      PIOClear(DATAONE);
      PIOClear(DATATWO);
    }
  }
  // PAM 2
  else if (DataNum == 2) {
    if (on) {
      PIOSet(DATAONE);
      PIOSet(DATATWO);
    } else {
      PIOClear(DATAONE);
      PIOClear(DATATWO);
    }
  }
  // PAM 3
  else if (DataNum == 3) {
    if (on) {
      PIOSet(DATATHREE);
      PIOClear(DATAFOUR);
    } else {
      PIOClear(DATATHREE);
      PIOClear(DATAFOUR);
    }

  }
  // PAM 4
  else if (DataNum == 4) {
    if (on) {
      PIOSet(DATATHREE);
      PIOSet(DATAFOUR);
    } else {
      PIOClear(DATATHREE);
      PIOClear(DATAFOUR);
    }
  } else if (DataNum == 0) {
    flogf("\n\t|DATA Zero. Run out of space?");
  }
  // Bad PAM
  else {
    if (PAMPort == 0)
      printf("\nBad TU Channel: PAM...");
    flogf("\n\t|Wrong PAM Port...");
    TUClose(PAMPort);
  }
  delayms(100);
}
/*
 * bool DATAExpectedReturn(short)
 */
bool DATAExpectedReturn(short expected, bool reboot) {

  if (DATA_Data() != expected) {
    dataInit(false);
    Delay_AD_Log(2);
    dataInit(true);
    Delay_AD_Log(1);
    switch (expected) {
    case 1:
      DATAGPS();
      break;
    case 2:
      DATADFP();
      break;
    case 3:
      DATADet(DATA.DETMAX);
      break;
    case 6:
      DATAExit();
      break;
    }
    if (DATA_Data() != expected)
      if (reboot) {
        DATASafeShutdown();
        Delay_AD_Log(2);
        DATAPower(true);
      }
    return false;
  } else
    return true;

} // DATAExpectedReturn //
