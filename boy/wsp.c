// wsp.c
#include <com.h>
#include <dat.h>

// gain num detMax detNum dutycycl port
WspInfo wsp = {
  2, 4, 10, 0, 360, NULL
};

bool SendwspGPS = false;
static int wspGPSSends;
bool wspOn;
float wspFreeSpace = 0.0;
int TotalDetections;
int dtxrqst;

// char* wspString;
// short wspStringLength=64;
// PAM TUPORT Setup
Serial PAMPort;
short PAM_RX, PAM_TX;

char *GetwspInput(float *);
void AppendDetections(char *, int);

static char wspfile[] = "c:wspFRS.DAT";
static char WspString[64];

bool wspStatus(void) { return wspOn; }

/*
 * Power is applied to the wsp boards
 */
void wspPower(bool power) {
  DBG0("wspPower()")
  // must call wspInit first
  if (power) {
    flogf("\n%s|wsp: ON", Time(NULL));
    PIOSet(wspPWR_ON);
    delayms(10);
    PIOClear(wspPWR_ON);
    wsp.on = true;
  } else {
    flogf("\n%s|wsp: OFF", Time(NULL));
    PIOSet(wspPWR_OFF);
    delayms(10);
    PIOClear(wspPWR_OFF);
    wsp.on = false;
    delayms(1000);        //??
  }
} // wspInit

