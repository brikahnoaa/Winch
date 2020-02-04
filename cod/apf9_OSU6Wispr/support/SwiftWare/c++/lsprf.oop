#ifndef LSPRF_OOP
#define LSPRF_OOP

#include <profile.oop>
#include <Vec.oop>
#include <oddnend.p>

#if (DOUBLE)
   #define PRECISION      double
   #define LSplineProfile dLSplineProfile
   #define Profile        dProfile
   #define GnuVecType     doubleAVec
   #include <double.AVec.h>
#else
   #define PRECISION      float
   #define LSplineProfile fLSplineProfile
   #define Profile        fProfile
   #define GnuVecType     floatAVec
   #include <float.AVec.h>
#endif

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * $Id: lsprf.cpp,v 1.1 2006/11/18 17:22:31 swift Exp $
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * RCS Log:
 *
 * $Log: lsprf.cpp,v $
 * Revision 1.1  2006/11/18 17:22:31  swift
 * Added new functionality to library.
 *
 * Revision 1.19  2001/05/13 10:40:07  navo
 * Modifications for porting from UW to NAVO.
 *
 * Revision 1.18  1999/01/20 16:31:35  swift
 * Modifications to arguments of `findbin()' function.
 *
 * Revision 1.17  1999/01/16 19:15:44  swift
 * Added virtual destructor to satisfy an updated C++ compiler.
 *
 * Revision 1.16  1997/10/26 18:47:01  swift
 * Addition of copy constructor and assignment operator.
 *
 * Revision 1.15  1997/01/05 17:08:06  swift
 * Added a few comments.
 *
 * Revision 1.14  1997/01/04 15:01:55  swift
 * Moved `#include <crecipes.p>' out of header section & into code
 * section to circumvent conflicts between crecipes `vector' and
 * STL `vector'.
 *
 * Revision 1.13  1997/01/01 17:36:57  swift
 * Add a feature to auto-resize the arrays during append operations.
 * Fixed a bug in the `Resize()' function so that new array elements
 * are initialized.
 *
 * Revision 1.12  1996/10/19 16:31:49  swift
 * Added a function to set the number of points.
 *
 * Revision 1.11  1996/06/14 15:25:14  swift
 * Modifications to eliminate compiler warnings with gcc -Wall ...
 *
 * Revision 1.10  1996/04/11 15:54:31  swift
 * Modified preprocessor directive that selects for double or single precision.
 *
 * Revision 1.9  1996/01/01 22:21:22  swift
 * Merge header & source files.
 *
 * Revision 1.9  1995/12/29 00:55:18  swift
 * Added function to locate the bin to which an abscissa
 * belongs.
 *
 * Revision 1.8  1995/10/05 22:17:28  swift
 * Add function to integrate profile with trapezoidal rule.
 *
 * Revision 1.7  1995/09/06 19:41:43  swift
 * Add "Get" and "Append" functions for profile management.
 *
 * Revision 1.6  1995/08/30 21:55:32  swift
 * Added a "size" variable.
 *
 * Revision 1.5  1995/08/11 18:46:49  swift
 * Fixed a bug caused by uninitialized data members.
 *
 * Revision 1.4  1995/08/10 17:02:23  swift
 * Renamed public functions so that first letter is capitalized.
 *
 * Revision 1.3  1995/08/10 16:03:36  swift
 * Altered implementation to use g++lib's AVec objects in place of
 * dynamically allocated (x,y) arrays.  The AVec objects allow
 * a much wider range of services.
 *
 * Revision 1.2  1995/08/02 17:32:26  swift
 * Added inline function to return size of allocated arrays.
 *
 * Revision 1.1  1995/06/30 17:52:08  swift
 * Initial revision
 *
 *========================================================================*/
/* Linear Spline Profile class declaration                                */
/*========================================================================*/
/*
   This class implements a linear spline connecting a finite & discrete set
   of ordered points. 

   written by Dana Swift
*/   
class LSplineProfile: public Profile
{
   // member data
   private:

      // define profile fingerprint
      static char const *fingerprint;

   protected:

      // define integers current index location
      unsigned ndx;

      // variable to hold size of arrays
      unsigned size;

      // define integer to record number of data points in arrays
      unsigned npnt;

   public:

      // define pointers to vector objects
      Vec<PRECISION,GnuVecType> x,y;

   // member functions
   protected:

      // function to copy data members
      inline virtual void copy(const LSplineProfile &lsprf);
      
   public:

      // constructor to dynamically allocate array space for x & y arrays
      LSplineProfile(int nalloc=100,PRECISION initval=0);

      // copy constructor
      LSplineProfile(const LSplineProfile &lsprf);

      // virtual destructor
      virtual ~LSplineProfile(void) {}
      
      // assignment operator
      LSplineProfile &operator=(const LSplineProfile &lsprf);
      
      // operator to return n(th) knot of linear spline
      Point operator[](unsigned n) {Point p={x[n],y[n],0}; return p;}

      
      // function to append a point to profile
      int Append(PRECISION x,PRECISION y);

      // diagnostic procedure ...reports data member values
      int DumpCore(ostream &dest=cout,const char *id="",const char *prefix="");

      // profile evaluator function...approximates function with linear splines
      PRECISION Eval(PRECISION xtrg);

      // function to fill x,y vectors with an initial value
      void Fill(PRECISION value,unsigned first=0,unsigned last=0);

      // function to retrieve a point from the profile
      int Get(unsigned n,PRECISION *x,PRECISION *y);

      // function to integrate the profile
      PRECISION Integrate(PRECISION a=ERROR,PRECISION b=ERROR);

      // function to invert profile
      PRECISION Invert(PRECISION ytrg);

      // function to learn profile from external x & y arrays
      virtual int Learn(const PRECISION *x,const PRECISION *y,unsigned npnt);
 
      // function to return ndx
      unsigned int &Ndx(void) {return ndx;}

      // function to return npnt
      unsigned int &NPnt(void) {return npnt;}
      void NPnt(unsigned npnt) {this->npnt=npnt;}
      
      // function to resize the vectors
      void Resize(unsigned size=0,PRECISION initval=0);

      // procedure to read puke file for data member values
      int ReadCore(istream &source);

      // function to seek a point
      int Seek(PRECISION x,PRECISION *y);

      // function to seek a point
      int Seek(PRECISION *x,PRECISION y);

      // procedures to seek and set extreme values of abscissa and ordinate
      void SeekExtremes(void);

      // function to return allocation size
      unsigned Size(void) {return size;}

      // function to determine the bin index of an abscissa
      unsigned XBin(PRECISION xo);
};

/*------------------------------------------------------------------------*/
/* constructor to initialize (x,y) arrays and data members                */
/*------------------------------------------------------------------------*/
inline LSplineProfile::LSplineProfile(int nalloc, PRECISION initval)
{
   ndx=npnt=0;

   // size the arrays
   Resize(nalloc); 

   // initialize the arrays
   if (Size()) Fill(initval);
}

/*------------------------------------------------------------------------*/
/* copy constructor                                                       */
/*------------------------------------------------------------------------*/
inline LSplineProfile::LSplineProfile(const LSplineProfile &lsprf): Profile(lsprf)
{
   copy(lsprf);
}

/*------------------------------------------------------------------------*/
/* assignment operator                                                    */
/*------------------------------------------------------------------------*/
inline LSplineProfile &LSplineProfile::operator=(const LSplineProfile &lsprf)
{
   copy(lsprf); return *this;
}

/*------------------------------------------------------------------------*/
/* function to copy data members                                          */
/*------------------------------------------------------------------------*/
inline void LSplineProfile::copy(const LSplineProfile &lsprf)
{
   Profile::copy(lsprf);

   ndx=lsprf.ndx; size=lsprf.size; npnt=lsprf.npnt; x=lsprf.x; y=lsprf.y;
}

#endif // LSPRF_OOP
#ifdef LSPRF_CPP

#include <crecipes.p>
#include <limits.h>

// initialize fingerprint for the LSplineProfile class
const char *LSplineProfile::fingerprint="LSplineProfile";

/*------------------------------------------------------------------------*/
/* function to append point to profile                                    */
/*------------------------------------------------------------------------*/
int LSplineProfile::Append(PRECISION x, PRECISION y)
{
   // get index of last point & increment counter
   unsigned n = NPnt()++;

   // check if auto-resizing is necessary
   if (NPnt()>=Size()) Resize();

   // add the point to the profile
   LSplineProfile::x[n]=x; LSplineProfile::y[n]=y; 

   return NPnt();
}

/*------------------------------------------------------------------------*/
/* procedure to print out data member values                              */
/*------------------------------------------------------------------------*/
int LSplineProfile::DumpCore(ostream &dest,const char *id,const char *prefix)
{
   // write header to core file
   HeadCore(dest,id,prefix,fingerprint,IOSFLAGS|ios::scientific);

   // find the number of significant decimal digits
   int precision = -(int)log10(MACHINE_EPS);

   // output base class diagnostics
   Profile::DumpCore(dest,id,indent);

   // print values of class variables
   dest << indent << "unsigned ndx = " << ndx << endl;
   dest << indent << "unsigned npnt = " << npnt << endl;
   dest << indent << "unsigned size = " << size << endl;

   dest << indent << "Vec (index:(x,y)) =\n" << indent << "{\n";

   // print data to file
   for (unsigned i=0; i<npnt && i<(unsigned)x.capacity(); i++)
   {
      dest << indent << "   " << setw(4) << i << ":("
      << setw(precision+8) << setprecision(precision) << x[i] << ", "
      << setw(precision+8) << setprecision(precision) << y[i] << ")\n";
   }

   // close block with a brace
   dest << indent << "}\n";

   // write trailer to core file
   TailCore();

   return 1;
}

/*------------------------------------------------------------------------*/
/* function to fill x,y vectors with an initial value                     */
/*------------------------------------------------------------------------*/
void LSplineProfile::Fill(PRECISION value,unsigned first,unsigned last)
{
   /* compute how many elements are to be initialized */
   unsigned n = (last<first) ? Size()-first+1 : last-first+1;
   
   x.fill(value,first,n); y.fill(value,first,n);
}

/*------------------------------------------------------------------------*/
/* function to get the n(th) point from the profile                       */
/*------------------------------------------------------------------------*/
int LSplineProfile::Get(unsigned n,PRECISION *x,PRECISION *y)
{
   // check to see if profile is overindexed
   if (n>=NPnt()) {*x=ERROR; *y=ERROR; return 0;}

   // get the point from the profile
   *x = LSplineProfile::x[n]; *y = LSplineProfile::y[n];

   return 1;
} 

/*------------------------------------------------------------------------*/
/* function to learn profile from external arrays                         */
/*------------------------------------------------------------------------*/
int LSplineProfile::Learn(const PRECISION *x,const PRECISION *y,unsigned npnt)
{
   // initialize ndx
   ndx=0;

   // initialize npnt
   this->npnt=npnt;

   // make sure we have enough space allocated
   if (Size()<npnt) Resize(npnt);

   // copy external array values to internal arrays
   memcpy(this->x.s,x,npnt*sizeof(PRECISION));
   memcpy(this->y.s,y,npnt*sizeof(PRECISION));
   
   // find the extremes in the arrays
   SeekExtremes();

   return 1;
}

/*------------------------------------------------------------------------*/
/* function to compute ordinate value at target abscissa                  */
/*------------------------------------------------------------------------*/
PRECISION LSplineProfile::Eval(PRECISION xtrg)
{
   PRECISION ytrg;
   long k=ndx;

   /* find bracketing abscissas warn user if bracket not found */
   if (findbin(x.s,npnt,xtrg,&k)) ndx=k;
   else
   {
      // scold the user for going out-of-bounds
      fprintf(swiftware_stderr,"Warning in LSplineProfile::Eval()..."
              "target value (%g) out-of-range: [%g,%g].\n",xtrg,xmin,xmax);
   }
   
   /* evalutate linear spline between bracketing points */
   ytrg = ((ndx+1)<npnt && x[ndx+1]!=x[ndx]) ? 
          (y[ndx+1]-y[ndx])*(xtrg-x[ndx])/(x[ndx+1]-x[ndx]) + y[ndx] : y[ndx];

   return ytrg;
}

/*------------------------------------------------------------------------*/
/* function to compute abscissa value at target ordinate                  */
/*------------------------------------------------------------------------*/
PRECISION LSplineProfile::Invert(PRECISION ytrg)
{
   PRECISION xtrg;
   long k=ndx;

   /* find bracketing abscissas warn user if bracket not found */
   if (findbin(y.s,npnt,ytrg,&k)) ndx=k;
   else
   {
      // scold the user for going out-of-bounds
      fprintf(swiftware_stderr,"Warning in LSplineProfile::invert()...target value (%g) out-of-range: [%g,%g].\n",ytrg,ymin,ymax); 
   }
 
   /* evalutate linear spline between bracketing points */
   xtrg = ((ndx+1)<npnt && y[ndx+1]!=y[ndx]) ? 
          (x[ndx+1]-x[ndx])*(ytrg-y[ndx])/(y[ndx+1]-y[ndx]) + x[ndx] : x[ndx];

   return xtrg;
}

/*------------------------------------------------------------------------*/
/* function to integrate a profile using trapezoidal method               */
/*------------------------------------------------------------------------*/
PRECISION LSplineProfile::Integrate(PRECISION a,PRECISION b)
{
   unsigned i,j; PRECISION f;

   // find the extremes in the profile
   SeekExtremes();

   // check for sentinel values of a,b
   if (a==ERROR) a=xmin; if (b==ERROR) b=xmax;

   // check if a is in range
   if (!inRange(xmin,a,xmax)&&a!=xmax) 
   {
      swifterr("error in LSplineProfile::Integrate()..."
               "integral limit (a=%g) out-of-range.\n",a);
   }
   
   // check if b is in range
   if (!inRange(xmin,b,xmax)&&b!=xmax) 
   {
      swifterr("error in LSplineProfile::Integrate()..."
               "integral limit (b=%g) out-of-range.\n",b);
   }

   // position index just below the range of integration
   for (j=0; x[j]<b && x[j]<a; j++) {};

   // compute the integral from xmin to b
   for (f=0,i=j; x[i+1]<b && (i+1)<npnt; i++)
   {
      f += (y[i+1]+y[i])*(x[i+1]-x[i])/2;
   }  f += (Eval(b)+y[i])*(b-x[i])/2;

   // compute the integral from xmin to a
   for (i=j; x[i+1]<a && (i+1)<npnt; i++)
   {
      f -= (y[i+1]+y[i])*(x[i+1]-x[i])/2;
   }  f -= (Eval(a)+y[i])*(a-x[i])/2;
   
   return f;
}

/*------------------------------------------------------------------------*/
/* procedure to parse puke file for data member values                    */
/*------------------------------------------------------------------------*/
int LSplineProfile::ReadCore(istream &source)
{
   char *loc,*key;
   unsigned len;

   // make sure Station class identifier was found
   key = "LSplineProfile::"; if (!(loc = strloc(source,key,NULL))) return 0;

   // extract Profile base class data member values
   Profile::ReadCore(source);

   // the allocation size comes next but let's skip that and extract the index
   key = "unsigned ndx ="; len = strlen(key);
   if (!(loc = strloc(source,key,NULL))) swifterr("error in LSplineProfile::ReadCore()...key \"%s\" not found.\n",key);
   loc += len; ndx = atoi(loc);

   // extract number of data points
   key = "unsigned npnt ="; len = strlen(key);
   if (!(loc = strloc(source,key,NULL))) swifterr("error in LSplineProfile::ReadCore()...key \"%s\" not found.\n",key);
   loc += len; unsigned n = atoi(loc);

   // make sure we have enough room for the arrays BEFORE assigning npnt
   if (n>Size()) {npnt=0; Resize(n);}

   // initialize class variable "npnt"
   npnt=n;

   for (unsigned i=0; i<npnt; i++)
   {
      char key[25]; sprintf(key,"%u:",i);

      // position file pointer to proper array index
      if (!(loc = strloc(source,key,NULL))) swifterr("error in LSplineProfile::ReadCore()...index(%s) not found.\n",key);

      // extract x[i] & y[i] from puke file
      strtok(loc,"("); x[i] = atof(strtok(NULL,",")); y[i] = atof(strtok(NULL,")"));
   }
   
   return 1;
}

/*------------------------------------------------------------------------*/
/* function to resize the vectors                                         */
/*------------------------------------------------------------------------*/
void LSplineProfile::Resize(unsigned size,PRECISION initval)
{
   // check for auto-resize request
   if (!size) size = (this->size) ? 2*this->size : 100;

   // resize the vectors
   x.resize(size); y.resize(size);

   // initialize the new elements
   if (size>this->size) Fill(initval,this->size,size-1);

   // record the new size of the arrays
   this->size=size;
}

/*------------------------------------------------------------------------*/
/* function to seek a point                                               */
/*------------------------------------------------------------------------*/
int LSplineProfile::Seek(PRECISION *x,PRECISION y)
{
   // initialize return value
   if (x) *x = ERROR;

   for (unsigned i=0; i<NPnt(); i++)
   {
      // get coordinates for current point
      PRECISION X=LSplineProfile::x[i],Y=LSplineProfile::y[i];

      // check if current point is found
      if (inRange(-fEPS,Y-y,fEPS)) {if (x) (*x)=X; return i;}
   }
   
   return ERROR;
}

/*------------------------------------------------------------------------*/
/* function to seek a point                                               */
/*------------------------------------------------------------------------*/
int LSplineProfile::Seek(PRECISION x,PRECISION *y)
{
   // initialize return value
   if (y) *y = ERROR;

   for (unsigned i=0; i<NPnt(); i++)
   {
      // get coordinates for current point
      PRECISION X=LSplineProfile::x[i],Y=LSplineProfile::y[i];

      // check if current point is found
      if (inRange(-fEPS,X-x,fEPS)) {if (y) (*y)=Y; return i;}
   }
   
   return ERROR;
}

/*------------------------------------------------------------------------*/
/* function to seek extremes in abscissa and ordinates                    */
/*------------------------------------------------------------------------*/
void LSplineProfile::SeekExtremes(void)
{
   // check for no points
   if (!npnt) {xmin=xmax=ymin=ymax=ERROR; return;}

   // initialize extreme values
   xmin=xmax=x[0]; ymin=ymax=y[0];

   // search for extremes
   for (unsigned i=1; i<npnt; i++)
   {
      if (xmin>x[i]) xmin=x[i];
      if (xmax<x[i]) xmax=x[i];
      if (ymin>y[i]) ymin=y[i];
      if (ymax<y[i]) ymax=y[i];
   }
}

/*------------------------------------------------------------------------*/
/* function to determine the bin index of an abscissa                     */
/*------------------------------------------------------------------------*/
unsigned LSplineProfile::XBin(PRECISION xo)
{
   // make sure profile exists
   if (!NPnt()) return UINT_MAX;

   // fix initial guess
   long unsigned k=(Ndx()<NPnt()) ? Ndx()+1 : 1; 

   // use c-recipes function to find bracketing points (note 1-offset convention)
   hunt(x.s-1,NPnt(),xo,&k); k--;

   // check if target point is out of range
   if (k>=NPnt()) k=UINT_MAX; else Ndx()=k;

   return k;
}

#endif // LSPRF_CPP
