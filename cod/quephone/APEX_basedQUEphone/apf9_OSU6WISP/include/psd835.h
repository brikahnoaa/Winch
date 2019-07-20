#ifndef PSD835_H
#define PSD835_H

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * $Id: psd835.c,v 1.4 2007/04/24 01:43:05 swift Exp $
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
 * $Log: psd835.c,v $
 * Revision 1.4  2007/04/24 01:43:05  swift
 * Added acknowledgement and funding attribution.
 *
 * Revision 1.3  2006/04/21 13:45:38  swift
 * Changed copyright attribute.
 *
 * Revision 1.2  2004/12/29 23:04:36  swift
 * Modified LogEntry() to use strings stored in the CODE segment.  This saves
 * lots of space in the DATA segment and significantly speeds code start-up.
 *
 * Revision 1.1  2004/04/14 20:48:21  swift
 * Revision E of the APF9 made some hardware changes to earlier versions.  The
 * PSD935 was replaced by a PSD835 and the 20mA serial loop was changed to be a
 * real serial IO port.
 *
 * Revision 1.3  2004/03/18 15:07:38  swift
 * Modify PSD935 registers PMMR0 and PMMR2 to save power.
 *
 * Revision 1.2  2003/11/20 18:59:35  swift
 * Added GNU Lesser General Public License to source file.
 *
 * Revision 1.1  2003/11/12 22:31:06  swift
 * Initial revision
 * \end{verbatim}
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
            
/* function prototypes */
int Psd835Init(void);
                                                          
/* PSD835 port definitions */
static unsigned char PMMR0        @ 0xfb0;
static unsigned char PMMR2        @ 0xfb4;

/*========================================================================*/
/* definitions for port A of the PSD835                                   */
/*========================================================================*/

/* define port registers */
static unsigned char PORTARD      @ 0xf00;
static unsigned char PORTAWR      @ 0xf04;
static unsigned char PORTADIR     @ 0xf06;
static unsigned char PORTADRIVE   @ 0xf08;

/* define masks used with port */
#define POSITIVE_PHASE     0xbc
#define NEGATIVE_PHASE     0x7c
#define RESET_MAGSWITCH    0x01  // out
#define RTC_CLK            0x02  // out
#define LATCH_PWR          0x04  // out
#define PTT_PON            0x08  // out
#define IRIDIUM_PON        0x08  // out
#define PTT_PLL_ON         0x10  // out
#define PTT_TX_ON          0x20  // out
#define PTT_PHASE          0x40  // out
#define PTT_PHASE_I        0x80  // out

/* inline functions to modify port state */
#define Psd835PortASet(MASK)   {EA=0; Psd835A |=  (MASK); PORTAWR=Psd835A; EA=1;}
#define Psd835PortAClear(MASK) {EA=0; Psd835A &= ~(MASK); PORTAWR=Psd835A; EA=1;}
#define Psd835PortAInit()      {PORTADIR=0xff; PORTADRIVE = 0; Psd835A=4; PORTAWR=Psd835A;}

/*========================================================================*/
/* definitions for port B of the PSD835                                   */
/*========================================================================*/

/* define port registers */
static unsigned char PORTBRD      @ 0xf01;
static unsigned char PORTBWR      @ 0xf05;
static unsigned char PORTBDIR     @ 0xf07;
static unsigned char PORTBDRIVE   @ 0xf09;

/* define masks used with port */
#define MAGSWITCH          0x01  // in
#define MAGSWITCH_MOVE     0x02  // in
#define FLASH_RB           0x04  // in
#define AIR_PUMP_ON        0x08  // out
#define SERIAL1_ON         0x10  // out
#define SERIAL2_ON         0x20  // out
#define CTD_PWR_CTRL       0x40  // out
#define CTD_PTS_FP         0x80  // out

/* inline functions to modify port state */
#define Psd835PortBSet(MASK)   {EA=0; Psd835B |=  (MASK); PORTBWR=Psd835B; EA=1;}
#define Psd835PortBClear(MASK) {EA=0; Psd835B &= ~(MASK); PORTBWR=Psd835B; EA=1;}
#define Psd835PortBInit()      {PORTBDIR=0xf8; PORTBDRIVE=0; Psd835B=0; PORTBWR=Psd835B;}

/*========================================================================*/
/* definitions for port C of the PSD835                                   */
/*========================================================================*/

/* define port registers */
static unsigned char PORTCRD      @ 0xf10;
static unsigned char PORTCWR      @ 0xf14;
static unsigned char PORTCDIR     @ 0xf16;
static unsigned char PORTCDRIVE   @ 0xf18;

