#ifndef ASSERT_H
#define ASSERT_H

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * $Id: assert.c,v 1.7 2007/04/24 01:43:05 swift Exp $
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/* Copyright University of Washington.   Written by Dana Swift.
 *
 * This software was developed at the University of Washington using funds
 * generously provided by the US Office of Naval Research, the National
 * Science Foundation, and NOAA.
 *  
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 2.1 of the License, or (at
 * your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser
 * General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */
/**
   This translation unit is a drop-in replacement for the assertion
   mechanism specified in the ANSI/ISO standard and its behavior is
   conformant with the ANSI/ISO standard.
 *  
 * RCS log of revisions to the C source code.
 *
 * \begin{verbatim}
 * $Log: assert.c,v $
 * Revision 1.7  2007/04/24 01:43:05  swift
 * Added acknowledgement and funding attribution.
 *
 * Revision 1.6  2006/04/21 13:45:38  swift
 * Changed copyright attribute.
 *
 * Revision 1.5  2003/11/20 18:59:35  swift
 * Added GNU Lesser General Public License to source file.
 *
 * Revision 1.4  2003/06/11 18:31:25  swift
 * Modifications to use doprnt() rather than fprintf() so that
 * trace() and assert() can be used without generating infinite
 * recursice loops.
 *
 * Revision 1.3  2003/06/07 20:43:26  swift
 * Modified fassert() to use stderr rather than stdout.
 *
 * Revision 1.2  2003/05/30 15:29:25  swift
 * Modifications for compliancy with the ANSI/ISO standard.
 *
 * Revision 1.1  2003/05/29 23:27:37  swift
 * Initial revision
 * \end{verbatim}
*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
#define AssertChangeLog "$RCSfile: assert.c,v $  $Revision: 1.7 $   $Date: 2007/04/24 01:43:05 $"

/* remove existing definition of assert() */
#undef assert

#ifdef NDEBUG

   /* define the benign form of the assert macro */
   #define assert(expr) ((void)0)

#else

   /* declare the prototype for the function that handles assertion failures */
   void _fassert(int line, const char *file, const char *expr);

   /* define a string-izing function */
   #ifndef __mkstr__
      #define __mkstr__(x) #x
   #endif /* __mkstr__ */

   /* define the active version of the assert macro */    
   #define assert(expr) ((expr) ? (void)0 : _fassert(__LINE__, __FILE__, __mkstr__(expr)))

#endif /* NDEBUG */
#endif /* ASSERT_H */
