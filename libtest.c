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

  int data_size;
  void (*pre)(void*), (*post)(void*);
};

int suite_count = 0, suite_capacity = 0;
struct test_suite* suites = NULL;

static void do_nothing(void* arg)
{
  (void)arg;
}

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
  suite->data_size = 0;
  suite->pre = suite->post = do_nothing;
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
  struct test_suite* suite = get_suite(testsuite);
  save_case(suite, testcase, fn);

  return 0;
}

const char* lastreport_suite = NULL;
int lastreport_passed = 0, lastreport_total = 0;
void libtest_default_report(const char* testsuite, const char* testcase, int failed, int error_buffer_size, const char* error_buffer)
{
  if(lastreport_suite && lastreport_suite != testsuite)
  {
    fprintf(stderr, "%d/%d passed.\n\n", lastreport_passed, lastreport_total);
    lastreport_passed = lastreport_total = 0;
  }

  if(lastreport_suite != testsuite)
  {
    lastreport_suite = testsuite;
    if(!testsuite)
    {
      return;
    }
    fprintf(stderr, "Running test suite '%s'\n", testsuite);
  }

  fprintf(stderr, "[%s] %s", (failed ? "FAIL" : " OK "), testcase);
  if(failed)
  {
    fprintf(stderr, " %s", error_buffer);
  }
  fprintf(stderr, "\n");

  ++lastreport_total;
  if(!failed)
    ++lastreport_passed;
}

void libtest_run_test_suite(struct test_suite*, libtest_report);
void libtest_run_all_suites()
{
  libtest_run_all_suites_report(libtest_default_report);
}

void libtest_run_all_suites_report(libtest_report report)
{
  for(struct test_suite* iter = suites, *end = iter + suite_count; iter != end; ++iter)
  {
    libtest_run_test_suite(iter, report);
  }
}


void libtest_run_test_suite(struct test_suite* suite, libtest_report report)
{
  const int error_buffer_size = 1024;
  char error_buffer[error_buffer_size];
  void* data = calloc(suite->data_size, 1);
  void (*pre)(void*) = suite->pre,
    (*post)(void*) = suite->post;
  for(struct test_case* iter = suite->cases, *end = iter + suite->case_count; iter != end; ++iter)
  {
    pre(data);
    int failed = iter->fn(error_buffer_size, error_buffer, data);
    post(data);
    report(suite->name, iter->name, failed, error_buffer_size, error_buffer);
  }
  free(data);
  report(NULL, NULL, 0, 0, NULL);
}


void libtest_config_suite(const char* testsuite, int data_size, void(*pre)(void*), void(*post)(void*))
{
  struct test_suite* suite = get_suite(testsuite);
  suite->data_size = data_size;
  suite->pre = pre;
  suite->post = post;
}
