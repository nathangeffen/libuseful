/**
    @file

    @brief Provides some useful generic algorithms using void pointers.

*/

#ifndef USEFUL_ALGORITHMS_H
#define USEFUL_ALGORITHMS_H

// Define this above the include of algorithms.h or useful.h if
// you want to use a different random number generator
#ifndef U_RANDOM
#define U_RANDOM rand()
#endif

#include <float.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/// Calculate offset for void pointer array
#define U_VOS(base, i, elem_size) (char *) (base) + ( (i) * (elem_size))

char *u_strdup(const char *src);

uint32_t u_rand_to(uint32_t to);

void u_swap(void *a, void *b, size_t size);

void u_shuffle(void *data, size_t nmemb, size_t size);

#endif
