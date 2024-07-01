#include <gtest/gtest.h>
#include <memory/destroy.h>

using namespace sqrl;

int reference_count = 0;
class TestObj {
public:
  TestObj() { reference_count++; }
  ~TestObj() { reference_count--; }
};

TEST(destroy, destroy_at) {
  TestObj *t = new TestObj();
  ASSERT_EQ(reference_count, 1);
  destroy_at(t);
  ASSERT_EQ(reference_count, 0);
}

TEST(destroy, destroy_n) {
  TestObj t[100];
  ASSERT_EQ(reference_count, 100);
  destroy_n(&t[0], 100);
  ASSERT_EQ(reference_count, 0);
}