/****************************************/
/*     member of utils library          */
/****************************************/
#ifndef UTILS_OOP
#define UTILS_OOP

using namespace std;

#include <defs.p>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <time.h>

// external declaration for IEEE representation of infinity
extern const double Inf;

// external declaration for IEEE representation of infinity
extern const double NaN;

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * $Id: utils.cpp,v 1.1 2006/11/18 17:22:31 swift Exp $
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * RCS Log:
 *
 * $Log: utils.cpp,v $
 * Revision 1.1  2006/11/18 17:22:31  swift
 * Added new functionality to library.
 *
 * Revision 1.5  2006/11/11 18:34:34  swift
 * Modifications to conform to current C++ standards.
 *
 * Revision 1.4  2001/06/02 07:34:47  swift
 * Fixes required by RedHat 7.1 - mostly minor fix-ups to prevent compiler warnings.
 *
 * Revision 1.3  1999/06/11 22:44:10  swift
 * Change handling of NaN's to reflect changes in the OS.
 *
 * Revision 1.2  1999/01/16 06:13:28  swift
 * Misc modifications.
 *
 * Revision 1.2  1996/01/01 22:43:00  swift
 * Merge header & source files.
 *
 * Revision 1.1  1995/06/30 18:01:11  swift
 * Initial revision
 *
 *========================================================================*/
/* structure to augment structure "tm" with additional data members       */
/*========================================================================*/
struct DTime: public tm
{
   int tm_hsec;    // hundredths of a second
   time_t tm_time; // number of seconds since 00:00:00 GMT on 1/1/70

   // constructors
   inline DTime(time_t t=0);
   inline DTime(struct tm &date,int hundredths=0);
};

/*------------------------------------------------------------------------*/
/* constructor of DTime class                                             */
/*------------------------------------------------------------------------*/
inline DTime::DTime(time_t t)
{
   if (!t) t = time(NULL);

   *this = DTime(*localtime(&t));
}

/*------------------------------------------------------------------------*/
/* constructor of DTime class                                             */
/*------------------------------------------------------------------------*/
inline DTime::DTime(struct tm &date,int hundredths): tm(date)
{
   tm_hsec=hundredths;

   // compute seconds since midnight on 1/1/70
   tm_time=mktime(&date);
}

char *strloc(istream &source,char *key,char *dest);
void  open_stream(const char *fname,_Ios_Openmode mode,filebuf &stream);
void  open_stream(const char *fname,_Ios_Openmode mode,fstream &stream);
void  rewind(istream &source);
int   fprintf(ostream &dest, const char *format, ...);

extern const int STDIN;
extern const int STDOUT;
extern const int STDERR;

#endif // UTILS_OOP
#ifdef UTILS_CPP

const int STDIN  = 0;
const int STDOUT = 1;
const int STDERR = 2;

#if defined(SunOS)

   #include <nan.h>

   // external declaration for IEEE representation of not-a-number
   const double NaN = quiet_nan(0);

   // external declaration for IEEE representation of infinity
   const double Inf = infinity();

#elif defined(__linux__)

   #include <bits/nan.h>

   // external declaration for IEEE representation of not-a-number
   const double NaN = NAN;

   // external declaration for IEEE representation of infinity
   const double Inf = HUGE_VAL;

#else

   // external declaration for IEEE representation of not-a-number
   const double NaN = HUGE_VAL;

   // external declaration for IEEE representation of infinity
   const double Inf = HUGE_VAL;

#endif
 
/*------------------------------------------------------------------------*/
/*                                                                        */
/*------------------------------------------------------------------------*/
int fprintf(ostream &dest, const char *format, ...)
{
   const int MaxBuf = 4095;
   static char buf[MaxBuf+1];

   int n=0;
   va_list arg_pointer;
   va_start(arg_pointer,format);
   n=vsnprintf(buf, MaxBuf, format, arg_pointer);
   va_end(arg_pointer);

   dest << buf;
   
   return (n>MaxBuf) ? MaxBuf : n;
}
/*========================================================================*/
/* function to open an fstream object                                     */
/*========================================================================*/
void open_stream(const char *fname,_Ios_Openmode mode,fstream &stream)
{
   // open stream with desired mode
   if (fname) stream.open(fname,mode);
   else fname = "";
                                                                                           
   if (!stream.is_open())
   {
      char cmode[512]="";
                                                                                           
      if (mode&ios::in)        strcat(cmode,"ios::in|");
      if (mode&ios::out)       strcat(cmode,"ios::out|");
      if (mode&ios::app)       strcat(cmode,"ios::app|");
      if (mode&ios::ate)       strcat(cmode,"ios::ate|");
      if (mode&ios::trunc)     strcat(cmode,"ios::trunc|");
                                                                                           
      cmode[strlen(cmode)-1]=0;
                                                                                           
      swifterr("\nUnable to open \"%s\" using mode \"%s\".\n",fname,cmode);
   }
   else stream.clear();
}

/*========================================================================*/
/* function to open an fstream object                                     */
/*========================================================================*/
void open_stream(const char *fname,_Ios_Openmode mode,filebuf &stream)
{
   // open stream with desired mode
   if (fname) stream.open(fname,mode);
   else fname = "";

   if (!stream.is_open())
   {
      char cmode[512]="";

      if (mode&ios::in)        strcat(cmode,"ios::in|");
      if (mode&ios::out)       strcat(cmode,"ios::out|");
      if (mode&ios::app)       strcat(cmode,"ios::app|");
      if (mode&ios::ate)       strcat(cmode,"ios::ate|");
      if (mode&ios::trunc)     strcat(cmode,"ios::trunc|");

      cmode[strlen(cmode)-1]=0;

      swifterr("\nUnable to open \"%s\" using mode \"%s\".\n",fname,cmode);
   }
}

/*------------------------------------------------------------------------*/
/*                                                                        */
/*------------------------------------------------------------------------*/
void  rewind(istream &source)
{
   source.clear(); source.seekg(0);
}

#endif // UTILS_CPP
