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

#ifdef __GNUG__
#pragma implementation
#endif
using namespace std;
#include <iostream>
#include <builtin.h>
#include "float.AVec.h"

/*
 The following brought to you by the department of redundancy department
*/

floatAVec& floatAVec::operator = (const floatAVec& v)
{
  if (len != 0 && len != v.capacity())
    error("nonconformant vectors.");
  if (len == 0)
    s = new float [len = v.capacity()];
  if (s != v.vec())
  {
    for (int i = 0; i < len; ++i)
      s[i] = v.vec()[i];
  }
  return *this;
}

floatAVec& floatAVec::operator = (float  f)
{
  for (int i = 0; i < len; ++i) s[i] = f;
  return *this;
}


floatAVec concat(floatAVec & a, floatAVec & b)
{
  int newl = a.capacity() + b.capacity();
  float* news = new float [newl];
  float* p = news;
  float* top = &(a.vec()[a.capacity()]);
  float* t = a.vec();
  while (t < top) *p++ = *t++;
  top = &(b.vec()[b.capacity()]);
  t = b.vec();
  while (t < top) *p++ = *t++;
  floatAVec temp(newl, news);
  return temp;
}


floatAVec combine(floatCombiner f, floatAVec& a, floatAVec& b)
{
  int newl = (a.capacity() < b.capacity())? a.capacity() : b.capacity();
  float* news = new float [newl];
  float* p = news;
  float* top = &(a.vec()[newl]);
  float* t = a.vec();
  float* u = b.vec();
  while (t < top) *p++ = (*f)(*t++, *u++);
  floatAVec temp(newl, news);
  return temp;
}

floatAVec reverse(floatAVec& a)
{
  float* news = new float [a.capacity()];
  if (a.capacity() != 0)
  {
    float* lo = news;
    float* hi = &(news[a.capacity() - 1]);
    while (lo < hi)
    {
      float tmp = *lo;
      *lo++ = *hi;
      *hi-- = tmp;
    }
  }
  floatAVec reverse(a.capacity(), news);
  return reverse;
}

floatAVec mapper(floatMapper f, floatAVec& a)
{
  float* news = new float [a.capacity()];
  float* p = news;
  float* top = &(a.vec()[a.capacity()]);
  float* t = a.vec();
  while(t < top) *p++ = (*f)(*t++);
  floatAVec mapper(a.capacity(), news);
  return mapper;
}

floatAVec floatAVec::at(int from, int n)
{
  int to;
  if (n < 0)
  {
    n = len - from;
    to = len - 1;
  }
  else
    to = from + n - 1;
  if ((unsigned)from > (unsigned)to)
    range_error();
  float* news = new float [n];
  float* p = news;
  float* t = &(s[from]);
  float* top = &(s[to]);
  while (t <= top) *p++ = *t++;
  floatAVec at(n, news);
  return at;
}

floatAVec merge(floatAVec & a, floatAVec & b, floatComparator f)
{
  int newl = a.capacity() + b.capacity();
  float* news = new float [newl];
  float* p = news;
  float* topa = &(a.vec()[a.capacity()]);
  float* as = a.vec();
  float* topb = &(b.vec()[b.capacity()]);
  float* bs = b.vec();

  for (;;)
  {
    if (as >= topa)
    {
      while (bs < topb) *p++ = *bs++;
      break;
    }
    else if (bs >= topb)
    {
      while (as < topa) *p++ = *as++;
      break;
    }
    else if ((*f)(*as, *bs) <= 0)
      *p++ = *as++;
    else
      *p++ = *bs++;
  }
  floatAVec temp(newl, news);
  return temp;
}

floatAVec operator + (floatAVec& a, floatAVec& b)
{
  a.check_len(b.capacity());
  float* news = new float [a.capacity()];
  float* p = news;
  float* top = &(a.vec()[a.capacity()]);
  float* t = a.vec();
  float* u = b.vec();
  while (t < top) *p++ = *t++ + *u++;
  floatAVec plus(a.capacity(), news);
  return plus;
}

floatAVec operator - (floatAVec& a, floatAVec& b)
{
  a.check_len(b.capacity());
  float* news = new float [a.capacity()];
  float* p = news;
  float* top = &(a.vec()[a.capacity()]);
  float* t = a.vec();
  float* u = b.vec();
  while (t < top) *p++ = *t++ - *u++;
  floatAVec minus(a.capacity(), news);
  return minus;
}

floatAVec  product (floatAVec& a, floatAVec& b)
{
  a.check_len(b.capacity());
  float* news = new float [a.capacity()];
  float* p = news;
  float* top = &(a.vec()[a.capacity()]);
  float* t = a.vec();
  float* u = b.vec();
  while (t < top) *p++ = *t++ * *u++;
  floatAVec product(a.capacity(), news);
  return product;
}

