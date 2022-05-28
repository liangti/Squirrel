#include <gtest/gtest.h>
#include <memory/smart_pointer.h>

using namespace sql;

class _test_allocator : _allocator_base {
private:
  static size_t _free_size;

public:
  template <typename T> static void clean(T *data) {
    _allocator_base::clean(data);
    _free_size += sizeof(T);
  }
  static size_t get_free_size() { return _free_size; }
  static void reset_free_size() { _free_size = 0; }
};

class Obj {
public:
  int x;
  Obj(int x) : x(x) {}
};

size_t _test_allocator::_free_size;

TEST(unique_ptr_tests, dereference) {
  int *i = new int(1);
  unique_ptr<int> my_ptr(i);
  ASSERT_EQ(*my_ptr, 1);
}

TEST(unique_ptr_tests, member_access) {
  Obj *obj = new Obj(3);
  unique_ptr<Obj> my_ptr(obj);
  ASSERT_EQ(my_ptr->x, 3);
}

TEST(unique_ptr_tests, recycle_memory_whatever) {
  Obj *obj = new Obj(3);
  _test_allocator::reset_free_size();
  {
    unique_ptr<Obj, _test_allocator> my_ptr(obj);
    ASSERT_EQ(_test_allocator::get_free_size(), 0);
  }
  ASSERT_EQ(_test_allocator::get_free_size(), sizeof(Obj));
}

TEST(unique_ptr_test, swap_owner_ship_when_copy) {
  Obj *obj = new Obj(3);
  unique_ptr<Obj> my_ptr(obj);
  unique_ptr<Obj> my_ptr2 = my_ptr;
  EXPECT_TRUE(my_ptr.is_null());
  EXPECT_FALSE(!my_ptr.is_null());
  ASSERT_EQ(my_ptr2->x, 3);
}

TEST(shared_ptr_tests, dereference) {
  int *i = new int(1);
  shared_ptr<int> my_ptr(i);
  ASSERT_EQ(*my_ptr, *i);
}

TEST(shared_ptr_tests, member_access) {
  Obj *obj = new Obj(3);
  shared_ptr<Obj> my_ptr(obj);
  ASSERT_EQ(my_ptr->x, 3);
}

TEST(shared_ptr_tests, recycle_memory_when_only_reference_release) {
  Obj *obj = new Obj(3);
  _test_allocator::reset_free_size();
  {
    shared_ptr<Obj, _test_allocator> my_ptr(obj);
    ASSERT_EQ(_test_allocator::get_free_size(), 0);
  }
  ASSERT_EQ(_test_allocator::get_free_size(), sizeof(int) + sizeof(Obj));
}

TEST(shared_ptr_tests, do_not_recycle_memory_when_not_all_release) {
  Obj *obj = new Obj(3);
  _test_allocator::reset_free_size();
  shared_ptr<Obj, _test_allocator> my_ptr(obj);
  ASSERT_EQ(my_ptr.get_count(), 1);
  {
    shared_ptr<Obj, _test_allocator> my_ptr2 = my_ptr;
    ASSERT_EQ(_test_allocator::get_free_size(), 0);
    ASSERT_EQ(my_ptr.get_count(), 2);
    ASSERT_EQ(my_ptr2.get_count(), 2);
  }
  ASSERT_EQ(my_ptr.get_count(), 1);
  ASSERT_EQ(_test_allocator::get_free_size(), 0);
}

TEST(weak_ptr_test, dereference) {
  int *i = new int(3);
  shared_ptr<int> sp(i);
  weak_ptr<int> wp(sp);
  ASSERT_EQ(*wp, *i);
}

TEST(weak_ptr_test, member_access) {
  Obj *obj = new Obj(3);
  shared_ptr<Obj> sp(obj);
  weak_ptr<Obj> wp(sp);
  ASSERT_EQ(wp->x, 3);
}

TEST(weak_ptr_test, rely_on_shared_pointer_reference_count) {
  int *i = new int(3);
  shared_ptr<int> sp(i);
  weak_ptr<int> wp(sp);
  ASSERT_EQ(wp.get_count(), 1);
  shared_ptr<int> sp2 = sp;
  ASSERT_EQ(wp.get_count(), 2);
}

// test all smart pointer will not implicitly cast to pointer type
namespace test_cast {
constexpr int ORIGIN_RETURN_CODE = 100;
constexpr int UNIQUE_RETURN_CODE = 200;
constexpr int SHARED_RETURN_CODE = 300;
constexpr int WEAK_RETURN_CODE = 400;
int cast(int* x) { return ORIGIN_RETURN_CODE; }

int cast(sql::unique_ptr<int> x) { return UNIQUE_RETURN_CODE; }

int cast(sql::shared_ptr<int> x) { return SHARED_RETURN_CODE; }

int cast(sql::weak_ptr<int> x) { return WEAK_RETURN_CODE; }
}; // namespace test_cast

TEST(explicit_test, all_smart_pointers) {
  {
    int *i = new int(3);
    ASSERT_EQ(test_cast::cast(i), test_cast::ORIGIN_RETURN_CODE);
    delete i;
  }
  {
    int *i = new int(3);
    shared_ptr<int> sp(i);
    weak_ptr<int> wp(sp);
    ASSERT_EQ(test_cast::cast(sp), test_cast::SHARED_RETURN_CODE);
    ASSERT_EQ(test_cast::cast(wp), test_cast::WEAK_RETURN_CODE);
  }
  {
    int *i = new int(3);
    unique_ptr<int> up(i);
    ASSERT_EQ(test_cast::cast(up), test_cast::UNIQUE_RETURN_CODE);
  }
}

// this test is disabled since it will cause compile-time error
#ifdef __SQL_TEST_EXPECT_COMPILE_TIME_FAILURE
TEST(compile_test, DISABLED_all_smart_pointers){
  sql::unique_ptr<int[]> u1(new int[5]);
  sql::unique_ptr<int[5]> u2(new int[5]);
  sql::unique_ptr<int*> u3(new int(5));
  sql::shared_ptr<int[]> s1(new int[5]);
  sql::shared_ptr<int[5]> s2(new int[5]);
  sql::shared_ptr<int*> s3(new int[5]);
}
#endif