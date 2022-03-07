#ifndef TESTS_HEADER_INCLUDED
#define TESTS_HEADER_INCLUDED

#include <stdio.h>


#define ASSERT_MSG(bool, msg) \
  if(!(bool)) \
    FAIL("Assertion failed: %s", (msg));

#define ASSERT_MSGF(bool, msg, ...) \
  if(!(bool)) \
    FAIL((msg), __VA_ARGS__);

#define ASSERT(bool) \
  ASSERT_MSG(bool, #bool)

#define ASSERT_OP(lhs, op, rhs) \
  ASSERT_MSG(((lhs) op (rhs)), (#lhs " " #op " " #rhs))

#define ASSERT_EQ(lhs, rhs) \
  ASSERT_OP(lhs, ==, rhs)

// https://stackoverflow.com/questions/5891221/variadic-macros-with-zero-arguments
#define VA_OPT(...) , ##__VA_ARGS__
#define FAIL(msg, ...) \
  do{ \
    snprintf(errormsg, errormsg_size, (msg) VA_OPT(__VA_ARGS__)); \
    return testresult_fail; \
  }while(0)


#define PASS() \
  return testresult_pass;

typedef enum {
  testresult_pass, testresult_fail
} libtest_testresult;

typedef libtest_testresult (*libtest_testfunction)(int, char*);

#define TEST_FUNCTION_NAME(testsuite, testcase) \
  _test_##testsuite_##testcase

#define DECL_TEST(testsuite, testcase) \
  libtest_testresult TEST_FUNCTION_NAME(testsuite, testcase) (int errormsg_size, char* errormsg)

#define TEST(testsuite, testcase) \
  DECL_TEST(testsuite, testcase); \
  __attribute__((constructor)) \
  void _libtest_internal_register_##testsuite_##testcase(void) \
  { \
    libtest_register_test(#testsuite, #testcase, TEST_FUNCTION_NAME(testsuite, testcase)); \
  } \
  DECL_TEST(testsuite, testcase)

#define RUN_TESTS() \
  libtest_run_all_suites()


// struct libtest_report
// {
//   void (*report)(struct libtest_report*, int )
// };

typedef void (*libtest_report)(const char* testsuite, const char* testcase, int failing, int error_buffer_size, const char* error_buffer);

int libtest_register_test(const char* testsuite, const char* testcase, libtest_testfunction fn);
void libtest_run_all_suites();
void libtest_run_all_suites_report(libtest_report);

void libtest_default_report(const char* testsuite, const char* testcase, int failed, int error_buffer_size, const char* error_buffer);



#endif
