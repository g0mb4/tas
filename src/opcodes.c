/*!
 * \file opcodes.c
 * \brief definitions of operations and addressing modes
 */

#include "asm.h"

/*!
 * \brief definitions of operations
 * 
 * columns: mnemonic, opcode, no_parameters, src_addressings, dest_addressings
 */
operation_t operations[16] = {
	/* mnemonic	, opcode	, no_parameters	, src_addressings	, dest_addressings */
	{"mov"		, 0x0		, 2				, "012345"			, "12345"  },
	{"cmp"		, 0x1		, 2				, "012345"			, "012345" },
	{"add"		, 0x2		, 2				, "012345"			, "12345"  },
	{"sub"		, 0x3		, 2				, "012345"			, "12345"  },
	{"mul"		, 0x4		, 2				, "012345"			, "12345"  },
	{"div"		, 0x5		, 2				, "012345"			, "12345"  },
	{"lea"		, 0x6		, 2				, "1"				, "12345"  },
	{"inc"		, 0x7		, 1				, ""				, "12345"  },
	{"dec"		, 0x8		, 1				, ""				, "12345"  },
	{"jnz"		, 0x9		, 1				, ""				, "1235"   },
	{"jnc"		, 0xA		, 1				, ""				, "1235"   },
	{"shl"		, 0xB		, 2				, "12345"			, "012345" },
	{"prn"		, 0xC		, 1				, ""				, "012345" },
	{"jsr"		, 0xD		, 1				, ""				, "1235"   },
	{"rts"		, 0xE		, 0				, ""				, ""       },
	{"hlt"		, 0xF		, 0				, ""				, ""       }
};

/*!
 * \brief definitions of addressing modes
 * 
 * columns: mode, additional word
 * 
 * \note the mode of the addressing mode is the same as its index in the array
 */
addressing_t addressings[6] = {
	/* mode				, additional word */
	{ INSTANT			, 1 },
	{ DIRECT			, 1 },
	{ INDIRECT			, 1 },
	{ RELATIVE			, 1 },
	{ DIRECT_REGISTER	, 0 },
	{ INDIRECT_REGISTER	, 0 }
};
