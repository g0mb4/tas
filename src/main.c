/*!
 * \file main.c
 * \brief entry point of the program
 * 
 * toy two pass assembler
 * 2020-2021, gmb
 */

#include "asm.h"

/* global tables, zero initialized  */
object_code_t g_object_code[TABLE_SIZE]; /*!< \brief object code */
uint16_t g_object_code_size = 0; /*!< \brief size of the object code */

uint16_t g_data_image[TABLE_SIZE]; /*!< \brief data image */
uint16_t g_data_image_size = 0; /*!< \brief size of the data image */

symbol_t g_symbol_table[TABLE_SIZE]; /*!< \brief symbol table */
uint16_t g_symbol_table_size = 0; /*!< \brief size of the symbol table */

link_object_t g_link_table[TABLE_SIZE]; /*!< \brief linker table */
uint16_t g_link_table_size = 0; /*!< \brief size of the linker table */

link_object_t g_external_table[TABLE_SIZE]; /*!< \brief table of externals */
uint16_t g_external_table_size; /*!< \brief size of the table of externals */

/* private variables */
static bool s_list_tables = false; /*!< \brief flag of table listing */
static bool s_no_output = false; /*!< \brief flag of no output */
static bool s_binary_out = false; /*!< \brief flag of binary output file */

/*!
 * \brief usage string
 * 
 */
const char * help = "toy two pass assembler by gmb\n\n"
                    "usage: tas <options> source-file\n\n"
                    "options:\n"
                    "  -l : prints debugging lists after each pass\n"
                    "  -n : creates NO output files\n"
                    "  -b : creates binary output file\n"
                    "  -h : shows this text\n";

/*!
 * \brief entry point of the application
 * 
 * \param argc	argument count
 * \param argv	argument values
 * \return		error code
 */
int main(int argc, char * argv[]) {
    int a;
    char * file_name = NULL;

    /*ther must be at lesast 2 argument (tas + source) */
    if (argc < 2) {
        printf("%s", help);
        return 1;
    }

    /* get command line switches */
    for (a = 0; a < argc; a++) {
        if (argv[a][0] == '-') {
            switch (argv[a][1]) {
            case 'l':
                s_list_tables = true;
                break;

            /* no output file */
            case 'n':
                s_no_output = true;
                break;

            case 'b':
                s_binary_out = true;
                break;

            case 'h':
                printf("%s", help);
                return 0;
            }
        } else {
            file_name = argv[a];
        }
    }

    /* do the first pass */
    uint16_t errors = first_pass(file_name);

    /* if pass was succesfull */
    if (errors == 0) {
        /* show partial results if flag is set */
        if (s_list_tables) {
            printf("\n--- Results of the first pass:\n");
            print_sym_table();
            print_link_table();
            print_data_image();
            print_object_code();
        }
        /* if not, exit */
    } else {
        fprintf(stderr, "first pass failed with %u error(s)\n", errors);
        return 2;
    }

    /* do the second pass */
    errors = second_pass(file_name, g_symbol_table, &g_symbol_table_size, g_data_image, &g_data_image_size,
                         g_object_code, &g_object_code_size, g_link_table, &g_link_table_size,
                         g_external_table, &g_external_table_size);

    /* if pass was succesfull */
    if (errors == 0) {
        /* show partial results if flag is set */
        if (s_list_tables) {
            printf("\n--- Results of the second pass:\n");
            print_sym_table();
            print_link_table();
            print_extern_table();
            print_object_code();
        }
        /* if not, exit */
    } else {
        fprintf(stderr, "second pass failed with %u error(s)\n", errors);
        return 3;
    }

    /* if output is desired */
    if (s_no_output == false) {
        if (s_binary_out) {
            if (g_external_table_size > 0) {
                fprintf(stderr, "unable to create binary file if source contains .extern-s\n");
                return 4;
            }

            errors = create_binary_file(file_name, g_object_code, g_object_code_size);
            if (errors != 0) {
                fprintf(stderr,
                        "binary file creation failed with %u error(s)\n",
                        errors);
                return 5;
            }
        } else {
            /* create object file from object code */
            errors = create_object_file(file_name, g_object_code, g_object_code_size,
                                        g_data_image_size, g_link_table, g_link_table_size,
                                        g_external_table, g_external_table_size);
            if (errors != 0) {
                fprintf(stderr, "object file creation failed with %u error(s)\n", errors);
                return 4;
            }
        }
    }

    return 0;
}
