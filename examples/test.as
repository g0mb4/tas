; test.as
; looong lineeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee

.entry MAIN
MAIN:	mov	 LENGTH, r1
		lea	 STR, r2
LOOP:	jnz	 END
		prn	 @r2
		sub	 #1, r1
		inc	 r2
		jnc	 *LOOP
END:	hlt
STR:	.string	 "abcdef"
LENGTH:	.data	6
