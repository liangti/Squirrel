#include <gtest/gtest.h>
#include <memory/_allocator_impl.h>
#include <memory/allocator.h>

using namespace sqrl;
class TestObj {
public:
  int x;
  int y;
  double z;
  TestObj() {}
  ~TestObj() {}
};
static Allocator<TestObj> obj_allocator;
static BlockViewer viewer;
static BlockManager manager;

class MemoryAllocatorTest : public ::testing::Test {
protected:
  void SetUp() {
    ASSERT_EQ(viewer.memory_size(), 0);
    ASSERT_EQ(viewer.memory_used_size(), 0);
  }
  void TearDown() {
    manager.clear();
    ASSERT_EQ(viewer.memory_size(), 0);
    ASSERT_EQ(viewer.memory_used_size(), 0);
    ASSERT_TRUE(viewer.safe_check());
  }
};

TEST_F(MemoryAllocatorTest, alloc_block) {
  ASSERT_EQ(viewer.memory_size(), 0);
  auto data = obj_allocator.allocate(1);
  ASSERT_EQ(viewer.memory_size(), alloc_size(sizeof(TestObj)));
  Block *block = get_block((word_t *)data);
  ASSERT_EQ(&block->data[0], (word_t *)data);
  EXPECT_TRUE(used(block));
  ASSERT_EQ(get_size(block), alloc_size(sizeof(TestObj)));
  ASSERT_EQ(block->next, nullptr);
  obj_allocator.deallocate(data, 1);
  ASSERT_EQ(viewer.memory_size(), 0);
  EXPECT_FALSE(used(block));
}

TEST_F(MemoryAllocatorTest, reuse_block) {
  auto data = obj_allocator.allocate(1);
  Block *block = get_block((word_t *)data);
  obj_allocator.deallocate(data, 1);
  EXPECT_FALSE(used(block));

  data = obj_allocator.allocate(1);
  Block *block2 = get_block((word_t *)data);
  ASSERT_EQ(block, block2);
  EXPECT_TRUE(used(block));
}

TEST_F(MemoryAllocatorTest, find_free_block_skip_too_large_block) {
  auto data = obj_allocator.allocate(3);
  EXPECT_EQ(viewer.memory_size(), alloc_size(sizeof(TestObj) * 3));
  obj_allocator.deallocate(data, 3);
  EXPECT_EQ(viewer.memory_size(), 0);
  data = obj_allocator.allocate(1);
  // allocator should create another block
  EXPECT_EQ(viewer.block_number(), 2);
  EXPECT_EQ(viewer.memory_size(), alloc_size(sizeof(TestObj)));
}

TEST_F(MemoryAllocatorTest, all_allocators_share_state) {
  Allocator<TestObj> allocator2;
  ASSERT_EQ(viewer.memory_size(), 0);
  auto data = obj_allocator.allocate(1);
  ASSERT_EQ(viewer.memory_size(), alloc_size(sizeof(TestObj)));
  obj_allocator.deallocate(data, 1);
  ASSERT_EQ(viewer.memory_size(), 0);
}

TEST_F(MemoryAllocatorTest, allocate_a_list_of_objects) {
  TestObj *ptr = obj_allocator.allocate(200);
  ASSERT_EQ(viewer.memory_size(), alloc_size(200 * sizeof(TestObj)));
  obj_allocator.deallocate(ptr, 1);
  ASSERT_EQ(viewer.memory_size(), 0);
}

TEST_F(MemoryAllocatorTest, allocate_large_block) {
  size_t request_size_before = viewer.memory_requested_size();
  size_t size = 1024 * 2048;
  TestObj *ptr = obj_allocator.allocate(size);
  // validate actual memory requested from OS
  ASSERT_EQ(viewer.memory_requested_size() - request_size_before,
            alloc_size(size * sizeof(TestObj)));
  ASSERT_EQ(viewer.memory_size(), alloc_size(size * sizeof(TestObj)));
  obj_allocator.deallocate(ptr, size);
  ASSERT_EQ(viewer.memory_size(), 0);
  // validate memory release back to OS
  ASSERT_EQ(viewer.memory_requested_size(), request_size_before);
}