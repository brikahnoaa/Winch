/******************************************************************************\
**	spdq.c			SPDQ: SPI Intrinsic Interval Data Acquisition
**	
**	Release:		B3.0	2011/06/24
*****************************************************************************
**	
**	COPYRIGHT (C) 2005-2011 PERSISTOR INSTRUMENTS INC., ALL RIGHTS RESERVED
**	
**	Developed by: John H. Godley for Persistor Instruments Inc.
**	153-A Lovell's Lane, Marstons Mills, MA 02648 USA
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
*****************************************************************************
**	2005/07/05:		First release
**	2007/05/17:		Sample rates below 62500Hz Fix:
**		This release fixes an error in the aggregate sample timing for rates
**		50KSPS and below where the sampling period appears to is 125ns longer
**		than expected. This is an artifact of the autonomous sequencing state
**		machine in the 6833x that implements the QSPI timing. A table of
**		constants is used to fill in key control fields that define the
**		various inter-sample delays that makes the precise SPDQ timing
**		possible. The original table values should work according to the
**		Freescale documentation and based on my earlier experiences. They do
**		for 64KSPS and 62.5KSPS, but we've determined by experiment that the
**		periods for all of the lower rates are extended by two system clock
**		cycles or 125ns. Decreasing the DSCKL control value by 2 should
**		provide the correction according to the Freescale docs but we further
**		determined that decreasing by 1 provides the proper value for all but
**		three of the table values and these have been removed in the updated
**		table.
**	2011/06/24:		Lower power with reduced clock and sample rates:
		struct spdqcb adds an system clock field at the top.
			This allows use of spdq with a limited number of lower system
			clock rates and proportionally lower sample rates:
				16MHz, 8MHz, 4MHz, 3.2MHz, 1.6MHz, 800kHz, 640kHz
		NOTE THAT THIS HAS ONLY BEEN TESTED FOR 1.6MHz AND 5000 SPS !!!
			The sys_clock field is near the top to force very rapid failure
			when compiled with tables constructed for the older spdq.
		spdqcb.tpwmch has been replaced with pwm_pin to support CF1.
			You will need to change your code to specify the connector C pin
			number for the TPU or CTM DASM pin,
\******************************************************************************/

//#define DEBUG			// comment this out to turn off diagnostics
#ifdef DEBUG
  #define	DBG(X)	X	// template:	DBG( cprintf("\n"); )
  #pragma message	("!!! "__FILE__ ": Compiling with DEBUG compile flag set!")
#else
  #define	DBG(X)		// nothing
#endif

#include	<cfxbios.h>		// Persistor BIOS and I/O Definitions
#include	<cfxpico.h>		// Persistor PicoDOS Definitions
#include	<ADS8344.h>		// Burr-Brown ADS8344 SPI A-D QPB Driver  

#include	<spdq.h>		// SPI Intrinsic Interval Data Acquisition

#include	<stdio.h>
#include	<string.h>
#include	<stdlib.h>
#include	<ctype.h>

static QPBDev ADS8344SPDQTemplate = 
	{
	NMPCS3,				// qslot			our qspi slot
	"ADS8344",			// devName			C string with device name (15 max)
	2000000,			// maxBaud			maximum baud rate in Hz for device
	13,					// bits				Bits Per Transfer
	iaLowSCK,			// clockPolar		SPI Clock Polarity
	captLead,			// clockPhase		SPI Clock Phase
	true,				// contCSMulti		Continue CS assert between mult xfrs
	true,				// autoTiming		Auto adjust timing to clock flag
	0,					// psDelaySCK		Min. Delay Before SCK (picoSecs)
	0,					// psDelayTXFR		Min. Delay After Transfer
	0,					// *rcvData			pointer to received data buffer
	0					// xfrCount			words transferred
	};

