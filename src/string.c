#include <stdarg.h>
#include <errno.h>
#include <stdbool.h>
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
        --dest->len;            // pop the end of string marker
        copy(dest, src);
}

void u_strcpy(struct u_string *dest, const char *src)
{
        dest->len = 0;          // empty the string
        copy(dest, src);
}

char *u_fgets(struct u_string *dest, FILE * file)
{
        char c;
        size_t i = 0;
        while ((c = getc(file)) != EOF && errno == 0) {
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
                int j =
                    u_array_grow((void *)&dest->str, i * sizeof(*dest->str), i);
                if (j == 0)
                        return 0;
                dest->capacity = j;
        }
        va_end(ap);
        va_start(ap, fmt);
        i = vsnprintf(dest->str, dest->capacity - 1, fmt, ap);
        dest->len = strlen(dest->str) + 1;
        va_end(ap);
        return i;
}

void u_string_pushchar(struct u_string *dest, char c)
{
        dest->str[dest->len - 1] = c;
        if (c != '\0')
                U_ARRAY_PUSH(*dest, str, '\0');
}

struct u_string u_substr(const struct u_string *string, size_t index, size_t n)
{
        U_STRING(result);
        for (size_t i = index; i < string->len && i < index + n; i++)
                u_string_pushchar(&result, string->str[i]);
        if (result.str[result.len - 1] != '\0')
                u_string_pushchar(&result, '\0');
        return result;
}

static bool in(const char *delims, char c)
{
        for (const char *s = delims; *s != '\0'; s++)
                if (c == *s)
                        return true;
        return false;
}

struct u_string_array u_string_split(const char *string_to_split,
                                     const char *delims)
{
        struct u_string_array result;
        U_ARRAY_NEW(result, strings);
        U_STRING(string);
        for (const char *s = string_to_split; errno == 0; s++) {
                if (in(delims, *s) || *s == '\0') {
                        if (strlen(string.str) > 0) {
                                U_STRING(new_elem);
                                u_strcpy(&new_elem, string.str);
                                U_ARRAY_PUSH(result, strings, new_elem);
                                U_STRING_EMPTY(string);
                        }
                        if (*s == '\0')
                                break;
                } else {
                        u_string_pushchar(&string, *s);
                }
        }
        U_STRING_FREE(string);
        return result;
}
