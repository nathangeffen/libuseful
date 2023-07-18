/**
   \file

   \brief Definitions of csv management functions

*/
#include "useful/csv.h"

struct string {
        size_t len;
        size_t capacity;
        char *str;
};

static struct u_csv_row u_csv_append_row(const char *strings[], size_t n)
{
        struct u_csv_row row;
        char *s;

        U_ARRAY(row, cells);
        for (size_t i = 0; i < n; ++i) {
                if (NULL == (s = u_strdup(strings[i])))
                        error(EXIT_FAILURE, errno,
                              "Failed to allocate space for cell.");
                U_ARRAY_PUSH(row, cells, s);
        }

        return row;
}

/**
   Creates a CSV structure.

   @param header True if the csv must have a header
   @param strings Values of the header columns
   @param n Number of columns

   @return Empty ready-to-use csv structure with a header if specified.
 */

struct u_csv u_csv_new(bool header, const char *strings[], size_t n)
{

        struct u_csv cs;
        if (header) {
                cs.header = u_csv_append_row(strings, n);
        } else {
                cs.header.len = cs.header.capacity = 0;
                cs.header.cells = NULL;
        }
        U_ARRAY(cs, rows);
        return cs;
}

/**
   Append a row to a CSV structure.

   @param strings Values of cell entries
   @param n Number of columns (or cells)
 */

void u_csv_append(struct u_csv *cs, const char *strings[], size_t n)
{
        struct u_csv_row row;
        row = u_csv_append_row(strings, n);
        U_ARRAY_PUSH(*cs, rows, row);
}

static struct u_csv_row u_csv_read_row(FILE * f, const char delim)
{
        struct u_csv_row row;
        struct string cell;
        bool inrow = true, inquote = false;
        char c, *s;

        U_ARRAY(row, cells);
        U_ARRAY(cell, str);
        while (inrow) {
                bool endcell = false;
                if ((c = fgetc(f)) == '"') {
                        if ((c = fgetc(f)) == '"' && inquote) {
                                U_ARRAY_PUSH(cell, str, c);
                        } else {
                                ungetc(c, f);
                                inquote = !inquote;
                        }
                } else if (inquote == false && c == delim) {
                        endcell = true;
                } else if (c == EOF || (c == '\n' && inquote == false)) {
                        inrow = false;
                        if (c == EOF)
                                ungetc(c, f);
                } else {
                        U_ARRAY_PUSH(cell, str, c);
                }
                if (endcell || inrow == false) {
                        U_ARRAY_PUSH(cell, str, '\0');
                        if (NULL == (s = u_strdup(cell.str)))
                                error(EXIT_FAILURE, errno,
                                      "Failed to allocate space for cell.");
                        if (strlen(s) || !(c == EOF || c == '\n'))
                                U_ARRAY_PUSH(row, cells, s);
                        else
                                free(s);
                        cell.len = 0;
                }
        }
        U_ARRAY_FREE(cell, str);
        return row;
}

/**
   Reads a CSV file.

   @param f Already opened file to read in.
   @param header Whether the CSV file has a header
   @param delim The CSV file delimiter, usually a comma

   @return Populated csv structure
 */

struct u_csv u_csv_read(FILE * f, bool header, char delim)
{
        struct u_csv cs;
        struct u_csv_row row;

        U_ARRAY(cs, rows);
        if (header)
                cs.header = u_csv_read_row(f, delim);
        else
                U_ARRAY(cs.header, cells);

        while ((row = u_csv_read_row(f, delim)).len > 0)
                U_ARRAY_PUSH(cs, rows, row);

        U_ARRAY_FREE(row, cells);

        return cs;
}

/**
   Get value of CSV cell.

   @param cs Structure to get cell from
   @param row Row index
   @param col Column index

   @return Value of cell[row, col]
 */

const char *u_csv_at(const struct u_csv *cs, size_t row, size_t col)
{
        if (row < cs->len && col < cs->rows[row].len)
                return cs->rows[row].cells[col];
        else
                return NULL;
}

static void u_csv_write_row(FILE * f, const struct u_csv_row *row)
{
        for (size_t i = 0; i < row->len && (i == 0 || fputc(',', f)); ++i)
                fprintf(f, "%s", row->cells[i]);
        fputc('\n', f);
}

/**
   Writes a CSV file.

   @param f Already opened file to write to
   @param cs Structure to write

   @return Value of cell[row, col]
 */

void u_csv_write(FILE * f, const struct u_csv *cs)
{
        u_csv_write_row(f, &cs->header);
        for (size_t i = 0; i < cs->len; ++i)
                u_csv_write_row(f, cs->rows + i);
}