/*
 * void wspData()
 * Incoming wsp ASCII Communication. Looks for certain commands starting with
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
short wspData(void) {
  float returndouble;
  int i;
  int wspFile;
  short returnvalue = 0;
  char DTXFilename[] = "c:00000000.DTX";

  // memset(wspString, 0, wspStringLength*(sizeof wspString[0]));

  // DataString =
  GetwspInput(
      &returndouble); // changed to 1.25 second wait time to receive the '$'

  if (strncmp(DataString, "$GPS", 4) == 0) {
    wspGPS(124.5, 45);
    return 1;
  }
  /*   else if(strncmp(DataString, "$TFP", 4)==0){

        flogf("\n\t|wsp%d: %0.2f%% Total Space", wsp.NUM, returndouble);
        if(returndouble ==0.0){
           DBG1("\t|Bad TFP return... tryaing again")
           wspTFP();
           return -3;
           }
        //Calculate what minimum free space should be for given detint.
        }
 */
  else if (strncmp(DataString, "$DFP", 4) == 0) {

    flogf("\n\t|wsp%d: %.2f%% Free Space", wsp.NUM, returndouble);

    if (returndouble == 0.0) {
      DBG1("\t|Bad DFP return...trying again")
      wspDFP();
      return -2;
    }

    wspFreeSpace = returndouble;

#ifndef SEAGLIDER
    UpdatewspFRS();
#endif

    if (returndouble < MIN_FREESPACE) {

      flogf("\n\t|wsp%d FreeSpace below Minimum", wsp.NUM);
      if (wsp.NUM < wspNUMBER) {
        flogf("\n\t|Incrementing wspNumber");
        Changewsp(wsp.NUM + 1);
        // Only location when to permanently increment wsp number: When free
        // space runs out. or from start script.
        VEEStoreShort(wspNUM_NAME, wsp.NUM);
      }

      else {
        flogf("\n\t|Exceeded wspNumber");
        wsp.DUTYCYCL = 0;
        VEEStoreShort(DUTYCYCLE_NAME, 0);
        flogf("\n\t|wsp Shutdown");
        // wspSafeShutdown(); //This creates a never ending loop.
        if (!wspExit())
          if (!wspExit()) {
            flogf(": Forcing Off");
            // ?? fetch storm warning
            wspPower(false);
          }
        Check_Timers(Return_ADSTIME());
        return 2;
      }

      return 2;
    }

    if (!SendwspGPS) {
      wspGPS(124.5, 45);
      delayms(150);
      TUTxFlush(PAMPort);
      TURxFlush(PAMPort);
      wspGain(-1);
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
    wspFile = open(DTXFilename, O_APPEND | O_RDWR | O_CREAT);
    delayms(25);
    if (wspFile <= 0)
      flogf("\nERROR  |wspData() %s open errno: %d", DTXFilename, errno);
    DBG(else flogf("\n\t|wspData() %s opened", DTXFilename);)

    if (wspFile <= 0) {
      flogf("\nERROR|wspData() open errno: %d", errno);
      return -3;
    }

    if ((int)returndouble < dtxrqst)
      dtxrqst = (int)returndouble;
    if ((int)returndouble == 0) {
      flogf("\n\t|Writing %d detections to %s", dtxrqst, DTXFilename);
      if (close(wspFile) < 0)
        flogf("\nERROR |wspData() %s close errno %d", DTXFilename, errno);
      DBG(else flogf("\n\t|wspData() %s closed", DTXFilename);)
      return 3;
    }

    else if (dtxrqst > 0) {
      DBG1("%s", DataString)
      AppendDetections(DataString, wspFile);
      if (dtxrqst > 0) {
        for (i = 0; i < dtxrqst; i++) {
          TickleSWSR();
          // memset(DataString, 0, 64*(sizeof DataString[0]));
          GetwspInput(&returndouble);
          if ((int)returndouble == -1) {
            flogf("\nERROR|Bad Return from GetwspInput()");
            break;
          }
          AppendDetections(DataString, wspFile);
        }
        TURxFlush(PAMPort);
#ifdef ACOUSTICMODEM // This if defined statement should be based on whether an
                     // AMODEM is implemented but rather a real-time detection
                     // turn around.
        if (wsp.DETNUM > 0)
          if (returndouble >= wsp.DETNUM)
            flogf("\n\t|Detection Number surpassed");
#endif
      }
    }
    close(wspFile);

    return 3;
  }

  else if (strncmp(DataString, "$DTX", 4) == 0) {
    DBG1("%s", DataString)
    return 4;
  }

  else if (strncmp(DataString, "$NGN", 4) == 0) {
    if (!SendwspGPS) {
      wspGPS();
      delayms(50);
    }

    wspGain(-1);

    return 5;
  }

  else if (strncmp(DataString, "$FIN", 4) == 0) {
    flogf(": Found Exit");
    delayms(2000);     // Gives a little bit of time to wsp to umount /mnt
    wspPower(false); // Powers off Data
    return 6;
  } else if (strcmp(DataString, NULL) == 0) {
    return 0;
  } else
    return -1;

  return 0;

} // wspData
/*
 * int wspDet()
This is where we inquire about the total number of detections every so often.
This function will call to the wsp board with max_detections. It first
repsonds with a $DXN...* com line then lists each actual detection
afterward with a $DTX...* com line. Each are up to ~60 bytes per line

 */
void wspDet(int dtx) {

  if (dtx < 0)
    return;                   // if negative number then don't call detections
  else if (dtx > wsp.DETMAX) // if received dtx reqeust > Maximum user set.
    dtx = wsp.DETMAX;

  dtxrqst = dtx;

  delayms(10);
  TUTxFlush(PAMPort);
  TURxFlush(PAMPort);
  TUTxPrintf(PAMPort, "$DX?,%d*\n", dtx);
  TUTxWaitCompletion(PAMPort);

  delayms(500);

} // wspDet
/*
 * int wspGPS()
This is where we send the GPS Time and Location to wsp Board at startup
 */
void wspGPS(void) {

  float minutes;
  float decimal;
  float LAT, LONG;
  char LATITUDE[17];
  char LONGITUDE[17];

  flogf("\n%s|wspGPS(%d):", Time(NULL), wsp.NUM);
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

  SendwspGPS = true;

} // wspGPS
/*
 * int wspGain()
We can update the gain parameters for the wsp Board.
Might want to do this at the start up of wsp Program when wsp Requests
for gain values.

 */
