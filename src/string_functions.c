/*!
 * \file string_functions.c
 * \brief string manipulations
 */

#include "asm.h"

/*!
 * \brief removes cheracters from the start of a string
 * 
 * \note returned string, if not NULL, must be free()-d
 * 
 * \param str	input string
 * \param chars	characters to remove
 * \return		output string or NULL
 */
char * string_trim(const char * str, const char * chars) {
    if (!str) {
        return NULL;
    }

    int len = (int)strlen(str);
    int len_chars = (int)strlen(chars);
    bool found = true;
    int i, j, offset = 0;

    for (i = 0; str[i] && i < len && found; i++) {
        found =
            false; /* not found a removeable character => first valid char */
        for (j = 0; j < len_chars; j++) {
            if (str[i] == chars[j]) {
                found = true;
                offset++;
                break;
            }
        }
    }

    return strdup(str + offset); /* must be free()-d */
}

/*!
 * \brief removes cheracters from the end of a string
 *
 * \note returned string, if not NULL, must be free()-d
 * 
 * \param str	input string
 * \param chars	characters to remove
 * \return		output string or NULL
 */
char * string_trim_end(const char * str, const char * chars) {
    if (!str) {
        return NULL;
    }

    int len = (int)strlen(str);
    int len_chars = (int)strlen(chars);
    bool found = true;
    int i, j, offset = 0;

    /* start from the end and go backwards */
    for (i = len - 1; str[i] && i >= 0 && found; i--) {
        found =
            false; /* not found a removeable character => first valid char */
        for (j = 0; j < len_chars; j++) {
            if (str[i] == chars[j]) {
                found = true;
                offset++;
                break;
            }
        }
    }

    /* strange error */
    if (offset > len) {
        return NULL;
    }

    char * ret = (char *)malloc((len - offset + 1)); /* must be free()-d */

    if (ret) {
        strncpy(ret, str, len - offset);
        ret[len - offset] = 0;
        return ret;
    } else {
        return NULL;
    }
}

/*!
 * \brief splits the input sting into substings based on the delimeter
 * 
 * \note returned string, if not NULL, must be free()-d
 * 
 * \param str	input string
 * \param delim	delimeter character
 * \param index	index of the returnable substring
 * \return		substring  or NULL
 */
char * string_split(const char * str, const char * delim, int index) {
    if (!str) {
        return NULL;
    }

    char * s = strdup(str);
    char * ret = NULL;
    char * token;
    int target = 0;

    token = strtok(s, delim);
    while (token != NULL) {
        if (index == target) {
            ret = strdup(token); /* must be free()-d */
            break;
        }

        token = strtok(NULL, delim);
        target++;
    }

    free(s);

    return ret;
}

/*!
 * \brief removes white spaces,tabs and comments (;) from a string
 * 
 * \note returned string, if not NULL, must be free()-d
 * 
 * \param line	line of a source file
 * \return		cleaned string or NULL
 */
char * clean_line(const char * line) {
    if (!line) {
        return NULL;
    }

    char * trimmed = string_trim(line, " \t"); /* remove starting junk */
    if (!trimmed) {
        return NULL;
    }

    int i, n = 0, len = (int)strlen(trimmed);
    char ch;

    char * ret = (char *)malloc(sizeof(char) * (len + 1));

    /* first char is copied automatically, it is trimmed */
    ret[0] = trimmed[0];
    n = 1;

    if (ret) {
        for (i = 1; trimmed[i] && i < len; i++) {
            ch = trimmed[i];

            /* start of comment, not needed */
            if (ch == ';') {
                break;
            }

            /* tabs to spaces */
            if (ch == '\t') {
                ch = ' ';
            }

            /* not add duplicated space or space after comma */
            if ((ret[n - 1] == ' ' || ret[n - 1] == ',') && ch == ' ') {
                continue;
            }

            /* not add space after a number */
            if (ret[n - 1] >= '0' && ret[n - 1] <= '9' && ch == ' ') {
                continue;
            }

            ret[n++] = ch;
        }

        ret[n] = 0;
    }

    free(trimmed);

    return string_trim_end(ret, " \t\r\n"); /* remove ending junk */
}
