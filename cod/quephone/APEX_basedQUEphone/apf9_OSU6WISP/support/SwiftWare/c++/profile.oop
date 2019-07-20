#ifndef PROFILE_OOP
#define PROFILE_OOP

#include <defs.oop>

#if (DOUBLE)
   #define PRECISION   double
   #define Profile     dProfile
   #define MACHINE_EPS dEPS
#else
   #define PRECISION   float
   #define Profile     fProfile
   #define MACHINE_EPS fEPS
#endif

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * $Id: profile.cpp,v 1.1 2006/11/18 17:22:31 swift Exp $
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * RCS Log:
 *
 * $Log: profile.cpp,v $
 * Revision 1.1  2006/11/18 17:22:31  swift
 * Added new functionality to library.
 *
 * Revision 1.13  2001/05/13 10:40:07  navo
 * Modifications for porting from UW to NAVO.
 *
 * Revision 1.12  1997/10/26 18:47:01  swift
 * Addition of copy constructor and assignment operator.
 *
 * Revision 1.11  1997/01/05 16:41:02  swift
 * Added a few comments.
 *
 * Revision 1.10  1997/01/04 15:07:37  swift
 * Remove parameter name in the argument of `Invert()' to avoid
 * compiler warning.
 *
 * Revision 1.9  1997/01/04 14:45:32  swift
 * Change `Invert()' method to generate warning error rather than fatal error.
 *
 * Revision 1.8  1997/01/01 23:57:34  swift
 * Delete binary read & write functions due to nonuse.
 *
 * Revision 1.7  1996/06/14 15:25:14  swift
 * Modifications to eliminate compiler warnings with gcc -Wall ...
 *
 * Revision 1.6  1996/04/11 15:54:31  swift
 * Modified preprocessor directive that selects for double or single precision.
 *
 * Revision 1.5  1996/01/01 22:21:22  swift
 * Merge header & source files.
 *
 * Revision 1.4  1995/08/11 18:49:05  swift
 * Moved some preprocessor directives to derived classes...where they
 * really belonged in the first place.
 *
 * Revision 1.3  1995/08/10 17:05:33  swift
 * Renamed public functions so that first letter is capitalized.
 *
 * Revision 1.2  1995/06/30 19:02:16  swift
 * Fix minor problem with preprocessor directive.
 *
 * Revision 1.1  1995/06/30 17:52:08  swift
 * Initial revision
 *
 *========================================================================*/
/* Profile class declaration                                              */
/*========================================================================*/
/*
   This pure virtual base class provides a uniform interface to a family of
   derived classes whose purpose is to model an arbitrary general
   mathematical function of the form: y=f(x).  This base class provides no
   functionality except as a repository for a few data members...its purpose
   is to unify the interface for defined classes.

   written by Dana Swift
*/
class Profile: public SwiftWare
{
   // member data
   private:

      // define profile fingerprint
      static char const *fingerprint;

   public:

      // define variables to contain extremes
      PRECISION xmin,xmax,ymin,ymax;

   // member functions
   protected:

      // function to copy data members
      inline virtual void copy(const Profile &prf);
      
      // function to make a header from a fingerprint
      char *make_delimiter(const char *delimiter,const char *fingerprint_);

      // function to search a FILE stream for a delimiter string
      int seek_delimiter(char *delimiter,FILE *source) {return strseek(delimiter,source);}

   public:

      // constructor to initialize extremes
      Profile(void) {xmin=ymin=xmax=ymax=ERROR;}

      // copy constructor
      Profile(const Profile &prf) {copy(prf);}

      // assignment operator
      Profile &operator=(const Profile &prf) {copy(prf); return *this;}
      
      // overloaded operator functions
      virtual PRECISION operator() (PRECISION xtrg) {return Eval(xtrg);}

      // function to dump core to ostream
      virtual int DumpCore(ostream &dest=cout,const char id[]="",const char prefix[]="");

      // virtual declaration for profile evaluator
      virtual PRECISION Eval(PRECISION xtrg) = 0;

      // virtual declaration for inverting profile
      virtual PRECISION Invert(PRECISION ytrg);

      // virtual declaration for shape-shifter
      virtual int Learn(const PRECISION *x,const PRECISION *y,unsigned n) = 0;
      
