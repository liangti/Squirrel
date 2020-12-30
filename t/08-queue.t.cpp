#include <gtest/gtest.h>
#include <container/queue.h>

using namespace sql;

// no default constructor
class A{
public:
    A(int x): data(new int(x)){}
    A() = delete;
    int* data;
};

// no assignment operator method
class B{
public:
    B(int x): data(new int(x)){};
    B& operator=(const B&) = delete;
    int* data;
};

TEST(test_queue, simple_create){
    Queue<int> q;
    q.push(1);
    ASSERT_EQ(q.front(), 1);
    q.pop();
    EXPECT_TRUE(q.empty());
}

TEST(test_queue, cycle_push_pop){
    size_t full_size = 50;
    Queue<int> q(full_size);
    // make queue._head in the middle
    size_t init_size = 20;
    for(int i = 0; i < init_size; i++){
        EXPECT_FALSE(q.full());
        q.push(i);
    }
    for(int i = 0; i < init_size; i++){
        ASSERT_EQ(q.front(), i);
        q.pop();
    }
    // make queue._tail go back and before queue._head
    for(int i = 0; i < full_size; i++){
        EXPECT_FALSE(q.full());
        q.push(i);
    }
    for(int i = 0; i < full_size; i++){
        ASSERT_EQ(q.front(), i);
        q.pop();
    }
}

TEST(test_queue, init_does_not_require_default_constructor){
    Queue<A> q(50);
    for(int i = 0; i < 50; i++){
        A temp(i);
        q.push(temp);
    }
    ASSERT_EQ(q.size(), 50);
    for(int i = 0; i < 50; i++){
        ASSERT_EQ((*q.front().data), i);
        q.pop();
    }
}

TEST(test_queue, emplace_does_no_copy){
    Queue<B> q(50);
    for(int i = 0; i < 50; i++){
        q.emplace(i);
    }
    ASSERT_EQ(q.size(), 50);
    for(int i = 0; i < 50; i++){
        ASSERT_EQ((*q.front().data), i);
        q.pop();
    }
}