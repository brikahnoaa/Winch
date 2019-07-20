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


#ifndef _floatVec_h
#ifdef __GNUG__
#pragma interface
#endif
#define _floatVec_h 1

#include <builtin.h>
#include "float.defs.h"

#ifndef _float_typedefs
#define _float_typedefs 1
typedef void (*floatProcedure)(float&);
typedef float  (*floatMapper)(float&);
typedef float  (*floatCombiner)(float&, float&);
typedef int  (*floatPredicate)(float&);
typedef int  (*floatComparator)(float&, float&);
#endif


class floatVec 
{
protected:      
  int                   len;
  float                   *s;                  

                        floatVec(int l, float* d);
public:
                        floatVec ();
                        floatVec (int l);
                        floatVec (int l, float fill_value);
                        floatVec (const floatVec&);
                        ~floatVec ();

  floatVec &              operator = (const floatVec & a);
  floatVec                at(int from = 0, int n = -1);

  int                   capacity() const;
  void                  resize(int newlen);                        

  float&                  operator [] (int n);
  float&                  elem(int n);

  friend floatVec         concat(floatVec & a, floatVec & b);
  friend floatVec         mapper(floatMapper f, floatVec & a);
  friend floatVec         merge(floatVec & a, floatVec & b, floatComparator f);
  friend floatVec         combine(floatCombiner f, floatVec & a, floatVec & b);
  friend floatVec         reverse(floatVec & a);

  void                  reverse();
  void                  sort(floatComparator f);
  void                  fill(float val, int from = 0, int n = -1);

  void                  apply(floatProcedure f);
  float                   reduce(floatCombiner f, float& base);
  int                   index(float& targ);

  friend int            operator == (floatVec& a, floatVec& b);
  friend int            operator != (floatVec& a, floatVec& b);

  void                  error(const char* msg);
  void                  range_error();
};

extern void default_floatVec_error_handler(const char*);
extern one_arg_error_handler_t floatVec_error_handler;

extern one_arg_error_handler_t 
        set_floatVec_error_handler(one_arg_error_handler_t f);


inline floatVec::floatVec()
{
  len = 0; s = 0;
}

inline floatVec::floatVec(int l)
{
  s = new float [len = l];
}


inline floatVec::floatVec(int l, float* d) :len(l), s(d) {}


inline floatVec::~floatVec()
{
  delete [] s;
}


inline float& floatVec::operator [] (int n)
{
  if ((unsigned)n >= (unsigned)len)
    range_error();
  return s[n];
}

inline float& floatVec::elem(int n)
{
  return s[n];
}


inline int floatVec::capacity() const
{
  return len;
}



inline int operator != (floatVec& a, floatVec& b)
{
  return !(a == b);
}

#endif
