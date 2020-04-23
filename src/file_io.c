/*!
 * \file file_io.c
 * \brief file Input/Output functions
 */

#include "asm.h"

/*!
 * \brief gets the base name from the path of the file
 * 
 * "C:\dir\file.txt" -> "file.txt"<br>
 * "/opt/dir/file.txt" -> "file.txt"
 * 
 * \param path	path of the file
 * \return		base name or NULL
 */
char * get_file_base_name(const char * path){
    if (path == NULL) {
		return NULL;
	}

	char * file_name = (char *)path;
	for (char * curr = (char *)path; *curr != '\0'; curr++){
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
char * get_file_name_no_ext(const char * path){
    int i, len = strlen(path);
    int dot_pos = -1;

    if(path){
		/* find the last dot */
        for(i = 0; i < len; i++){
            if(path[i] == '.'){
                dot_pos = i;
            }
        }

		/* if path contains a dot */
        if(dot_pos != -1){
            char * file_base_name_no_ext = (char *)malloc(dot_pos);
            if(file_base_name_no_ext){
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
 * \brief creates an ascii base16 object file from the object code
 * 
 * \param file_name		file name of the source file
 * \param objectc		object code
 * \param objectc_len	length of the object code
 * \param data_len		length of the data image
 * \return				number of errors
 */
uint16_t create_object_file(const char * file_name, object_code_t * objectc, uint16_t objectc_len, uint16_t data_len){
    char * file_name_no_ext = get_file_name_no_ext(file_name);
    FILE * fp = NULL;
    uint16_t i;
    uint16_t errors = 0;

    if(!file_name_no_ext){
        return 1;
    }

    char * object_name = (char *)malloc(strlen(file_name_no_ext) + 3 + 1);	/* ".oc" + NULL */
    if(object_name){
        strcpy(object_name, file_name_no_ext);
        strcat(object_name, ".oc");

        fp = fopen(object_name, "w");

        if(fp){
			/* header: length_of_the_instructions length_of_the_data */
            fprintf(fp, "%x %x\n", objectc_len - data_len, data_len);
            for(i = 0; i < objectc_len; i++){
                object_code_t * o = &objectc[i];
				/* object code: address machine_word type */
				fprintf(fp, "%04x %04x %c\n", i, o->value, o->type);
            }
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
 * \brief creates an ascii entry file from the link table 
 * 
 * \param file_name			file name of the source file
 * \param link_table		link table
 * \param link_table_len	length of the link table
 * \return					number of errors
 */
uint16_t create_entry_file(const char * file_name, link_object_t * link_table, uint16_t link_table_len) {
	char * file_name_no_ext = get_file_name_no_ext(file_name);
	FILE * fp = NULL;
	uint16_t i;
	uint16_t errors = 0;

	if (!file_name_no_ext) {
		return 1;
	}

	char * entry_name = (char *)malloc(strlen(file_name_no_ext) + 4 + 1);	/* ".ent" + NULL */
	if (entry_name) {
		strcpy(entry_name, file_name_no_ext);
		strcat(entry_name, ".ent");

		fp = fopen(entry_name, "w");

		if (fp) {
			for (i = 0; i < link_table_len; i++) {
				link_object_t * obj = &link_table[i];

				if (obj->type == 'n') {
					/* object code: name_of_the_entry address */
					fprintf(fp, "%s %04x\n", obj->name, obj->value);
				}
			}
			fclose(fp);
		}
		else {
			errors++;
		}
	}
	else {
		errors++;
	}

	free(entry_name);
	return errors;
}

/*!
 * \brief creates an ascii extern file from the extern table
 *
 * \param file_name			file name of the source file
 * \param extern_table		extern table
 * \param extern_table_len	length of the extern table
 * \return					number of errors
 */
uint16_t create_extern_file(const char * file_name, link_object_t * extern_table, uint16_t extern_table_len) {
	char * file_name_no_ext = get_file_name_no_ext(file_name);
	FILE * fp = NULL;
	uint16_t i;
	uint16_t errors = 0;

	if (!file_name_no_ext) {
		return 1;
	}

	char * extern_name = (char *)malloc(strlen(file_name_no_ext) + 4 + 1);	/* ".ext" + NULL */
	if (extern_name) {
		strcpy(extern_name, file_name_no_ext);
		strcat(extern_name, ".ext");

		fp = fopen(extern_name, "w");

		if (fp) {
			for (i = 0; i < extern_table_len; i++) {
				link_object_t * obj = &extern_table[i];
				/* object code: name_of_the_entry address */
				fprintf(fp, "%s %04x\n", obj->name, obj->value);
			}
			fclose(fp);
		}
		else {
			errors++;
		}
	}
	else {
		errors++;
	}

	free(extern_name);
	return errors;
}