void wspGain(short c) {

  if (c < 0 || c > 3)
    c = wsp.GAIN;

  flogf(" & Gain: %d", c);
  TUTxFlush(PAMPort);
  TUTxPrintf(PAMPort, "$NGN,%d*\n", c);
  TUTxWaitCompletion(PAMPort);
  delayms(2);

} // wspGain
/*
 * int wspDFP()
 */
void wspDFP(void) {

  DBG0("\t|wspDFP(%d)", wsp.NUM)
  TURxFlush(PAMPort);
  TUTxFlush(PAMPort);
  TUTxPrintf(PAMPort, "$DFP*\n");
  TUTxWaitCompletion(PAMPort);
  delayms(250);

} // wspDFP
/*
 * int wspTFP()
 */
void wspTFP(void) {

  DBG0("\t|wspTFP(%d)", wsp.NUM)
  TURxFlush(PAMPort);
  TUTxFlush(PAMPort);
  TUTxPrintf(PAMPort, "$TFP*\n");
  TUTxWaitCompletion(PAMPort);
  delayms(250);

} // wspDFP
/*
 * int wspExit()
 */
bool wspExit(void) {

  flogf("\n\t|wspExit()");

  TUTxFlush(PAMPort);
  TURxFlush(PAMPort);
  TUTxPrintf(PAMPort, "$EXI*\n");
  TUTxWaitCompletion(PAMPort);
  Delay_AD_Log(150);  delayms(200);

  wspData();

  if (!wspOn) {

    return true;
  } else
    return false;

} // wspExit
/*
 * char* GetwspInput()
1. Gets incoming serial data from ActivePAM on MPC
2. If it is -1 it breaks and exits, once it sees a '*' it stops taking serial
3. Look for the appropriate wsp Command (DFP, EXI, DXN, DTX)
4. returns numchars if we are looking for DFP
5. will return number of detections if we get DXN. for which we will run a for
loop to get each detection in this same function
6. Should return char* of DTX if we need to get each detection.

 */
char *GetwspInput(float *numchars) {
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
    } // if we see an * we call that the last of this wsp Input
    else if (in == -1) {
      TURxFlush(PAMPort);
      return NULL;
    } else
      r[i] = in;
  }

  // Looks for end of wspInput, gets length, appends to returnline
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
 ** void Changewsp()
 */
void Changewsp(short wnum) {

  // Shut off wsp
  if (!wspExit()) {
    if (!wspExit()) {
      flogf(": Forcing Off");
      wspPower(false);
    }
  }

  wspInit(false);
  delayms(100);
  wsp.NUM = wnum;
  wspInit(true);
  wspPower(true);
}
/*
 * void GetwspSettings()
 */