static const struct spdq_qctl
	{
	ulong	rate;
	uchar	bits;
	uchar	baud;
	uchar	dt;
	uchar	dtl;
	uchar	dsck;
	uchar	dsckl;
	} spdq_qctl[] = {						       // 8MHz   4MHz 3.2MHz 1.6MHz 800kHz 640kHz
    //RATE    BITS BAUD DT  DTL  DSCK DSCKL    /2     /4     /5     /10   /20   /25
      96000,  13,  3,   0,  0,   1,   3, 
      90000,  13,  3,   0,  0,   1,   4,
      80000,  13,  3,   0,  0,   1,   5,    //this may or may not work with SYSCLK=19.2MHz HM 9/08/2011
      64000,  13,  4,   0,  0,   1,   4,    // 32000  16000  12800  6400  3200  2560
      62500,  13,  4,   0,  0,   1,   7,    // 31250  15625  12500  6250  3125  2500
      50000,  13,  4,   1,  1,   1,   23,   // 25000  12500  10000  5000  2500  2000
      40000,  13,  4,   1,  2,   1,   31,   // 20000  10000  8000   4000  2000  1600
      32000,  13,  4,   1,  4,   1,   17,   // 16000  8000   6400   3200  1600  1280
      31250,  13,  4,   1,  4,   1,   23,   // 15625  ---    6250   3125  ---   1250
      25000,  13,  4,   1,  6,   1,   23,   // 12500  6250   5000   2500  1250  1000
      20000,  13,  4,   1,  9,   1,   7,    // 10000  5000   4000   2000  1000  800
      16000,  13,  4,   1,  12,  1,   11,   // 8000   4000   3200   1600  800   640
      15625,  13,  4,   1,  12,  1,   23,   // 0      ---    3125   ---   ---   625
      12800,  13,  4,   1,  16,  1,   8,    // 6400   3200   2560   1280  640   512
      12500,  13,  4,   1,  16,  1,   23,   // 6250   3125   2500   1250  625   500
      10000,  13,  4,   1,  21,  1,   23,   // 5000   2500   2000   1000  500   400
      8000,   13,  4,   1,  27,  1,   31,   // 4000   2000   1600   800   400   320
      6400,   13,  4,   1,  35,  1,   25,   // 3200   1600   1280   640   320   256
      6250,   13,  4,   1,  36,  1,   23,   // 3125   ---    1250   625   ---   250
      5000,   13,  4,   1,  46,  1,   23,   // 2500   1250   1000   500   250   200
      4000,   13,  4,   1,  59,  1,   7,    // 2000   1000   800    400   200   160
      3200,   13,  4,   1,  74,  1,   27,   // 1600   800    640    320   160   128
      3125,   13,  4,   1,  76,  1,   23,   // 0      ---    625    ---   ---   125
      2560,   13,  4,   1,  94,  1,   12,   // 1280   640    512    256   128   0
      2500,   13,  4,   1,  96,  1,   23,   // 1250   625    500    250   125   100
      2000,   13,  4,   1,  121, 1,   23,   // 1000   500    400    200   100   80
      1600,   13,  4,   1,  152, 1,   31,   // 800    400    320    160   80    64
      1250,   13,  4,   1,  196, 1,   23,   // 625    ---    250    125   63    50
      1024,   13,  4,   1,  240, 1,   28,   // 512    256    ---    ---   ---   0
      1000,   13,  4,   1,  246, 1,   23,   // 500    250    200    100   50    40
      0,      0,   0,   0,  0,   0,   0
      };


static const struct sysclk_timing
	{
	ushort	sysclk;
	ushort	clkdiv;
	} sysclk_timing[] = 
	  {19200,   1, //test only
	  	16000,	1,
		8000,	2,
		4000,	4,
		3200,	5,
		1600,	10,
		800,	20,
		640,	25,
		0,		0
	  };


#define	RB_QPKT_SIZE	8UL		// 4 2-byte words (8 is asm index limit)
static long		rb_mem_size;	// ring buffer size after init and alloc
static ushort	rb_max_index;	// maximum index value for qpkts
static ushort	rb_wrap_mask;	// same as rb_max_index, better context readability

typedef struct { ushort	adata[RB_QPKT_SIZE / sizeof(ushort)]; }	rb_qpkts;
static rb_qpkts	*rb_pkt_buf;				// dynamically allocated at run time
static ushort	*rb_wp;						// word pointer for interrupt handlers
static QPB		*spdq_qpb;					// just address, we setup manually
static short	rb_head_idx, rb_tail_idx;	// don't resize these!

static vfptr	saved_irq_handler;
static vfptr	saved_qspi_handler;
static uchar	irq_bit;			// \ setup in spdq_open, used in and for 
static short	irq_autovector;		// / ADC4to7RuptHandlerCore management

#define	SPQ_IRQ_LOWER_INIT_DIAG		PIOSet(1)
#define	SPQ_IRQ_LOWER_ENTRY_DIAG	asm ( bclr	#4,0xfffffa13 )
#define	SPQ_IRQ_LOWER_EXIT_DIAG		asm ( bset	#4,0xfffffa13 )
#define	SPQ_IRQ_LOWER_CLOSE_DIAG	PIORead(1)

#define	SPQ_IRQ_UPPER_INIT_DIAG		PIOSet(42)
#define	SPQ_IRQ_UPPER_ENTRY_DIAG	asm ( bclr	#0,0xfffffa1b );
#define	SPQ_IRQ_UPPER_EXIT_DIAG		asm ( bset	#0,0xfffffa1b );
#define	SPQ_IRQ_UPPER_CLOSE_DIAG	PIORead(42)

