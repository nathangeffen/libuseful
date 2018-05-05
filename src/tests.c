/**
   Runs a suite of tests to check that the Useful library is working.

   To check with Valgrind in an automated test suite:

   valgrind --leak-check=full --error-exitcode=1 ./tests

 */
/// \cond


#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <useful.h>

#define ELEMS 200

#define rng NULL

static void test_the_tester(struct test_group * test_group)
{
	char *s1 = "Hello";
	char *s2 = "Hello";
	char *s3 = "hello";
	struct test_group testing_test_group;

	++(*test_group).successes;
	test_group_new(&testing_test_group);
	testing_test_group.verbose = false;
	TEST(s1, s2, strcmp, 0, testing_test_group);
	TEST(s1, s3, strcmp, 0, testing_test_group);
	TEST(testing_test_group.successes, 1, EQ, true, *test_group);
	TESTEQ(testing_test_group.successes, 1, *test_group);
	TESTEQ(testing_test_group.failures, 1, *test_group);
}

static void test_shuffling(struct test_group * test_group)
{
	int arr[ELEMS];
	bool shuffled;

	// Test corner cases (0 and 1 element)
	// 0 elements
	for (int i = 0; i < ELEMS; ++i) arr[i] = i;
	shuffle(arr, 0, sizeof(int), rng, rand_to);
	shuffled = false;
	for (int i = 1; i < ELEMS; ++i)
		if (arr[i] < arr[i - 1]) shuffled = true;
	TESTEQ(shuffled, false, *test_group);

	for (int i = 0; i < ELEMS; ++i) arr[i] = i;
	shuffle(arr, 1, sizeof(int), rng, rand_to);
	shuffled = false;
	for (int i = 1; i < ELEMS; ++i)
		if (arr[i] < arr[i - 1]) shuffled = true;
	TESTEQ(shuffled, false, *test_group);

	// Test that ordered array doesn't stay in order
	for (int i = 0; i < ELEMS; ++i) arr[i] = i;
	shuffle(arr, ELEMS, sizeof(int), rng, rand_to);
	shuffled = false;
	for (int i = 1; i < ELEMS; ++i)
		if (arr[i] < arr[i - 1]) {
			shuffled = true;
			break;
		}
	TESTEQ(shuffled, true, *test_group);
}


struct coord {
	double x;
	double y;
};

static double distance(const void *p1, const void *p2)
{
	const struct coord a = * (struct coord *) p1;
	const struct coord b = * (struct coord *) p2;
	double x_2 = (a.x - b.x) * (a.x - b.x);
	double y_2 = (a.y - b.y) * (a.y - b.y);
	double d = sqrt(x_2 + y_2);
	return d;
}

static void test_least_dist(struct test_group * test_group)
{
	struct coord a = {5.0, 7.0};
	struct coord arr[ELEMS];

	for (size_t i = 0; i < ELEMS; ++i) arr[i].x = arr[i].y = i;
	size_t index = least_dist(&a,arr,ELEMS, sizeof(struct coord), distance);
	TESTEQ(index, 6, *test_group);
}

#define MALE 0
#define FEMALE 1
#define HOMOSEXUAL 0
#define HETEROSEXUAL 1

struct agent {
	unsigned id;
	unsigned age;
	unsigned sex;
	unsigned sexor;
	unsigned risk;
	unsigned cluster;
	struct agent * partner;
};

static void calc_cluster(struct agent * agent)
{
	unsigned total;
	total = agent->age + agent->sex * 10 +
		agent->sexor * 100 + agent->risk * 1000;
	agent->cluster = total;
}

static void calc_clusters(struct agent ** agents, size_t nmemb)
{
	for(size_t i = 0; i < nmemb; ++i)
		calc_cluster(agents[i]);
}

static int cmp_cluster(const void *a, const void *b)
{
	const struct agent a_ = ** (struct agent **) a;
	const struct agent b_ = ** (struct agent **) b;
	if (a_.cluster < b_.cluster) return -1;
	if (a_.cluster > b_.cluster) return 1;
	return 0;
}

static bool has_partner(const void *a)
{
	const struct agent *a_ = * (struct agent **) a;
	return a_->partner;
}

static void set_partners(void *a, void *b)
{
	struct agent *a_ = * (struct agent **) a;
	struct agent *b_ = * (struct agent **) b;

	assert(a_->partner == NULL);
	assert(b_->partner == NULL);
	a_->partner = b_;
	b_->partner = a_;
}