/**
   Checks if a CSV struct is in RFC4180 format (more or less)
   https://tools.ietf.org/html/rfc4180.

   @param cs Structure to check
   @param verbose Output non-conformities

   @return True if it confirms, else false
*/

bool u_csv_isvalid(const struct u_csv *cs, bool verbose)
{
        size_t cols = 0;
        bool valid = true;
        size_t i;

        cols = cs->header.len;

        if (cols) {
                i = 0;
        } else {
                if (cs->len > 0)
                        cols = cs->rows[0].len;
                i = 1;
        }

        for (; i < cs->len; ++i)
                if (cs->rows[i].len != cols) {
                        valid = false;
                        if (verbose)
                                printf("Row %zu has %zu colums, "
                                       "but expected %zu.\n",
                                       i, cs->rows[i].len, cols);
                        else
                                break;
                }

        return valid;
}

static void u_csv_free_row(struct u_csv_row *row)
{
        for (size_t i = 0; i < row->len; ++i)
                free(row->cells[i]);
        U_ARRAY_FREE(*row, cells);
}

/**
   Frees a csv struct.

   @param cs struct to free
*/

void u_csv_free(struct u_csv *cs)
{

        u_csv_free_row(&cs->header);

        for (size_t i = 0; i < cs->len; ++i)
                u_csv_free_row(&cs->rows[i]);
        U_ARRAY_FREE(*cs, rows);
}

static void u_csv_row_to_df_row(const struct u_csv *cs, struct u_dataframe *df,
                              size_t row, size_t cols,
                              const enum u_val_type col_types[])
{
        for (size_t i = 0; i < cols; ++i) {
                if (col_types[i] == str) {
                        df->vals[row * cols + i].str =
                            u_strdup(u_csv_at(cs, row, i));
                } else {
                        char *ptr;
                        df->vals[row * cols + i].dbl =
                            strtod(u_csv_at(cs, row, i), &ptr);
                        if (strlen(ptr) > 0 || strlen(u_csv_at(cs, row, i)) == 0)
                                fprintf(stderr, "Error converting '%s' to "
                                        "double.\n", cs->rows[row].cells[i]);
                        errno = EINVAL;
                }
        }
}

static struct u_csv_row rowdup(const struct u_csv_row *row)
{
        struct u_csv_row result;
        char *s;

        U_ARRAY(result, cells);
        for (size_t i = 0; i < row->len; ++i) {
                if (NULL == (s = u_strdup(row->cells[i])))
                        error(EXIT_FAILURE, errno,
                              "Failed to allocate space for "
                              "string in duplicate row.");
                U_ARRAY_PUSH(result, cells, s);
        }

        return result;
}

/**
   Creates a new dataframe. The dataframe must be freed when finished using it
   with u_dataframe_free().

   @param cols number of columns
   @param array of strings representing names of columns
   @param types types (str or dbl) of each column

   @return a ready-to-use dataframe
 */

struct u_dataframe u_dataframe_new(size_t cols, const char *strings[],
                               const enum u_val_type types[])
{
        struct u_dataframe df;
        df.vals = NULL;
        df.rows = 0;
        df.cols = cols;

        df.header = u_csv_append_row(strings, cols);

        if (NULL == (df.type = malloc(cols * sizeof(enum u_val_type))))
                error(EXIT_FAILURE, errno,
                      "Failed to allocate space for dataframe types.");

        for (size_t i = 0; i < cols; ++i)
                df.type[i] = types[i];

        return df;
}

/**
   Converts a csv to a dataframe. Typically you'd read in a CSV file into a
   struct csv, then convert to the dataframe.

   Note, that the csv must be valid else behaviour is undefined. In debug mode
   (i.e. with assert defined), validity is checked for, but not in optimised
   production code.

   @param cs csv structure to convert
   @param col_types type for each column (either str or dbl)

   @return Dataframe with same number of rows and columns as the csv.
*/

struct u_dataframe u_csv_to_dataframe(const struct u_csv *cs,
                                  const enum u_val_type col_types[])
{
        struct u_dataframe df;

        assert(u_csv_isvalid(cs, false));
        assert(cs->len > 0);

        size_t rows = cs->len;
        size_t cols = cs->rows[0].len;

        df.rows = rows;
        df.cols = cols;
        df.vals = malloc(rows * cols * sizeof(union u_str_dbl));
        if (NULL == df.vals)
                error(EXIT_FAILURE, errno,
                      "Failed to allocate space for dataframe cells.");
        df.type = malloc(cols * sizeof(enum u_val_type));
        if (NULL == df.type)
                error(EXIT_FAILURE, errno,
                      "Failed to allocate space for dataframe types.");
        for (size_t i = 0; i < cols; ++i)
                df.type[i] = col_types[i];

        for (size_t i = 0; i < rows; ++i)
                u_csv_row_to_df_row(cs, &df, i, cols, col_types);

        df.header = rowdup(&cs->header);

        return df;
}

