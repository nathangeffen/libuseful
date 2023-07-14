/**
    @file

    @brief Macros and functions for simple unit testing.

    Declare a u_test_group struct and initialize it with u_test_group_new().
    Use the macros U_TEST, U_TESTEQ, U_TESTGT and U_TESTLT to run tests.  The
    function u_test_group_summary() prints the number of successes and failures
    in your test group.  To reduce the amount of output to stdout set the
    u_test_group's verbose value to false.
*/


#ifndef USEFUL_TEST_H
#define USEFUL_TEST_H

#include <stdio.h>
#include <stdbool.h>

/**
   Convenient wrapper for debug output.
*/

#define U_DBG(...)							\
	printf("DEBUG %s %d: ", __FILE__, __LINE__);			\
	printf(__VA_ARGS__);						\
	printf("\n")

/**
   Holds information about a test group, including number of successes,
   failures, and whether to use verbose output in tests.
 */

struct u_test_group {
	unsigned successes;
	unsigned failures;
	bool verbose;
};

#define U_EQ(X, Y) ( (X) == (Y) )

#define U_GT(X, Y) ( (X) > (Y) )

#define U_LT(X, Y) ( (X) < (Y) )

#define U_GE(X, Y) ( (X) >= (Y) )

#define U_LE(X, Y) ( (X) <= (Y) )

/**
   Macro that compares two operands for a test in a test group.

   \param X the first operand of the test
   \param Y the second operand fo the test
   \param comparator the operand use to compare X and Y, e.g. EQ, GT, LT
   \expected the expected result of the comparison, typically true or false
   \u_test_group test group to tally the success or failure in
 */
#define U_TEST(X, Y, comparator, expected, u_test_group)			\
	do {								\
		if ( comparator( (X), (Y) ) == (expected) )		\
			(u_test_group).successes++;			\
		else {							\
			(u_test_group).failures++;			\
			if ( (u_test_group).verbose)			\
				printf("Failed test in file %s "	\
					"at line %d: "			\
					"%s %s %s %s\n",		\
					__FILE__, __LINE__, #X, #Y,	\
					#comparator, #expected);	\
		}							\
	} while(0)


/**
   Macro that checks if two operands are equal for a test in a test group.

   \param X the first operand of the test
   \param Y the second operand fo the test
   \u_test_group test group to tally the success or failure in
 */
#define U_TESTEQ(X, Y, u_test_group) U_TEST( (X), (Y), U_EQ, true, u_test_group)

/**
   Macro that checks if the first operand is greater than the second
   operand for a test in a test group.

   \param X the first operand of the test
   \param Y the second operand fo the test
   \u_test_group test group to tally the success or failure in
 */
#define U_TESTGT(X, Y, u_test_group) U_TEST( (X), (Y), U_GT, true, u_test_group)

/**
   Macro that checks if the first operand is less than the second
   operand for a test in a test group.

   \param X the first operand of the test
   \param Y the second operand fo the test
   \u_test_group test group to tally the success or failure in
 */
#define U_TESTLT(X, Y, u_test_group) U_TEST( (X), (Y), U_LT, true, u_test_group)

/**
   Macro that checks if the first operand is greater than or equal to the second
   operand for a test in a test group.

   \param X the first operand of the test
   \param Y the second operand fo the test
   \u_test_group test group to tally the success or failure in
 */
#define U_TESTGE(X, Y, u_test_group) U_TEST( (X), (Y), U_GE, true, u_test_group)

/**
   Macro that checks if the first operand is less than or equal to the second
   operand for a test in a test group.

   \param X the first operand of the test
   \param Y the second operand fo the test
   \u_test_group test group to tally the success or failure in
 */
#define U_TESTLE(X, Y, u_test_group) U_TEST( (X), (Y), U_LE, true, u_test_group)


void u_test_group_new(struct u_test_group* test_group);
void u_test_group_summary(const struct u_test_group* test_group);

#endif