static double agent_distance(const struct agent *a, const struct agent *b,
	bool check_partner)
{
	double total;

	if (check_partner && b->partner) return DBL_MAX;

	total = fabs( (double) a->age - b->age);
	total += fabs( (double )a->risk - b->risk) * 2;
	if (a->sexor != b->sexor) {
		total += 10.0;
	} else if (a->sexor == HOMOSEXUAL && a->sex != b->sex) {
		total += 10.0;
	} else if (a->sexor == HETEROSEXUAL && a->sex == b->sex) {
		total += 10.0;
	}
	return total;
}

static double agent_distance_v(const void *a, const void *b)
{

	const struct agent *a_ = * (struct agent **) a;
	const struct agent *b_ = * (struct agent **) b;
	return agent_distance(a_, b_, true);
}

void test_cspm(struct test_group * test_group)
{
	struct agent *agents[ELEMS];
	for (size_t i = 0; i < ELEMS; ++i) {
		struct agent * a = malloc(sizeof(struct agent));
		a->id = i;
		a->age = rand_to(5, rng);
		a->sex = rand_to(2, rng);
		a->sexor = rand_to(2, rng);
		a->risk =  rand_to(4, rng);
		a->partner = NULL;
		agents[i] = a;
	}
	calc_clusters(agents, ELEMS);

	cspm(agents, ELEMS, 10, 10, cmp_cluster, has_partner,
		set_partners, agent_distance_v, rng, rand_to);
	double total = 0.0;
	struct test_group t;
	test_group_new(&t);
	for (size_t i = 0; i < ELEMS; ++i) {
		assert(agents[i]->partner);
		assert(agents[i]->partner->partner);
		total += agent_distance(agents[i], agents[i]->partner, false);
		TESTEQ(agents[i], agents[i]->partner->partner, t);
	}
	TESTEQ(t.failures, 0, *test_group);
	double avg = total / (2 * ELEMS);
	TESTLT(avg, 2.0, *test_group);
	for (struct agent ** i = agents; i < agents + ELEMS; ++i) free(*i);

}

static void test_arrays(struct test_group * test_group)
{
	struct agent {
		size_t id;
		double match_weight;
		struct agent * partner;
		bool partner_set;
		void * data;
	};

	struct population {
		size_t len;
		size_t capacity;
		struct agent ** agents;
	};

	struct natural {
		size_t len;
		size_t capacity;
		unsigned *x;
	};

	struct population population;
	ARRAY_NEW(population, agents);
	for (size_t i = 0; i < ELEMS; ++i) {
		struct agent *a = malloc(sizeof(*a));
		a->id = i;
		a->match_weight = 20.0 * i;
		a->partner = NULL;
		a->partner_set = false;
		a->data = NULL;
		ARRAY_PUSH(population, agents, a);
	}

	do {
		struct test_group t;
		test_group_new(&t);
		for (size_t i = 0; i < ELEMS; ++i)
			TESTEQ(population.agents[i]->id, i, t);
		TESTEQ(t.successes, ELEMS, *test_group);
	} while(0);

	ARRAY_FREE_ELEMS(population, agents);

	struct natural natural;

	ARRAY_NEW(natural, x);
	for (size_t i = 0; i < ELEMS; ++i)
		ARRAY_PUSH(natural, x, i);

	do {
		struct test_group t;
		test_group_new(&t);
		for (size_t i = 0; i < ELEMS; ++i) TESTEQ(natural.x[i], i, t);
		TESTEQ(t.successes, ELEMS, *test_group);
	} while(0);

	TESTEQ(natural.len, ELEMS, *test_group);
	unsigned y;
	ARRAY_POP(natural, x, y);
	TESTEQ(y, ELEMS - 1, *test_group);
	TESTEQ(natural.len, ELEMS - 1, *test_group);
	ARRAY_FIND(natural, x, ELEMS / 2, y);
	TESTEQ(y, ELEMS / 2, *test_group);

	ARRAY_FREE(natural, x);
}

