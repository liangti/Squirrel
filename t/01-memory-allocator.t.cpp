#include <gtest/gtest.h>
#include <memory/allocator.h>

using namespace sql;

class MemoryAllocatorTest: public ::testing::Test {
protected:
    void SetUp() override{
        begin = (char *)sbrk(0);
    }
    void TearDown(){
        reset(begin);
    }
private:
    char* begin;
};


TEST_F(MemoryAllocatorTest, memory_align){
    ASSERT_EQ(align(word_s - 1), word_s);
    ASSERT_EQ(align(word_s / 2), word_s);
    ASSERT_EQ(align(word_s), word_s);
    ASSERT_EQ(align(word_s + 1), word_s * 2);
    ASSERT_EQ(align(word_s * 2), word_s * 2);
}

TEST_F(MemoryAllocatorTest, get_block_header){
    Block *block = request_from_os(sizeof(size_t));
    word_t *data = block->data;
    Block *header = get_block_header(data);
    ASSERT_EQ(block, header);
}

TEST_F(MemoryAllocatorTest, alloc_block){
    auto data = alloc(sizeof(size_t) * 2);
    Block *block = get_block_header(data);
    EXPECT_TRUE(block->used);
    ASSERT_EQ(block->size, sizeof(size_t) * 2);
    free(data);
    EXPECT_FALSE(block->used);
}

TEST_F(MemoryAllocatorTest, reuse_block){
    auto data = alloc(sizeof(size_t));
    Block *block = get_block_header(data);
    free(data);
    
    data = alloc(sizeof(size_t));
    Block *block2 = get_block_header(data);
    ASSERT_EQ(block, block2);
    EXPECT_TRUE(block->used);
}

TEST_F(MemoryAllocatorTest, split_block){
    auto data = alloc(sizeof(size_t) * 2);
    Block *block = get_block_header(data);
    free(data);
    EXPECT_FALSE(block->used);

    Block *block2 = split_block(block, sizeof(size_t));
    EXPECT_TRUE(block2->next != nullptr);
    EXPECT_FALSE(block2->next->used);
    ASSERT_EQ(block2->next->size, sizeof(size_t));
}

TEST_F(MemoryAllocatorTest, coalesce_block){
    auto data = alloc(sizeof(size_t));
    auto data2 = alloc(sizeof(size_t));
    Block *block = get_block_header(data);
    free(data);
    free(data2);

    Block *block2 = coalesce_block(block);
    ASSERT_EQ(block, block2);
    ASSERT_EQ(block2->size, sizeof(size_t) * 2);
}