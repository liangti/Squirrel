#include <gtest/gtest.h>
#include <memory/smart_pointer.h>

using namespace sql;

TEST(shared_ptr_tests, init_smart_pointer){
    int *i = new int(1);
    shared_ptr<int> my_ptr(i);

    ASSERT_EQ(*(*my_ptr), *i);
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