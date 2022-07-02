#include <gtest/gtest.h>
#include <memory/utility.h>

using namespace sqrl;
struct TestObj {
  int x;
  int y;
  double z;
};

TEST(memory_allocator_utils_test, memory_align) {
  ASSERT_EQ(align(word_s - 1), word_s);
  ASSERT_EQ(align(word_s / 2), word_s);
  ASSERT_EQ(align(word_s), word_s);
  ASSERT_EQ(align(word_s + 1), word_s * 2);
  ASSERT_EQ(align(word_s * 2), word_s * 2);
}

TEST(memory_allocator_utils_test, get_header) {
  Block *block = request_from_os(sizeof(TestObj));
  word_t *data = block->data;
  Block *header = get_header(data);
  ASSERT_EQ(block, header);
}

TEST(memory_allocator_utils_test, request_from_os) {
  Block *block = request_from_os(sizeof(TestObj));
  ASSERT_EQ(size_get(block), sizeof(TestObj));
  EXPECT_TRUE(used(block));
}

TEST(memory_allocator_utils_test, split_block) {
  Block *block = request_from_os(sizeof(TestObj) * 4);
  // split will not change used block
  split(block, sizeof(TestObj));
  ASSERT_EQ(size_get(block), sizeof(TestObj) * 4);
  // split will not change block if with given size larger than block
  used_clear(block);
  split(block, sizeof(TestObj) * 30);
  ASSERT_EQ(size_get(block), sizeof(TestObj) * 4);

  split(block, sizeof(TestObj) * 2);
  EXPECT_TRUE(block->next != nullptr);
  EXPECT_FALSE(used(block->next));
  ASSERT_EQ(size_get(block->next), sizeof(TestObj) * 2 - sizeof(BlockHeader));
}

TEST(memory_allocator_utils_test, coalesce_block) {
  Block *block = request_from_os(sizeof(TestObj));
  Block *block2 = request_from_os(sizeof(TestObj));
  used_clear(block);
  used_clear(block2);
  block->next = block2;
  coalesce_block(block);
  ASSERT_EQ(size_get(block), sizeof(TestObj) * 2);
}