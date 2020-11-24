#include <gtest/gtest.h>
#include <memory/allocator.h>

using namespace sql;


TEST(memory_allocator_unittest, memory_align){
    ASSERT_EQ(align(word_s - 1), word_s);
    ASSERT_EQ(align(word_s / 2), word_s);
    ASSERT_EQ(align(word_s), word_s);
    ASSERT_EQ(align(word_s + 1), word_s * 2);
    ASSERT_EQ(align(word_s * 2), word_s * 2);
}

TEST(memory_allocator_unittest, get_block_header){
    Block *block = request_from_os(sizeof(size_t));
    word_t *data = block->data;
    Block *header = get_block_header(data);
    ASSERT_EQ(block, header);
}

TEST(memory_allocator_unittest, alloc_block){
    auto data = alloc(sizeof(size_t) * 2);
    Block *block = get_block_header(data);
    EXPECT_TRUE(block->used);
    ASSERT_EQ(block->size, sizeof(size_t) * 2);
    free(data);
    EXPECT_FALSE(block->used);
}

TEST(memory_allocator_unittest, reuse_block){
    auto data = alloc(sizeof(size_t));
    Block *block = get_block_header(data);
    free(data);
    
    data = alloc(sizeof(size_t));
    Block *block2 = get_block_header(data);
    ASSERT_EQ(block, block2);
}

TEST(memory_allocator_unittest, split_block){
    auto data = alloc(sizeof(size_t) * 2);
    Block *block = get_block_header(data);
    free(data);
    EXPECT_FALSE(block->used);

    Block *block2 = split_block(block, sizeof(size_t));
    EXPECT_TRUE(block2->next != nullptr);
    EXPECT_FALSE(block2->next->used);
    ASSERT_EQ(block2->next->size, sizeof(size_t));
}

TEST(memory_allocator_unittest, coalesce_block){
    auto data = alloc(sizeof(size_t));
    auto data2 = alloc(sizeof(size_t));
    Block *block = get_block_header(data);
    Block *block2 = coalesce_block(block);
    free(data);
    free(data2);

    ASSERT_EQ(block, block2);
    ASSERT_EQ(block2->size, sizeof(size_t) * 2);
}