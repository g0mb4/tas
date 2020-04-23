;file k.as includes routine PRTSTR. This routine prints the string.

	.entry PRTSTR
PRTSTR:	mov STRADD, r2

		; r2 now hold the memory location of the string.

LOOP:	cmp	#0,@r2
				jnz	BYE
				prn	@r2
				inc	r2
				jnc	LOOP
BYE:		rts
				.extern STRADD
