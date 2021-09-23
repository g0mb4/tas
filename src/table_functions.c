/*!
 * \file table_functions.c
 * \brief table operations
 */

#include "asm.h"

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