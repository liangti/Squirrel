#include <gtest/gtest.h>
#include <memory/_block.h>

#define SQRL_ALLOCATOR_MMAP_THRESHOLD 131072

struct TestObj {
  int x;
  int y;
  double z;
};

TEST(memory_block_test, memory_align) {
  ASSERT_EQ(align(word_s - 1), word_s);
  ASSERT_EQ(align(word_s / 2), word_s);
  ASSERT_EQ(align(word_s), word_s);
  ASSERT_EQ(align(word_s + 1), word_s * 2);
  ASSERT_EQ(align(word_s * 2), word_s * 2);
}

TEST(memory_block_test, get_header) {
  Block *block = request_block_from_os(sizeof(TestObj));
  word_t *data = block->data;
  BlockHeader *header = get_header(data);
  ASSERT_EQ(block, (Block *)header);
}

TEST(memory_block_test, request_from_os) {
  Block *block = request_block_from_os(sizeof(TestObj));
  ASSERT_EQ(get_size(block), alloc_size(sizeof(TestObj)));
  release_block(block);
}

TEST(memory_block_test, request_large_block_from_os) {
  Block *block = request_large_block_from_os(sizeof(TestObj));
  // early stop the test if fail
  ASSERT_TRUE(block != NULL);
  ASSERT_EQ(get_size(block), alloc_size(sizeof(TestObj)));
  TestObj *test = (TestObj *)(block->data);
  // test this piece of memory is read/write able
  test->x = 1;
  test->y = 2;
  test->z = 3;
  EXPECT_EQ(test->x, 1);
  EXPECT_EQ(test->y, 2);
  EXPECT_EQ(test->z, 3);
  EXPECT_EQ(get_size(get_block((word_t *)test)), alloc_size(sizeof(TestObj)));
  // early stop the test if fail
  ASSERT_EQ(release_large_block_to_os(block), 0);
}

TEST(memory_block_test, split_block) {
  Block *block = request_block_from_os(sizeof(TestObj) * 4);
  // split will not change used block
  split(block, sizeof(TestObj));
  ASSERT_EQ(get_size(block), alloc_size(sizeof(TestObj) * 4));
  // split will not change block if with given size larger than block
  used_clear(block);
  split(block, sizeof(TestObj) * 30);
  ASSERT_EQ(get_size(block), alloc_size(sizeof(TestObj) * 4));

  split(block, sizeof(TestObj) * 2);
  EXPECT_TRUE(block->next != nullptr);
  EXPECT_FALSE(used(block->next));
  ASSERT_EQ(get_size(block->next),
            alloc_size(sizeof(TestObj) * 2) - sizeof(BlockHeader));
}

TEST(memory_block_test, coalesce_block) {
  Block *block = request_block_from_os(sizeof(TestObj));
  Block *block2 = request_block_from_os(sizeof(TestObj));
  used_clear(block);
  used_clear(block2);
  block->next = block2;
  coalesce_block(block);
  ASSERT_EQ(get_size(block), alloc_size(sizeof(TestObj)) * 2);
}