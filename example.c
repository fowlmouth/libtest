#include "libtest.h"

int main()
{
  RUN_TESTS();
}

TEST(suite1, fail1)
{
  ASSERT_EQ(0, 1);

  PASS();
}

TEST(suite1, fail2)
{
  if(1)
  {
    FAIL("error message");//, 0);
  }

  PASS();
}
