#ifndef X24C16_H
#define X24C16_H

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * $Id: x24c16.c,v 1.4 2007/04/24 01:43:05 swift Exp $
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
 * $Log: x24c16.c,v $
 * Revision 1.4  2007/04/24 01:43:05  swift
 * Added acknowledgement and funding attribution.
 *
 * Revision 1.3  2006/04/21 13:45:38  swift
 * Changed copyright attribute.
 *
 * Revision 1.2  2004/04/14 20:48:21  swift
 * Revision E of the APF9 made some hardware changes to earlier versions.  The
 * PSD935 was replaced by a PSD835 and the 20mA serial loop was changed to be a
 * real serial IO port.
 *
 * Revision 1.1  2004/02/05 23:44:18  swift
 * API for the Xicor X24C16 eeprom.
 *
 * \end{verbatim}
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
#define x24c16ChangeLog "$RCSfile: x24c16.c,v $ $Revision: 1.4 $ $Date: 2007/04/24 01:43:05 $"

/* function prototypes */
unsigned char EERead(unsigned int addr);
float         EEReadFloat(unsigned int addr);
long int      EEReadLong(unsigned int addr);
unsigned int  EEReadWord(unsigned int addr);
void          EETest(void);
void          EEWrite(unsigned char b, unsigned int addr);
void          EEWriteFloat(float f, unsigned int addr);
void          EEWriteLong(long l, unsigned int addr);
void          EEWriteWord(unsigned int w, unsigned int addr);

#endif /* X24C16_H */

#include <apf9.h>
#include <psd835.h>
#include <logger.h>

/* prototypes for functions with static linkage */
static unsigned char EErd(void);
static void EEstart(void);
static void EEstop(void);
static unsigned char EEwr(unsigned char b);

/*------------------------------------------------------------------------*/
/* function to read a byte from a specified address of the EEPROM         */
/*------------------------------------------------------------------------*/
/**
   This function reads and returns a single byte from a specified address of
   the EEPROM.
*/
unsigned char EERead(unsigned int addr)
{
   unsigned char devaddr, wordaddr, b;

   WatchDog();
   devaddr = (addr & 0x700) >> 7;
   devaddr = devaddr | 0xa0;
   wordaddr = addr & 0xff;
   EEstart();
   EEwr(devaddr);
   EEwr(wordaddr);
   EEstart();
   devaddr = devaddr | 1;
   EEwr(devaddr);
   b = EErd();
   EEstop();
   return(b);
}

/*------------------------------------------------------------------------*/
/* function to read a long integer from a specified address of the EEPROM */
/*------------------------------------------------------------------------*/
/**
   This function reads and returns a long integer from a specified address of
   the EEPROM.
*/
long int EEReadLong(unsigned int addr)
{
   long l;

   unsigned char *q = (unsigned char *)&l;
   q[0] = EERead(addr);
   q[1] = EERead(addr + 1);
   q[2] = EERead(addr + 2);
   q[3] = EERead(addr + 3);
   return(l);
}

/*------------------------------------------------------------------------*/
/* function to read a float from a specified address of the EEPROM        */
/*------------------------------------------------------------------------*/
/**
   This function reads and returns a float from a specified address of the
   EEPROM.
*/
float EEReadFloat(unsigned int addr)
{
   float f;

   unsigned char *q = (unsigned char *)&f;
   q[0] = EERead(addr);
   q[1] = EERead(addr + 1);
   q[2] = EERead(addr + 2);
   q[3] = EERead(addr + 3);
   return(f);
}

/*------------------------------------------------------------------------*/
/* read a 2-byte unsigned integer from a specified address of the EEPROM  */
/*------------------------------------------------------------------------*/
/**
   This function reads and returns a 2-byte unsigned integer from a
   specified address of the EEPROM.
*/
unsigned int EEReadWord(unsigned int addr)
{
   unsigned int w;

   unsigned char *q = (unsigned char *)&w;
   q[0] = EERead(addr);
   q[1] = EERead(addr + 1);
   return(w);
}

/*------------------------------------------------------------------------*/
/* function to write a single byte to a specified address of the EEPROM   */
/*------------------------------------------------------------------------*/
/**
   This function writes a single byte to a specified address of the EEPROM.
*/   
void EEWrite(unsigned char b, unsigned int addr)
{
   unsigned char devaddr, wordaddr;

   WatchDog();
   devaddr = (addr & 0x700) >> 7;
   devaddr = devaddr | 0xa0;
   wordaddr = addr & 0xff;
   EEstart();
   EEwr(devaddr);
   EEwr(wordaddr);
   EEwr(b);
   EEstop();
   Wait(12);
}

