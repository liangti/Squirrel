#include <gtest/gtest.h>
#include <memory/allocator.h>
#include <memory/utility.h>

using namespace sql;
static bool call_destructor = false;
class TestObj {
public:
  int x;
  int y;
  double z;
  TestObj() { call_destructor = false; }
  ~TestObj() { call_destructor = true; }
};
static Allocator<TestObj> allocator;
static BlockViewer viewer;
static BlockManager manager;

class MemoryAllocatorTest : public ::testing::Test {
protected:
  void SetUp() { ASSERT_EQ(viewer.size(), 0); }
  void TearDown() {
    manager.clear();
    ASSERT_EQ(viewer.size(), 0);
    blockSafeCheck();
  }

  void blockSafeCheck() { manager.safe_check(); }
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

TEST_F(MemoryAllocatorTest, alloc_block) {
  ASSERT_EQ(viewer.size(), 0);
  auto data = allocator.allocate(1);
  ASSERT_EQ(viewer.size(), sizeof(TestObj));
  Block *block = get_header((word_t *)data);
  ASSERT_EQ(&block->data[0], (word_t *)data);
  EXPECT_TRUE(used(block));
  ASSERT_EQ(size_get(block), sizeof(TestObj));
  ASSERT_EQ(block->next, nullptr);
  allocator.deallocate(data, 0);
  ASSERT_EQ(viewer.size(), 0);
  EXPECT_FALSE(used(block));
}

TEST_F(MemoryAllocatorTest, reuse_block) {
  auto data = allocator.allocate(1);
  Block *block = get_header((word_t *)data);
  allocator.deallocate(data, 0);
  EXPECT_FALSE(used(block));

  data = allocator.allocate(1);
  Block *block2 = get_header((word_t *)data);
  ASSERT_EQ(block, block2);
  EXPECT_TRUE(used(block));
}

TEST_F(MemoryAllocatorTest, find_free_block_skip_too_large_block) {
  auto data = allocator.allocate(2);
  Block *block = get_header((word_t *)data);
  ASSERT_EQ(viewer.size(), align(sizeof(TestObj) * 2));
  allocator.deallocate(data, 0);
  data = allocator.allocate(1);
  EXPECT_TRUE(viewer.size() < align(sizeof(TestObj) * 2));
}

TEST_F(MemoryAllocatorTest, memory_size) {
  ASSERT_EQ(viewer.size(), 0);
  allocator.allocate(2);
  ASSERT_EQ(viewer.size(), 2 * sizeof(TestObj));
}

TEST_F(MemoryAllocatorTest, deallocate_call_destructor) {
  auto data = allocator.allocate(1);
  allocator.deallocate(data, 1);
  EXPECT_TRUE(call_destructor);
}

TEST_F(MemoryAllocatorTest, all_allocators_share_state) {
  Allocator<TestObj> allocator2;
  ASSERT_EQ(viewer.size(), 0);
  auto data = allocator.allocate(1);
  ASSERT_EQ(viewer.size(), sizeof(TestObj));
  allocator.deallocate(data, 1);
  ASSERT_EQ(viewer.size(), 0);
}