/*!
 * \file first_pass.c
 * \brief first pass of the assembling 
 * 
 * The goal is to check the syntax, get the addresses of the symbols and fill the tables as complete as its possible.
 * 
 * Simplified algorithm:
 * 1. instruction counter(IC) = 0, data counter(DC) = 0
 * 2. read line
 * 3. if no lines left, done
 * 4. if first column is a symbol, get parameter of the symbol:
 *    - if operation: add to symbol table, value = IC, get next column, proceed
 *	  - if .data/.string: add to the symbol table, value = DC, get next column, proceed
 *    - else proceed
 * 5. process column:
 *    - .data/.string: add data to data image, DC += words added
 *	  - .entry/.extern: add symbol to link table, value = IC
 *    - instruction: get the addressing mode, get length(L), add instruction word to object code, IC += L
 * 6. goto 2
 */

#include "asm.h"

/* global variables */
extern object_code_t g_object_code[TABLE_SIZE];
extern uint16_t g_object_code_size;

extern uint16_t g_data_image[TABLE_SIZE];
extern uint16_t g_data_image_size;

extern symbol_t g_symbol_table[TABLE_SIZE];
extern uint16_t g_symbol_table_size;

extern link_object_t g_link_table[TABLE_SIZE];
extern uint16_t g_link_table_size;

/* static variables, used "globally" trhough first pass */
/* error macros need them */
static uint32_t line_number; /* current line number of the source code */
static char * file_base_name; /* name of the source file */
static int errors; /* number of errors though first pass  */

/*!
 * \brief adding symbol to the table
 * 
 * \param s	symbol to be added
 */
#define ADD_SYM(s)                                   \
    if (g_symbol_table_size >= TABLE_SIZE) {         \
        ERROR("symbol table is full");               \
    } else {                                         \
        g_symbol_table[g_symbol_table_size++] = (s); \
    }

/*!
* \brief adding data to the data image
* 
* \param d 16-bit data word
*/
#define ADD_DATA(d)                              \
    if (g_data_image_size >= TABLE_SIZE) {       \
        ERROR("data image is full");             \
    } else {                                     \
        g_data_image[g_data_image_size++] = (d); \
    }

/*!
 * \brief adding instruction to the object code
 * 
 * \param i 16-bit machine word 
 */
#define ADD_OBJECT_CODE(i)                       \
    if (g_object_code_size >= TABLE_SIZE) {      \
        ERROR("object code is full");            \
    } else {                                     \
        object_code_t o;                         \
        o.value = (i);                           \
        o.type = 'a';                            \
        g_object_code[g_object_code_size++] = o; \
    }

/*!
 * \brief adding placeholder data to the object code
 */
#define ADD_DUMMY_WORD()                         \
    if (g_object_code_size >= TABLE_SIZE) {      \
        ERROR("object code is full");            \
    } else {                                     \
        object_code_t o;                         \
        o.value = 0xFFFF;                        \
        o.type = '?';                            \
        g_object_code[g_object_code_size++] = o; \
    }

/*!
  * \brief adding link object to its table
  */
#define ADD_LINK_OBJECT(o)                       \
    if (g_link_table_size >= TABLE_SIZE) {       \
        ERROR("link table is full");             \
    } else {                                     \
        g_link_table[g_link_table_size++] = (o); \
    }

/*!
 * \brief main function of the first pass
 * 
 * \param file_name		path of the source file
 * \return				number of errors during first pass
 */
uint16_t first_pass(const char * file_name) {
    FILE * fp;
    char line[256];

    /* initialise the variables */
    file_base_name = get_file_base_name(file_name);
    line_number = 1;
    errors = 0;

    fp = fopen(file_name, "r");
    if (fp == NULL) {
        ERROR("unable to open '%s'", file_name);
        return 1;
    }

    while (fgets(line, sizeof(line), fp) != NULL) {
        if (strlen(line) > 80) {
            WARN("line is longer than 80 characters");
        }
        /* remove whitespaces, tabs, spaces after commas and comments */
        char * clean = clean_line(line);

        if (!clean) {
            ERROR("unable to clean the line: %s", line);
        } else {
            if (strlen(clean) == 0) {
                /* empty line  */
            } else if (clean[0] == ';') {
                /* comment */
            } else {
                first_process_line(clean, 0);
            }
        }

        free(clean);

        line_number++;
    }

    fclose(fp);

    return errors;
}

