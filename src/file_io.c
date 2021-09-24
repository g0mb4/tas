/*!
 * \file file_io.c
 * \brief file Input/Output functions
 */

#include "asm.h"

/* global variables */
extern object_code_t g_object_code[TABLE_SIZE];
extern uint16_t g_object_code_size;

extern uint16_t g_data_image_size;

extern symbol_t g_symbol_table[TABLE_SIZE];
extern uint16_t g_symbol_table_size;

extern link_object_t g_link_table[TABLE_SIZE];
extern uint16_t g_link_table_size;

extern link_object_t g_external_table[TABLE_SIZE];
extern uint16_t g_external_table_size;

/*!
 * \brief gets the base name from the path of the file
 * 
 * "C:\dir\file.txt" -> "file.txt"<br>
 * "/opt/dir/file.txt" -> "file.txt"
 * 
 * \param path	path of the file
 * \return		base name or NULL
 */
char * get_file_base_name(const char * path) {
    if (path == NULL) {
        return NULL;
    }

    char * file_name = (char *)path;
    char * curr = (char *)path;
    for (; *curr != '\0'; curr++) {
        /* step the pointer at a director separator */
        if (*curr == '/' || *curr == '\\') {
            file_name = curr + 1;
        }
    }

    return file_name;
}

/*!
 * \brief gets the file name (containing the path) without extension
 * 
 * "C:\dir\file.txt" -> "C:\dir\file"<br>
 * "/opt/dir/file.txt" -> "/opt/dir/file"
 * 
 * \param path	path of the file
 * \return		path without extension or NULL
 */
char * get_file_name_no_ext(const char * path) {
    int i, len = (int)strlen(path);
    int dot_pos = -1;

    if (path) {
        /* find the last dot */
        for (i = 0; i < len; ++i) {
            if (path[i] == '.') {
                dot_pos = i;
            }
        }

        /* if path contains a dot */
        if (dot_pos != -1) {
            char * file_base_name_no_ext = (char *)malloc(dot_pos);
            if (file_base_name_no_ext) {
                strncpy(file_base_name_no_ext, path, dot_pos);
                file_base_name_no_ext[dot_pos] = 0;

                return file_base_name_no_ext;
            } else {
                return NULL;
            }
        } else {
            return (char *)path;
        }
    } else {
        return NULL;
    }
}

/*!
 * \brief creates an ascii base16 object file
 * 
 * \param file_name		    name of the source file
 * \return				    number of errors
 */
uint16_t create_object_file(const char * file_name) {
    char * file_name_no_ext = get_file_name_no_ext(file_name);
    FILE * fp = NULL;
    uint16_t i;
    uint16_t errors = 0;

    if (!file_name_no_ext) {
        return 1;
    }

    char * object_name = (char *)malloc(strlen(file_name_no_ext) + 3 + 1); /* ".oc" + NULL */

    if (object_name) {
        strcpy(object_name, file_name_no_ext);
        strcat(object_name, ".oc");

        fp = fopen(object_name, "w");

        if (fp) {
            fprintf(fp, ".cbegin\n");
            /* header: length_of_the_instructions length_of_the_data */
            fprintf(fp, "%x %x\n", g_object_code_size - g_data_image_size, g_data_image_size);
            for (i = 0; i < g_object_code_size; ++i) {
                object_code_t * o = &g_object_code[i];
                /* object code: address machine_word type */
                fprintf(fp, "%04x %04x %c\n", i, o->value, o->type);
            }
            fprintf(fp, ".cend\n");
            fprintf(fp, ".lbegin\n");
            for (i = 0; i < g_link_table_size; ++i) {
                link_object_t * obj = &g_link_table[i];

                if (obj->type == 'n') {
                    /* object code: name_of_the_entry address */
                    fprintf(fp, "%s %04x\n", obj->name, obj->value);
                }
            }
            fprintf(fp, ".lend\n");
            fprintf(fp, ".ebegin\n");
            for (i = 0; i < g_external_table_size; ++i) {
                link_object_t * obj = &g_external_table[i];
                /* object code: name_of_the_entry address */
                fprintf(fp, "%s %04x\n", obj->name, obj->value);
            }
            fprintf(fp, ".eend\n");
            fclose(fp);
        } else {
            errors++;
        }
    } else {
        errors++;
    }

    free(object_name);
    return errors;
}

/*!
 * \brief creates a binary file from the object code
 *
 * \param file_name		file name of the source file
 * \return				number of errors
 */
uint16_t create_binary_file(const char * file_name) {
    char * file_name_no_ext = get_file_name_no_ext(file_name);
    FILE * fp = NULL;
    uint16_t i;
    uint16_t errors = 0;

    if (!file_name_no_ext) {
        return 1;
    }

    char * binary_name = (char *)malloc(strlen(file_name_no_ext) + 4 + 1); /* ".bin" + NULL */

    if (binary_name) {
        strcpy(binary_name, file_name_no_ext);
        strcat(binary_name, ".bin");

        fp = fopen(binary_name, "wb"); /* write binary */

        if (fp) {
            for (i = 0; i < g_object_code_size; ++i) {
                object_code_t * o = &g_object_code[i];
                if (fwrite(&o->value, sizeof(uint16_t), 1, fp) != 1) {
                    errors++;
                    break;
                }
            }
            fclose(fp);
        } else {
            errors++;
        }
    } else {
        errors++;
    }

    free(binary_name);
    return errors;
}
