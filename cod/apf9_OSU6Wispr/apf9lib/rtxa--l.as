; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
; $Id: rtxa--l.as,v 1.1 2003/06/07 20:40:52 swift Exp $
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
; RCS Log:
;
; $Log: rtxa--l.as,v $
; Revision 1.1  2003/06/07 20:40:52  swift
; Initial revision
;
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
;
;	Runtime startoff module for HI-TECH C
;	Philips XA, Large model
;
;	Clyde Smith-Stubbs, September 1995
;
;	Copyright (C) 1995, HI-TECH Software
;	All Rights Reserved
;
;	This software remains the property of HI-TECH SOFTWARE and is
;	supplied under licence only. The use of this software is
;	permitted under the terms of that licence only. Copying of
;	this software except for the purpose of making backup or
;	working copies for the use of the licensee on a single
;	processor is prohibited.
;
	psect	vectors,ovrld,class=CODE,align=2,space=0
	psect	text,class=CODE,align=2,space=0
	psect	code,class=CODE,space=0
	psect	rbss,size=1024,class=DATA,space=1,reloc=2
	psect	rdata,size=1024,class=DATA,space=0,reloc=2
	psect	rbit,bit,size=512,class=BITSEG,space=1
	psect	bss,class=DATA,space=1,reloc=2
	psect	data,class=DATA,space=0,reloc=2
	psect	const,class=DATA,space=0,reloc=2
	psect	strings,class=DATA,space=0,reloc=2
	psect	nvram,class=DATA,space=1,reloc=2
	psect	farnvram,class=FARDATA,space=1,reloc=2
	psect	stack,space=1,reloc=2
	psect	heap,space=1,reloc=2
	psect	farbss,space=1,class=FARDATA
	psect	fardata,space=0,class=FARDATA
;
	global	start,_main,_exit		;entry point, main() and exit()
	global	__Lrbss,__Hrbss			;rbss psect bounds
	global	__Lrbit,__Hrbit			;rbit psect bounds
	global	__Lrdata,__Hrdata,__Brdata	;rdata psect bounds, load addr
	global	__Lbss,__Hbss			;bss psect bounds
	global	__Ldata,__Hstrings,__Bdata	;data/const/strings bounds, load
	global	__Lfardata,__Hfardata,__Bfardata	;far data
	global	__Lfarbss,__Hfarbss		;far bss
	global	__Hstack			;stack address
	global	powerup,start

SSEL	equ	403h			;Segment select register
ES	equ	442h			;Extra segment register
CS	equ	443h			;Code segment register
;
	psect	vectors
	dw	8F00h,powerup		;point RESET vector at powerup code, set system mode
;
;	Copyright message, and initialization code
;
	psect	text
	db	"C Library Copyright (C) 1995 HI-TECH Software",0dh,0ah
	db	"Brisbane, QLD. Australia",0dh,0ah,0,26
	db	"$Id: rtxa--l.as,v 1.1 2003/06/07 20:40:52 swift Exp $"
;
start:
	and.b	440h,#0FEh		;reset page zero bit
	mov	sp,#__Hstack-2		;initialize SP
	add	sp,#2
	mov.b	SSEL,#0

;
;	Clear the RBIT psect
;
	mov	r4,#0		;Set the upper word of the length to zero
	mov	r0,#((__Hrbit-__Lrbit+7)/8) & 127
	mov	r1,#__Lrbit/8
	call	clear
;
;	Clear the RBSS psect
;
	mov	r0,#__Hrbss-__Lrbss
	mov	r1,#__Lrbss
	call	clear
;
;	Copy the RDATA psect
;
	mov	r0,#__Hrdata-__Lrdata
	mov	r1,#__Brdata & 0ffffh
	mov.b	CS,#__Brdata shr 16
	setb	SSEL.1
	mov	r2,#__Lrdata
	call	copy
	clr	SSEL.1

	mov.b	SSEL,#0

;
;	Now call the main() function
;
	fcall	_main
_exit:
	br	start
;
;	clear:	zero "r4/r0" bytes of memory at address "r1",
;		does nothing if "r0" is zero.
;
clear:
	mov	r5,r0
	or	r5,r4
	beq	2f
	adds	r4,#1		;compensate for djnz later
1:
	movs.b	[r1+],#0	;clear 1 byte
	or	r1,r1
	bne	3f
	adds.b	ES,#1		;increment ES if we wrapped around
3:
	djnz	r0,1b
	djnz	r4,1b
2:
	ret
;
;	copy:	copy "r4/r0" bytes from address "r1" to address "r2",
;		"r1" is treated as a code pointer. does nothing
;		if "r0" is zero.
;
copy:
	mov	r5,r0		;test for zero count
	or	r5,r4
	beq	2f
	adds	r4,#1		;compensate for djnz later
1:
	movc	r3l,[r1+]	;get source byte
	mov	[r2+],r3l	;store to destination
	or	r2,r2
	bne	3f
	adds.b	ES,#1		;increment ES if we wrapped around
3:
	or	r1,r1
	bne	3f
	adds.b	CS,#1		;increment ES if we wrapped around
3:
	djnz	r0,1b
	djnz	r4,1b
2:
	ret
;
	end	start
;
;	End of file: "$Id: rtxa--l.as,v 1.1 2003/06/07 20:40:52 swift Exp $"
;
