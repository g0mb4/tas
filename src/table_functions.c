/*!
 * \file table_functions.c
 * \brief table operations
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

/*!
 * \brief counts a symbol/link_object based on its type in a table
 *
 * \param type		type to be counted
 * \param table		table of symbols/link objects
 * \param len		length of the table
 * \return			number of elements
 */
uint16_t count_table_objects_type(char type, link_object_t * table, uint16_t len) {
    uint16_t i, ret = 0;

    for (i = 0; i < len; i++) {
        if (table[i].type == type) {
            ret++;
        }
    }

    return ret;
}

/*!
 * \brief counts a symbol/link_object based on its name in a table
 * 
 * \param name		name to be counted
 * \param table		table of symbols/link objects
 * \param len		length of the table
 * \return			number of elements
 */
uint16_t count_table_objects_name(char * name, link_object_t * table, uint16_t len) {
    uint16_t i, ret = 0;

    for (i = 0; i < len; i++) {
        if (strcmp(table[i].name, name) == 0) {
            ret++;
        }
    }

    return ret;
}

/*!
 * \brief prints the symbol table
 */
void print_sym_table(void) {
    uint16_t i;

    printf("\nTable of symbols (name address type):\n");
    for (i = 0; i < g_symbol_table_size; i++) {
        symbol_t * sym = &g_symbol_table[i];
        printf("  %-10s %04x %c\n", sym->name, sym->value, sym->type);
    }
}

/*!
 * \brief prints the link table
 */
void print_link_table(void) {
    uint16_t i;

    printf("\nTable of link objects (name addr type):\n");
    for (i = 0; i < g_link_table_size; i++) {
        link_object_t * obj = &g_link_table[i];
        printf("  %-10s %04x %c\n", obj->name, obj->value, obj->type);
    }
}

/*!
 * \brief prints the extern table
 */
void print_extern_table(void) {
    uint16_t i;

    printf("\nTable of externals (name address):\n");
    for (i = 0; i < g_external_table_size; i++) {
        link_object_t * obj = &g_external_table[i];
        printf("  %-10s %04x\n", obj->name, obj->value);
    }
}

/*!
 * \brief prints the data image
 */
void print_data_image(void) {
    uint16_t i;

    printf("\nContent of the data image (address value):\n");
    for (i = 0; i < g_data_image_size; i++) {
        printf("  %04x %04x\n", i, g_data_image[i]);
    }
}

/*!
 * \brief prints the object code
 */
void print_object_code(void) {
    uint16_t i;

    printf("\nContent of the object code (address value type):\n");
    for (i = 0; i < g_object_code_size; i++) {
        object_code_t * o = &g_object_code[i];
        printf("  %04x %04x %c\n", i, o->value, o->type);
    }
}
