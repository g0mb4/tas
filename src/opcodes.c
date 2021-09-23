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
// clang-format off
operation_t g_operations[16] = {
    /* mnemonic	, opcode	, no_parameters	, src_addressings	, dest_addressings */
    { "mov", 0x0, 2, "01234", "1234" }, { "cmp", 0x1, 2, "01234", "01234" },
    { "add", 0x2, 2, "01234", "1234" }, { "sub", 0x3, 2, "01234", "1234" },
    { "mul", 0x4, 2, "01234", "1234" }, { "div", 0x5, 2, "01234", "1234" },
    { "lea", 0x6, 2, "1", "1234" },     { "inc", 0x7, 1, "", "1234" },
    { "dec", 0x8, 1, "", "1234" },      { "jnz", 0x9, 1, "", "124" },
    { "jnc", 0xA, 1, "", "124" },       { "shl", 0xB, 2, "1234", "01234" },
    { "prn", 0xC, 1, "", "01234" },     { "jsr", 0xD, 1, "", "124" },
    { "rts", 0xE, 0, "", "" },          { "hlt", 0xF, 0, "", "" }
};
// clang-format on

/*!
 * \brief definitions of addressing modes
 * 
 * columns: mode, additional word
 * 
 * \note the mode of the addressing mode is the same as its index in the array
 */
// clang-format off
addressing_t g_addressings[5] = {
    /* mode				, additional word */
    { INSTANT           , 1 },
    { DIRECT            , 1 },
    { INDIRECT          , 1 },
    { DIRECT_REGISTER   , 0 },
    { INDIRECT_REGISTER , 0 }
};
// clang-format on