void GetwspSettings(void) {
  char *p;
  p = VEEFetchData(DETECTIONMAX_NAME).str;
  wsp.DETMAX = atoi(p ? p : DETECTIONMAX_DEFAULT);
  DBG1("DETMAX=%u (%s)", wsp.DETMAX, p ? "vee" : "def")
  if (wsp.DETMAX > MAX_DETECTIONS) {
    wsp.DETMAX = MAX_DETECTIONS;
    VEEStoreShort(DETECTIONMAX_NAME, wsp.DETMAX);
  }

  //"g" 0-3 gain values
  p = VEEFetchData(wspGAIN_NAME).str;
  wsp.GAIN = atoi(p ? p : wspGAIN_DEFAULT);
  DBG1("wspGAIN=%u (%s)", wsp.GAIN, p ? "vee" : "def")
  if (wsp.GAIN > 3) {
    wsp.GAIN = 3;
    VEEStoreShort(wspGAIN_NAME, wsp.GAIN);
  } else if (wsp.GAIN < 0) {
    wsp.GAIN = 0;
    VEEStoreShort(wspGAIN_NAME, wsp.GAIN);
  }

  // "N" Number of detections per DETINT to trigger AModem
  p = VEEFetchData(DETECTIONNUM_NAME).str;
  wsp.DETNUM = atoi(p ? p : DETECTIONNUM_DEFAULT);
  DBG1("DETECTNUM=%u (%s)", wsp.DETNUM, p ? "vee" : "def")

  //"C" dutycycle
  p = VEEFetchData(DUTYCYCLE_NAME).str;
  wsp.DUTYCYCL = atoi(p ? p : DUTYCYCLE_DEFAULT);
  DBG1("DUTYCYCLE=%d (%s)", wsp.DUTYCYCL, p ? "vee" : "def")
  if (wsp.DUTYCYCL > MAX_DUTYCYCLE) {
    wsp.DUTYCYCL = MAX_DUTYCYCLE;
    VEEStoreShort(DUTYCYCLE_NAME, wsp.DUTYCYCL);
  } else if (wsp.DUTYCYCL < MIN_DUTYCYCLE) {
    wsp.DUTYCYCL = MIN_DUTYCYCLE;
    VEEStoreShort(DUTYCYCLE_NAME, wsp.DUTYCYCL);
  }

  // "x" wspnum
  p = VEEFetchData(wspNUM_NAME).str;
  wsp.NUM = atoi(p ? p : wspNUM_DEFAULT);
  DBG1("wspNUM=%d (%s)", wsp.NUM, p ? "vee" : "def")
  if (wsp.NUM < 1) {
    wsp.NUM = 1;
    VEEStoreShort(wspNUM_NAME, wsp.NUM);
  } else if (wsp.NUM > wspNUMBER) {
    wsp.NUM = wspNUMBER;
    VEEStoreShort(wspNUM_NAME, wsp.NUM);
  }

} // GetwspSettings
/*
 * void wspSafeShutdown()
 */
void wspSafeShutdown(void) {

  int i;

  if (wspOn) {
    wspDFP();
    i = wspData();
    if (i != 2) {
      wspDFP();
      i = wspData();
    }

    // Shut off wsp for Data Transmission
    if (!wspExit()) {
      if (!wspExit()) {
        flogf(": Forcing Off");
        wspPower(false);
      }
    }

  } else
    return;

} // wspSafeShutdown
/*
 * Void wspWriteFile()
 */
void wspWriteFile(int uploadfilehandle) {
  char detfname[] = "c:00000000.dtx";
  int byteswritten;

  memset(WriteBuffer, 0, BUFSZ);

  flogf("\n\t|wspWriteFile()");

  if (wsp.DUTYCYCL > 0) {
    sprintf(WriteBuffer, "\n---wsp%d---\nGPS Sends: %d\nGain:%d\nFree "
                         "Space:%4.2f%%\nDuty Cycle:%d%%\nMax "
                         "Detections:%02d\nTotal Detections:%d\n\0",
            wsp.NUM, wspGPSSends, wsp.GAIN, wspFreeSpace, wsp.DUTYCYCL,
            wsp.DETMAX, TotalDetections);

    DBG1("%s", WriteBuffer)
    /*
            #ifdef REALTIME
               if(wsp.DETNUM>0){
               sprintf(stringadd, "\nCall upon %04d Detections\n", wsp.DETNUM);
               strncat(buf, stringadd, 27*sizeof(char));
               }
            #endif
 */

  } else {
    sprintf(WriteBuffer, "\nwsp-OFF\n\0");
  }

  byteswritten = write(uploadfilehandle, WriteBuffer, strlen(WriteBuffer));
  DBG1("wspWrite File: Number of Bytes written: %d", byteswritten)

  // If more than one wsp, add total free space of all wsps to Write File.
  if (wspNUMBER > 1)
    Append_Files(uploadfilehandle, "C:wspFRS.DAT", false, 0);
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

} // wspWriteFile
/*
 * float GetwspFreeSpace
 * Read "wspfs.bat" file. return free space of current wsp? ||  return free
space of last wsp
 */