static void ADC0to3RuptHandlerCore(void);	// From SPIF on ch7/wrd15 wrap to ch0/wrd0
static void ADC0to3RuptVectorHWDiag(void);	// 	using pin-1 hardware diagnostics
static void ADC0to3RuptVectorNoDiag(void);	//	no diagnostics

static void ADC4to7RuptHandlerCore(void);	// From PCS0-->IRQ low on ch4/wrd8 start
static void ADC4to7RuptVectorHWDiag(void);	// 	using pin-1 hardware diagnostics
static void ADC4to7RuptVectorNoDiag(void);	//	no diagnostics


/******************************************************************************\
**	spdq_open		open and prepare the SPDQ driver
\******************************************************************************/
short spdq_open(spdqcb *spdq)
	{
	short	i, j, sysclk_idx, rtab_idx;
	uchar	pcsn2NMPCS[] = { NMPCS0, NMPCS1, NMPCS2, NMPCS3 };
	uchar	pcsn2pin[] = { 21, 19, 15, 17 };
	static const uchar ADS8344SglChSel[8] = 	// convert chan to single-ended selector
		{ 0x00, 0x40, 0x10, 0x50, 0x20, 0x60, 0x30, 0x70 };
	static const uchar ADS8344DifChSel[8] = 	// convert chan to differential selector
		{ 0x00, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70 };
	bool	irqconnected = true;	// assume so, but check ahead
/*
	const int SPDQ_BETA = 3;
	cprintf("\n\n  %s", "!#!#!#!#!#!#!#!#!#!#!#!#!#!#!#!#!#!#!#!#!#!#!#!#!#!#!#!#!");
	cprintf("\n  !!! BETA %2d !!!   %s  %s   !!! BETA %2d !!!"
		,SPDQ_BETA, __DATE__, __TIME__, SPDQ_BETA);
	cprintf("\n  %s\n\n", "!#!#!#!#!#!#!#!#!#!#!#!#!#!#!#!#!#!#!#!#!#!#!#!#!#!#!#!#!");
*/
	spdq->errcode = 0;
	
	if (spdq->p1_diag_lwr)
		SPQ_IRQ_LOWER_INIT_DIAG;
	if (spdq->p42_diag_up)
		SPQ_IRQ_UPPER_INIT_DIAG;

	DBG( if (spdq->con_dbg) cprintf(" spdq->chans = %d \n", spdq->chans); cdrain(); )
	if (! (spdq->chans == 1 || spdq->chans == 2 || spdq->chans == 4 || spdq->chans == 8)
		|| (spdq->chans == 8 && spdq->inp_end == 0))
		{
		if (spdq->con_dbg) cprintf("\n%d chans is not 1, 2, 4, or 8!\n", spdq->chans);
		return spdq->errcode = spdqInvalidChanCount;
		}
	
	DBG( if (spdq->con_dbg) cprintf(" spdq->ring_buf_size = %ld \n", spdq->ring_buf_size); cdrain(); )
	if (! (spdq->ring_buf_size == 65536 || spdq->ring_buf_size == 131072 || spdq->ring_buf_size == 262144))
		{
		if (spdq->con_dbg) cprintf("\n%ld ring_buf_size is not valid!\n", spdq->ring_buf_size); cdrain(); 
		return spdq->errcode = spdqInvalidRingBufSize;
		}

	DBG( if (spdq->con_dbg) cprintf(" spdq->pre_trig_size = %ld \n", spdq->pre_trig_size); cdrain(); )
	if (spdq->pre_trig_size > spdq->ring_buf_size - spdq->req_blk_size || (spdq->pre_trig_size % spdq->req_blk_size))
		{
		if (spdq->con_dbg) cprintf("\n%ld pre_trig_size is not valid!\n", spdq->pre_trig_size); cdrain(); 
		return spdq->errcode = spdqInvalidPreTrigSize;
		}

	DBG( if (spdq->con_dbg) cprintf(" spdq->sys_clk = %lu \n", spdq->sys_clk); cdrain(); )
	for (sysclk_idx = 0; sysclk_timing[sysclk_idx].sysclk; sysclk_idx++)
		if (sysclk_timing[sysclk_idx].sysclk == spdq->sys_clk)
			break;
	if (sysclk_timing[sysclk_idx].sysclk == 0)
		{
		if (spdq->con_dbg) cprintf("\n%ld sys_clk is not valid!\n", spdq->sys_clk); cdrain(); 
		return spdq->errcode = spdqInvalidSystemClock;
		}

	DBG( if (spdq->con_dbg) cprintf(" spdq->agg_sps = %ld \n", spdq->agg_sps); cdrain(); )
	for (rtab_idx = 0; spdq_qctl[rtab_idx].rate; rtab_idx++)
		if (spdq_qctl[rtab_idx].rate == spdq->agg_sps * sysclk_timing[sysclk_idx].clkdiv)
			break;

	DBG( if (spdq->con_dbg) cprintf(" spdq_qctl[%d].rate = %ld \n", rtab_idx, spdq_qctl[rtab_idx].rate); cdrain(); )
	if (spdq_qctl[rtab_idx].rate == 0)
		{
		if (spdq->con_dbg) cprintf("\n%ld is not a valid aggregate rate !\n", spdq->agg_sps); cdrain();
		return spdq->errcode = spdqInvalidRateRequest;
		}

	DBG( if (spdq->con_dbg) cprintf(" spdq->irq_pin = %d \n", spdq->irq_pin); cdrain(); )
	switch(spdq->irq_pin)
		{
		case 39: irq_bit = 5; irq_autovector = level5InterruptAutovector; break;
		case 40: irq_bit = 7; irq_autovector = level7InterruptAutovector; break;
		case 41: irq_bit = 2; irq_autovector = level2InterruptAutovector; break;
		default: 
			if (spdq->con_dbg) cprintf("\n%d chans is not 39, 40, or 41!\n", spdq->irq_pin);
			return spdq->errcode = spdqInvalidIrqPin;
		}

	PIOSet(pcsn2pin[spdq->irq_pcs]);
	if (PIORead(spdq->irq_pin) == 0)
		irqconnected = false;
	PIOClear(pcsn2pin[spdq->irq_pcs]);
	if (PIORead(spdq->irq_pin) != 0)
		irqconnected = false;
	PIOSet(pcsn2pin[spdq->irq_pcs]);
	
	DBG( if (spdq->con_dbg) cprintf(" PCS%u (pin-%u) %s connected to pin-%u) \n", spdq->irq_pcs, pcsn2pin[spdq->irq_pcs], irqconnected ? "is" : "ISN'T", spdq->irq_pin); cdrain(); )
	if (! irqconnected)
		{
		if (spdq->con_dbg) cprintf("\nmissing PSC%u to pin-%u connection !\n", spdq->irq_pcs, spdq->irq_pin); cdrain();
		return spdq->errcode = spdqNoIRQConnection;
		}
		
	rb_mem_size = spdq->ring_buf_size;
	rb_wrap_mask = rb_max_index = (rb_mem_size / RB_QPKT_SIZE) - 1;

	DBG( if (spdq->con_dbg) cprintf(" rb_mem_size = %ld, rb_wrap_mask = %04X \n", rb_mem_size, rb_wrap_mask); cdrain(); )

	QPBInit(true);	// force sane state
	ADS8344SPDQTemplate.qslot = pcsn2NMPCS[spdq->adc_pcs];
	spdq_qpb = QPBInitSlot(&ADS8344SPDQTemplate);
	if (! QPBLockSlot(spdq_qpb))	// lock it down, we want to own the QSPI
		{
		if (spdq->con_dbg) cprintf("\nCouldn't lock and own A-D with QSPI!\n"); cdrain();
		spdq_close(spdq);
		return spdq->errcode = spdqQPBLockFailed;
		}

	rb_pkt_buf = (rb_qpkts *) rb_wp = spdq->rbcp = calloc(rb_mem_size, 1);
	if (rb_pkt_buf == 0)
		{
		if (spdq->con_dbg) cprintf("\nCouldn't allocate the raw data buffer!\n"); cdrain();
		spdq_close(spdq);
		return spdq->errcode = spdqAllocFailed;
		}
	rb_head_idx = rb_tail_idx = 0;

	if (spdq->pwm_pin)
		spdq_pwm_init(spdq->pwm_pin, rb_max_index, 1);

//	FORCE HARDWARE COMPLIANCE
	PITInit(SIM_PITR_SPDQ_IPL);		// don't allow IPL6 chores to mess us up!
	CSSetSysAccessSpeeds(90, nsRAMStd, nsCFStd, nsMotoSpecAdj); // for pre-401 CF2s
	TMGSetSpeed(spdq->sys_clk);

	if (spdq->adc_shdn)
		PIOSet(spdq->adc_shdn);	// enable REF and AD on R216AU and MRCP

	QSM.SPCR0 = 	0x8000		// MSTR - spi master
				|	(spdq_qctl[rtab_idx].bits << 10)
				|	spdq_qctl[rtab_idx].baud
				;	// WOMQ=0, CPOL=0, CPHA=0
	DBG( if (spdq->con_dbg) cprintf(" QSM.SPCR0 = %04X \n", QSM.SPCR0); cdrain(); )
	QSM.SPSR &= ~0x80;		// clear SPIF interrupt
	QSM.SPCR1 = (spdq_qctl[rtab_idx].dsckl << 8) | spdq_qctl[rtab_idx].dtl;	// DSCKL | DTL
	DBG( if (spdq->con_dbg) cprintf(" QSM.SPCR1 = %04X \n", QSM.SPCR1); cdrain(); )
	for (i = j = 0; j < 8; j++)
		{
		QTR[i++] = ( (spdq->inp_end	? (ADS8344SglChSel[j & (spdq->chans - 1)] | ADS8344SGL)
								: (ADS8344DifChSel[j & (spdq->chans - 1)] | ADS8344DIF))
					| ADS8344CMD | ADS8344EXT ) << 4;
		QTR[i++] = 0;	// second half of command
		DBG( if (spdq->con_dbg) cprintf(" QTR[%u] = %04X, QTR[%u] = %04X \n", i-2, QTR[i-2], i-1, QTR[i-1] ); cdrain(); )
		}

	for (i = 0; i < 16; )
		{
		QCR[i++] =		0x80	// CONT=1, continue CS for first word
					|	0x40	// BITSE=1, use SPCR0
					|	(spdq_qctl[rtab_idx].dt << 5)	
					|	(spdq_qctl[rtab_idx].dsck << 4)	
					|	pcsn2NMPCS[spdq->adc_pcs]
					;
		QCR[i++] =		0x00	// CONT=0, release CS at end of second word
					|	0x40	// BITSE=1, use SPCR0
					|	(spdq_qctl[rtab_idx].dt << 5)	
					|	(spdq_qctl[rtab_idx].dsck << 4)	
					|	pcsn2NMPCS[spdq->adc_pcs]
					;
		DBG( if (spdq->con_dbg) cprintf(" QCR[%u] = %02X, QCR[%u] = %02X \n", i-2, QCR[i-2], i-1, QCR[i-1] ); cdrain(); )
		}
	QCR[8] &= (0xF0 | pcsn2NMPCS[spdq->irq_pcs]);	// assert IRQ PCS half way
	DBG( if (spdq->con_dbg) cprintf(" QCR[8] = %02X\n", QCR[8] ); cdrain(); )
	
	QSM.SPCR2 = 0xCF00;		// rupt, wrap, wrap to, 0 : end : 0 : new
	DBG( if (spdq->con_dbg) cprintf(" QSM.SPCR2 = %04X \n", QSM.SPCR2); cdrain(); )
	QSM.SPSR &= ~0x80;		// clear SPIF interrupt

	saved_qspi_handler = IEVInsertAsmFunct(spdq->p42_diag_up ? ADC4to7RuptVectorHWDiag : ADC4to7RuptVectorNoDiag, qspiVector);
	saved_irq_handler = IEVInsertAsmFunct(spdq->p1_diag_lwr ? ADC0to3RuptVectorHWDiag : ADC0to3RuptVectorNoDiag, irq_autovector);
	PIOBusFunct(spdq->irq_pin);

	return 0;
	
	}	//____ spdq_open() ____//


