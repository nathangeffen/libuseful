#ifndef USEFUL_STRING_H
#define USEFUL_STRING_H

#include <stdio.h>
#include "array.h"

struct u_string {
        size_t len;
        size_t capacity;
        char *str;
};

#define U_STRING(s) \
    struct u_string (s); \
    U_ARRAY_NEW((s), str); \
    U_ARRAY_PUSH(s, str, '\0')

#define U_STRING_FREE(s) U_ARRAY_FREE(s, str)

void u_strcat(struct u_string *dest, const char *src);
void u_strcpy(struct u_string *dest, const char *src);
char *u_fgets(struct u_string *dest, FILE * stream);
int u_sprintf(struct u_string *dest, const char *format, ...);

#endif
