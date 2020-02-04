#ifndef VEC_OOP
#define VEC_OOP 

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * $Id
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * RCS Log:
 *
 * $Log: Vec.cpp,v $
 * Revision 1.1  2001/06/02 03:15:57  navo
 * Initial revision
 *
 * Revision 1.6  1999/01/16 19:21:15  swift
 * Fixed assignment operator.
 *
 * Revision 1.5  1997/10/26 18:49:04  swift
 * Addition of assignment operator.
 *
 * Revision 1.4  1997/03/01 17:18:14  swift
 * Change constructor argument from reference to value.
 *
 * Revision 1.3  1995/08/10 17:55:14  swift
 * Add preprocessor directives to avoid multiple definitions
 * of template class.
 *
 * Revision 1.2  1995/08/08 22:22:08  swift
 * Recommented the source code to reflect shift to a template class
 * from an explicit class.
 *
 * Revision 1.1  1995/08/08 22:14:57  swift
 * Initial revision
 *
 *========================================================================*/
/* class to coerce libg++ Vec classes to reveal protected arrays          */
/*========================================================================*/
/*
   This class is a wrapper for the libg++ Vec classes.  Its sole purpose is
   to coerce the base class (<TYPE>Vec) into promoting its internal array from
   protected to public.  This is accomplished using a reference to a <TYPE>
   pointer. 

   The base class (<TYPE>Vec) is created using the (genclass) script according
   to the commands:
      genclass TYPE ref Vec
      genclass TYPE ref defs
   where TYPE is one of char, int, float, double, etc. and the second
   command creates a header file (<TYPE>.defs.h) only.  The <TYPE>.Vec.cc 
   file must be compiled under gcc with the -DUSE_LIBGXX_INLINES
   option turned on.  I learned (empirically) that the file must have a (cc)
   suffix rather than my own preferred (cpp) suffix.

   written by Dana Swift 7/17/95
*/
template <class FType,class GnuVecType> class Vec: public GnuVecType
{
   public:

      // reference to hidden data
      FType *const &s;

      // constructors
      Vec(void): s(GnuVecType::s) {};
      Vec(int l): GnuVecType(l),s(GnuVecType::s) {};
      Vec(int l,FType fill_value): GnuVecType(l,fill_value),s(GnuVecType::s) {};

      // copy constructor
      Vec(GnuVecType &v): GnuVecType(v),s(GnuVecType::s) {};

      // assignment operator
      Vec<FType,GnuVecType> &operator=(const Vec<FType,GnuVecType> &vec)
      {
         // make the vectors conformant & copy
         resize(vec.capacity()); GnuVecType::operator=(vec);

         return *this;
      }
};

#endif // VEC_OOP
