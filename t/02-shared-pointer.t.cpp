#include <gtest/gtest.h>
#include <memory/smart_pointer.h>

using namespace sql;

size_t _counter_allocator::_free_size; 

TEST(shared_ptr_tests, init_smart_pointer){
    int *i = new int(1);
    shared_ptr<int> my_ptr(i);
}

TEST(shared_ptr_tests, dereference){
    int *i = new int(1);
    shared_ptr<int> my_ptr(i);
    ASSERT_EQ(*my_ptr, *i);
}

TEST(shared_ptr_tests, member_access){
    class A{
    public:
        int x;
        A(int x): x(x){};
    };
    A *a = new A(3);
    shared_ptr<A> my_ptr(a);
    ASSERT_EQ(my_ptr->x, 3);
}

TEST(shared_ptr_tests, recycle_memory_when_only_reference_release){
    int *i = nullptr;
    _counter_allocator::reset_free_size();
    {
        shared_ptr<int, _counter_allocator> my_ptr(i);
        ASSERT_EQ(_counter_allocator::get_free_size(), 0);
    }
    ASSERT_EQ(_counter_allocator::get_free_size(), sizeof(int) + sizeof(int));
}

TEST(shared_ptr_tests, do_not_recycle_memory_when_not_all_release){
    int *i = nullptr;
    _counter_allocator::reset_free_size();
    shared_ptr<int, _counter_allocator> my_ptr(i);
    ASSERT_EQ(my_ptr.get_count(), 1);
    {
        shared_ptr<int, _counter_allocator> my_ptr2 = my_ptr;
        ASSERT_EQ(_counter_allocator::get_free_size(), 0);
        ASSERT_EQ(my_ptr.get_count(), 2);
        ASSERT_EQ(my_ptr2.get_count(), 2);
    }
    ASSERT_EQ(my_ptr.get_count(), 1);
    ASSERT_EQ(_counter_allocator::get_free_size(), 0);
}