/*!
 * \brief process a line during the first pass
 * 
 * \param line			line to process
 * \param column_index	starting column index
 */
void first_process_line(char * line, int column_index) {
    char * col_str = string_split(line, " ", column_index); /* split te line into columns, and get the desired one */
    column_t col = column_type(col_str); /* get the type of the column */

    /* process the column */
    switch (col) {
    case LABEL:
        first_process_label(line);
        break;

    case DIRECTIVE_ENTRY:
        first_process_entry(line, column_index + 1);
        break;

    case DIRECTIVE_EXTERN:
        first_process_extern(line, column_index + 1);
        break;

    case DIRECTIVE_NUMBER:
        first_process_numbers(line, column_index + 1);
        break;

    case DIRECTIVE_STRING:
        first_process_string(line, column_index + 1);
        break;

    case OPERATION:
        first_process_operation(line, column_index);
        break;

    case UNKNOWN:
    default:
        ERROR("unknown column type: %s", col_str);
    }

    free(col_str);
}

/*!
 * \brief process a line starting with a label during the first pass
 * 
 * \param line	line starting with the label
 */
void first_process_label(char * line) {
    char * label = string_split(line, " ", 0); /* get the label, first column of the line */
    int len = (int)strlen(label);

    label[len - 1] = '\0'; /* remove ':' */
    symbol_t sym;
    sym.name = (char *)malloc(strlen(label));

    if (!sym.name) {
        ERROR("unable to allocate memory for symbol '%s'", label);
    } else {
        strcpy(sym.name, label);

        /* get the next column */
        char * col2_str = string_split(line, " ", 1);
        column_t col2 = column_type(col2_str);

        /* decide symbol type based on the next column */
        switch (col2) {
        case OPERATION:
            sym.value = g_object_code_size; /* current position in the object code */
            sym.type = 'a'; /* absolute */

            /* add symbol, if it not defined earlier */
            if (count_table_objects_name(label, g_symbol_table, g_symbol_table_size) > 0) {
                ERROR("symbol is already defined: %s", label);
            } else {
                ADD_SYM(sym);
            }

            first_process_line(line, 1); /* recursively process the line, starting with the second column */
            break;

        case DIRECTIVE_ENTRY:
        case DIRECTIVE_EXTERN:
            WARN("label in front of a compiler directive: %s", line);
            break;

        case DIRECTIVE_NUMBER:
        case DIRECTIVE_STRING:
            sym.value = g_data_image_size; /* current position in the data image */
            sym.type = 'r'; /* relocatable */

            /* add symbol, if it not defined earlier */
            if (count_table_objects_name(label, g_symbol_table, g_symbol_table_size) > 0) {
                ERROR("symbol is already defined: %s", label);
            } else {
                ADD_SYM(sym);
            }

            first_process_line(line, 1); /* recursively process the line, starting with the second column */
            break;

        default:
            ERROR("unknown label type: %s", col2_str);
            break;
        }

        free(col2_str);
    }

    free(label);
}

/*!
 * \brief process an .entry object during the first pass
 * 
 * \note column_index must be point the column containing the label
 * 
 * \param line			line containing the label
 * \param column_index	column containing the parameter of the entry, a label
 */
void first_process_entry(char * line, int column_index) {
    char * label = string_split(line, " ", column_index);

    if (!label) {
        ERROR("expected LABEL: %s", line);
        return;
    }

    /* check if label is valid */
    if (is_valid_label_name(label, 0, 0) == false) {
        ERROR("invalid LABEL: %s", label);
    } else {
        link_object_t obj;
        obj.name = (char *)malloc(strlen(label) + 1);
        if (!obj.name) {
            ERROR("unable to allocate memory for link object: %s", line);
        } else {
            strcpy(obj.name, label); /* set the name */
            obj.value = 0xFFFF; /* it does not matter */
            obj.type = 'n'; /* entry */

            ADD_LINK_OBJECT(obj); /* add it to the table */
        }
    }

    free(label);
}

/*!
 * \brief process an .extern object during the first pass
 * 
 * \note column_index must be point the column containing the label
 * 
 * \param line			line containing the label
 * \param column_index	column containing the parameter of the extern, a label
 */
