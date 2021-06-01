/*!
 * \file parser.c
 * \brief parsing functions
 */

#include "asm.h"

extern operation_t operations[16]; /*!< \brief array of operations */
extern addressing_t addressings[5]; /*!< \brief array of addressing modes */

/*!
 * \brief checks if the input sting is a valid numeric literal
 * 
 * regex equivalent: ^[-+]?[0-9]+$
 * 
 * \note start_index is there, because we want to apply the same function to "1" and "#1"
 * 
 * \param str			string containing the numeric literal
 * \param start_index	start of the numeric literal in the input string
 * \return				valid or not
 */
bool is_valid_numeric_literal(char * str, int start_index) {
    if (!str || strlen(str) < 1) {
        return false;
    }

    int i, len = (int)strlen(str);
    bool valid = false;
    char ch;

    for (i = start_index; i < len; i++) {
        ch = str[i];

        /* first character can be [-+][0-9] */
        if (i == start_index) {
            valid = (ch >= '0' && ch <= '9') || ch == '-' || ch == '+';
            /* other characters can be [0-9] */
        } else {
            valid = ch >= '0' && ch <= '9';
        }

        if (valid == false) {
            return false;
        }
    }

    return true;
}

/*!
 * \brief checks if the input sting is a valid label name
 *
 * regex equivalent: ^(?:r[0-7])|^[A-Za-z][A-Za-z0-9]*$
 * 
 * \note start_index is there, because we want to apply the same function to "LABEL" and "@LABEL"
 * \note end_offset is there, because we want to apply the same function to "LABEL" and "LABEL:"
 *
 * \param str			string containing the label name
 * \param start_index	start of the label name in the input string
 * \param end_offset	offset from the end of the string to the end of the label name
 * \return				valid or not
 */
