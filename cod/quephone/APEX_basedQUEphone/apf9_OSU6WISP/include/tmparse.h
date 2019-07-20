#ifndef TMPARSE_H
#define TMPARSE_H

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * $Id: tmparse.c,v 1.6 2007/04/24 01:43:29 swift Exp $
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Copyright University of Washington.   Written by Dana Swift.
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
 *
 * RCS Log:
 *
 * $Log: tmparse.c,v $
 * Revision 1.6  2007/04/24 01:43:29  swift
 * Added acknowledgement and funding attribution.
 *
 * Revision 1.5  2006/04/21 13:45:40  swift
 * Changed copyright attribute.
 *
 * Revision 1.4  2004/12/29 23:06:50  swift
 * Modified LogEntry() to use strings stored in the CODE segment.  This saves
 * lots of space in the DATA segment and significantly speeds code start-up.
 *
 * Revision 1.3  2003/11/20 18:59:42  swift
 * Added GNU Lesser General Public License to source file.
 *
 * Revision 1.2  2003/07/19 22:46:23  swift
 * Fixed a memory leak caused by neglecting to release the regex object.
 * Modified the regex to make the seconds field optional.
 *
 * Revision 1.1  2003/05/16 20:26:06  swift
 * Initial revision
 *
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
#define ParseTimeChangeLog "$RCSfile: tmparse.c,v $  $Revision: 1.6 $   $Date: 2007/04/24 01:43:29 $"

#include <time.h>

/* function prototype */
int ParseTime(const char *timestr, time_t *sec);

#endif /* TMPARSE_H */