/******************************************************************************\
**	spdq_close			close SPDQ driver and release all resouces
\******************************************************************************/
void spdq_close(spdqcb *spdq)
	{

	DBG( if (spdq->con_dbg) cprintf("\nspdq_close: rb_pkt_buf=%lx\n", rb_pkt_buf); )

	spdq_idle();	// force state
	PIORead(spdq->irq_pin);
	QSM.SPCR2 = 0x0000;		// no rupt, no wrap, no wrap to, 0 : end : 0 : new

	IEVInsertAsmFunct(saved_irq_handler, irq_autovector);
	IEVInsertAsmFunct(saved_qspi_handler, qspiVector);

	if (spdq->pwm_pin)
		PIOClear(spdq->pwm_pin);	// back to ouput low

	if (spdq->p1_diag_lwr)
		SPQ_IRQ_LOWER_CLOSE_DIAG;

	if (spdq->p42_diag_up)
		SPQ_IRQ_UPPER_CLOSE_DIAG;

	if (spdq->adc_shdn)
		PIORead(spdq->adc_shdn);	// disable REF and AD on R216AU and MRCP

	QPBInit(true);				// clean up the mess we made 

	PITInit(SIM_PITR_DEF_IPL);	// back to normal
	
	if (rb_pkt_buf)
		free (rb_pkt_buf);
	QPBUnlockSlot(spdq_qpb);
	
	}	//____ spdq_close() ____//


