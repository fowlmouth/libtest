#include "libtest.h"

#include <stdlib.h>
#include <string.h>

struct test_case
{
  const char* name;
  libtest_testfunction fn;
};

struct test_suite
{
  const char* name;

  int case_count, case_capacity;
  struct test_case* cases;
};

int suite_count = 0, suite_capacity = 0;
struct test_suite* suites = NULL;

struct test_suite* get_suite(const char* name)
{
  for(struct test_suite* suite = suites, *end = suite + suite_count;
    suite != end;
    ++suite)
  {
    if(!strcmp(name, suite->name))
      return suite;
  }
  if(suite_count == suite_capacity)
  {
    suite_capacity = suite_capacity ? suite_capacity * 2 : 4;
    suites = realloc(suites, sizeof(struct test_suite) * suite_capacity);
  }
  struct test_suite* suite = suites + suite_count++;
  suite->name = name;
  suite->case_count = suite->case_capacity = 0;
  suite->cases = NULL;
  return suite;
}

void save_case(struct test_suite* suite, const char* name, libtest_testfunction fn)
{
  if(suite->case_count == suite->case_capacity)
  {
    suite->case_capacity = suite->case_capacity ? suite->case_capacity * 2 : 4;
    suite->cases = realloc(suite->cases, sizeof(struct test_case) * suite->case_capacity);
  }
  struct test_case* this_case = suite->cases + suite->case_count++;
  this_case->name = name;
  this_case->fn = fn;
}

int libtest_register_test(const char* testsuite, const char* testcase, libtest_testfunction fn)
{
  printf("registering suite='%s' case='%s'\n", testsuite, testcase);

  struct test_suite* suite = get_suite(testsuite);
  save_case(suite, testcase, fn);

  return 0;
}

void libtest_run_test_suite(struct test_suite* suite);
void libtest_run_all_suites()
{
  for(struct test_suite* iter = suites, *end = iter + suite_count; iter != end; ++iter)
  {
    libtest_run_test_suite(iter);
  }
}

void libtest_run_test_suite(struct test_suite* suite)
{
  const int error_buffer_size = 1024;
  char error_buffer[error_buffer_size];
  for(struct test_case* iter = suite->cases, *end = iter + suite->case_count; iter != end; ++iter)
  {
    int failures = 0;
    if(iter->fn(error_buffer_size, error_buffer))
    {
      fprintf(stderr, "%s:%s failed: %s\n", suite->name, iter->name, error_buffer);
      ++failures;
    }
  }
}