float GetwspFreeSpace(void) {

  return wspFreeSpace;

} // GetwspFreeSpace

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
 * GatherwspFreeSpace()
 * Only should come here upon MPC.STARTUPS==0 && wspNUMBER>1
 */
void GatherwspFreeSpace(void) {
  short wret = 0, i, count = 0, wnum = 1;
  int byteswritten = 0;
  int writenum = 0;
  bool gain = false, dfp = false;
  int wspfilehandle;
  char wspbuff[16];
  static char *filename = "C:wspFRS.DAT";

  if (wspOn) {
    wspExit();
    delayms(2500);
  }
  wspPower(true);

  wspfilehandle = open(filename, O_CREAT | O_TRUNC | O_RDWR);
  if (wspfilehandle <= 0) {
    flogf("file handle: %d", wspfilehandle);
    flogf("errno: %d", errno);
    return;
  }
  for (i = 1; i <= wspNUMBER; i++) {
    sprintf(wspbuff, "W%d:00.00%,", i); // sprintf adds trailing \0
    byteswritten =
        write(wspfilehandle, wspbuff, strlen(wspbuff) * sizeof(char));
    flogf("\n\t|Bytes written: %d", byteswritten);
  }
  close(wspfilehandle);

  while (wnum <= wspNUMBER) {
    while (!dfp && count <= 3) {

      Sleep();

      if (AD_Check()) {
        count++;
        if (gain || count == 2) {
          DBG1("\t|GWFS: DFP2")
          wspDFP();
          delayms(150);
          if (wspData() == 2)
            dfp = true;
        }
      }

      if (tgetq(PAMPort)) {
        wret = wspData();
        if (wret == 1) {
          delayms(150);
          wret = wspData();
          if (wret == 5) {
            gain = true;
            delayms(150);
            DBG1("\t|GWFS: DFP1")
            wspDFP();
            delayms(150);
            if (wspData() == 2)
              dfp = true;
          }
        }
      }
    }

    TURxFlush(PAMPort);
    TUTxFlush(PAMPort);
    if (!dfp)
      wspFreeSpace = 00.00;

    dfp = false;
    gain = false;
    count = 0;
    wnum++;
    if (wnum > wspNUMBER)
      break;
    //      if(wspFreeSpace>=MIN_FREESPACE)
    Changewsp(wnum);
  }
  DBG1("\t|wnum: %d, wsp.NUM: %d", wnum, wsp.NUM)
  wnum = 1;

  // Shut off wsp && Close TUPort
  if (!wspExit()) {
    if (!wspExit()) {
      flogf(": Forcing Off");
      wspPower(false);
    }
  }

  wspInit(false);
  delayms(100);
  wsp.NUM = wnum;
  VEEStoreShort(wspNUM_NAME, wsp.NUM);

} //GatherwspFreeSpace
/*
 * void UpdatewspFRS()
 * 
 * Write to file: wspFRS.DAT with the current wsp's free space
 */