void first_process_extern(char * line, int column_index) {
    char * label = string_split(line, " ", column_index);

    if (!label) {
        ERROR("expected LABEL: %s", line);
        return;
    }

    /* check if label is valid */
    if (is_valid_label_name(label, 0, 0) == false) {
        ERROR("invalid LABEL: %s", label);
    } else {
        link_object_t obj;
        obj.name = (char *)malloc(strlen(label) + 1);

        if (!obj.name) {
            ERROR("unable to allocate memory for link object: %s", line);
        } else {
            strcpy(obj.name, label); /* set the name */
            obj.value = 0xFFFF; /* it does not matter */
            obj.type = 'e'; /* extern */

            ADD_LINK_OBJECT(obj); /* add it to the table */
        }
    }

    free(label);
}

/*!
 * \brief process a .data object during the first pass
 * 
 * \note column_index must be point the column containing the number/list of numbers
 * 
 * \param line			line containing the number/numbers
 * \param column_index	column containing the parameter of the data, a number/list of numbers
 */
void first_process_numbers(char * line, int column_index) {
    char * list = string_split(line, " ", column_index);
    int i = 0;

    if (!list) {
        ERROR("expected numbers, got: %s", line);
        return;
    }

    char * number =
        string_split(list, ",", i); /* get the first number from the list */
    /* while there is numbers */
    while (number) {
        /* check if number is valid */
        if (is_valid_numeric_literal(number, 0) == false) {
            ERROR("not a valid numeric literal: '%s'", number);
            return;
        }

        uint16_t val = get_number(number, 0); /* get the value */

        ADD_DATA(val); /* add the value to the data image */

        free(number);
        number = string_split(list, ",", ++i); /* get the next number in the list */
    }
    free(list);
}

/*!
 * \brief process a .string object during the first pass
 * 
 * \note column_index must be point the column containing the string
 * 
 * \param line			line containing the string
 * \param column_index	column containing the parameter of the string, a string (what a surprise)
 */
void first_process_string(char * line, int column_index) {
    char * string = string_split(line, " ", column_index);
    uint32_t i = 1;

    /* check if the parameter is a valid string literal */
    if (string[0] != '"') {
        ERROR("not a valid string literal: '%s'", string);
    } else {
        /* copy the contents of the literal into tha data image */
        while (string[i] && string[i] != '"') {
            uint16_t val = (uint16_t)string[i];

            ADD_DATA(val);

            i++;
        }

        ADD_DATA(0); /* add terminating NULL */

        if (i != strlen(string) - 1) {
            WARN("unclosed string literal: '%s'", string);
        }
    }

    free(string);
}

/*!
 * \brief process an operation object during the first pass
 * 
 * \note column_index must be point the column containing the operation, NOT the operands
 * 
 * \param line			line containing the operation
 * \param column_index	column containing the operation, NOT the operands
 */
