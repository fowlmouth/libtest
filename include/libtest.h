#ifndef TESTS_HEADER_INCLUDED
#define TESTS_HEADER_INCLUDED

#include <stdio.h>


#define ASSERT_MSG(bool, msg) \
  if(!(bool)) \
    FAIL("Assertion failed: %s (file= %s line= %d)", (msg), (__FILE__), (__LINE__));

#define ASSERT_MSGF(bool, msg, ...) \
  if(!(bool)) \
    FAIL((msg), __VA_ARGS__);

#define ASSERT(bool) \
  ASSERT_MSG(bool, #bool)

#define ASSERT_OP(lhs, op, rhs) \
  ASSERT_MSG(((lhs) op (rhs)), (#lhs " " #op " " #rhs))

#define ASSERT_EQ(lhs, rhs) \
  ASSERT_OP(lhs, ==, rhs)

#define ASSERT_LT(lhs, rhs) \
  ASSERT_OP(lhs, <, rhs)

#define ASSERT_GT(lhs, rhs) \
  ASSERT_OP(lhs, >, rhs)

// https://stackoverflow.com/questions/5891221/variadic-macros-with-zero-arguments
#define VA_OPT(...) , ##__VA_ARGS__
#define FAIL(msg, ...) \
  do{ \
    snprintf(errormsg, errormsg_size, (msg) ,##__VA_ARGS__); \
    *testresult = testresult_fail; \
    return; \
  }while(0)


#define PASS() \
  do{ \
    *testresult = testresult_pass; \
    return; \
  }while(0)

typedef enum {
  testresult_pass, testresult_fail
} libtest_testresult;

typedef void (*libtest_testfunction)(int, char*, void*, libtest_testresult*);

#define TEST_FUNCTION_NAME(testsuite, testcase) \
  _test_##testsuite##_##testcase

#define DECL_TEST(testsuite, testcase) \
  void TEST_FUNCTION_NAME(testsuite, testcase) (int errormsg_size, char* errormsg, void* data, libtest_testresult* testresult)

#define TEST(testsuite, testcase) \
  DECL_TEST(testsuite, testcase); \
  __attribute__((constructor)) \
  void _libtest_internal_register_##testsuite##_##testcase(void) \
  { \
    libtest_register_test(#testsuite, #testcase, TEST_FUNCTION_NAME(testsuite, testcase)); \
  } \
  DECL_TEST(testsuite, testcase)

#define SUITE(testsuite, data_size, pre_function, post_function) \
  __attribute__((constructor)) \
  void _libtest_internal_suitecfg_##testsuite(void) \
  { \
    libtest_config_suite(#testsuite, data_size, pre_function, post_function); \
  }

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

void libtest_config_suite(const char* testsuite, int data_size, void(*pre)(void*), void(*post)(void*));


#endif
