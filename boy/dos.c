// dos.c - utility stuff
#include <com.h>
#include <dos.h>

/*
int sysOSCmd(char *command, long filenum, char *ext, char *extt) {
  char Com[64];
  int r = 0;
  static char fname[] = "c:00000000.";
  memset(Com, 0, 64);

  if (strstr(command, "copy") != NULL) {
    strncpy(Com, "copy ", 5);
    sprintf(fname, "c:%08ld.", filenum);
    strncat(Com, fname, 11);
    strncat(Com, ext, 3);
    strncat(Com, " c:", 3);
    if (extt == NULL)
      strncat(Com, ext, 3);
    else
      strncat(Com, extt, 3);
    strncat(Com, "\\", 1);
    strncat(Com, &fname[2], 9);
    if (extt == NULL)
      strncat(Com, ext, 3);
    else
      strncat(Com, extt, 3);
  } else if (strstr(command, "del") != NULL) {
    strncpy(Com, "del ", 4);
    sprintf(fname, "c:%08ld.", filenum);
    strncat(Com, fname, 11);
    strncat(Com, ext, 3);
  } else if (strstr(command, "ren") != NULL) {
    strncpy(Com, "ren ", 4);
    sprintf(fname, "c:%08ld.", filenum);
    strncat(Com, fname, 11);
    strncat(Com, ext, 3);
    strncat(Com, " ", 1);
    strncat(Com, fname, 11);
    strncat(Com, extt, 3);
  }

  else if (strstr(command, "move") != NULL) {

    sysOSCmd("copy", filenum, ext, extt);
    sysOSCmd("del", filenum, ext, NULL);
    return r;
  }

  flogf("\n%s|COMDos() %s", clockTime(scratch), Com);
  putflush();
  CIOdrain();
  execstr(Com);
  utlDelay(250);
  return r;
} // sysOSCmd

///
// Setup directories for files not needing to be access anymore.
void dirSetup(char *path) {
  char DOSCommand[64];
  memset(DOSCommand, 0, 64);
  strncpy(DOSCommand, "mkdir ", 6);
  strncat(DOSCommand, path, 3);

  flogf("\n\t|MakeDirectory() %s", DOSCommand);
  putflush();
  CIOdrain();
  execstr(DOSCommand);
  utlDelay(1000);
} // sysDirS

///
//      GetFileName
//
//      Search the C drive for specific "FileType" i.e. "DAT" or "SNT" or "LOG"
// This function can return the "Lowest" filename of specified FileType for
// Data transferring of the oldest file on the system. or "!Lowest" to see which
// file name is most recent.
//      "incIndex" will increment the filename by one if "!Lowest" is also true.
// A long pointer "fcounter" will be pointed to the filename (8 digit number)
// of which ever file the function is searching for.
char *GetFileName(bool Lowest, bool incIndex, long *fcounter,
                  const char *FileType) {

  long counter = -1; // 2003-08-21
  long filecounter = 0;
  long val;
  long minval = 99999999; // 2003-08-21
  long maxval = -1;       // 2003-08-21
  static char dfname[] = "x:00000000.LOG";
  static char path[] = "x:";
  DIRENT de;
  short err;
  short i;

  //
  path[0] = dfname[0] = 'C'; // C: drive
  DBG0(flogf("\n\t|GetFileName(.%3s): %s", FileType,
            Lowest ? "Lowest" : "Highest");)

  // Do this with *log extension next
  if ((err = DIRFindFirst(path, &de)) != dsdEndOfDir) {
    do {
      if (err != 0) {
        flogf("\n%s|GetFileName(): did not find Lowest File.", Time(NULL));
        break;
      }
      if (de.d_name[9] == FileType[0] && de.d_name[10] == FileType[1] &&
          de.d_name[11] == FileType[2]) {
        filecounter++;
        for (i = val = 0; i < 8; i++)
          if (de.d_name[i] >= '0' && de.d_name[i] <= '9')
            val = (val * 10) + de.d_name[i] - '0';
          else
            break;
        if (i == 8) { // all digits
          if (Lowest && val < minval) {
            // DBG1(flogf("\nNew lowest value: %ld", val);)
            minval = val;
          } else if (val > maxval) {
            // DBG1(flogf("\nNew highest value: %ld", val);)
            maxval = val;
          }
        }
      }
    } while (DIRFindNext(&de) != dsdEndOfDir);


    if (Lowest) {
      if (minval < 0 && counter < 0 || minval == 99999999) {
        DBG1(flogf("\n\t|Did not find any .%3s files", FileType);)
        return NULL;
      } else if (minval > counter) {
        DBG1(flogf("\n\t|New min val: %ld", minval);)
        counter = minval;
      }
    } else {
      if (maxval < 0 && counter < 0) // 2003-08-27
        counter = 0;                 // 2003-08-21

      else if (maxval >= counter) { // 2003-08-21
        DBG1(flogf("\n\t|New maxval: %ld", maxval);)
        counter = maxval;
        if (incIndex)
          counter++;
      }
    }
  }


  // A new file name
  sprintf(&dfname[2], "%08lu.%3s", counter, FileType);
  if (Lowest)
    *fcounter = filecounter;
  else
    *fcounter = counter;

  DBG1(flogf("\n%s|GetFileName(): %s", Time(NULL), dfname);)
  DBG1(flogf("\n\t|filecounter: %ld", filecounter);)

  return dfname;

} //____ GetNextDATFileName() ____//

 */
