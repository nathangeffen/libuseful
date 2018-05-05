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