      // overloaded function from SwiftWare base class
      virtual int ReadCore(istream &source);

      // virtual procedure for seeking extremes in data
      virtual void SeekExtremes(void) = 0;

      // virtual procedure for setting extremes in data
      virtual void SetExtremes(PRECISION xmin,PRECISION xmax,PRECISION ymin,PRECISION ymax);
};

/*------------------------------------------------------------------------*/
/* function to copy data members                                          */
/*------------------------------------------------------------------------*/
inline void Profile::copy(const Profile &prf)
{
   xmin=prf.xmin; xmax=prf.xmax; ymin=prf.ymin; ymax=prf.ymax;
}

/*------------------------------------------------------------------------*/
/* function to set the class' data members for data extremes              */
/*------------------------------------------------------------------------*/
inline void Profile::SetExtremes(PRECISION xmin,PRECISION xmax,PRECISION ymin,PRECISION ymax) 
{
   Profile::xmin=xmin; 
   Profile::xmax=xmax; 
   Profile::ymin=ymin; 
   Profile::ymax=ymax;
}

#endif // PROFILE_OOP
#ifdef PROFILE_CPP

// initialize fingerprint for the Profile class
const char *Profile::fingerprint="Profile";

/*------------------------------------------------------------------------*/
/* function to print out data member values                               */
/*------------------------------------------------------------------------*/
int Profile::DumpCore(ostream &dest,const char *id,const char *prefix)
{
   // write header to core file
   HeadCore(dest,id,prefix,fingerprint,IOSFLAGS|ios::scientific);

   // find the number of significant decimal digits
   int precision = -(int)log10(MACHINE_EPS);

   // print values of xmin and xmax
   dest << indent << "PRECISION (xmin,xmax) = ("
        << setw(precision+8) << setprecision(precision) << xmin << ','
        << setw(precision+8) << setprecision(precision) << xmax << ")\n";

   // print values of ymin and ymax
   dest << indent << "PRECISION (ymin,ymax) = ("
        << setw(precision+8) << setprecision(precision) << ymin << ','
        << setw(precision+8) << setprecision(precision) << ymax << ")\n";

   // write trailer to core file
   TailCore();

   return 1;
}
 
/*------------------------------------------------------------------------*/
/* stub for virtual function to invert profile                            */
/*------------------------------------------------------------------------*/
PRECISION Profile::Invert(PRECISION)
{
   message("error in Profile::Invert()...Noninvertible profile.\n");

   return ERROR;
}

/*------------------------------------------------------------------------*/
/* function to construct a file delimiter from a fingerprint              */
/*------------------------------------------------------------------------*/
char *Profile::make_delimiter(const char *fingerprint_,const char *delimiter)
{
   // reserve static room for delimiter
   static char hdr[MAXSTRLEN];

   // create delimiter in static buffer
   sprintf(hdr,"$%s::%s",fingerprint_,delimiter);

   // return the pointer 
   return hdr;

   #undef MAXSTRLEN
}

/*------------------------------------------------------------------------*/
/* function to parse a core file                                          */
/*------------------------------------------------------------------------*/
int Profile::ReadCore(istream &source)
{
   char *loc,*key;
   int len;

   // make sure Profile class identifier was found
   key = "Profile::"; if (!(loc = strloc(source,key,NULL))) return 0;

   // the fingerprint comes next but skip that and extract xmin and xmax
   key = "PRECISION (xmin,xmax) = ("; len=strlen(key);
   if (!(loc = strloc(source,key,NULL))) swifterr("error in Profile::ReadCore()...key \"%s\" not found.\n",key);
   else {loc+=len; xmin=atof(strtok(loc,",")); xmax=atof(strtok(NULL,")"));}

   // extract ymin and ymax
   key = "PRECISION (ymin,ymax) = ("; len=strlen(key);
   if (!(loc = strloc(source,key,NULL))) swifterr("error in Profile::ReadCore()...key \"%s\" not found.\n",key);
   else {loc+=len; ymin=atof(strtok(loc,",")); ymax=atof(strtok(NULL,")"));}

   return 1;
}

#endif // PROFILE_CPP