bool is_valid_label_name(char * str, int start_index, int end_offset) {
    if (!str || strlen(str) < 1) {
        return false;
    }

    int i, len = (int)strlen(str);
    bool valid = false;
    char ch;

    /* if it is a valid register name, it cant be a label */
    if (is_valid_register_name(str, start_index) == true) {
        return false;
    }

    for (i = start_index; i < len - end_offset; i++) {
        ch = str[i];

        /* first character can be [A-Za-z] */
        if (i == start_index) {
            valid = (ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z');
            /* other characters can be [A-Za-z0-9] */
        } else {
            valid = (ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z') ||
                    (ch >= '0' && ch <= '9');
        }

        if (valid == false) {
            return false;
        }
    }

    return true;
}

/*!
 * \brief checks if the input sting is a valid register name
 *
 * regex equivalent: ^r[0-7]$
 * 
 * \note start_index is there, because we want to apply the same function to "r1" and "@r1"
 *
 * \param str			string containing the name of the register
 * \param start_index	start of the register name in the input string
 * \return				valid or not
 */
bool is_valid_register_name(char * str, int start_index) {
    int len = (int)strlen(str);

    if (!str || len < 1) {
        return false;
    }

    /* string is too short to be a register name */
    if (len < start_index + 1) {
        return false;
    }

    /* first character should be 'r' */
    if (str[start_index] != 'r') {
        return false;
    }

    /* second character should be [0-7] */
    if (str[start_index + 1] < '0' || str[start_index + 1] > '7') {
        return false;
    }

    /* third 'character' should be NULL (end of the string) */
    if (str[start_index + 2] != 0) {
        return false;
    }

    return true;
}

/*!
 * \brief check if the addressing mode is valid for the operation
 * 
 * \param op	operation
 * \param addr	addressing mode
 * \param dest	is the operand is the destination or the source
 * \return		addressing mode is valid or not
 */
bool is_valid_addressing(operation_t * op, addressing_t * addr, bool dest) {
    char legal_modes[7];
    uint32_t i = 0;
    addressing_mode_t mode;

    strcpy(legal_modes, dest ? op->dest_legal : op->src_legal);

    if (!addr) {
        /* no addressing is allowed */
        if (strlen(op->dest_legal) == 0) {
            return true;
        } else {
            return false;
        }
    } else {
        /* allowed modes stored in a string eg. "0123" */
        for (i = 0; i < strlen(legal_modes); i++) {
            mode =
                legal_modes[i] - '0'; /* extract the numeric value '0' -> 0 */

            if (mode == addr->mode) {
                return true;
            }
        }

        return false;
    }
}

/*!
 * \brief gets the numeric value of a register string
 * 
 * "r7" -> 7
 * 
 * \note string must pass is_valid_register_name()
 * \note start_index is there, because we want to apply the same function to "r1" and "@r1"
 * 
 * \param str			string containing the name of the register
 * \param start_index	start of the regigter name in the input string
 * \return				numeric value
 */
uint8_t get_register(char * str, int start_index) {
    return str[start_index + 1] - '0';
}

/*!
 * \brief gets a 16bit integer number from a string
 * 
 * \note string must pass is_valid_numeric_literal()
 * \note 2's complement for negative numbers
 * \note start_index is there, because we want to apply the same function to "1" and "#1"
 * 
 * \param str			string containing the number
 * \param start_index	start of the number in the input string
 * \return				numeric value
 */
uint16_t get_number(char * str, int start_index) {
    if (!str) {
        return 0;
    }

    uint16_t value = 0;
    bool negative = false;
    int i, len = (int)strlen(str);

    /* check for -+ signs */
    if (str[start_index] == '-') {
        negative = true;
        start_index++;
    } else if (str[start_index] == '+') {
        start_index++;
    }

    /* start from backwards
	   "123" -> 3 * 1 + 2 * 10 + 1 * 100
	*/
    for (i = len - 1; i >= start_index; i--) {
        value *= 10;
        value += str[i] - '0'; /* ascii to number */
    }

    /* if negative, create 2's complement */
    if (negative == true) {
        value = ~value; /* 1's complement */
        value += 1; /* 2's complement */
    }

    return value;
}

/*!
 * \brief gets the type of the input sting as it is a column
 * 
 * \note colum is a string literal between wthite spaces e.g. "MAIN:", "mov", "1,2,3"
 *  
 * \param str	string of the column
 * \return		type of the column
 */
column_t column_type(char * str) {
    int i, len = (int)strlen(str);

    /* not a valid column */
    if (!str || len <= 1) {
        return UNKNOWN;
        /* starts with '.' */
    } else if (str[0] == '.') {
        if (strcmp(str, ".data") == 0) {
            return DIRECTIVE_NUMBER;
        } else if (strcmp(str, ".string") == 0) {
            return DIRECTIVE_STRING;
        } else if (strcmp(str, ".entry") == 0) {
            return DIRECTIVE_ENTRY;
        } else if (strcmp(str, ".extern") == 0) {
            return DIRECTIVE_EXTERN;
        } else {
            return UNKNOWN;
        }
        /* ends width ':' */
    } else if (str[len - 1] == ':') {
        if (is_valid_label_name(str, 0, 1)) {
            return LABEL;
        } else {
            return UNKNOWN;
        }
    } else {
        for (i = 0; i < 16; i++) {
            if (strcmp(operations[i].mnemonic, str) == 0) {
                return OPERATION;
            }
        }

        return UNKNOWN;
    }
}

/*!
 * \brief gets the struct of the operation from the mnemonic
 * 
 * "mov" -> {"mov", 0x0, 2, "012345", "12345" }
 * 
 * \param mnemonic	string of mnemonic
 * \return			operation or NULL
 */
operation_t * get_operation(char * mnemonic) {
    int i;

    /* operations is an array defined in opcodes.c
	   so we don't have to copy the value, just get the address of the struct
	   this way we dont have to worry about freeing */
    for (i = 0; i < 16; i++) {
        if (strcmp(operations[i].mnemonic, mnemonic) == 0) {
            return &operations[i];
        }
    }

    return NULL;
}

/*!
 * \brief gets the struct of the addresssing from the operand
 * 
 * "#1" -> { INSTANT, 1 }
 * 
 * \param operand	string of operand
 * \return			addressing or NULL
 */
addressing_t * get_addressing(char * operand) {
    if (!operand) {
        return NULL;
    }

    /* addressings is an array defined in opcodes.c
	   so we don't have to copy the value, just get the address of the struct
       this way we dont have to worry about freeing */
    if (operand[0] == '#') {
        return is_valid_numeric_literal(operand, 1) ? &addressings[INSTANT]
                                                    : NULL;
    } else if (operand[0] == '@') {
        if (is_valid_label_name(operand, 1, 0)) {
            return &addressings[INDIRECT];
        } else if (is_valid_register_name(operand, 1)) {
            return &addressings[INDIRECT_REGISTER];
        } else {
            return NULL;
        }
    } else if (is_valid_label_name(operand, 0, 0)) {
        return &addressings[DIRECT];
    } else if (is_valid_register_name(operand, 0)) {
        return &addressings[DIRECT_REGISTER];
    } else {
        return NULL;
    }
}

/*!
 * \brief creates the 16-bit machine word from an instruction struct
 * 
 * \param instruction	instruction struct
 * \return				16-bit machine word
 */
uint16_t instruction_to_word(instruction_t instruction) {
    uint16_t word = 0;
    word |= (instruction.op & 0xF) << 12; /* bits: 15-12 */
    word |= (instruction.src_addr & 0x7) << 9; /* bits: 11-9 */
    word |= (instruction.src_reg & 0x7) << 6; /* bits: 8-6 */
    word |= (instruction.dest_addr & 0x7) << 3; /* bits: 5-3 */
    word |= (instruction.dest_reg & 0x7); /* bits: 0-2 */
    return word;
}