/*------------------------------------------------------------------------*/
/* function to write a long integer to a specified address of the EEPROM  */
/*------------------------------------------------------------------------*/
/**
   This function writes a long integer to a specified address of the EEPROM.
*/   
void EEWriteLong(long l, unsigned int addr)
{
   unsigned char *q = (unsigned char *)&l;
   EEWrite(q[0], addr);
   EEWrite(q[1], addr + 1);
   EEWrite(q[2], addr + 2);
   EEWrite(q[3], addr + 3);
}

/*------------------------------------------------------------------------*/
/* function to write a float to a specified address of the EEPROM         */
/*------------------------------------------------------------------------*/
/**
   This function writes a float to a specified address of the EEPROM.
*/   
void EEWriteFloat(float f, unsigned int addr)
{
   unsigned char *q = (unsigned char *)&f;
   EEWrite(q[0], addr);
   EEWrite(q[1], addr + 1);
   EEWrite(q[2], addr + 2);
   EEWrite(q[3], addr + 3);
}


/*------------------------------------------------------------------------*/
/* write a 2-byte unsigned integer to a specified address of the EEPROM   */
/*------------------------------------------------------------------------*/
/**
   This function writes a 2-byte unsigned integer a specified address of the
   EEPROM.
*/   
void EEWriteWord(unsigned int w, unsigned int addr)
{
   unsigned char *q = (unsigned char *)&w;
   EEWrite(q[0], addr);
   EEWrite(q[1], addr + 1);
}

/*------------------------------------------------------------------------*/
/* function to write a test pattern to EEPROM and verify by rereading     */
/*------------------------------------------------------------------------*/
/**
   This function tests the EEPROM by first writing a test pattern and then
   rereading and verifying the pattern.
*/
void EETest(void)
{
   unsigned char b;
   unsigned int i, err=0;

   printf("WARNING: EEPROM test is destructive to stored data.\n");

   printf("Writing blocks:");
   for(i = 4;i < 2048;i++)
	{
      EEWrite((unsigned char)((i + 3) & 0xff), i);
      if((i % 128) == 0) printf(" %d", i);
	}

   printf("\nReading blocks:");
   for(i = 4;i < 2048;i++)
	{
      b = EERead((unsigned int)i);
      if((i % 128) == 0) printf(" %d", i);
      if(b != (unsigned char)((i + 3) & 0xff)) ++err;
	}
   if(err == 0) printf("\nEEPROM test was successful.\n");
   else printf("\nEEPROM test failed with %d errors\n", err);
}

/*------------------------------------------------------------------------*/
/* low-level function to read one byte from EEPROM                        */
/*------------------------------------------------------------------------*/
static unsigned char EErd(void)
{
   unsigned char i;

   bd1 = 0;
   for(i = 0;i < 8;i++)
	{
      bd1 = bd1 << 1;
      Psd835PortDSet(EE_SCL);
      WaitTicks(5);
      BD1_LSB = EE_SDA;
      Psd835PortDClear(EE_SCL);
      WaitTicks(5);
	}
   return(bd1);
}

/*------------------------------------------------------------------------*/
/* low-level function (I haven't taken time to understand yet -dds)       */
/*------------------------------------------------------------------------*/
static void EEstart(void)
{
   EE_SDA = 1;
   WaitTicks(5);
   Psd835PortDSet(EE_SCL);
   WaitTicks(5);
   EE_SDA = 0;
   WaitTicks(5);
   Psd835PortDClear(EE_SCL);
   WaitTicks(5);
}

/*------------------------------------------------------------------------*/
/* low-level function (I haven't taken time to understand yet -dds)       */
/*------------------------------------------------------------------------*/
static void EEstop(void)
{
   Psd835PortDClear(EE_SCL);
   WaitTicks(5);
   EE_SDA = 0;
   WaitTicks(5);
   Psd835PortDSet(EE_SCL);
   WaitTicks(5);
   EE_SDA = 1;
   WaitTicks(5);
}

/*------------------------------------------------------------------------*/
/* low-level function to write one byte to the EEPROM                     */
/*------------------------------------------------------------------------*/
static unsigned char EEwr(unsigned char b)
{
   unsigned char i, ok;

   bd1 = b;
   for(i = 0;i < 8;i++)
	{
      EE_SDA = BD1_MSB;			/* output bit */
      WaitTicks(5);
      Psd835PortDSet(EE_SCL);
      WaitTicks(5);
      Psd835PortDClear(EE_SCL);
      bd1 = bd1 << 1;
	}
   WaitTicks(5);
   EE_SDA = 1;						/* release bus */
   Psd835PortDSet(EE_SCL);
   WaitTicks(5);
   if(EE_SDA == 0) ok = 1;		/* check for ack */
   else ok = 0;
   Psd835PortDClear(EE_SCL);
   return(ok);
}
