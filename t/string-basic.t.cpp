#include <gtest/gtest.h>
#include <memory/_allocator_impl.h>
#include <memory/block.h>
#include <string/string.h>

using namespace sqrl;

static BlockViewer viewer;

TEST(test_string_utils, strcpy) {
  char *c = new char[3];
  _strcpy(c, "abc");
  ASSERT_STREQ(c, "abc");
  delete[] c;
}
TEST(test_string_utils, strlen) {
  char *c = new char[3];
  _strcpy(c, "ab");
  ASSERT_EQ(_strlen(c), 2);
  delete[] c;
}

TEST(test_string_utils, strcmp) {
  const char *a = "abc";
  const char *b = "abedf";
  const char *c = "abc";
  EXPECT_TRUE(_strncmp(a, b, 0));
  EXPECT_TRUE(_strncmp(a, b, 1));
  EXPECT_TRUE(_strncmp(a, b, 2));
  EXPECT_FALSE(_strncmp(a, b, 3));
  EXPECT_FALSE(_strncmp(a, b, 4));
  EXPECT_FALSE(_strncmp(a, b, 5));
  EXPECT_TRUE(_strcmp(a, c));
  EXPECT_FALSE(_strcmp(a, b));
}

TEST(test_string_basic, simple_constructor) {
  String s("abc");
  ASSERT_STREQ(s.c_str(), "abc");
  ASSERT_EQ(s.size(), 3);
  ASSERT_EQ(viewer.memory_size(), alloc_size(3));
}

TEST(test_string_basic, copy_constructor) {
  String s("abc");
  String ss(s);
  ASSERT_STREQ(ss.c_str(), "abc");
  ASSERT_EQ(ss.size(), 3);
  ASSERT_EQ(viewer.memory_size(), alloc_size(3) * 2);
}

TEST(test_string_basic, move_constructor) {
  String s("abc");
  String ss(std::move(s));
  ASSERT_STREQ(ss.c_str(), "abc");
  EXPECT_TRUE(s.c_str() == nullptr);
  ASSERT_EQ(s.size(), 0);
  ASSERT_EQ(ss.size(), 3);
  ASSERT_EQ(viewer.memory_size(), alloc_size(3));
}

TEST(test_string_basic, copy_assignment) {
  String s("abc");
  String ss = s;
  ASSERT_STREQ(ss.c_str(), "abc");
  ASSERT_EQ(s.size(), 3);
  ASSERT_EQ(ss.size(), 3);
  ASSERT_EQ(viewer.memory_size(), alloc_size(3) * 2);
}

TEST(test_string_basic, move_assignment) {
  String s("abc");
  String ss = std::move(s);
  ASSERT_STREQ(ss.c_str(), "abc");
  EXPECT_TRUE(s.c_str() == nullptr);
  ASSERT_EQ(s.size(), 0);
  ASSERT_EQ(ss.size(), 3);
  ASSERT_EQ(viewer.memory_size(), alloc_size(3));
}

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wself-assign-overloaded"
#endif
TEST(test_string_basic, copy_self_assignment) {
  String s("abc");
  s = s;
  ASSERT_STREQ(s.c_str(), "abc");
  ASSERT_EQ(s.size(), 3);
  ASSERT_EQ(viewer.memory_size(), alloc_size(3));
}
#ifdef __clang__
#pragma clang diagnostic pop
#elif __GNUC__
#pragma GCC diagnostic pop
#endif

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wself-move"
#elif __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wself-move"
#endif
TEST(test_string_basic, move_self_assignment) {
  String s("abc");
  s = std::move(s);
  ASSERT_STREQ(s.c_str(), "abc");
  ASSERT_EQ(s.size(), 3);
  ASSERT_EQ(viewer.memory_size(), alloc_size(3));
}
#ifdef __clang__
#pragma clang diagnostic pop
#elif __GNUC__
#pragma GCC diagnostic pop
#endif

TEST(test_string_basic, string_concatenation) {
  String a("abc");
  String b("def");
  String c = a + b;
  ASSERT_STREQ(a.c_str(), "abc");
  ASSERT_STREQ(b.c_str(), "def");
  ASSERT_STREQ(c.c_str(), "abcdef");
}

TEST(test_string_basic, string_comparison) {
  String a("abc");
  String b("def");
  String c("abcas");
  EXPECT_TRUE(a == a);
  EXPECT_FALSE(a == b);
  EXPECT_FALSE(a == c);
}