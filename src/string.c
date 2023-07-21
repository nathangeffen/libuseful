#include <stdarg.h>
#include <errno.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "useful/string.h"

/**
 * Copies src string to dest.
 */
static inline void copy(struct u_string *dest, const char *src)
{
        const char *s = src;
        do {
                U_ARRAY_PUSH(*dest, str, *s);
        } while (errno == 0 && *s++ != 0);
}

/**
 * Concatenates src to dest, managing memory in the process.
 *
 * \param dest String to concat to
 * \param src Null terminated string to append to dest
 */
void u_strcat(struct u_string *dest, const char *src)
{
        --dest->len;            // pop the end of string marker
        copy(dest, src);
}

/**
 * Copies src to dest, managing memory in the process.
 *
 * \param dest String to copy to
 * \param src Null terminated string to copy from
 */
void u_strcpy(struct u_string *dest, const char *src)
{
        dest->len = 0;          // empty the string
        copy(dest, src);
}

/**
 * Implements the stdlib fgets function, in addition taking care of memory.
 *
 * \param dest String in which to store line from file
 * \param file File to read next line from
 *
 * \return Upon success the null-terminated string, dest->str else NULL
 */
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

static int u_vsprintf(struct u_string *dest, const char *fmt, va_list ap)
{
        va_list ap2;
        va_copy(ap2, ap);
        int i = vsnprintf(NULL, 0, fmt, ap) + 1;
        if (i >= dest->capacity) {
                size_t j =
                    u_array_grow((void *)&dest->str, i * sizeof(*dest->str), i);
                if (j == 0)
                        return -1;
                dest->capacity = j;
        }
        i = vsnprintf(dest->str, dest->capacity - 1, fmt, ap2);
        dest->len = strlen(dest->str) + 1;
        va_end(ap2);
        return i;
}

/**
 * Implements the sprintf function, in addition taking care of memory.
 *
 * \param dest String in which to place output
 * \param fmt String that specifies format - see sprintf for details
 * \param ... See sprintf for details
 *
 * \return Number of characters in dest->str (as reported by vsnprintf) upon
 * success else -1 upon failure.
 */
int u_sprintf(struct u_string *dest, const char *fmt, ...)
{
        va_list ap;
        va_start(ap, fmt);
        int i = u_vsprintf(dest, fmt, ap);
        va_end(ap);
        return i;
}

/**
 * Combines sprinttf and concat operations, taking care of memory. This function
 * prints a string to the end of another string.
 *
 * \param dest String in which to place output
 * \param fmt String that specifies format - see sprintf for details
 * \param ... See sprintf for details
 *
 * \return Number of characters appended to dest->str (as reported by vsnprintf) upon
 * success else -1 upon failure.
 */
int u_sprintf_cat(struct u_string *dest, const char *fmt, ...)
{
        va_list ap;
        va_start(ap, fmt);

        U_STRING(t);
        int i = u_vsprintf(&t, fmt, ap);
        if (errno == 0)
                u_strcat(dest, t.str);
        else
                i = -1;
        U_STRING_FREE(t);
        return i;
}

/**
 * Appends a character to the end of a string, taking care of memory.
 *
 * \param dest String which must be appended
 * \param c Character to append
 */
void u_pushchar(struct u_string *dest, char c)
{
        dest->str[dest->len - 1] = c;
        if (c != '\0')
                U_ARRAY_PUSH(*dest, str, '\0');
}

/**
 * Creates a new string that is a substring of another.
 *
 * \param string String from which substring must be taken
 * \param index Index in string->str to start the substr
 * \param n Number of characters to include in the substr (if this goes
 * beyond the string->str's null character then the substring stops there)
 *
 * \return the null-terminated substring stored in a struct u_string
 */
struct u_string u_substr(const struct u_string *string, size_t index, size_t n)
{
        U_STRING(result);
        for (size_t i = index; i < string->len && i < index + n && errno == 0;
             i++)
                u_pushchar(&result, string->str[i]);
        if (result.str[result.len - 1] != '\0' && errno == 0)
                u_pushchar(&result, '\0');
        return result;
}

/**
 * Checks if a character is in a set of delimiters.
 */
static inline bool in(const char *delims, char c)
{
        for (const char *s = delims; *s != '\0'; s++)
                if (c == *s)
                        return true;
        return false;
}

/**
 * Splits a string into an array of strings. Characters in delims
 * indicate the delimiters. No zero length strings are included in the output.
 *
 * \param string_to_split String to split
 * \param delims Set of chars that are the delimiters of string_to_split
 *
 * \return Array of struct u_strings with null-terminated strings
 */
struct u_string_array u_string_split(const char *string_to_split,
                                     const char *delims)
{
        struct u_string_array result;
        U_ARRAY(result, strings);
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
                        u_pushchar(&string, *s);
                }
        }
        U_STRING_FREE(string);
        return result;
}

/**
 * Concatenates an array of strings into a single string.

 * \param array Array of strings to join
 * \param delim Delimiter to between each concatenated string
 *
 * \return A u_string with the concatenated members of the array
 */
struct u_string u_join(const struct u_string_array *array, const char *delim)
{
        U_STRING(result);
        for (size_t i = 0; i < array->len; ++i) {
                u_strcat(&result, array->strings[i].str);
                if (i + 1 < array->len)
                        u_strcat(&result, delim);
        }
        return result;
}

struct u_string u_join_conv(size_t n, size_t bytes, const void *array,
                            const char *delim, char *(*conv)(const void *), bool free_str)
{
        U_STRING(result);
        size_t e = n * bytes;
        for (size_t i = 0; i < e; i += bytes) {
                char *s = conv(array + i);
                u_strcat(&result, s);
                if (i + bytes < e)
                        u_strcat(&result, delim);
                if (free_str) free(s);
        }
        return result;
}
