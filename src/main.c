/*!
 * \file main.c
 * \brief entry point of the program
 * 
 * toy two pass assembler
 * 2020, gmb
 */

#include "asm.h"

/* tables */
object_code_t object_code[1000]; /*!< \brief object code */
uint16_t object_ctr = 0; /*!< \brief size of the object code */

uint16_t data_image[1000]; /*!< \brief data image */
uint16_t data_ctr = 0; /*!< \brief size of the data image */

symbol_t sym_table[1000]; /*!< \brief symbol table */
uint16_t sym_ctr = 0; /*!< \brief size of the symbol table */

link_object_t link_table[1000]; /*!< \brief linker table */
uint16_t link_ctr = 0; /*!< \brief size of the linker table */

link_object_t external_table[1000]; /*!< \brief table of externals */
uint16_t external_ctr; /*!< \brief size of the table of externals */

bool list_tables = false; /*!< \brief flag of table listing */
bool no_output = false; /*!< \brief flag of no output */
bool binary_out = false; /*!< \brief flag of binary output file */

/*!
 * \brief usage string
 * 
 */
char * help = "toy two pass assembler by gmb\n\n"
              "usage: tas <options> source-file\n\n"
              "options:\n"
              "  -l : prints debugging lists after each pass\n"
              "  -n : creates NO output files\n"
              "  -b : creates binary output file\n"
              "  -h : shows this text\n";

/* prototypes */
void print_sym_table(void);
void print_data_image(void);
void print_object_code(void);
void print_link_table(void);
void print_extern_table(void);

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
                list_tables = true;
                break;

            /* no output file */
            case 'n':
                no_output = true;
                break;

            case 'b':
                binary_out = true;
                break;

            case 'h':
                printf("%s", help);
                return 0;
            }
        } else {
            file_name = argv[a];
        }
    }

    /* initialise all tables */
    memset(sym_table, 0, sizeof(sym_table));
    memset(data_image, 0, sizeof(data_image));
    memset(object_code, 0, sizeof(object_code));
    memset(link_table, 0, sizeof(link_table));
    memset(external_table, 0, sizeof(external_table));

    /* do the first pass */
    uint16_t errors =
        first_pass(file_name, sym_table, &sym_ctr, data_image, &data_ctr,
                   object_code, &object_ctr, link_table, &link_ctr);
    /* if pass was succesfull */
    if (errors == 0) {
        /* show partial results if flag is set */
        if (list_tables) {
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
    errors = second_pass(file_name, sym_table, &sym_ctr, data_image, &data_ctr,
                         object_code, &object_ctr, link_table, &link_ctr,
                         external_table, &external_ctr);

    /* if pass was succesfull */
    if (errors == 0) {
        /* show partial results if flag is set */
        if (list_tables) {
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
    if (no_output == false) {
        if (binary_out) {
            if (external_ctr > 0) {
                fprintf(stderr, "unable to create binary file if source "
                                "contains .extern-s\n");
                return 4;
            }

            errors = create_binary_file(file_name, object_code, object_ctr);
            if (errors != 0) {
                fprintf(stderr,
                        "binary file creation failed with %u error(s)\n",
                        errors);
                return 5;
            }
        } else {
            /* create object file from object code */
            errors = create_object_file(file_name, object_code, object_ctr,
                                        data_ctr, link_table, link_ctr,
                                        external_table, external_ctr);
            if (errors != 0) {
                fprintf(stderr,
                        "object file creation failed with %u error(s)\n",
                        errors);
                return 4;
            }
        }
    }

    return 0;
}

/*!
 * \brief prints the symbol table
 */
void print_sym_table(void) {
    int i;
    printf("\nTable of symbols (name address type):\n");
    for (i = 0; i < sym_ctr; i++) {
        symbol_t * sym = &sym_table[i];
        printf("  %-10s %04x %c\n", sym->name, sym->value, sym->type);
    }
}

/*!
 * \brief prints the link table
 */
void print_link_table(void) {
    int i;

    printf("\nTable of link objects (name addr type):\n");
    for (i = 0; i < link_ctr; i++) {
        link_object_t * obj = &link_table[i];
        printf("  %-10s %04x %c\n", obj->name, obj->value, obj->type);
    }
}

/*!
 * \brief prints the extern table
 */
void print_extern_table(void) {
    int i;

    printf("\nTable of externals (name address):\n");
    for (i = 0; i < external_ctr; i++) {
        link_object_t * obj = &external_table[i];
        printf("  %-10s %04x\n", obj->name, obj->value);
    }
}

/*!
 * \brief prints the data image
 */
void print_data_image(void) {
    int i;

    printf("\nContent of the data image (address value):\n");
    for (i = 0; i < data_ctr; i++) {
        printf("  %04x %04x\n", i, data_image[i]);
    }
}

/*!
 * \brief prints the object code
 */
void print_object_code(void) {
    int i;

    printf("\nContent of the object code (address value type):\n");
    for (i = 0; i < object_ctr; i++) {
        object_code_t * o = &object_code[i];
        printf("  %04x %04x %c\n", i, o->value, o->type);
    }
}