/**
   Gets a dataframe entry.

   @param df dataframe with entry to get
   @param row row of dataframe cell to get
   @param col col of dataframe cell to get

   @return Dataframe with same number of rows and columns as the csv.
*/

union u_str_dbl u_dataframe_at(const struct u_dataframe *df, size_t row, size_t col)
{
        assert(row < df->rows);
        assert(col < df->cols);

        return df->vals[row * df->cols + col];
}

/**
   Gets type for dataframe col

   @param df dataframe with entry to get
   @param col col of dataframe to get

   @return type (either str or dbl) of the dataframe column
*/

enum u_val_type u_dataframe_col_type(struct u_dataframe *df, size_t col)
{
        assert(col < df->cols);
        return df->type[col];
}

/**
   Appends a new row to a dataframe

   @param df dataframe to append to
   @param vals array of values to append
*/

void u_dataframe_append(struct u_dataframe *df, const union u_str_dbl vals[])
{
        size_t rows = df->rows + 1, cols = df->cols;

        df->vals = realloc(df->vals, rows * cols * sizeof(union u_str_dbl));
        if (NULL == df->vals)
                error(EXIT_FAILURE, errno,
                      "Failed to allocate space for dataframe column.");

        for (size_t i = 0, j = df->rows * cols; i < cols; ++i, ++j) {
                switch (df->type[i]) {
                case str:
                        df->vals[j].str = u_strdup(vals[i].str);
                        if (NULL == df->vals[j].str)
                                error(EXIT_FAILURE, errno, "Failed to allocate "
                                      "space for dataframe cell.");
                        break;
                case dbl:
                        df->vals[j].dbl = vals[i].dbl;
                        break;
                default:
                        fprintf(stderr, "Unknown data frame column type.\n");
                }
        }
        df->rows = rows;
}

/**
   Appends a new row to a dataframe. Takes a variable number of arguments.
   After the first argument, each subsequent argument is either a string or
   double of each corresponding cell in the dataframe. This is a risky function
   Use with care.

   @param df dataframe to append to
*/

void u_dataframe_append_var(struct u_dataframe *df, ...)
{
        size_t rows = df->rows + 1, cols = df->cols;
        va_list ap;

        va_start(ap, df);
        df->vals = realloc(df->vals, rows * cols * sizeof(union u_str_dbl));
        if (NULL == df->vals)
                error(EXIT_FAILURE, errno,
                      "Failed to allocate space for dataframe column.");

        for (size_t i = 0, j = df->rows * cols; i < cols; ++i, ++j) {
                switch (df->type[i]) {
                case str:
                        df->vals[j].str = u_strdup(va_arg(ap, char *));
                        if (NULL == df->vals[j].str)
                                error(EXIT_FAILURE, errno, "Failed to allocate "
                                      "space for dataframe cell.");
                        break;
                case dbl:
                        df->vals[j].dbl = va_arg(ap, double);
                        break;
                default:
                        fprintf(stderr, "Unknown data frame column type.\n");
                }
        }
        df->rows = rows;
        va_end(ap);
}

/**
   Writes a dataframe to a file

   @parm f already opened file for writing
   @param df dataframe to write out
*/

void u_dataframe_write(FILE * f, const struct u_dataframe *df)
{
        u_csv_write_row(f, &df->header);
        for (size_t i = 0; i < df->rows; ++i) {
                for (size_t j = 0; j < df->cols && (j == 0 || fputc(',', f));
                     ++j)
                        if (df->type[j] == dbl)
                                fprintf(f, "%f",
                                        df->vals[i * df->cols + j].dbl);
                        else
                                fprintf(f, "%s",
                                        df->vals[i * df->cols + j].str);
                fputc('\n', f);
        }
}

/**
   Converts a dataframe to a csv structure

   @param df dataframe to convert

   @return csv structure
*/

