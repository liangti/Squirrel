#include <gtest/gtest.h>
#include <metaprogramming/cast.h>

#ifdef __SQRL_TEST_EXPECT_COMPILE_TIME_FAILURE
struct NotPolymorphism {};
TEST(test_dyn_cast, not_pointer) {
  NotPolymorphism a;
  NotPolymorphism b = sqrl::dyn_cast<NotPolymorphism>(a);
}
TEST(test_dyn_cast, no_virtual_table) {
  NotPolymorphism *a = nullptr;
  NotPolymorphism *b = nullptr;
  sqrl::dyn_cast(a, b);
}
#endif

struct Base {
  int base = 129;
  virtual int func(int x) { return 0; }
};

TEST(test_dyn_cast, same_type) {
  Base *a = new Base;
  Base *b = sqrl::dyn_cast<Base *>(a);
  ASSERT_EQ(a, b);
}

struct AddTwo : public Base {
  int addtwo = 235;
  int func(int x) { return x + 2; }
};

/*
Memory layout of a:
+------------------------
+ vtable(to AddTwo::func)
+ int base
+ int addtwo
+------------------------
Memory layout of b:
+------------------------
+ vtable(to Base::func)
+ int base
+------------------------
*/
TEST(test_dyn_cast, cast_to_base) {
  AddTwo *a = new AddTwo();
  Base *b = sqrl::dyn_cast<Base *>(a);
  ASSERT_EQ(b->func(1), 3); // since the vtable points to Addtwo::func
  ASSERT_EQ(b->base, 129);
  EXPECT_TRUE(sqrl::is_a<AddTwo>(b));
  EXPECT_FALSE(sqrl::is_a<Base>(b));
  EXPECT_TRUE(sqrl::is_a<AddTwo *>(b)); // pointer type is also acceptable
  EXPECT_FALSE(sqrl::is_a<Base *>(b));
}
/*
Memory layout of b:
+------------------------
+ vtable(to Base::func)
+ int base
+------------------------

So that downcast b to derived class is invalid since `addtwo`
member is not initialized
*/
TEST(test_dyn_cast, cast_to_derived) {
  Base *b = new Base();
  sqrl::type_info_register<Base, AddTwo>();
  AddTwo *a = sqrl::dyn_cast<AddTwo *>(b);
  ASSERT_EQ(a, nullptr);
  AddTwo *aa = new AddTwo();
  Base *bb = sqrl::dyn_cast<Base *>(aa);
  ASSERT_EQ(bb, aa);
  AddTwo *ab = sqrl::dyn_cast<AddTwo *>(bb);
  ASSERT_EQ(ab, aa);

  EXPECT_TRUE(sqrl::is_a<Base>(b));
  // EXPECT_TRUE(sqrl::is_a<Base>(a)); nullptr undefined yet.
  EXPECT_TRUE(sqrl::is_a<AddTwo>(aa));
  EXPECT_TRUE(sqrl::is_a<AddTwo>(bb));
  EXPECT_TRUE(sqrl::is_a<AddTwo>(ab));
}