# Useful C library (libuseful)

Useful is a small and simple C library that provides a few useful algorithms,
macros and data structures for C programmers.

Useful includes:

- Very simple testing framework (see test.h)
- Array management macros and functions (see array.h)
- A few algorithms that are not already in glib or gsl (see algorithms.h)
- CSV file input and output, and conversion to and from R-like dataframes (see csv.h)

# Dependencies

Useful is installed using the [Meson build system](http://mesonbuild.com/)
(which is much better designed and easier to use than Autotools or CMake).

Here is how you'd install Meson on Ubuntu-like systems:

```bash
sudo apt-get install python3 ninja-build
pip3 install --user meson
```

See the [Meson website](http://mesonbuild.com/Quick-guide.html) for details.

# Installing

Download Useful from its public repository, unzip it and go into the unzipped folder.

Then on linux systems run:

```bash
meson build
cd build
sudo ninja install
```

For variations on the above see the [Running Meson](http://mesonbuild.com/Running-Meson.html/).

# Example

There are several example programs in the doc/examples folder.

## Array example

```C
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
	ARRAY_NEW(numbers, vals);
	for (size_t i = 0; i < 20; ++i)
		ARRAY_PUSH(numbers, vals, i);
	for (size_t i = 0; i < numbers.len; ++i)
		    printf("%d ", numbers.vals[i]);
	putchar('\n');
	ARRAY_FREE(numbers, vals);

	// Demonstrate freeing array elements
	// Print three arbitrary strings
	ARRAY_NEW(strings, str);
	ARRAY_PUSH(strings, str, strdup("CAT"));
	ARRAY_PUSH(strings, str, strdup("DOG"));
	ARRAY_PUSH(strings, str, strdup("MOUSE"));
	for (size_t i = 0; i < strings.len; ++i)
		puts(strings.str[i]);

	// Now there are two ways to free the array elements
	// for (size_t i = 0; i < strings.len; ++i)
	//	free(strings.str[i]);
	// ARRAY_FREE(strings, str);

	// But this is the easier way
	ARRAY_FREE_ELEMS(strings, str);

	return 0;
}
```

## CSV file processing example

```C
/*
Uses the file eg1.csv in the doc/examples/ subfolder:

First,Last,Age,Height
Joe,Bloggs,22.5,185
Jane,Doe,31.2,163
Peter,Piper,17,170
Colonel,Mustard,91.3,181.2
*/

#include <stdio.h>
#include <stdlib.h>

#include <useful.h>

int main(int argc, char *argv[])
{
	struct csv cs;
	FILE *f = fopen ("eg1.csv", "r");
	if (f == NULL) {
		fprintf(stderr, "Can't open file.\n");
		exit(1);
	}

	// Read eg1.csv into a csv struct. There is a header and the values
	// are delimted by a comma, hence parameters two and three
	// respectively.
	cs = csv_read(f, true, ',');

	// Check if the csv file is valid, i.e. conforms to RFC4180.
	printf("CSV is %s\n", csv_isvalid(&cs, true) ? "valid" : "invalid");

	// Print the number of rows
	printf("Rows: %zu\n", cs.len);

	// Print the header
	for (size_t i = 0; i < cs.len; ++i) {
		if (i > 0) putchar(',');
		printf("%s", cs.header.cells[i]);
	}
	putchar('\n');

	// Print all the csv values
	for (size_t i = 0; i < cs.len; ++i) {
		for (size_t j = 0; j < cs.rows[i].len; ++j) {
			if (j > 0) putchar(',');
			printf("%s",(csv_at(&cs, i, j)));
		}
		putchar('\n');
	}

	// Call this to avoid memory leaks
	csv_free(&cs);

	fclose(f);

	return 0;
}
```

# Compiling a program that links to Useful

The easiest way to compile programs using Useful is to use pkg-config. E.g.

```bash
gcc myprog.c `pkg-config --cflags --libs useful` -o myprog
```

# LICENSE

See LICENSE or COPYING for details.