struct u_csv u_dataframe_to_csv(const struct u_dataframe *df)
{
        struct u_csv cs = u_csv_new(true, (const char **)df->header.cells,
                                  df->header.len);
        const size_t rows = df->rows, cols = df->cols;
        for (size_t i = 0; i < rows; ++i) {
                char *strings[cols];
                struct u_csv_row row;
                for (size_t j = 0; j < cols; ++j) {
                        if (df->type[j] == dbl) {
                                char s[50];
                                snprintf(s, 50, "%f",
                                         df->vals[i * cols + j].dbl);
                                if ((strings[j] = u_strdup(s)) == NULL)
                                        error(EXIT_FAILURE, errno,
                                              "Failed to allocate space for "
                                              "csv cell.");
                        } else {
                                strings[j] =
                                    u_strdup(df->vals[i * cols + j].str);
                                if (NULL == strings[j])
                                        error(EXIT_FAILURE, errno,
                                              "Failed to allocate space for "
                                              "csv cell.");
                        }
                }
                row = u_csv_append_row((const char **)strings, cols);
                U_ARRAY_PUSH(cs, rows, row);
                for (size_t j = 0; j < cols; ++j)
                        free(strings[j]);
        }
        return cs;
}

/**
   Converts a dataframe to a matrix

   @param df dataframe to convert

   @return matrix structure
*/

struct u_matrix u_dataframe_to_matrix(const struct u_dataframe *df)
{
        struct u_matrix mat;

        const size_t rows = df->rows;
        const size_t cols = df->cols;
        mat.rows = rows;
        mat.cols = cols;
        mat.vals = malloc(rows * cols * sizeof(double));
        if (NULL == mat.vals)
                error(EXIT_FAILURE, errno, "Failed to allocate matrix.");

        for (size_t i = 0; i < rows; ++i) {
                for (size_t j = 0; j < cols; ++j) {
                        if (df->type[j] == str) {
                                char *ptr;
                                mat.vals[i * cols + j] =
                                    strtod(u_dataframe_at(df, i, j).str, &ptr);
                                if (strlen(ptr) > 0 ||
                                    strlen(u_dataframe_at(df, i, j).str) == 0) {
                                        fprintf(stderr, "Error converting '%s' "
                                                "to double.\n",
                                                u_dataframe_at(df, i, j).str);
                                        errno = EINVAL;
                                }
                        } else {
                                mat.vals[i * cols + j] =
                                    u_dataframe_at(df, i, j).dbl;
                        }
                }
        }
        return mat;
}

/**
   Frees a dataframe after use

   @param df dataframe to free
*/

void u_dataframe_free(struct u_dataframe *df)
{
        for (size_t i = 0; i < df->rows; ++i) {
                for (size_t j = 0; j < df->cols; ++j) {
                        if (df->type[j] == str)
                                free(df->vals[i * df->cols + j].str);
                }
        }
        u_csv_free_row(&df->header);
        free(df->vals);
        free(df->type);
        df->rows = df->cols = 0;
}

/**
   Convert a csv to a matrix of real numbers.

   @param cs struct to free
   @return matrix of real numbers
*/

struct u_matrix u_csv_to_matrix(const struct u_csv *cs)
{
        struct u_matrix mat;

        assert(u_csv_isvalid(cs, false));
        assert(cs->len > 0);

        size_t rows = cs->len;
        size_t cols = cs->rows[0].len;
        mat.rows = rows;
        mat.cols = cols;
        mat.vals = malloc(rows * cols * sizeof(double));
        if (NULL == mat.vals)
                error(EXIT_FAILURE, errno, "Failed to allocate matrix.");

        for (size_t i = 0; i < rows; ++i) {
                for (size_t j = 0; j < cols; ++j) {
                        char *ptr;
                        mat.vals[i * cols + j] =
                            strtod(cs->rows[i].cells[j], &ptr);
                        if (strlen(ptr) > 0 ||
                            strlen(cs->rows[i].cells[j]) == 0) {
                                fprintf(stderr, "Error converting '%s' "
                                        "to double.\n", cs->rows[i].cells[j]);
                                errno = EINVAL;
                        }
                }
        }
        return mat;
}

/**
   Get value of matrix cell

   @param mat Matrix to get value from
   @param row Row of cell
   @param col Column of cell

   @return Value returned
 */

inline double u_matrix_at(const struct u_matrix *mat, size_t row, size_t col)
{
        assert(row < mat->rows);
        assert(col < mat->cols);
        return mat->vals[row * mat->cols + col];
}

/**
   Set value of matrix cell

   @param mat Matrix to change
   @param row Row of cell
   @param col Column of cell
   @param val Value at which to set it
 */

inline void u_matrix_set(struct u_matrix *mat, size_t row, size_t col, double val)
{
        assert(row < mat->rows);
        assert(col < mat->cols);
        mat->vals[row * mat->cols + col] = val;
}

/**
   Free a matrix when done with it.

   @param mat Matrix to free
 */

void u_matrix_free(struct u_matrix *matrix)
{
        free(matrix->vals);
        matrix->rows = matrix->cols = 0;
}