static void test_csv(struct test_group * test_group)
{
	const char * fieldnames[] = {"First", "Last", "age"};
	const char * dfieldnames[] = {"V1", "V2", "V3", "V4"};
	const enum val_type types[] = {str, str, dbl};
	const enum val_type dtypes[] = {dbl, dbl, dbl, dbl};

	const union str_dbl rec1[] = { {"John"}, {"James"}, {.dbl = 37.12345}};
	const union str_dbl rec2[] = { {"Leon"}, {"The Lion"}, {.dbl = 21}};
	union str_dbl rec3[] = { {"Igor"}, {"The \"Gentle\" Giant"},
				       {.dbl = 22.222222222}};
	union str_dbl rec4[] = { {"Lola"}, {"Lillyfield"},
				       {.dbl = 89.999999999999999999999999999}};

	struct dataframe df = dataframe_new(3, fieldnames, types);
	struct csv cs;
	struct matrix mat;

	dataframe_append_var(&df, "Joe", "Bloggs", 23.123457);
	dataframe_append_var(&df, "Jane", "Doe", 30.123456789);
	dataframe_append_var(&df, "Mary-Jane", "Smith", 29.234);
	dataframe_append_var(&df, "Peter", "Piper", 56.1);
	dataframe_append(&df, rec1);
	dataframe_append(&df, rec2);
	dataframe_append(&df, rec3);
	dataframe_append_var(&df, "Gloria", "Gaynor", 18.2);
	dataframe_append(&df, rec4);
	TESTEQ(df.rows, 9, *test_group);
	TESTEQ(df.cols, 3, *test_group);
	cs = dataframe_to_csv(&df);
	FILE * tmp = tmpfile();
	if (tmp)
		csv_write(tmp, &cs);
	else
		error(EXIT_FAILURE, errno, "Failed to open file.");
	rewind(tmp);
	dataframe_free(&df);
	csv_free(&cs);
	cs = csv_read(tmp, true, ',');
	df = csv_to_dataframe(&cs, types);
	TESTEQ(df.rows, 9, *test_group);
	TESTEQ(df.cols, 3, *test_group);
	TESTEQ(strcmp(dataframe_at(&df, 0, 0).str, "Joe"), 0, *test_group);
	TESTEQ(dataframe_at(&df, 8, 2).dbl, 90.0, *test_group);
	dataframe_free(&df);
	csv_free(&cs);

	df = dataframe_new(4, dfieldnames, dtypes);
	dataframe_append_var(&df, 1.0, 2.0, 3.0, 4.0);
	dataframe_append_var(&df, 10.0, 20.0, 30.0, 40.0);
	dataframe_append_var(&df, 100.0, 200.0, 300.0, 400.0);
	dataframe_append_var(&df, 1000.0, 2000.0, 3000.0, 4000.0);
	dataframe_append_var(&df, 10000.0, 20000.0, 30000.0, 40000.0);
	cs = dataframe_to_csv(&df);
	mat = csv_to_matrix(&cs);
	TESTEQ(matrix_at(&mat, 0, 0), 1.0, *test_group);
	TESTEQ(matrix_at(&mat, 4, 1), 20000.0, *test_group);
	matrix_set(&mat, 4, 1, 27.0);
	TESTEQ(matrix_at(&mat, 0, 3), 4.0, *test_group);
	TESTEQ(matrix_at(&mat, 4, 3), 40000.0, *test_group);
	TESTEQ(matrix_at(&mat, 4, 1),27.0, *test_group);
	matrix_free(&mat);
	csv_free(&cs);
	mat = dataframe_to_matrix(&df);
	TESTEQ(matrix_at(&mat, 0, 0), 1.0, *test_group);
	TESTEQ(matrix_at(&mat, 4, 1), 20000.0, *test_group);
	matrix_set(&mat, 4, 1, 27.0);
	TESTEQ(matrix_at(&mat, 0, 3), 4.0, *test_group);
	TESTEQ(matrix_at(&mat, 4, 3), 40000.0, *test_group);
	TESTEQ(matrix_at(&mat, 4, 1),27.0, *test_group);
	matrix_free(&mat);
	dataframe_free(&df);
	fclose(tmp);
}

int main()
{
	struct test_group test_group;

	test_group_new(&test_group);

	test_the_tester(&test_group);
	test_shuffling(&test_group);
	test_least_dist(&test_group);
	test_cspm(&test_group);
	test_arrays(&test_group);
	test_csv(&test_group);

	test_group_summary(&test_group);

	return test_group.failures;
}

/// \endcond
