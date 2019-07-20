#ifndef TRACE_H
#define TRACE_H

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * $Id: trace.c,v 1.6 2007/04/24 01:43:05 swift Exp $
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
/** RCS log of revisions to the C source code.
 *
 * \begin{verbatim}
 * $Log: trace.c,v $
 * Revision 1.6  2007/04/24 01:43:05  swift
 * Added acknowledgement and funding attribution.
 *
 * Revision 1.5  2006/04/21 13:45:38  swift
 * Changed copyright attribute.
 *
 * Revision 1.4  2003/11/20 18:59:35  swift
 * Added GNU Lesser General Public License to source file.
 *
 * Revision 1.3  2003/06/11 18:43:35  swift
 * Modifications to use doprnt() rather than fprintf() so that
 * trace() and assert() can be used without generating infinite
 * recursice loops.
 *
 * Revision 1.2  2003/06/09 23:54:43  swift
 * Modifications to stack-related variables.
 *
 * Revision 1.1  2003/06/07 20:29:30  swift
 * Initial revision
 *
 * \end{verbatim}
*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
#define traceChangeLog "$RCSfile: trace.c,v $ $Revision: 1.6 $ $Date: 2007/04/24 01:43:05 $"

/* remove existing definition of trace() */
#undef trace

#ifndef TRACE

   /* define the benign form of the trace macro */
   #define trace(func,msg) ((void)0)

#else

   /* declare the prototype for the function that handles traces */
   void ftrace(const char *function_name, const char *msg);

   /* define the active version of the trace macro */    
   #define trace(func,msg) ftrace(func,msg);

#endif /* TRACE */
#endif /* TRACE_H */
