/* Generic C functions */

#include <error.h>
#include <errno.h>
#include <useful/algorithms.h>

/**
   Makes a duplicate of a string. It is the caller's responsibility to
   free the copy.

   \param src Null terminated string.
   \return Copy of the parameter string
 */

char *u_strdup(const char *src)
{
        char *dest = malloc(strlen(src) + 1), *p = dest;
        if (dest == NULL)
                return NULL;
        while (*src)
                *p++ = *src++;
        *p = 0;
        return dest;
}

/**
   Generates a random number in the semi-open range 0 .. to - 1.
   \param to Upper bound of semi-open range from which to draw random number
   \param rng NULL for now
 */

uint32_t u_rand_to(uint32_t to)
{
        uint32_t num_bins = to;
        uint32_t num_rand = ~(to & 0);
        uint32_t bin_size = num_rand / num_bins;
        uint32_t defect = num_rand % num_bins;

        uint32_t x;
        do {
                x = U_RANDOM;
        } while (num_rand - defect <= (unsigned long)x);

        return x / bin_size;
}

/**
   Swaps the values of *a* and *b*.

   \param size The number of bytes contained in *a* and *b*.
 */

void u_swap(void *a, void *b, size_t size)
{
        char t[size];
        memmove(t, a, size);
        memmove(a, b, size);
        memmove(b, t, size);
}

/**
   Shuffles an array.

   \param data Array to shuffle
   \param nmemb Number of elements in the array
   \param size Size of an element in chars (or bytes)
   \param gen Random number generator (can be NULL)
   \param rand_to Function that returns random int than its first parameter.
   Its second parameter is gen.
 */
void u_shuffle(void *data, size_t nmemb, size_t size)
{
        for (size_t i = nmemb; i-- > 1;) {
                size_t __to__ = i + 1;
                size_t j = U_RANDOM % __to__;
                u_swap(U_VOS(data, i, size), U_VOS(data, j, size), size);
        }
}

