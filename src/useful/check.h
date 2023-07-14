/**
  @file

  @brief Macros for dynamic error checking to make C a little easier and
  quicker to use.
  */

#ifndef USEFUL_CHECK_H
#define USEFUL_CHECK_H

/**
  Runs a portion of code and if it generates an error, executes error handling.

  @param code Executes this code
  @param cond Error check - if it evaluates to true then there's an error
  @param on_cond Code to execute if there's an error
  */
#define U_CHECK(code, cond, on_error) \
    do { \
        code; \
        if (cond) { \
            on_error; \
        } \
    } while(0)

#endif
