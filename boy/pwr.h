// pwr.h
#ifndef H_PWR
#define H_PWR

typedef struct PwrInfo {
  bool logging;
  bool track;           // track and log battery capacity
  float amps;
  float charge;         // minimum system capacity to run
  float chargeMin;      // minimum system capacity to run
  float volts;          // minimum system voltage to run
  float voltsMin;       // minimum system voltage to run
  int log;              // log filehandle
  long batCap;          // for lithium
  short interval;       // deciSecs for PIT timer interrupt
} PwrInfo;

static void pwrIrq4RxISR(void);
static void pwrLog(void);
static void pwrLogWrite(ushort *);
static void pwrOpenLog(long);

bool pwrCheck(void);
float pwrMonitor(ulong, int, ulong *);
float pwrVolts(void);
int pwrNap(long sec);
void pwrInit(void);
void pwrStop(void);


// here on was hps.h ---------
//
// hps.h
// #ifndef H_HPS
// #define H_HPS

/*******************************************************************************
** ADS8345.h based on Max146.h (compatible with ADS8344)
**
** by Haru Matsumoto
**
** April 29, 2002 (Blue Moon)
*****************************************************************************
**	
**	Licensed by:	Persistor Instruments Inc. for the Peristor CF1
**	info@persistor.com - http://www.persistor.com
**	
*****************************************************************************
**	
**	Developed by:	John H. Godley dba Peripheral Issues
**	jhgodley@periph.com - http://www.periph.com
**	Copyright (C) 1996-1998 Peripheral Issues.	All rights reserved.
**	
*****************************************************************************
**	
**	Copyright and License Information
**	
**	Peripheral Issues (hereafter, PERIPH) grants you (hereafter, Licensee)
**	a non-exclusive, non-transferable license to use the software source
**	code contained in this single source file. Licensee may distribute
**	binary derivative works using this software to third parties without
**	fee or restrictions.
**	
**	PERIPH MAKES NO REPRESENTATIONS OR WARRANTIES ABOUT THE SUITABILITY OF THE
**	BINARY SOFTWARE, EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO
**	THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
**	PURPOSE, OR NON-INFRINGEMENT. PERIPH SHALL NOT BE LIABLE FOR ANY DAMAGES
**	SUFFERED BY LICENSEE AS A RESULT OF USING, MODIFYING OR DISTRIBUTING THE
**	BINARY SOFTWARE OR ITS DERIVATIVES.
**	
**	By using or copying this Software, Licensee agrees to abide by the
**	copyright law and all other applicable laws of the U.S. including, but
**	not limited to, export control laws, and the terms of this license. PERIPH
**	shall have the right to terminate this license immediately by written
**	notice upon Licensee's breach of, or non-compliance with, any of its
**	terms. Licensee may be held legally responsible for any copyright
**	infringement or damages resulting from Licensee's failure to abide by
**	the terms of this license. 
**	
*******************************************************************************/

/*
extern const uchar Ads8345SglChSel[8];	// convert chan to single-ended selector
extern const uchar Ads8345DifChSel[8];	// convert chan to differential selector
enum
{ Ads8345CMD = 0x80 };
enum
{ Ads8345SGL = 0x04, Ads8345DIF = 0x00 };
enum
{ Ads8345UNI = 0x00, Ads8345BIP = 0x00 };
enum
{ Ads8345PDFull = 0x00, Ads8345PDFast = 0x01, Ads8345INT = 0x02, Ads8345EXT =
    0x03
};
bool Ads8345Init (ushort qslot);
void Ads8345PowerDown (ushort qslot, bool pdfull);
ushort Ads8345Sample (ushort qslot, ushort chan, bool uni, bool sgl, bool pd);
ushort Ads8345Repeat (ushort qslot);
ushort *Ads8345SampleBlock (ushort qslot, ushort first, ushort count,
			    vfptr asyncf, bool uni, bool sgl, bool pd);
bool Ads8345Lock (ushort qslot);
bool Ads8345Unlock (ushort qslot);
ushort *ADS8345QueueToArray (void *qp, ushort count);
ushort Ads8345SampleOrigin (ushort qslot, ushort chan, bool uni, bool sgl,
			    bool pd);
float Ads8345RawToVolts (ushort raw, float vref, bool);

// #endif //      __Ads8345_H

//int hpsStats(void);

typedef struct HpsStats {
  float curr;
  float volt;
  float pres;
  float humi;
} HpsStats;
int hpsRead(HpsStats *hps);
int readStat (float *ret, int c);

// #endif // H_HPS
 */
#endif // H_PWR
