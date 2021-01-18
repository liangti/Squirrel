#include <gtest/gtest.h>
#include <memory/allocator.h>

using namespace sql;
static bool call_destructor = false;
class TestObj{
public:
    int x;
    int y;
    double z;
    ~TestObj(){
        call_destructor = true;
    }
};
static Allocator<TestObj> allocator;

class MemoryAllocatorTest: public ::testing::Test {
protected:
    void SetUp(){
        ASSERT_EQ(allocator.allocated_size(), 0);
    }
    void TearDown(){
        allocator.clear();
        ASSERT_EQ(allocator.allocated_size(), 0);
    }   
};

TEST(memory_allocator_utils_test, memory_align){
    ASSERT_EQ(align(word_s - 1), word_s);
    ASSERT_EQ(align(word_s / 2), word_s);
    ASSERT_EQ(align(word_s), word_s);
    ASSERT_EQ(align(word_s + 1), word_s * 2);
    ASSERT_EQ(align(word_s * 2), word_s * 2);
}

TEST(memory_allocator_utils_test, get_block_header){
    Block* block = request_from_os(sizeof(TestObj));
    word_t *data = block->data;
    Block *header = get_block_header(data);
    ASSERT_EQ(block, header);
}

TEST(memory_allocator_utils_test, request_from_os){
    Block* block = request_from_os(sizeof(TestObj));
    ASSERT_EQ(block->size, sizeof(TestObj));
    EXPECT_TRUE(block->used);
}

TEST(memory_allocator_utils_test, split_block){
    Block* block = request_from_os(sizeof(TestObj) * 2);
    split_block(block, sizeof(TestObj));
    block->used = false;
    split_block(block, sizeof(TestObj) * 3);
    ASSERT_EQ(block->size, sizeof(TestObj) * 2);
    split_block(block, sizeof(TestObj));
    EXPECT_TRUE(block->next != nullptr);
    EXPECT_FALSE(block->next->used);
    ASSERT_EQ(block->next->size, sizeof(TestObj));
}

TEST(memory_allocator_utils_test, coalesce_block){
    Block* block = request_from_os(sizeof(TestObj));
    Block* block2 = request_from_os(sizeof(TestObj));
    block->used = false;
    block2->used = false;
    block->next = block2;
    coalesce_block(block);
    ASSERT_EQ(block->size, sizeof(TestObj) * 2);
}

TEST_F(MemoryAllocatorTest, alloc_block){
    auto data = allocator.allocate(1);
    Block *block = get_block_header((word_t*)data);
    ASSERT_EQ(&block->data[0], (word_t*)data);
    EXPECT_TRUE(block->used);
    ASSERT_EQ(block->size, sizeof(TestObj));
    ASSERT_EQ(block->next, nullptr);
    allocator.deallocate(data, 0);
    EXPECT_FALSE(block->used);
}

TEST_F(MemoryAllocatorTest, reuse_block){
    auto data = allocator.allocate(1);
    Block *block = get_block_header((word_t*)data);
    allocator.deallocate(data, 0);
    EXPECT_FALSE(block->used);

    data = allocator.allocate(1);
    Block *block2 = get_block_header((word_t*)data);
    ASSERT_EQ(block, block2);
    EXPECT_TRUE(block->used);
}

TEST_F(MemoryAllocatorTest, memory_size){
    ASSERT_EQ(allocator.allocated_size(), 0);
    allocator.allocate(1);
    ASSERT_EQ(allocator.allocated_size(), sizeof(TestObj));
    allocator.allocate(1);
    ASSERT_EQ(allocator.allocated_size(), 2 * sizeof(TestObj));
}

TEST_F(MemoryAllocatorTest, deallocate_call_destructor){
    auto data = allocator.allocate(1);
    call_destructor = false;
    allocator.deallocate(data, 1);
    EXPECT_TRUE(call_destructor);
}