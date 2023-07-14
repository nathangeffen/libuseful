/**
   \file

   \brief Definitions of array management functions

*/
#include <error.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <math.h>

#include "useful/array.h"

/**
   Grows an array if its capacity is full.
 */
size_t u_array_grow(void **array, size_t current_capacity, size_t object_size)
{
        size_t new_capacity = current_capacity == 0
            ? U_INIT_CAPACITY : current_capacity * U_GROWTH;
        size_t new_size = new_capacity * object_size;
        void *__t = realloc(*array, new_size);
        if (__t)
                *array = __t;
        else
                new_capacity = 0;
        return new_capacity;
}
