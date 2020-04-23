;file cs.as - includes routine count.
;This routine counts the length of the string

		.entry		COUNT
		
		.extern		STRADD

		.extern		LEN
COUNT:	mov		STRADD,	r1
		
		cmp		#0, @r1
		
		jnz		ENDCOUNT
		
		inc		LEN

		inc		r1

		jnc		COUNT

ENDCOUNT:		rts
