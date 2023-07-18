/**
   @file

   @brief Structs and functions for managing CSV files.

   Most of the time you will want to use the u_csv_read() function to read a CSV
   file into a 2-d array structure of strings (struct u_csv). Then when you're
   done with it, call u_csv_free().

   To create a csv structure and then csv files use u_csv_new(), u_csv_append()
   and u_csv_write().

   Use u_csv_at() to get the string value of a particular cell.
   Use u_csv_to_matrix() to convert a csv consisting of numbers to a matrix of
   doubles. Remember to free your matrix when done with it using matrix_free().

   Everything else provided here is just cute, or perhaps occasionally useful.
*/

#ifndef USEFUL_CSV_H
#define USEFUL_CSV_H

#include <assert.h>
#include <error.h>
#include <errno.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "useful/array.h"
#include "useful/test.h"
#include "useful/algorithms.h"

/**
   Holds a single row in a csv file
 */

struct u_csv_row {
        size_t len;
        size_t capacity;
        char **cells;
};

/**
   Holds an entire csv file, either after reading it in, or for writing it out.
 */

struct u_csv {
        struct u_csv_row header;
        size_t len;
        size_t capacity;
        struct u_csv_row *rows;
};

/**
   Hold a matrix of real-valued numbers.
 */

struct u_matrix {
        size_t rows;
        size_t cols;
        double *vals;
};

/**
   Used to keep track of type in str_dbl union.
 */

enum u_val_type {
        dbl,
        str
};

/**
   Type for dataframe cell. Holds either a string or double.
 */

union u_str_dbl {
        char *str;
        double dbl;
};

/**
   Represents something akin to an R dataframe.
 */

struct u_dataframe {
        struct u_csv_row header;
        size_t rows;
        size_t cols;
        enum u_val_type *type;
        union u_str_dbl *vals;
};

struct u_csv u_csv_read(FILE * f, bool header, char delim);
const char *u_csv_at(const struct u_csv *cs, size_t row, size_t col);
void u_csv_write(FILE * f, const struct u_csv *cs);
bool u_csv_isvalid(const struct u_csv *cs, bool verbose);
void u_csv_free(struct u_csv *cs);
struct u_dataframe u_csv_to_dataframe(const struct u_csv *cs,
                                  const enum u_val_type col_types[]);
struct u_dataframe u_dataframe_new(size_t cols, const char *strings[],
                               const enum u_val_type types[]);
union u_str_dbl u_dataframe_at(const struct u_dataframe *df, size_t row, size_t col);
void u_dataframe_append(struct u_dataframe *df, const union u_str_dbl vals[]);
void u_dataframe_append_var(struct u_dataframe *df, ...);
enum u_val_type u_dataframe_col_type(struct u_dataframe *df, size_t col);
void u_dataframe_write(FILE * f, const struct u_dataframe *df);
struct u_csv u_dataframe_to_csv(const struct u_dataframe *df);
struct u_matrix u_dataframe_to_matrix(const struct u_dataframe *df);
void u_dataframe_free(struct u_dataframe *df);
struct u_matrix u_csv_to_matrix(const struct u_csv *cs);
double u_matrix_at(const struct u_matrix *mat, size_t row, size_t col);
void u_matrix_set(struct u_matrix *mat, size_t row, size_t col, double val);
void u_matrix_free(struct u_matrix *matrix);

#endif