void first_process_operation(char * line, int column_index) {
    char * operation = string_split(line, " ", column_index); /* get the operation */
    char * operands = string_split(line, " ", column_index + 1); /* get the operands */
    char * operand1 = string_split(operands, ",", 0); /* get the 1st operand */
    char * operand2 = string_split(operands, ",", 1); /* get the 2nd operand */

    operation_t * op = get_operation(operation); /* identify the operation */

    if (!operation || !op) {
        ERROR("invalid operation: %s", line);
    } else {
        /* count the number of the operands */
        uint8_t number_of_operands = 0;
        number_of_operands += operand1 ? 1 : 0;
        number_of_operands += operand2 ? 1 : 0;

        if (number_of_operands != op->operands) {
            ERROR("wrong number of operands at '%s', expected %u, got %u",
                  operation, op->operands, number_of_operands);
        } else {
            switch (op->operands) {
            /* operations with no operands */
            case 0: {
                uint16_t inst = first_create_instruction(op, NULL, NULL); /* create instruction from operation */

                ADD_OBJECT_CODE(inst); /* add instruction to the object code */
            } break;

            /* operations with 1 operand */
            case 1: {
                addressing_t * dest_mode = get_addressing(operand1); /* get the addressing mode of the operand */

                /* check if addressing is valid for this operation */
                if (is_valid_addressing(op, dest_mode, 1) == false) {
                    ERROR("wrong destination addressing mode '%s'", operand1);
                } else {
                    uint16_t inst = first_create_instruction(op, NULL, operand1); /* create instruction from operation */

                    ADD_OBJECT_CODE(inst); /* add instruction to the object code */
                    /* if addressing requires an additional word */
                    if (dest_mode->add_word) {
                        ADD_DUMMY_WORD(); /* add placeholder to the object code */
                    }
                }
            } break;

            /* operations with 2 operands */
            case 2: {
                addressing_t * src_mode = get_addressing(operand1); /* get the addressing mode of the 1st operand */
                addressing_t * dest_mode = get_addressing(operand2); /* get the addressing mode of the 2nd operand */

                /* check if 1st addressing is valid for this operation */
                if (is_valid_addressing(op, src_mode, 0) == false) {
                    ERROR("wrong source addressing mode '%s'", operand1);
                } else {
                    /* check if 2nd addressing is valid for this operation */
                    if (is_valid_addressing(op, dest_mode, 1) == false) {
                        ERROR("wrong destination addressing mode '%s", operand2);
                        return;
                    } else {
                        uint16_t inst = first_create_instruction(op, operand1, operand2); /* create instruction from operation */

                        ADD_OBJECT_CODE(inst); /* add instruction to the object code */

                        /* if 1st addressing requires an additional word */
                        if (src_mode->add_word) {
                            ADD_DUMMY_WORD(); /* add placeholder to the object code */
                        }
                        /* if 2nd addressing requires an additional word */
                        if (dest_mode->add_word) {
                            ADD_DUMMY_WORD(); /* add placeholder to the object code */
                        }
                    }
                }
            } break;
            }
        }
    }

    /* free resources */
    if (operand1) {
        free(operand1);
    }

    if (operand2) {
        free(operand2);
    }

    if (operands) {
        free(operands);
    }

    free(operation);
}

/*!
 * \brief creates the 16-bit instruction word from the operation and the operand(s)
 * 
 * \param op	operation
 * \param src	source operand
 * \param dest	destination operand
 * \return		16-bit instruction word
 */
uint16_t first_create_instruction(operation_t * op, char * src, char * dest) {
    instruction_t inst;

    inst.op = op->opcode;

    switch (op->operands) {
    /* operations with no operands */
    case 0:
        /* only the opcode matters, everithing else is 0 */
        inst.src_addr = 0;
        inst.src_reg = 0;
        inst.dest_addr = 0;
        inst.dest_reg = 0;

        break;

    /* operations with 1 operand */
    case 1: {
        /* if addressing is register addressing, need to fill the required fields */
        addressing_t * dest_mode = get_addressing(dest);

        inst.src_addr = 0;
        inst.src_reg = 0;
        inst.dest_addr = dest_mode->mode; /* set the mode */
        /* get the register, if needed */
        switch (dest_mode->mode) {
        case DIRECT_REGISTER:
            inst.dest_reg = get_register(dest, 0); /* rx */
            break;
        case INDIRECT_REGISTER:
            inst.dest_reg = get_register(dest, 1); /* @rx */
            break;
        default:
            inst.dest_reg = 0;
        }
    }

    break;

    /* operations with 2 operands */
    case 2: {
        /* if addressing is register addressing, need to fill the required fields */
        addressing_t * src_mode = get_addressing(src);
        addressing_t * dest_mode = get_addressing(dest);

        inst.src_addr = src_mode->mode; /* set the mode */
        /* get the register, if needed */
        switch (src_mode->mode) {
        case DIRECT_REGISTER:
            inst.src_reg = get_register(src, 0); /* rx */
            break;
        case INDIRECT_REGISTER:
            inst.src_reg = get_register(src, 1); /* @rx */
            break;
        default:
            inst.src_reg = 0;
        }

        inst.dest_addr = dest_mode->mode; /* set the mode */
        /* get the register, if needed */
        switch (dest_mode->mode) {
        case DIRECT_REGISTER:
            inst.dest_reg = get_register(dest, 0); /* rx */
            break;
        case INDIRECT_REGISTER:
            inst.dest_reg = get_register(dest, 1); /* @rx */
            break;
        default:
            inst.dest_reg = 0;
        }
    }

    break;
    }

    return instruction_to_word(inst); /* create the 16-bit instruction word from struct */
}