/******************************************************************************\
**	spdq_idle					bring acquisition engine to orderly stop
\******************************************************************************/
void spdq_idle(void)
	{
	
	QSM.SPCR2 &= ~0x4000;		// turn off wrap to allow cycle completion 
	while (spdq_is_active())	// will auto-disable when complete,
		;						//   so wait for SPE low

	}	//____ spdq_idle() ____//


#if 0	// inline macro in spdq.h
/******************************************************************************\
**	spdq_activate				allow acquisition engine to run
\******************************************************************************/
void _spdq_activate(void)
	{

	QSM.SPCR2 |= 0xC000;		// enable rupts (SPIFIE=1) and wrap (WREN=1)
	QSM.SPCR1 |= 0x8000;		// enable QSPI

	}	//____ spdq_activate() ____//
#endif

#if 0	// inline macro in spdq.h
/******************************************************************************\
**	spdq_is_active				return true if acquisition engine is running
\******************************************************************************/
bool spdq_is_active(void)
	{

	return QSM.SPCR1 & 0x8000 ? true : false;

	}	//____ spdq_is_active() ____//
#endif

/******************************************************************************\
**	spdq_request_rb_contig_block		pull data block from ring buffer
\******************************************************************************/
bool spdq_request_rb_contig_block(spdqcb *spdq)
	{
	short	delta = rb_head_idx - rb_tail_idx;
	short	contig_pkts = delta >= 0 ? delta : rb_max_index - rb_tail_idx + 1;
	long	contig_bytes = contig_pkts * RB_QPKT_SIZE;
	
	if (spdq->pwm_pin)
		spdq_pwm_update(spdq->pwm_pin, rb_max_index, delta ? delta & rb_wrap_mask : 1);

	if ((QSM.SPCR2 & 0x8000) == 0)		// check overflow signal from rupt handler
		spdq->errcode = spdqRBOverflow;	// pass back, but don't deny data request
		
	if (spdq->contig_bytes || contig_bytes < spdq->req_blk_size)
		return false;
		
	if (contig_bytes > spdq->max_req_size)
		contig_bytes = spdq->max_req_size;

	spdq->rbcp = &rb_pkt_buf[rb_tail_idx];
	spdq->contig_bytes = (contig_bytes / spdq->req_blk_size) * spdq->req_blk_size;
	return true;
	
	}	//____ spdq_request_rb_contig_block() ____//


