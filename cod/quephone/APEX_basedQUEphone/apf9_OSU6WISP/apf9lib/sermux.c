#ifndef SERMUX_H
#define SERMUX_H

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * $Id: sermux.c,v 1.3 2007/04/24 01:43:05 swift Exp $
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
/*
 *  This module is a simple API to the APF9's serial muxer PCB.  Its proper
 *  function depends on having the APF9's Com1 port connected to the serial
 *  muxer's P1 input port with the pins mapped straight through:
 *  Com1:1->P1:1, Com1:2->P1:2, ... Com1:7->P1:7.  Similarly, the APF9's Com2
 *  port must be connected to the serial muxer's P2 input port with the pins
 *  mapped straight through.
*/
/** RCS log of revisions to the C source code.
 *
 * \begin{verbatim}
 * $Log: sermux.c,v $
 * Revision 1.3  2007/04/24 01:43:05  swift
 * Added acknowledgement and funding attribution.
 *
 * Revision 1.2  2006/04/21 13:45:38  swift
 * Changed copyright attribute.
 *
 * Revision 1.1  2006/03/25 01:32:52  swift
 * Added serial muxer to the apf9lib.
 *
 * Revision 1.1  2006/01/06 23:45:01  swift
 * Added serial muxer to libraries.
 *
 * Revision 1.1  2005/02/27 01:21:29  swift
 * API to the Apf9's serial muxer PCB.
 *
 * \end{verbatim}
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
#define sermuxChangeLog "$RCSfile: sermux.c,v $ $Revision: 1.3 $ $Date: 2007/04/24 01:43:05 $"

/* function prototypes */
int Com1SelectMux1(void);
int Com1SelectMux3(void);
int Com2SelectMux2(void);
int Com2SelectMux4(void);

#endif /* SERMUX_H */

/* prototypes for functions with external linkage */
int Com1RtsAssert(void);
int Com1RtsClear(void);
int Com2RtsAssert(void);
int Com2RtsClear(void);

/*------------------------------------------------------------------------*/
/* function to select muxer port 1                                        */
/*------------------------------------------------------------------------*/
int Com1SelectMux1(void)
{
   return Com1RtsAssert();
}

/*------------------------------------------------------------------------*/
/* function to select muxer port 2                                        */
/*------------------------------------------------------------------------*/
int Com2SelectMux2(void)
{
   return Com2RtsAssert();
}

/*------------------------------------------------------------------------*/
/* function to select muxer port 3                                        */
/*------------------------------------------------------------------------*/
int Com1SelectMux3(void)
{
   return Com1RtsClear();
}

/*------------------------------------------------------------------------*/
/* function to select muxer port 4                                        */
/*------------------------------------------------------------------------*/
int Com2SelectMux4(void)
{
   return Com2RtsClear();
}