void UpdatewspFRS(void) {
  // global *DataString;
  int wspfilehandle;
  struct stat fileinfo;
  char *wspnum = "W0:";
  char *p;
  int length;
  int bytes;
  long filesize;

  flogf("\n%s|Update %s ", Time(NULL), wspfile);
  delayms(10);
  // sprintf(&wspfile[2], "wspFRS.DAT");
  delayms(20);
  if (stat(wspfile, &fileinfo) != 0) {
    flogf("%s file does not exist. making file...", wspfile);
    GatherwspFreeSpace();
    stat(wspfile, &fileinfo);
  }
  filesize = fileinfo.st_size;

  DBG1("\t|File size: %ld", filesize)

  wspfilehandle = open(wspfile, O_RDWR);
  delayms(25);
  if (wspfilehandle <= 0)
    flogf("\nERROR  |UpdatewspFRS(): file open errno: %d", errno);
  DBG(else flogf("\n\t|UpdatewspFRS() %s opened", wspfile);)

  read(wspfilehandle, DataString, fileinfo.st_size);
  // flogf("\n\t|%s", DataString);
  if (wsp.NUM > wspNUMBER)
    wsp.NUM = wspNUMBER;
  sprintf(wspnum, "W%d:", wsp.NUM);

  p = strstr(DataString, wspnum);
  if (p == NULL)
    flogf("\nERROR  |No String search found for: %s", wspnum);
  length = p - DataString;
  if (length > 30) {
    flogf("\nERROR |Returning due to bad filde position for %s", wspfile);
    close(wspfilehandle);
    return;
  }

  p = strtok(p + 3, ",");

  DBG1("Updating wspFRS at position %d from %5.2f to %5.2f", length, atof(p), wspFreeSpace)

  sprintf(wspnum, "%5.2f", wspFreeSpace);
  lseek(wspfilehandle, length + 3, SEEK_SET);
  bytes = write(wspfilehandle, wspnum, 5);
  DBG1("Bytes written: %d", bytes)
  lseek(wspfilehandle, 0, SEEK_SET);
  read(wspfilehandle, DataString, fileinfo.st_size);
  flogf("%s", DataString);

  close(wspfilehandle);

} // UpdatewspFRS
/*
 * void wspInit()
 */
void wspInit(bool on) {
  // global DataStr
  int DataNum;

  DataNum = wsp.NUM;
  flogf("\n\t|%s wsp%d TUPort", on ? "Open" : "Close", DataNum);
  if (on) {
    PAM_RX = TPUChanFromPin(28);
    PAM_TX = TPUChanFromPin(27);
    PAMPort = TUOpen(PAM_RX, PAM_TX, wspBAUD, 0);
  } else if (!on) {
    TUTxFlush(PAMPort);
    TURxFlush(PAMPort);
    TUClose(PAMPort);
    delayms(1000);
  }

  PIOClear(wspPWR_ON);
  PIOClear(wspPWR_OFF);

  // PAM 1
  if (DataNum == 1) {
    if (on) {
      PIOSet(wspONE);
      PIOClear(wspTWO);
    } else {
      PIOClear(wspONE);
      PIOClear(wspTWO);
    }
  }
  // PAM 2
  else if (DataNum == 2) {
    if (on) {
      PIOSet(wspONE);
      PIOSet(wspTWO);
    } else {
      PIOClear(wspONE);
      PIOClear(wspTWO);
    }
  }
  // PAM 3
  else if (DataNum == 3) {
    if (on) {
      PIOSet(wspTHREE);
      PIOClear(wspFOUR);
    } else {
      PIOClear(wspTHREE);
      PIOClear(wspFOUR);
    }

  }
  // PAM 4
  else if (DataNum == 4) {
    if (on) {
      PIOSet(wspTHREE);
      PIOSet(wspFOUR);
    } else {
      PIOClear(wspTHREE);
      PIOClear(wspFOUR);
    }
  } else if (DataNum == 0) {
    flogf("\n\t|wsp Zero. Run out of space?");
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
 * bool wspExpectedReturn(short)
 */
bool wspExpectedReturn(short expected, bool reboot) {

  if (wspData() != expected) {
    wspInit(false);
    Delay_AD_Log(2);
    wspInit(true);
    Delay_AD_Log(1);
    switch (expected) {
    case 1:
      wspGPS();
      break;
    case 2:
      wspDFP();
      break;
    case 3:
      wspDet(wsp.DETMAX);
      break;
    case 6:
      wspExit();
      break;
    }
    if (wspData() != expected)
      if (reboot) {
        wspSafeShutdown();
        Delay_AD_Log(2);
        wspPower(true);
      }
    return false;
  } else
    return true;

} // wspExpectedReturn //
