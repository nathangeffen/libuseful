/**
  @file

  @brief Macros and functions for dynamic arrays (think very simple C++
  std::vector).

  To use it declare a struct which contains a pointer array, and two integer
  fields (usually size_t), size (the number of elements in the array) and
  capacity (the number of elements the array can currently store). E.g.

  @verbatim

  struct MyArray {
  int *my_int_array;
  size_t len;
  size_t capacity;
// You can add additional elements to the struct here if necessary.
};

@endverbatim

Use U_ARRAY to initialize MyArray variables, ARRAY_APPEND to add elements
to it and U_ARRAY_FREE to return the array to the heap. ARRAY_APPEND increases
the capacity of the array if it is full by calling realloc.

Note: The programmer is responsible for allocating and freeing elements of the
array if these contain dynamically allocated memory.

Here is an example:
#include <stdlib.h>
#include <string.h>
#include "useful/string.h"

void u_strcat(char *dest, const char *src)
{
        size_t len_d = strlen(dest);
        size_t len_s = strlen(src);

        char *s = realloc(dest, len_s + len_d + 1);
        if (s != NULL) {
                dest = s;
                strcat(dest, src);
        }
}

@include arrays.c

*/
#ifndef USEFUL_ARRAY_H
#define USEFUL_ARRAY_H
#include <stdlib.h>
#include <stddef.h>

/**
  Initial capacity of the array.
  */
#ifndef U_INIT_CAPACITY
#define U_INIT_CAPACITY 10
#endif
/**
  Factor to grow the array by when it it is full..
  */
#ifndef U_GROWTH
#define U_GROWTH 3 / 2
#endif
size_t u_array_grow(void **array, size_t current_capacity, size_t object_size);

/**
  Creates a new array.

  @param array Name of the struct variable containing the array
  @param element Name of the struct pointer element that represents the array
  */
#define U_ARRAY(array, element) do {					\
    (array).len = 0;					\
    (array).element = NULL;					\
    (array).capacity = u_array_grow( (void *) &(array).element, \
            0, sizeof(*(array).element));	\
} while(0)

/**
  Appends an element to the back of an array.

  @param array Struct variable containing the array
  @param element Struct pointer element that represents the array
  @param object Variable to append to the array

    array.capacity will be set to 0 if there was an error.

  */
#define U_ARRAY_PUSH(array, element, object)				\
    do {								\
        if ((array).len == (array).capacity)			\
        (array).capacity = u_array_grow( (void *)		\
                &(array).element,	\
                (array).capacity,	\
                sizeof(*(array).element)); \
        if ((array).capacity == 0) break; \
        (array).element[(array).len] = object; \
        ++(array).len; \
    } while (0)

/**
  Removes an element from the back of an array.

  @param array Struct variable containing the array
  @param element Struct pointer element that represents the array
  @param object Variable to place the last element of the array into
  */
#define U_ARRAY_POP(array, element, object)				\
    do {								\
        assert((array).len);					\
        --(array).len;						\
        object = (array).element[(array).len];			\
    } while(0)

/**
  Finds an element in an array.

  @param array Struct variable containing the array
  @param element Struct pointer element that represents the array
  @param key value to find
  @return index in array of the element or array.len if not found
  */
#define U_ARRAY_FIND(array, element, key, index)				\
    do {								\
        index = (array).len;					\
        for (size_t i = 0; i < (array).len; ++i)		\
        if ((array).element[i] == (key))		\
        index = i;				\
    } while(0)

/**
  Frees an array. Note it doesn't free the elements of the array.
  Either do that manually or use U_ARRAY_FREE_ELEM

  @param array Struct variable containing the array
  @param element Struct pointer element that represents the array
  */
#define U_ARRAY_FREE(array, element) do {		\
    free((array).element);		\
} while(0)

/**
  Converts two-parameter freeing function to single parameter one that
  doesn't require data parameter.
  */
#define U_ONE_PARM_CUSTOM_FREE_FUNC(func, x, _) func( (x) )

/**
  Converts two parameter function to standard free.
  */
#define U_ONE_PARM_FREE_FUNC(x, y) U_ONE_PARM_CUSTOM_FREE_FUNC(free, x, y)

/**
  Frees an array and its elements, using a custom user-provided free function.

  @param array Struct variable containing the array
  @param element Struct pointer element that represents the array
  @param free_func User provided freeing function
  @param data User provided data passed as second parameter to freeing function
  */
#define U_ARRAY_FREE_ELEMS_CUSTOM(array, element, free_func, data) do {	\
    for (size_t i = 0; i < (array).len; ++i)		\
    free_func( (array).element[i], data);		\
    U_ARRAY_FREE( (array), element );				\
} while(0)

/**
  Frees an array and its elements.

  @param array Struct variable containing the array
  @param element Struct pointer element that holds the array's data
  */
#define U_ARRAY_FREE_ELEMS(array, element) do {				\
    U_ARRAY_FREE_ELEMS_CUSTOM((array), element,		\
            U_ONE_PARM_FREE_FUNC, NULL);		\
} while(0)

#endif
