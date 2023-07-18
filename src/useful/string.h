/** @file
 *
 * @brief Macros and functions for managing strings and there memory (think
 * very simple C++ std::vector). It's a convenience to spare programmers the
 * misery of managing the memory for C strings.
 *
 * To use it declare a string, say it's called my_string, as follows:
 *
 @verbatim U_STRING(my_string); @endverbatim
 *
 * This creates a struct u_string and allocates memory for it.  A struct
 * u_string simply consists of a char * called str, and two size_t variables
 * len (the length of str including null terminator) and capacity (how much
 * memory is currently available for str).
 *
 * All the macros and functions expect null terminated strings.  All the macros
 * and functions that modify or create strings will leave them null terminated.
 * If used properly, it should always be ok to use my_string.str as a normal
 * C string so long as it is kept constant. If you modify it outside of this
 * API, then you're responsible for ensuring len and capacity are set correctly.
 *
 */

#ifndef USEFUL_STRING_H
#define USEFUL_STRING_H

#include <stdio.h>
#include "array.h"


/**
 * Holds a string.
 */
struct u_string {
        size_t len;
        size_t capacity;
        char *str;
};

/*
 * Holds an array of strings.
 */
struct u_string_array {
        size_t len;
        size_t capacity;
        struct u_string *strings;
};

/*
 * Creates a new struct u_string string, allocates space for it and null
 * terminates it.
 *
 * @param string Name of the string variable
 */
#define U_STRING(string) \
    struct u_string (string); \
    U_ARRAY((string), str); \
    U_ARRAY_PUSH(string, str, '\0')

/**
 * Frees strings.
 *
 * @param string String variable to free.
 */
#define U_STRING_FREE(string) U_ARRAY_FREE(string, str)

/**
 * Empties a string and null terminates it.
 *
 * @param string String to empty and null terminate.
 */
#define U_STRING_EMPTY(string) \
    (string).len = 1; \
    (string).str[0] = '\0'

/**
 * Creates an array of strings
 *
 * @param array Name of variable in which to store the array.
 */
#define U_STRING_ARRAY(array) \
    struct u_string_array array; \
    U_ARRAY(array, strings)

/**
 * Frees an array of strings.
 *
 * @param array Variable containing the array of strings that must be freed.
 */
#define U_STRING_ARRAY_FREE(array) \
    U_ARRAY_FREE_ELEMS_CUSTOM(array, strings, U_ARRAY_FREE, str)

void u_strcat(struct u_string *dest, const char *src);
void u_strcpy(struct u_string *dest, const char *src);
char *u_fgets(struct u_string *dest, FILE * stream);
int u_sprintf(struct u_string *dest, const char *format, ...);
void u_pushchar(struct u_string *dest, char c);
struct u_string u_substr(const struct u_string *string, size_t index, size_t n);
struct u_string_array u_string_split(const char *string_to_split,
                                     const char *delims);

#endif