/******************************************************************************\
**	spdq_release_rb_contig_block		reconcile previous data block pull
\******************************************************************************/
void spdq_release_rb_contig_block(spdqcb *spdq)
	{

	rb_tail_idx = (rb_tail_idx + (spdq->contig_bytes / RB_QPKT_SIZE)) & rb_wrap_mask;
	spdq->contig_bytes = 0;

	}	//____ spdq_release_rb_contig_block() ____//


/******************************************************************************\
**	spdq_flush_ring_buffer		make any remaining buffered data available
**
**	NOTES:	1. May return with up to ring_buf_size in contig_bytes
**			2. MUST BE CALLED TWICE TO FULLY EMPTY!
**				Recommended concluding sequence:
**					while (spdq_flush_ring_buffer(spdq))
**						{
**						fwrite(spdq->rbcp, 1, spdq->contig_bytes, fp);
**						spdq_release_rb_contig_block(spdq);
**						}
**
\******************************************************************************/
bool spdq_flush_ring_buffer(spdqcb *spdq)
	{
	long	save_req_blk_size = spdq->req_blk_size;
	long	save_max_req_size = spdq->max_req_size;
	bool	result;
	
	spdq->req_blk_size = RB_QPKT_SIZE;			// any
	spdq->max_req_size = spdq->ring_buf_size;	// and all
	result = spdq_request_rb_contig_block(spdq);
	spdq->req_blk_size = save_req_blk_size;
	spdq->max_req_size = save_max_req_size;
	
	return result;

	}	//____ spdq_flush_ring_buffer() ____//


/******************************************************************************\
**	spdq_pre_trigger_idle	discard buffer contents older than pre-trigger req
\******************************************************************************/
void spdq_pre_trigger_idle(spdqcb *spdq);
void spdq_pre_trigger_idle(spdqcb *spdq)
	{
	short	delta = (rb_head_idx - rb_tail_idx) & rb_wrap_mask;

	if (delta >= (spdq->pre_trig_size + spdq->req_blk_size ) / RB_QPKT_SIZE)
		rb_tail_idx = (rb_tail_idx + (spdq->req_blk_size / RB_QPKT_SIZE)) & rb_wrap_mask;
	
	}	//____ spdq_pre_trigger_idle() ____//


/******************************************************************************\
**	spdq_rb_fill_level		return latest fill level in bytes
\******************************************************************************/
long spdq_rb_fill_level(void)
	{

	return (long) ((rb_head_idx - rb_tail_idx) & rb_wrap_mask) * RB_QPKT_SIZE;

	}	//____ spdq_rb_fill_level() ____//


