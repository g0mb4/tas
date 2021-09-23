/*!
 * \file asm.h
 * \brief definitions for the whole project
 */

#ifndef ASM_H
#define ASM_H

/* standard headers */
#include <stdarg.h> /* for variable arguments in error()/warning() */
#include <stdbool.h> /* for bool */
#include <stdint.h> /* for uint8_t, uint16_t, ... */
#include <stdio.h> /* for displaying and file operations */
#include <stdlib.h> /* for malloc(),free() */
#include <string.h> /* for strcpy(), len(), tok() ... */

/*!
 * \brief maximum size of any table
 * 
 * \note this is the theoretical maximum size
 */
#define TABLE_SIZE 2000

/*!
 * \brief error reporting
 * 
 * \note file_base_name, line_number and errors must be defined in the file, where this macro is ussed
 * 
 * \param s	message of the error
 */
#define ERROR(s) error(file_base_name, line_number, s), errors++

/*!
 * \brief formatted error reporting
 * 
 * \note file_base_name, line_number and errors must be defined in the file, where this macro is ussed
 * 
 * \param s		message of the error (printf style format string)
 * \param ...	argument list
 */
#define ERROR_F(s, ...) \
    error(file_base_name, line_number, s, __VA_ARGS__), errors++

/*!
 * \brief warning reporting
 * 
 * \note file_base_name, line_number and errors must be defined in the file, where this macro is ussed
 * 
 * \param s	message of the error
 */
#define WARN(s) warning(file_base_name, line_number, s)

/*!
 * \brief formatted warning reporting
 * 
 * \note file_base_name, line_number and errors must be defined in the file, where this macro is ussed
 * 
 * \param s		message of the error (printf style format string)
 * \param ...	argument list
 */
#define WARN_F(s, ...) warning(file_base_name, line_number, s, __VA_ARGS__)

/*!
 * \brief possible types of a column
 */
typedef enum column_e {
    UNKNOWN = 0, /*!< unknown/error */
    LABEL, /*!< label (x:) */
    DIRECTIVE_ENTRY, /*!< entry (.entry) */
    DIRECTIVE_EXTERN, /*!< extern (.extern) */
    DIRECTIVE_NUMBER, /*!< number (.data) */
    DIRECTIVE_STRING, /*!< string (.string) */
    OPERATION /*!< mnemonic (mov, add, ...) */
} column_t;

/*!
 * \brief possible addressing modes
 */
typedef enum addressing_mode_e {
    INSTANT = 0, /*!< instant (mov #-1, r2) */
    DIRECT, /*!< direct (mov x, r2) */
    INDIRECT, /*!< indirect (mov @@x, r2) */
    DIRECT_REGISTER, /*!< direct register (mov r1, r2) */
    INDIRECT_REGISTER /*!< indirect register (mov @@r1, r2) */
} addressing_mode_t;

/*!
 * \brief description of an operation
 */
typedef struct operation_s {
    char mnemonic[4]; /*!< \brief string representation of the operartion */
    uint8_t opcode; /*!< \brief operation code */
    uint8_t operands; /*!< \brief number of operands */
    char src_legal[7]; /*!< \brief legal addressing modes for the source operand*/
    char dest_legal[7]; /*!< \brief legal addressing modes for the destination operand*/
} operation_t;

/*!
 * \brief description of an addressin
 */
typedef struct addressing_s {
    addressing_mode_t mode; /*!< \brief addressing mode */
    uint8_t add_word; /*!< \brief number of additional words (0|1) */
} addressing_t;

/*!
 * \brief description of an instruction
 */
typedef struct instruction_s {
    uint8_t op; /*!< \brief operator */
    uint8_t src_addr; /*!< \brief source addressing mode */
    uint8_t src_reg; /*!< \brief source register */
    uint8_t dest_addr; /*!< \brief destination addressing mode */
    uint8_t dest_reg; /*!< \brief destination register */
} instruction_t;

