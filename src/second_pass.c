/*!
 * \file second_pass.c
 * \brief second pass of the assembling
 *
 * The goal is to complete the tables and create the full object code and link objects.
 * 
 * Simplified algorithm:
 * 1. instruction counter(IC) = 0
 * 2. read line
 * 3. if no lines left, done
 * 4. process column:
 *    - label, get next column, goto 4
 *    - .data/.string/.entry/.extern: goto 2
 *    - instruction:
 *		-# if operand is symbol, get value from symtable, complete object code at IC
 *		-# get the addressing mode, get length(L), add instruction word to object code, IC += L
 * 5. goto 2
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

extern link_object_t g_external_table[TABLE_SIZE];
extern uint16_t g_external_table_size;

/* static variables, used "globally" trhough the second pass */
/* error macros need them */
static uint32_t line_number; /* current line number of the source code */
static char * file_base_name; /* name of the source file */
static int errors; /* number of errors though second pass  */

static uint16_t s_object_code_size;
static uint16_t s_object_code_first_size;

/*!
 * \brief icrease the obcject code counter, to keep track of position
 * 
 */
#define ADD_DUMMY_OBJECT_CODE() s_object_code_size++;
/*!
 * \brief adds an object word with type to the table
 * 
 * \param w	16-bit object word
 * \param t type
 */
#define ADD_OBJECT_WORD(w, t)                    \
    {                                            \
        object_code_t o;                         \
        o.value = (w);                           \
        o.type = (t);                            \
        g_object_code[s_object_code_size++] = o; \
    }
/*!
 * \brief adds an external object to the external table
 * 
 * \param e external object
 */
#define ADD_EXTERNAL(e) g_external_table[g_external_table_size++] = (e);

/*!
 * \brief main function of the second pass 
 * 
 * \param file_name		path of the source file
 * \return				number of errors during second pass
 */
uint16_t second_pass(const char * file_name) {
    FILE * fp;
    char line[128];

    /* initialise variables */
    file_base_name = get_file_base_name(file_name);
    line_number = 1;
    errors = 0;

    s_object_code_first_size = g_object_code_size;

    fp = fopen(file_name, "r");
    if (fp == NULL) {
        ERROR_F("unable to open '%s'", file_name);
        goto exit;
    }

    /* update the tables */
    second_update_tables();

    while (fgets(line, sizeof(line), fp) != NULL) {
        /* remove whitespaces, tabs, spaces after commas and comments */
        char * clean = clean_line(line);

        if (!clean) {
            ERROR_F("unable to clean the line: %s", line);
        } else {
            if (strlen(clean) == 0) {
                /* empty line  */
            } else if (clean[0] == ';') {
                /* comment */
            } else {
                second_process_line(clean, 0);
            }
        }

        free(clean);

        line_number++;
    }

    fclose(fp);

    g_object_code_size = s_object_code_size + g_data_image_size; /* object code and data image had been merged */

exit:
    return errors;
}

/*!
 * \brief update te tables after the first pass
 * 
 */
void second_update_tables(void) {
    uint32_t i, j;

    /* update data locations */
    for (i = 0; i < g_symbol_table_size; i++) {
        if (g_symbol_table[i].type == 'r') {
            g_symbol_table[i].value += s_object_code_first_size;
        }
    }

    /* update extern/entry labels */
    for (i = 0; i < g_symbol_table_size; i++) {
        for (j = 0; j < g_link_table_size; j++) {
            if (strcmp(g_symbol_table[i].name, g_link_table[j].name) == 0) {
                switch (g_link_table[i].type) {
                /* extern */
                case 'e':
                    g_symbol_table[i].type = 'e';
                    g_link_table[j].value = g_symbol_table[i].value;
                    break;

                /* entry */
                case 'n':
                    g_link_table[j].value = g_symbol_table[i].value;
                    break;
                }
            }
        }
    }

    /* append data to object code */
    for (i = 0, j = s_object_code_first_size; i < g_data_image_size; i++, j++) {
        object_code_t o;
        o.value = g_data_image[i];
        o.type = ' ';
        g_object_code[j] = o;
    }
}

/*!
 * \brief process a line during the second pass
 *
 * only the lines containing an operation are processed
 * 
 * \param line			line to process
 * \param column_index	starting column index
 */