/******************************************************************************\
**	spdq_reset_ring_buffer		discard buffer contents and reset indices
\******************************************************************************/
void spdq_reset_ring_buffer(spdqcb *spdq)
	{
	bool	active = spdq_is_active();
	
	if (active)
		spdq_idle();

	spdq->contig_bytes = rb_head_idx = rb_tail_idx = 0;
	spdq->rbcp = (char *) rb_wp;
	
	if (active)
		spdq_activate();

	}	//____ spdq_reset_ring_buffer() ____//


/******************************************************************************\
**	ADC0to3RuptHandler		// From IRQ PCS0 low on ch4/wrd8 start
**
**	Measured low-time on pin 1:						16.2 us
**	Interrupt (42) and RTE (26) clocks overhead:	 4.2 us
**	Total (- roughly 0.2us for one diag edge):		20.0 us
\******************************************************************************/
static void ADC0to3RuptVectorNoDiag(void)
	{
	ADC0to3RuptHandlerCore();
	RTE();
	}	//____ ADC0to3RuptVectorNoDiag() ____//

static void ADC0to3RuptVectorHWDiag(void)
	{
	SPQ_IRQ_LOWER_ENTRY_DIAG;
	ADC0to3RuptHandlerCore();
	SPQ_IRQ_LOWER_EXIT_DIAG;
	RTE();
	}	//____ ADC0to3RuptVectorHWDiag() ____//