/*!
 * \brief description of a symbol
 */
typedef struct symbol_s {
    char * name; /*!< \brief label */
    uint16_t value; /*!< \brief address of the label */
    char type; /*!< \brief type ('e'xternal|'r'elocatable|'a'bsolute|(e'n'try for link object)) */
} symbol_t;

/*!
 * \brief for easier reading
 */
typedef symbol_t link_object_t;

/*!
 * \brief object code entry
 */
typedef struct object_code_s {
    uint16_t value; /*!< \brief 16-bit machine word */
    char type; /*!< \brief type ('e'xternal|'r'elocatable|'a'bsolute) */
} object_code_t;

/* error.c */
void error(const char * file_name, uint32_t line, char * fmt, ...);
void warning(const char * file_name, uint32_t line, char * fmt, ...);

/* string_functions.c */
char * string_trim(const char * str, const char * chars);
char * string_trim_end(const char * str, const char * chars);
char * string_split(const char * str, const char * delim, int index);

char * clean_line(const char * str);

/* parser.c */
bool is_valid_numeric_literal(char * str, int start_index);
bool is_valid_label_name(char * str, int start_index, int end_offset);
bool is_valid_register_name(char * str, int start_index);
bool is_valid_addressing(operation_t * op, addressing_t * addr, bool dest);

uint8_t get_register(char * str, int start_index);
uint16_t get_number(char * str, int start_index);

column_t column_type(char * str);
operation_t * get_operation(char * mnemonic);
addressing_t * get_addressing(char * operand);

uint16_t instruction_to_word(instruction_t instruction);

/* table_functions.c */
uint16_t count_table_objects_type(char type, link_object_t * table,
                                  uint16_t len);
uint16_t count_table_objects_name(char * name, link_object_t * table,
                                  uint16_t len);

/* first_pass.c */
uint16_t first_pass(const char * file_name, symbol_t * symt,
                    uint16_t * symt_ctr, uint16_t * datai, uint16_t * datai_ctr,
                    object_code_t * objectc, uint16_t * objectc_ctr,
                    link_object_t * linko, uint16_t * linko_ctr);
void first_process_line(char * line, int column_index);
void first_process_label(char * line);
void first_process_numbers(char * line, int column_index);
void first_process_string(char * line, int column_index);
void first_process_operation(char * line, int column_index);
void first_process_entry(char * line, int column_index);
void first_process_extern(char * line, int column_index);

uint16_t first_create_instruction(operation_t * op, char * src, char * dest);

/* second_pass.c */
uint16_t second_pass(const char * file_name, symbol_t * symt,
                     uint16_t * symt_ctr, uint16_t * datai,
                     uint16_t * datai_ctr, object_code_t * objectc,
                     uint16_t * objectc_ctr, link_object_t * linko,
                     uint16_t * linko_ctr, link_object_t * exts,
                     uint16_t * exts_ctr);
void second_update_tables(void);
void second_process_line(char * line, int column_index);
void second_process_label(char * line);
void second_process_operation(char * line, int column_index);

uint16_t second_get_symbol_value(char * symbol, int start_index, bool * ext);

void second_create_words(operation_t * op, char * src, char * dest,
                         uint16_t * word1, uint16_t * word2, bool * ext1,
                         bool * ext2);
uint16_t second_get_word(char * operand, bool * ext);
void second_add_object_word(char * operand, uint16_t word, bool ext);
void second_add_external(char * operand);

/* file_io.c */
char * get_file_base_name(const char * path);
char * get_file_name_no_ext(const char * file);

uint16_t create_object_file(const char * file_name, object_code_t * objectc,
                            uint16_t objectc_len, uint16_t data_len,
                            link_object_t * link_table, uint16_t link_table_len,
                            link_object_t * extern_table,
                            uint16_t extern_table_len);

uint16_t create_binary_file(const char * file_name, object_code_t * objectc,
                            uint16_t objectc_len);

#endif