void second_process_line(char * line, int column_index) {
    char * col_str = string_split(line, " ", column_index);
    column_t col = column_type(col_str);

    switch (col) {
    case LABEL:
        second_process_label(
            line); /* just because it can contain an operation */
        break;

    case DIRECTIVE_ENTRY:
    case DIRECTIVE_EXTERN:
    case DIRECTIVE_NUMBER:
    case DIRECTIVE_STRING:
        /* had been dealt with during the first pass */
        break;
    case OPERATION:
        second_process_operation(
            line,
            column_index); /* this is the main purpuse of the second pass */
        break;

    case UNKNOWN:
    default:
        ERROR_F("unknown column type: %s", col_str);
    }

    free(col_str);
}

/*!
* \brief process a line starting with a label during the second pass
 * 
 * only the lines containing an operation are processed
 * 
 * \param line
 */
void second_process_label(char * line) {
    char * col2_str = string_split(line, " ", 1);
    column_t col2 = column_type(col2_str);

    /* decide symbol type based on the next column */
    switch (col2) {
    case OPERATION:
        second_process_line(line, 1); /* process the operation */
        break;
    case DIRECTIVE_ENTRY:
    case DIRECTIVE_EXTERN:
    case DIRECTIVE_NUMBER:
    case DIRECTIVE_STRING:
        /* had been dealt with during the first pass */
        break;
    default:
        ERROR_F("unknown column type: %s", col2_str);
        break;
    }

    free(col2_str);
}

/*!
 * \brief adds an additional word if the addressing mode requires it
 * 
 * \param operand	operand
 * \param word		word to add
 * \param ext		is it external
 */
void second_add_object_word(char * operand, uint16_t word, bool ext) {
    addressing_t * addr_mode =
        get_addressing(operand); /* get addressing mode */
    char type;

    /* check if addressing mode requires the additional word */
    switch (addr_mode->mode) {
    case INSTANT:
        type = 'a'; /* absolute */
        ADD_OBJECT_WORD(word, type); /* add the word to the object code */
        break;
    case DIRECT:
    case INDIRECT:
        type = ext ? 'e' : 'r'; /* extern | reallocatable */
        ADD_OBJECT_WORD(word, type); /* add the word to the object code */
        break;
    case DIRECT_REGISTER:
    case INDIRECT_REGISTER:
        /* noting to do */
        break;
    }
}

/*!
 * \brief adds an external symbol to the external table 
 * 
 * \param operand label marked as external
 */
void second_add_external(char * operand) {
    addressing_t * addr_mode = get_addressing(operand);
    char * real_symbol = (char *)malloc(strlen(operand) + 1);
    int start_index = 0;
    link_object_t obj;

    /* get tel label name based on the addressing */
    switch (addr_mode->mode) {
    case DIRECT:
        start_index = 0; /* LABEL */
        break;
    case INDIRECT:
        start_index = 1; /* @LABEL */
        break;
    case INSTANT:
    case DIRECT_REGISTER:
    case INDIRECT_REGISTER:
    default:
        ERROR_F("expected EXTERN LABEL with DIRECT|INDIRECT addressing, got: "
                "%s",
                operand);
        return;
    }

    if (!real_symbol) {
        ERROR_F("cannot allocate memory for symbol: %s", operand);
        return;
    }

    strcpy(real_symbol, operand + start_index);

    obj.name = (char *)malloc(strlen(real_symbol) + 1);
    if (!obj.name) {
        ERROR_F("unable ot allocate memory for external symbol: %s",
                real_symbol);
    } else {
        strcpy(obj.name, real_symbol);
        obj.type = 'e';
        obj.value = s_object_code_size;

        ADD_EXTERNAL(obj); /* edd external object */
    }
}

/*!
 * \brief processes a line containing an operation during second pass
 * 
 * column_index must be point the column containing the operation, NOT the operands
 * 
 * \param line			line containing the operation
 * \param column_index	column containing the operation, NOT the operands
 */
