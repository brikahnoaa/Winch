; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
; $Id: powerup.as,v 1.1 2003/05/30 17:58:20 swift Exp $
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
; RCS Log:
;
; $Log: powerup.as,v $
; Revision 1.1  2003/05/30 17:58:20  swift
; Initial revision
;
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
;	powerup routine
;	watchdog delay = 1.13 seconds (4 * 4096 * 255 / 3686400)

	global	powerup,start
	psect	text,align=2
powerup:
	mov.b	046Ah,#02h			; BCR = 1, 8 bit data bus, 20 address lines
	mov.b	0469h,#050h	   		; BTRH
	mov.b	0468h,#040h	   		; BTRL
	mov.b	045Fh,#255			; WDL = 255
	mov.b	045Dh,#0A5h			; WFEED1 = 0xa5
	mov.b	045Eh,#05Ah			; WFEED2 = 0x5a
	jmp	start
