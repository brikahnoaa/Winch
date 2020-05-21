/******************************************************************************\
**	#include	<spdq.h>		// SPDQ: SPI Intrinsic Interval Data Acquisition
**	
**	Release:		B3.0	2011/06/24
*****************************************************************************
**	
**	COPYRIGHT (C) 2005-2011 PERSISTOR INSTRUMENTS INC., ALL RIGHTS RESERVED
**	
**	Developed by: John H. Godley for Persistor Instruments Inc.
**	254-J Shore Road, Bourne, MA 02532  USA
**	jhgodley@persistor.com - http://www.persistor.com
**	
*****************************************************************************
**	
**	Copyright and License Information
**	
**	Persistor Instruments Inc. (hereafter, PII) grants you (hereafter,
**	Licensee) a non-exclusive, non-transferable license to use the software
**	source code contained in this single source file with hardware products
**	sold by PII. Licensee may distribute binary derivative works using this
**	software and running on PII hardware products to third parties without
**	fee or other restrictions.
**	
**	PII MAKES NO REPRESENTATIONS OR WARRANTIES ABOUT THE SUITABILITY OF THE
**	SOFTWARE, EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
**	IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE,
**	OR NON-INFRINGEMENT. PII SHALL NOT BE LIABLE FOR ANY DAMAGES SUFFERED BY
**	LICENSEE AS A RESULT OF USING, MODIFYING OR DISTRIBUTING THE SOFTWARE OR
**	ITS DERIVATIVES.
**	
**	By using or copying this Software, Licensee agrees to abide by the
**	copyright law and all other applicable laws of the U.S. including, but
**	not limited to, export control laws, and the terms of this license. PII
**	shall have the right to terminate this license immediately by written
**	notice upon Licensee's breach of, or non-compliance with, any of its
**	terms. Licensee may be held legally responsible for any copyright
**	infringement or damages resulting from Licensee's failure to abide by
**	the terms of this license. 
**	
\******************************************************************************/
#include <cfxbios.h>
#include <cfxpico.h>

#ifndef		__spdqlp
#define		__spdqlp

#define	SIM_PITR_SPDQ_IPL	1	// spdq_open changes IPL 6 to 1 and cancels all chores

#define	SPDQ_ERRORS		3900	// SPDQ Storage Errors
enum
	{
	  spdqErrorStart = SPDQ_ERRORS
	, spdqInvalidSystemClock	// invalid system clock requested
	, spdqInvalidChanCount		// invalid channel count
	, spdqInvalidRingBufSize	// invalid ring buffer size
	, spdqInvalidRateRequest	// invalid aggregate rate requested
	, spdqInvalidPreTrigSize	// invalid pre-trigger size
	, spdqInvalidIrqPin			// invalid IRQ pin specified
	, spdqNoIRQConnection		// missing PSCn to IRQ connection
	, spdqAllocFailed			// couldn't allocate memory
	, spdqQPBLockFailed			// couldn't lock and own QPB slot
	, spdqRBOverflow			// ring buffer overflow
	};

typedef struct spdq_control_block
	{
	ushort	sys_clk;		// --> 16000,8000,4000,3200,1600,800,640 kHz
	ulong	agg_sps;		// --> aggregate sampling rate in samples per second
	uchar	chans;			// --> 1, 2, 4, or 8 ONLY!
	uchar	inp_end;		// --> 1 for single-ended, 0 for differential
	uchar	adc_shdn;		// --> adc and ref shutdown pin (28 for R216AU and MRCP)
	uchar	adc_pcs;		// --> QSPI PSCn (R2s use 3) controlling the ADC 
	uchar	irq_pcs;		// --> QSPI PSCn (typ 2) connected to IRQ
	uchar	irq_pin;		// --> IRQ5=39(typ) or IRQ7=40 or IRQ2=41
	uchar	pwm_pin;		// --> timer pin for buffer activity PWM, 0 for none
	uchar	con_dbg;		// --> 1 to debug setup with console messages
	uchar	p1_diag_lwr;	// --> 1 to activate pin-1 diagnostics for lower queue half
	uchar	p42_diag_up;	// --> 1 to activate pin-42 diagnostics for upper queue half
	ulong	ring_buf_size;	// --> ring buffer allocation: 1, 2, or 4 * 65536
	ulong	req_blk_size;	// --> block size for ring buffer requests (typ. 4096)
	ulong	max_req_size;	// --> maximum bytes to return from one request (typ. 4096)
	ulong	pre_trig_size;	// --> bytes to retain for pre-trigger event recording
	ulong	contig_bytes;	// <-> contiguous bytes from last request call
	uchar	*rbcp;			// <-- ring buffer current pointer (set by request)
	short	errcode;		// <-- zero or error code from library call
	} spdqcb;
//#define spiiidaq	spdqcb	// uncomment for pre-b3 builds if you must 

short spdq_open(spdqcb *spdq);
void spdq_close(spdqcb *spdq);
void spdq_idle(void);
void spdq_activate(void) = { 0x0078, 0xC000, 0xFC1C, 0x0078, 0x8000, 0xFC1A };
bool spdq_is_active(void) = { 0x3038, 0xFC1A, 0xEE48, 0xE048 };
long spdq_rb_fill_level(void);
bool spdq_request_rb_contig_block(spdqcb *spdq);
void spdq_release_rb_contig_block(spdqcb *spdq);
void spdq_pre_trigger_idle(spdqcb *spdq);
bool spdq_flush_ring_buffer(spdqcb *spdq);
void spdq_reset_ring_buffer(spdqcb *spdq);
bool spdq_write_header(spdqcb *spdq, void *buf, ushort nbyte);
void spdq_pwm_init(short pin, short period, short hightime);
void spdq_pwm_update(short pin, short period, short hightime);


#endif	//	__spdqlp


