#ifndef USEFUL_STRING_H
#define USEFUL_STRING_H

#include <stdio.h>
#include "array.h"

struct u_string {
        size_t len;
        size_t capacity;
        char *str;
};

struct u_string_array {
        size_t len;
        size_t capacity;
        struct u_string *strings;
};

#define U_STRING(s) \
    struct u_string (s); \
    U_ARRAY_NEW((s), str); \
    U_ARRAY_PUSH(s, str, '\0')

#define U_STRING_FREE(s) U_ARRAY_FREE(s, str)

#define U_STRING_EMPTY(s) \
    (s).len = 1; \
    (s).str[0] = '\0'

#define U_STRING_ARRAY(arr) \
    struct u_string_array arr; \
    U_ARRAY_NEW(arr, strings)

#define U_STRING_ARRAY_FREE(array) \
    U_ARRAY_FREE_ELEMS_CUSTOM(array, strings, U_ARRAY_FREE, str)

void u_strcat(struct u_string *dest, const char *src);
void u_strcpy(struct u_string *dest, const char *src);
char *u_fgets(struct u_string *dest, FILE * stream);
int u_sprintf(struct u_string *dest, const char *format, ...);
void u_string_pushchar(struct u_string *dest, char c);
struct u_string u_substr(const struct u_string *string, size_t index, size_t n);
struct u_string_array u_string_split(const char *string_to_split, const char *delims);

#endif
