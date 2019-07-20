// This may look like C code, but it is really -*- C++ -*-
/* 
Copyright (C) 1988 Free Software Foundation
    written by Doug Lea (dl@rocky.oswego.edu)

This file is part of the GNU C++ Library.  This library is free
software; you can redistribute it and/or modify it under the terms of
the GNU Library General Public License as published by the Free
Software Foundation; either version 2 of the License, or (at your
option) any later version.  This library is distributed in the hope
that it will be useful, but WITHOUT ANY WARRANTY; without even the
implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the GNU Library General Public License for more details.
You should have received a copy of the GNU Library General Public
License along with this library; if not, write to the Free Software
Foundation, 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/


#ifndef _floatAVec_h
#ifdef __GNUG__
#pragma interface
#endif
#define _floatAVec_h 1

#include "float.Vec.h"

class floatAVec : public floatVec
{
protected:
  void                  check_len(int l);
  float*                  vec();
  const float*		vec() const;
                        floatAVec(int l, float* d);
  public:
                        floatAVec ();
                        floatAVec (int l);
                        floatAVec (int l, float  fill_value);
                        floatAVec (floatAVec&);
                        ~floatAVec ();

  floatAVec&              operator =  (const floatAVec& a);
  floatAVec&              operator =  (float  fill_value);

// vector by scalar -> vector operations

  friend floatAVec        operator +  (floatAVec& a, float  b);
  friend floatAVec        operator -  (floatAVec& a, float  b);
  friend floatAVec        operator *  (floatAVec& a, float  b);
  friend floatAVec        operator /  (floatAVec& a, float  b);

  floatAVec&              operator += (float  b);
  floatAVec&              operator -= (float  b);
  floatAVec&              operator *= (float  b);
  floatAVec&              operator /= (float  b);

// vector by vector -> vector operations

  friend floatAVec        operator +  (floatAVec& a, floatAVec& b);
  friend floatAVec        operator -  (floatAVec& a, floatAVec& b);
  floatAVec&              operator += (floatAVec& b);
  floatAVec&              operator -= (floatAVec& b);

  floatAVec               operator - ();

  friend floatAVec        product(floatAVec& a, floatAVec& b);
  floatAVec&              product(floatAVec& b);
  friend floatAVec        quotient(floatAVec& a, floatAVec& b);
  floatAVec&              quotient(floatAVec& b);

// vector -> scalar operations

  friend float            operator * (floatAVec& a, floatAVec& b);

  float                   sum();
  float                   min();
  float                   max();
  float                   sumsq();

// indexing

  int                   min_index();
  int                   max_index();

// redundant but necesssary
  friend floatAVec        concat(floatAVec& a, floatAVec& b);
  friend floatAVec        mapper(floatMapper f, floatAVec& a);
  friend floatAVec        merge(floatAVec& a, floatAVec& b, floatComparator f);
  friend floatAVec        combine(floatCombiner f, floatAVec& a, floatAVec& b);
  friend floatAVec        reverse(floatAVec& a);
  floatAVec               at(int from = 0, int n = -1);
};

inline floatAVec::floatAVec() {}
inline floatAVec::floatAVec(int l) :floatVec(l) {}
inline floatAVec::floatAVec(int l, float  fill_value) : floatVec (l, fill_value) {}
inline floatAVec::floatAVec(floatAVec& v) :floatVec(v) {}
inline floatAVec::floatAVec(int l, float* d) :floatVec(l, d) {}
inline floatAVec::~floatAVec() {}


inline float* floatAVec::vec()
{
  return s;
}

inline const float* floatAVec::vec() const
{
  return s;
}

inline void floatAVec::check_len(int l)
{
  if (l != len)
    error("nonconformant vectors.");
}

#endif
