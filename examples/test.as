; test.as
; Prints the string "abcdef".

	.entry MAIN	; file contains the definition of MAIN
MAIN:	mov LEN, r1	; move LEN(=6) to r1
	lea STR, r2	; load the address of STR to r2
LOOP:	prn @r2	; print the character at the memory location that r2 holds
	inc r2		; r2 = r2 + 1
        sub #1, r1	; r1 = r1 - 1
	jnz LOOP	; jump to LOOP if the zero flag is not set (sub sets it)
END:	hlt		; end of the program
STR:	.string "abcdef"; string to print
LEN:	.data 6	; length of the string
