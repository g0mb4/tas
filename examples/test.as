; test.as - prints "abcdef" to the display

.entry MAIN
MAIN:	mov	 LENGTH, r1
		lea	 STR, r2
LOOP:	prn	 @r2
		inc	 r2
		sub	 #1, r1
		jnz  LOOP
END:	hlt
STR:	.string	 "abcdef"
LENGTH:	.data	6