void second_process_operation(char * line, int column_index) {
    char * operation =
        string_split(line, " ", column_index); /* get the operation */
    char * operands =
        string_split(line, " ", column_index + 1); /* get the operands */
    char * operand1 = string_split(operands, ",", 0); /* get the 1st operand */
    char * operand2 = string_split(operands, ",", 1); /* get the 2nd operand */

    uint16_t word1 = 0, word2 = 0;
    bool ext1 = false, ext2 = false;

    operation_t * op = get_operation(operation); /* identify the operation */

    if (!operation || !op) {
        ERROR_F("invalid operation: %s", line);
    } else {
        switch (op->operands) {
        /* operations with no operands */
        case 0:
            /* nothing to do */
            ADD_DUMMY_OBJECT_CODE(); /* step position */
            break;

        /* operations with 1 operand */
        case 1:
            second_create_words(op, NULL, operand1, NULL, &word1, NULL,
                                &ext1); /* create the additional words */

            ADD_DUMMY_OBJECT_CODE(); /* step position */

            /* if operand is external */
            if (ext1) {
                second_add_external(
                    operand1); /* add it to the external table */
            }
            second_add_object_word(operand1, word1,
                                   ext1); /* add word to the object code */

            break;

        case 2:
            second_create_words(op, operand1, operand2, &word1, &word2, &ext1,
                                &ext2); /* create the additional words */

            ADD_DUMMY_OBJECT_CODE(); /* step position */

            /* if operand is external */
            if (ext1) {
                second_add_external(
                    operand1); /* add it to the external table */
            }
            second_add_object_word(operand1, word1,
                                   ext1); /* add word to the object code */

            /* if operand is external */
            if (ext2) {
                second_add_external(
                    operand2); /* add it to the external table */
            }
            second_add_object_word(operand2, word2,
                                   ext2); /* add word to the object code */

            break;
        }
    }

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
 * \brief creates a mechine word based on the operands
 * 
 * \param operand	operand
 * \param ext		set this if operand is extern
 * \return			16-bit machine word
 */
uint16_t second_get_word(char * operand, bool * ext) {
    addressing_t * addr_mode = get_addressing(operand);

    switch (addr_mode->mode) {
    case INSTANT:
        *ext = false; /* can't be external */
        return get_number(operand, 1); /* get the numeric value */
    case DIRECT:
        return second_get_symbol_value(operand, 0, ext); /* LABEL */
    case INDIRECT:
        return second_get_symbol_value(operand, 1, ext); /* @LABEL */
    /* code could not reach this point */
    case DIRECT_REGISTER:
    case INDIRECT_REGISTER:
    default:
        return 0;
    }
}

/*!
 * \brief creates the additional word(s) for the operations, if needed
 * 
 * \param op	operation
 * \param src	source operand
 * \param dest	destination operand
 * \param word1	created word1
 * \param word2	created word2
 * \param ext1	set this if word1 is extern
 * \param ext2	set this if word2 is extern
 */
void second_create_words(operation_t * op, char * src, char * dest,
                         uint16_t * word1, uint16_t * word2, bool * ext1,
                         bool * ext2) {
    switch (op->operands) {
    /* operations with no operands */
    case 0:
        /* noting to do */
        break;
    /* operations with 1 operand */
    case 1:
        *word2 = second_get_word(
            dest, ext2); /* only the destination operands are valid */
        break;
    /* operations with 2 operands */
    case 2:
        *word1 = second_get_word(src, ext1);
        *word2 = second_get_word(dest, ext2);
        break;
    }
}

/*!
 * \brief gets the value (address) of a symbol from the table
 * 
 * start_index is there, because we want to apply the same function to "LABEL" and "@LABEL"
 * 
 * \param symbol		string containing the name of the symbol
 * \param start_index	start of the name in the input string
 * \param ext			set if symbol si external
 * \return				value of the symbol
 */
uint16_t second_get_symbol_value(char * symbol, int start_index, bool * ext) {
    uint32_t i;

    char * real_symbol = (char *)malloc(strlen(symbol) + 1);
    if (!real_symbol) {
        ERROR_F("cannot allocate memory for symbol: %s", symbol);
        return 0;
    }

    strcpy(real_symbol, symbol + start_index);

    /* search in the symbol table */
    for (i = 0; i < g_symbol_table_size; i++) {
        symbol_t * sym = &g_symbol_table[i];
        if (strcmp(sym->name, real_symbol) == 0) {
            *ext = false; /* not an external symbol */
            return sym->value; /* get the value */
        }
    }

    /* search in the extern table */
    for (i = 0; i < g_link_table_size; i++) {
        link_object_t * obj = &g_link_table[i];
        if (strcmp(obj->name, real_symbol) == 0) {
            if (obj->type == 'e') {
                *ext = true; /* external symbol*/
                return 0xFFFF; /* value does not matter */
            }
        }
    }

    free(real_symbol);
    ERROR_F("symbol is not defined and not external: %s", real_symbol);
    return 0xFFFF;
}
