#include <gtest/gtest.h>
#include <sql_memory.h>

using namespace sql;

TEST(use_shared_ptr, general_test){
    int *i = new int(1);
    shared_ptr<int> my_ptr(i);

    ASSERT_EQ(*(*my_ptr), *i);
}