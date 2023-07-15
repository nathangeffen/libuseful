#include <stdarg.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include "useful/string.h"

static inline void copy(struct u_string *dest, const char *src)
{
        const char *s = src;
        do {
                U_ARRAY_PUSH(*dest, str, *s);
        } while (errno == 0 && *s++ != 0);
}

void u_strcat(struct u_string *dest, const char *src)
{
        --dest->len; // pop the end of string marker
        copy(dest, src);
}

void u_strcpy(struct u_string *dest, const char *src)
{
        dest->len = 0; // empty the string
        copy(dest, src);
}

char *u_fgets(struct u_string *dest, FILE * file)
{
        char c;
        size_t i = 0;
        while ( (c = getc(file)) != EOF && errno == 0) {
                U_ARRAY_PUSH(*dest, str, c);
                i++;
                if (c == '\n')
                        break;
        }
        if (errno == 0) {
                U_ARRAY_PUSH(*dest, str, '\0');
                if (i > 0) {
                        return dest->str;
                } else {
                        return NULL;
                }
        }
        return NULL;
}

int u_sprintf(struct u_string *dest, const char *fmt, ...)
{
        va_list ap;
        va_start(ap, fmt);

        int i = vsnprintf(NULL, 0, fmt, ap) + 1;
        if (i >= dest->capacity) {
            int j = u_array_grow( (void *) &dest->str, i * sizeof(*dest->str), i);
            if (j == 0) return 0;
            dest->capacity = j;
        }
        va_end(ap);
        va_start(ap, fmt);
        i = vsnprintf(dest->str, dest->capacity - 1, fmt, ap);
        dest->len = strlen(dest->str) + 1;
        va_end(ap);
        return i;
}
