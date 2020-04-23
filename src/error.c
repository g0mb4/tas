/*!
 * \file error.c
 * \brief error/warning reporting functions
 */

#include "asm.h"

/*!
 * \brief prints a formatted warning message to standard error stream (stderr)
 * 
 * \param file_name	name of the source file where the error occured
 * \param line		line number of the error
 * \param fmt		printf style format string
 * \param ...		printf style variable argument list
 */
void error(const char * file_name, uint32_t line, char * fmt, ...){
    va_list list;

    fprintf(stderr, "%s:%u: error: ", file_name ? file_name : "", line);
    va_start(list, fmt);
	vfprintf(stderr, fmt, list);
	va_end(list);
    fprintf(stderr, "\n");
}

/*!
 * \brief prints a formatted warning message to standard error stream (stderr)
 * 
 * \param file_name	name of the source file where the warning occured
 * \param line		line number of the warning
 * \param fmt		printf style format string
 * \param ...		printf style variable argument list
 */
void warning(const char * file_name, uint32_t line, char * fmt, ...){
    va_list list;

    fprintf(stderr, "%s:%u: warning: ", file_name ? file_name : "", line);
    va_start(list, fmt);
	vfprintf(stderr, fmt, list);
	va_end(list);
    fprintf(stderr, "\n");
}
