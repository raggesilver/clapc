#pragma once

// This file defines macros for a simple test framework.

#ifndef CTEST_FAILED_ASSERTION_ABORTS
#define CTEST_FAILED_ASSERTION_ABORTS false
#endif

// If an assertion fails, return from the test function. In practice, we sill
// run the code, but we skip the assertions.
#ifndef CTEST_FAILED_ASSERTION_SKIPS
#define CTEST_FAILED_ASSERTION_SKIPS true
#endif

#ifndef CTEST_VERBOSE_EXPECT
#define CTEST_VERBOSE_EXPECT false
#endif

#ifdef CI_TEST
#define BOLD ""
#define RESET ""
#define RED ""
#define GREEN ""
#define BOLD_RED ""
#define BOLD_GREEN ""
#define BOLD_YELLOW ""
#define BOLD_UNDERLINE ""
#else
#define BOLD "\033[1m"
#define RESET "\033[0m"
#define RED "\033[31m"
#define GREEN "\033[32m"
#define BOLD_RED "\033[1;31m"
#define BOLD_GREEN "\033[1;32m"
#define BOLD_YELLOW "\033[1;33m"
#define BOLD_UNDERLINE "\033[1;4m"
#endif

#define RESULT_FAIL 1

// Test framework ==============================================================

static int result = 0;
static int assertions = 0;
static int failed_assertions = 0;

static int function_assertions = 0;
static int function_skipped_assertions = 0;
static int function_failed_assertions = 0;
static bool function_skip = false;

#define TEST_RESULTS result

#define begin_test(func)                                                       \
  do {                                                                         \
    function_assertions = 0;                                                   \
    function_skipped_assertions = 0;                                           \
    function_failed_assertions = 0;                                            \
    function_skip = false;                                                     \
    if (!CTEST_VERBOSE_EXPECT) {                                               \
      fprintf(stdout, "Test: " BOLD_UNDERLINE "%s" RESET, #func);              \
    } else {                                                                   \
      fprintf(stdout, "Test: " BOLD_UNDERLINE "%s\n" RESET, #func);            \
    }                                                                          \
    func();                                                                    \
  } while (0)

#define end_test()                                                             \
  do {                                                                         \
    if (CTEST_VERBOSE_EXPECT) {                                                \
      fprintf(stdout, BOLD_UNDERLINE "%s " RESET "completed", __func__);       \
    }                                                                          \
    fprintf(stdout,                                                            \
      " %s" RESET " (%d total assertions, %d failed assertions)\n" RESET,      \
      function_skip                       ? BOLD_YELLOW "SKIP"                 \
        : function_failed_assertions == 0 ? BOLD_GREEN "OK"                    \
                                          : BOLD_RED "FAIL",                   \
      function_assertions, function_failed_assertions);                        \
  } while (0)

// We don't need to do anything here, but we need to define these macros to
// future-proof the framework.
#define begin_suite()                                                          \
  do {                                                                         \
  } while (0)

#define end_suite()                                                            \
  ({                                                                           \
    fprintf(stdout, "\nTest suite completed %s\n" RESET,                       \
      result == 0 ? BOLD_GREEN "successfully" : BOLD_RED "with failures");     \
    fprintf(stdout, "Total assertions: %d\n", assertions);                     \
    fprintf(stdout, "Failed assertions: %d\n", failed_assertions);             \
    TEST_RESULTS;                                                              \
  })

/**
 * @brief Assert that a condition is true. Since expect calls may be skipped,
 * it is recommended that your tests don't rely on conditions being evaluated.
 *
 * @example
 *
 * // This is fine.
 * autor result = my_function();
 * expect(result == 0);
 *
 * // This is might cause issues!
 * expect(my_function() == 0);
 */
#define expect(cond)                                                           \
  do {                                                                         \
    function_assertions++;                                                     \
    if ((CTEST_FAILED_ASSERTION_SKIPS && function_failed_assertions > 0)       \
      || function_skip) {                                                      \
      function_skipped_assertions++;                                           \
      fprintf(stderr,                                                          \
        BOLD_YELLOW "[ SKIP ] " RESET "Assertion skipped (" __FILE__           \
                    ":%d %s): " #cond "\n",                                    \
        __LINE__, __func__);                                                   \
      break;                                                                   \
    }                                                                          \
    if (!(cond)) {                                                             \
      fprintf(stderr,                                                          \
        BOLD_RED "[ FAIL ] " RESET "Assertion failed (" __FILE__               \
                 ":%d %s): " #cond "\n",                                       \
        __LINE__, __func__);                                                   \
      failed_assertions++;                                                     \
      function_failed_assertions++;                                            \
      result = RESULT_FAIL;                                                    \
      if (CTEST_FAILED_ASSERTION_ABORTS) {                                     \
        abort();                                                               \
      }                                                                        \
    } else {                                                                   \
      if (CTEST_VERBOSE_EXPECT) {                                              \
        fprintf(                                                               \
          stdout, BOLD_GREEN "[  OK  ]" RESET " %s " #cond "\n", __func__);    \
      }                                                                        \
    }                                                                          \
    assertions++;                                                              \
  } while (0)

#define test_skip()                                                            \
  do {                                                                         \
    if (function_assertions > 0) {                                             \
      fprintf(stderr,                                                          \
        BOLD_YELLOW "Warning: " RESET                                          \
                    "You are skipping a test after calling expect() in it. "   \
                    "This is likely a mistake.\n");                            \
    }                                                                          \
    function_skip = true;                                                      \
  } while (0)

#define test(func)                                                             \
  do {                                                                         \
    begin_test(func);                                                          \
    end_test();                                                                \
  } while (0)

#define bool_to_str(b) ((b) ? "true" : "false")