floatAVec quotient(floatAVec& a, floatAVec& b)
{
  a.check_len(b.capacity());
  float* news = new float [a.capacity()];
  float* p = news;
  float* top = &(a.vec()[a.capacity()]);
  float* t = a.vec();
  float* u = b.vec();
  while (t < top) *p++ = *t++ / *u++;
  floatAVec quotient(a.capacity(), news);
  return quotient;
}

floatAVec operator + (floatAVec& a, float  b)
{
  float* news = new float [a.capacity()];
  float* p = news;
  float* top = &(a.vec()[a.capacity()]);
  float* t = a.vec();
  while (t < top) *p++ = *t++ + b;
  floatAVec plus(a.capacity(), news);
  return plus;
}

floatAVec operator - (floatAVec& a, float  b)
{
  float* news = new float [a.capacity()];
  float* p = news;
  float* top = &(a.vec()[a.capacity()]);
  float* t = a.vec();
  while (t < top) *p++ = *t++ - b;
  floatAVec minus(a.capacity(), news);
  return minus;
}

floatAVec operator * (floatAVec& a, float  b)
{
  float* news = new float [a.capacity()];
  float* p = news;
  float* top = &(a.vec()[a.capacity()]);
  float* t = a.vec();
  while (t < top) *p++ = *t++ * b;
  floatAVec minus(a.capacity(), news);
  return minus;
}

floatAVec operator / (floatAVec& a, float  b)
{
  float* news = new float [a.capacity()];
  float* p = news;
  float* top = &(a.vec()[a.capacity()]);
  float* t = a.vec();
  while (t < top) *p++ = *t++ / b;
  floatAVec divide(a.capacity(), news);
  return divide;
}

floatAVec floatAVec::operator - ()
{
  float* news = new float [len];
  float* p = news;
  float* top = &(s[len]);
  float* t = s;
  while (t < top) *p++ = -(*t++);
  floatAVec minus(len, news);
  return minus;
}

floatAVec& floatAVec::operator += (floatAVec& b)
{
  check_len(b.capacity());
  float* u = b.vec();
  float* top = &(s[len]);
  float* t = s;
  while (t < top) *t++ += *u++;
  return *this;
}

floatAVec& floatAVec::operator -= (floatAVec& b)
{
  check_len(b.capacity());
  float* u = b.vec();
  float* top = &(s[len]);
  float* t = s;
  while (t < top) *t++ -= *u++;
  return *this;
}

floatAVec& floatAVec::product(floatAVec& b)
{
  check_len(b.capacity());
  float* u = b.vec();
  float* top = &(s[len]);
  float* t = s;
  while (t < top) *t++ *= *u++;
  return *this;
}

floatAVec& floatAVec::quotient(floatAVec& b)
{
  check_len(b.capacity());
  float* u = b.vec();
  float* top = &(s[len]);
  float* t = s;
  while (t < top) *t++ /= *u++;
  return *this;
}

floatAVec& floatAVec::operator += (float  b)
{
  float* top = &(s[len]);
  float* t = s;
  while (t < top) *t++ += b;
  return *this;
}

floatAVec& floatAVec::operator -= (float  b)
{
  float* top = &(s[len]);
  float* t = s;
  while (t < top) *t++ -= b;
  return *this;
}

floatAVec& floatAVec::operator *= (float  b)
{
  float* top = &(s[len]);
  float* t = s;
  while (t < top) *t++ *= b;
  return *this;
}

floatAVec& floatAVec::operator /= (float  b)
{
  float* top = &(s[len]);
  float* t = s;
  while (t < top) *t++ /= b;
  return *this;
}

float floatAVec::max()
{
  if (len == 0)
    return 0;
  float* top = &(s[len]);
  float* t = s;
  float res = *t++;
  for (; t < top; ++t) if (*t > res) res = *t;
  return res;
}

int floatAVec::max_index()
{
  if (len == 0)
    return -1;
  int ind = 0;
  for (int i = 1; i < len; ++i)
    if (s[i] > s[ind])
      ind = i;
  return ind;
}

float floatAVec::min()
{
  if (len == 0)
    return 0;
  float* top = &(s[len]);
  float* t = s;
  float res = *t++;
  for (; t < top; ++t) if (*t < res) res = *t;
  return res;
}

int floatAVec::min_index()
{
  if (len == 0)
    return -1;
  int ind = 0;
  for (int i = 1; i < len; ++i)
    if (s[i] < s[ind])
      ind = i;
  return ind;
}

float floatAVec::sum()
{
  float res = 0;
  float* top = &(s[len]);
  float* t = s;
  while (t < top) res += *t++;
  return res;
}


float floatAVec::sumsq()
{
  float res = 0;
  float* top = &(s[len]);
  float* t = s;
  for (; t < top; ++t) res += *t * *t;
  return res;
}

float operator * (floatAVec& a, floatAVec& b)
{
  a.check_len(b.capacity());
  float* top = &(a.vec()[a.capacity()]);
  float* t = a.vec();
  float* u = b.vec();
  float res = 0;
  while (t < top) res += *t++ * *u++;
  return res;
}
