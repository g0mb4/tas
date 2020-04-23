;file rs.as - includes routine reverse. This routine reverses the string


	.entry		REVERSE
	.extern		LEN
REVERSE: mov		STRADD, r1
	 mov		LASTCHAR, r2
	 mov		LEN, r3

;Exchanging places

LOOP:	cmp		#0, r3
	jnz		END
	cmp		r1, r2

	jnz		END
	mov		@r1, r4
	mov		@r2, @r1
	mov		r4, @r2
	sub		#2, r3
	jnc		LOOP
END:	rts
	.extern		LASTCHAR
	.extern		STRADD