static void ADC0to3RuptHandlerCore(void)
	{
	asm ( movem.l	d0-d1/a0-a1,-(a7)	); // just need these

	asm ( movea.l	rb_pkt_buf,a1		);	// *d++ = *s++ << 13 | *s++;
	asm ( move.w	rb_head_idx,d0		);
	asm ( lea		(a1,d0.w*8),a1		);

	asm ( lea		QRR[0],a0			);
	asm ( moveq		#13,d1				);

	asm ( move.w    (a0)+,d0			);
	asm ( lsl.w     d1,d0				);
	asm ( or.w      (a0)+,d0			);
	asm ( move.w    d0,(a1)+			);

	asm ( move.w    (a0)+,d0			);
	asm ( lsl.w     d1,d0				);
	asm ( or.w      (a0)+,d0			);
	asm ( move.w    d0,(a1)+			);

	asm ( move.w    (a0)+,d0			);
	asm ( lsl.w     d1,d0				);
	asm ( or.w      (a0)+,d0			);
	asm ( move.w    d0,(a1)+			);

	asm ( move.w    (a0)+,d0			);
	asm ( lsl.w     d1,d0				);
	asm ( or.w      (a0)+,d0			);
	asm ( move.w    d0,(a1)+			);

	asm ( move.l	a1,rb_wp			);	// pass shorthand to 2nd batch

	asm ( movem.l   (a7)+,d0-d1/a0-a1	);	// pop used registers
	asm ( rts							);	// not really needed for C funct exit
	}	//____ ADC0to3RuptHandlerCore() ____//


/******************************************************************************\
**	ADC4to7RuptHandler		// From SPIF on ch7/wrd15 wrap to ch0/wrd0
**
**	Measured low-time on pin 42: 					18.6 us
**	Interrupt (42) and RTE (26) clocks overhead:	 4.2 us
**	Total (- roughly 0.2us for one diag edge):		22.4 us
\******************************************************************************/
static void ADC4to7RuptVectorNoDiag(void)
	{
	ADC4to7RuptHandlerCore();
	RTE();
	}	//____ ADC4to7RuptVectorNoDiag() ____//

static void ADC4to7RuptVectorHWDiag(void)
	{
	SPQ_IRQ_UPPER_ENTRY_DIAG;
	ADC4to7RuptHandlerCore();
	SPQ_IRQ_UPPER_EXIT_DIAG;
	RTE();
	}	//____ ADC4to7RuptVectorHWDiag() ____//

static void ADC4to7RuptHandlerCore(void)
	{
	asm ( movem.l	d0-d1/a0-a1,-(a7)	); // just need these

	asm ( lea		QRR[8],a0			);
	asm ( movea.l	rb_wp,a1			);	// was saved from first batch
	asm ( moveq		#13,d1				);

	asm ( move.w    (a0)+,d0			);	// *d++ = *s++ << 13 | *s++;
	asm ( lsl.w     d1,d0				);
	asm ( or.w      (a0)+,d0			);
	asm ( move.w    d0,(a1)+			);

	asm ( move.w    (a0)+,d0			);
	asm ( lsl.w     d1,d0				);
	asm ( or.w      (a0)+,d0			);
	asm ( move.w    d0,(a1)+			);

	asm ( move.w    (a0)+,d0			);
	asm ( lsl.w     d1,d0				);
	asm ( or.w      (a0)+,d0			);
	asm ( move.w    d0,(a1)+			);

	asm ( move.w    (a0)+,d0			);
	asm ( lsl.w     d1,d0				);
	asm ( or.w      (a0)+,d0			);
	asm ( move.w    d0,(a1)+			);

	asm ( move.w	rb_head_idx,d0		);	// adjust ring buffer head index
	asm ( addq.w	#2,d0				);	// for both 0-3 and 4-7
	asm ( and.w		rb_wrap_mask,d0		);
	asm ( cmp.w		rb_tail_idx,d0		);	// if (rb_head_idx != rb_tail_idx)
	asm ( bne.s     okay				);	

overflow:	// prepare for orderly stop while preserving current buffer contents
	asm ( move.w	irq_bit,d0			);	// revert IRQ fron bus interrupt to I/O
	asm ( bclr		d0,0xfffffa1f		);	
	QSM.SPCR2 &= ~0xC000;					// no rupts (SPIFIE=0) and no wrap (WREN=0)
	QSM.SPSR &= ~0x80;						// clear SPIF interrupt that got us here
	IEVEnableAll();							// allow other interrupts ...
	spdq_idle();							// ... while wait out final cycle
	asm ( bset		#5,0xfffffa1f		);	// prepare for possibly resumption
	asm ( movem.l   (a7)+,d0-d1/a0-a1	);	// pop used registers either way
	asm ( rts							);	// not really needed for C funct exit
	// NOTE: SPIFIE == 0 is the only signal to the main process that an
	//		 overflow has occurred and that the engine is stopped. When called,
	//		 the spdq_request_rb_contig_block will monitor this and set errcode
	//		 to spdqRBOverflow. Your main loop must watch for SPIFIE == 0 or
	//		 spdq->errcode == spdqRBOverflow and take some application
	//		 appropriate action. To restart the engine, it must clear space in
	//		 the buffer and then call spdq_activate
okay:
	asm ( move.w	d0,rb_head_idx		);
	asm ( bclr		#7,0xfffffc1f		);	// clear SPIF interrupt
	asm ( movem.l   (a7)+,d0-d1/a0-a1	);	// pop used registers either way
	asm ( rts							);	// not really needed for C funct exit

	}	//____ ADC4to7RuptHandlerCore() ____//


/******************************************************************************\
**	spdq_pwm_init		Initialize connector C pin as Pulse Width Modulator
\******************************************************************************/
void spdq_pwm_init(short pin, short period, short hightime)
	{
  #if CFX == CF2
	void TPWMInit(short tch, short tcr, short period, short hightime, TPUPriority priority);
	TPWMInit(TPUChanFromPin(pin), 1, period, hightime, tpuMiddlePriority);
  #else
	pin = period = hightime = 0;	// quiet the compiler warnings
/* Future effort for somebody else
	CTDASetupOPWM(CTDAGetIDFromPin(pin), true, true, 12, 0, 0);
	*CTMCModulusReg(MCSM31) = 61440;
	CTMPrescalerRun(true);
*/
  #endif
	
	}	//____ spdq_pwm_init() ____//


/******************************************************************************\
**	spdq_pwm_update	Update period and high time for connector C pin
\******************************************************************************/
void spdq_pwm_update(short pin, short period, short hightime)
	{
  #if CFX == CF2
	void TPWMUpdate(short tch, short period, short hightime);
	TPWMUpdate(TPUChanFromPin(pin), period, hightime);
  #else
	pin = period = hightime = 0;	// quiet the compiler warnings
/* Future effort for somebody else
	ushort		width;
	width = ((PLRawHead - PLRawTail) & PL_RAW_DATA_BUF_MASK) + 1;
	if (width < 100)
		width = 100;		// want to be able to see something
	*CTDADataRegB(CTDAGetIDFromPin(pin)) = width;	
*/
  #endif
	
	}	//____ spdq_pwm_update() ____//


/******************************************************************************\
**	spdq_write_header		
must be stopped
must be at start position
nbyte nust be less both WRITE_SIZE and 32768
\******************************************************************************/
bool spdq_write_header(spdqcb *spdq, void *buf, ushort nbyte)
	{

	if (spdq_is_active())
		return false;

	if (rb_head_idx || rb_tail_idx)
		return false;

	if (nbyte >= spdq->req_blk_size || nbyte >= 32768U)
		return false;

	memcpy(spdq->rbcp, buf, nbyte);
		rb_head_idx += (nbyte + RB_QPKT_SIZE - 1) / RB_QPKT_SIZE;

	return true;
	
	}	//____ spdq_write_header() ____//


