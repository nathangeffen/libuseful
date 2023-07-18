#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <useful.h>


// Array of natural numbers
struct natural_nums {
	// The array must have a len member (declared as size_t)
	size_t len;
	// The array must have a capacity member (declared as size_t)
	size_t capacity;
	// The array needs a member where the values are stored,
	// though you can call the member any valid member name.
	unsigned * vals;
};


// Array of strings
struct string_arr {
	size_t len;
	size_t capacity;
	char ** str;
};

// Duplicate a string. Useful for the example.
char * strdup(const char * s)
{
	size_t n = strlen(s) + 1;
	char * res = malloc(sizeof(char) * n);
	if (res != NULL) strncpy(res, s, n);
	return res;
}

int main(int argc, char *argv[])
{
	struct natural_nums numbers;
	struct string_arr strings;

	// Print first 20 natural numbers
	U_ARRAY(numbers, vals);
	for (size_t i = 0; i < 20; ++i)
		U_ARRAY_PUSH(numbers, vals, i);
	for (size_t i = 0; i < numbers.len; ++i)
		    printf("%d ", numbers.vals[i]);
	putchar('\n');
	U_ARRAY_FREE(numbers, vals);

	// Demonstrate freeing array elements
	// Print three arbitrary strings
	U_ARRAY(strings, str);
	U_ARRAY_PUSH(strings, str, strdup("CAT"));
	U_ARRAY_PUSH(strings, str, strdup("DOG"));
	U_ARRAY_PUSH(strings, str, strdup("MOUSE"));
	for (size_t i = 0; i < strings.len; ++i)
		puts(strings.str[i]);

	// Now there are two ways to free the array elements
	// for (size_t i = 0; i < strings.len; ++i)
	//	free(strings.str[i]);
	// U_ARRAY_FREE(strings, str);

	// But this is the easier way
	U_ARRAY_FREE_ELEMS(strings, str);

	return 0;
}