/* define masks used with port */
#define FLASH_CE        0x10  // out
#define FLASH_CLE       0x20  // out
#define FLASH_ALE       0x40  // out
#define ANALOG_PWR_ON   0x80  // out

/* inline functions to modify port state */
#define Psd835PortCSet(MASK)   {EA=0; Psd835C |=  (MASK); PORTCWR=Psd835C; EA=1;}
#define Psd835PortCClear(MASK) {EA=0; Psd835C &= ~(MASK); PORTCWR=Psd835C; EA=1;}
#define Psd835PortCInit()      {PORTCDIR=0xff; PORTCDRIVE=0; Psd835C=0; PORTCWR=Psd835C;}

/*========================================================================*/
/* definitions for port D of the PSD835                                   */
/*========================================================================*/

/* define port registers */
static unsigned char PORTDRD      @ 0xf11;
static unsigned char PORTDWR      @ 0xf15;
static unsigned char PORTDDIR     @ 0xf17;
static unsigned char PORTDDRIVE   @ 0xf19;

/* define masks used with port */
#define ADC_DIN      0x02 // out
#define ADC_CS       0x04 // out
#define EE_SCL       0x08 // out

/* inline functions to modify port state */
#define Psd835PortDSet(MASK)   {EA=0; Psd835D |=  (MASK); PORTDWR=Psd835D; EA=1;}
#define Psd835PortDClear(MASK) {EA=0; Psd835D &= ~(MASK); PORTDWR=Psd835D; EA=1;}
#define Psd835PortDInit()      {PORTDDIR=0xe; PORTDDRIVE=0; Psd835D=8; PORTDWR = Psd835D;}

/*========================================================================*/
/* definitions for port E of the PSD835                                   */
/*========================================================================*/

/* define port registers */
static unsigned char PORTERD      @ 0xf30;
static unsigned char PORTECONTROL @ 0xf32;
static unsigned char PORTEWR      @ 0xf34;
static unsigned char PORTEDIR     @ 0xf36;
static unsigned char PORTEDRIVE   @ 0xf38;

/* define masks used with port */
#define TP1          0x10  // out
#define ADC_CLK      0x20  // out
#define RTC_RST      0x40  // out
#define ADC_MUX_CS   0x80  // out

/* inline functions to modify port state */
#define Psd835PortESet(MASK)   {EA=0; Psd835E |=  (MASK); PORTEWR=Psd835E; EA=1;}
#define Psd835PortEClear(MASK) {EA=0; Psd835E &= ~(MASK); PORTEWR=Psd835E; EA=1;}
#define Psd835PortEInit()      {PORTECONTROL=0; PORTEDIR=0xf0; PORTEDRIVE=0; \
                                Psd835E=0; PORTEWR=Psd835E;}

/*========================================================================*/
/* definitions for port F of the PSD835                                   */
/*========================================================================*/

/* define port registers */
static unsigned char PORTFCONTROL @ 0xf42;
static unsigned char PORTFDIR     @ 0xf46;

/* inline functions to modify port state */
#define Psd835PortFInit() {PORTFCONTROL=0xf0; PORTFDIR=0xf0;}

/*========================================================================*/
/* definitions for port G of the PSD835                                   */
/*========================================================================*/

/* define port registers */
static unsigned char PORTGRD      @ 0xf41;
static unsigned char PORTGCONTROL @ 0xf43;
static unsigned char PORTGWR      @ 0xf45;
static unsigned char PORTGDIR     @ 0xf47;
static unsigned char PORTGDRIVE   @ 0xf49;

/* define masks used with port */
#define PISTON_OUT      0x10  // out
#define PISTON_IN       0x20  // out
#define AIR_CLOSE       0x40  // out
#define AIR_OPEN        0x80  // out

/* inline functions to modify port state */
#define Psd835PortGSet(MASK)   {EA=0; Psd835G |=  (MASK); PORTGWR=Psd835G; EA=1;}
#define Psd835PortGClear(MASK) {EA=0; Psd835G &= ~(MASK); PORTGWR=Psd835G; EA=1;}
#define Psd835PortGInit()      {PORTGCONTROL=0xf; PORTGDIR=0xff; PORTGDRIVE=0; \
                                Psd835G=0; PORTGWR=Psd835G;}

/* object to retain PSD port data during program execution */
extern unsigned char near Psd835A,Psd835B,Psd835C,Psd835D,Psd835E,Psd835G;

#endif /* PSD835_H */
