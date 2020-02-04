#ifndef EEPROM_H
#define EEPROM_H (0x01f0U)

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * $Id: eeprom.c,v 1.7 2007/05/08 18:10:39 swift Exp $
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
 */
/** RCS log of revisions to the C source code.
 *
 * \begin{verbatim}
 * $Log: eeprom.c,v $
 * Revision 1.7  2007/05/08 18:10:39  swift
 * Added attribution just below the copyright in the main comment section.
 *
 * Revision 1.6  2006/10/11 20:59:51  swift
 * Integrated the new flashio file system.
 *
 * Revision 1.5  2006/04/21 13:45:42  swift
 * Changed copyright attribute.
 *
 * Revision 1.4  2004/12/29 23:11:27  swift
 * Modified LogEntry() to use strings stored in the CODE segment.  This saves
 * lots of space in the DATA segment and significantly speeds code start-up.
 *
 * Revision 1.3  2004/02/05 23:55:27  swift
 * Cleaved basic eeprom functionality into a library API.
 *
 * Revision 1.2  2003/11/20 18:59:47  swift
 * Added GNU Lesser General Public License to source file.
 *
 * Revision 1.1  2003/09/10 17:50:56  swift
 * Initial revision
 * \end{verbatim}
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
#define eepromChangeLog "$RCSfile: eeprom.c,v $ $Revision: 1.7 $ $Date: 2007/05/08 18:10:39 $"

#include <control.h>

/* function prototypes */
struct MissionParameters *MissionParametersRead(struct MissionParameters *mission);
int          MissionParametersWrite(struct MissionParameters *mission);
int          PrfIdGet(void);
int          PrfIdSet(unsigned int pid);
int          PrfIdIncrement(void);
enum State   StateGet(void);
int          StateSet(enum State state);
int          Tc58v64BadBlockListPut(void);
int          Tc58v64BadBlockListGet(void);
unsigned int Tc58v64ErrorsCorrectable(void);
int          Tc58v64ErrorsInit(void);
unsigned int Tc58v64ErrorsNoncorrectable(void);
void         Tc58v64IncrementErrorsCorrectable(void);
void         Tc58v64IncrementErrorsNoncorrectable(void);

#endif /* EEPROM_H */
