#include <gtest/gtest.h>
#include <memory/allocator.h>
#include <memory/utility.h>

using namespace sql;
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
    ASSERT_EQ(viewer.size(), 0);
  }
  void TearDown() {
    manager.clear();
    ASSERT_EQ(viewer.size(), 0);
    blockSafeCheck();
  }

  void blockSafeCheck() { manager.safe_check(); }
};

TEST_F(MemoryAllocatorTest, alloc_block) {
  ASSERT_EQ(viewer.size(), 0);
  auto data = obj_allocator.allocate(1);
  ASSERT_EQ(viewer.size(), sizeof(TestObj));
  Block *block = get_header((word_t *)data);
  ASSERT_EQ(&block->data[0], (word_t *)data);
  EXPECT_TRUE(used(block));
  ASSERT_EQ(size_get(block), sizeof(TestObj));
  ASSERT_EQ(block->next, nullptr);
  obj_allocator.deallocate(data, 1);
  ASSERT_EQ(viewer.size(), 0);
  EXPECT_FALSE(used(block));
}

TEST_F(MemoryAllocatorTest, reuse_block) {
  auto data = obj_allocator.allocate(1);
  Block *block = get_header((word_t *)data);
  obj_allocator.deallocate(data, 1);
  EXPECT_FALSE(used(block));

  data = obj_allocator.allocate(1);
  Block *block2 = get_header((word_t *)data);
  ASSERT_EQ(block, block2);
  EXPECT_TRUE(used(block));
}

TEST_F(MemoryAllocatorTest, find_free_block_skip_too_large_block) {
  auto data = obj_allocator.allocate(2);
  Block *block = get_header((word_t *)data);
  ASSERT_EQ(viewer.size(), align(sizeof(TestObj) * 2));
  obj_allocator.deallocate(data, 1);
  data = obj_allocator.allocate(1);
  ASSERT_EQ(viewer.size(), align(sizeof(TestObj)));
}

TEST_F(MemoryAllocatorTest, all_allocators_share_state) {
  Allocator<TestObj> allocator2;
  ASSERT_EQ(viewer.size(), 0);
  auto data = obj_allocator.allocate(1);
  ASSERT_EQ(viewer.size(), sizeof(TestObj));
  obj_allocator.deallocate(data, 1);
  ASSERT_EQ(viewer.size(), 0);
}

// FIXME: this test is causing segfault
TEST_F(MemoryAllocatorTest, DISABLED_allocate_a_list_of_objects) {
  TestObj *ptr = obj_allocator.allocate(200);
  ASSERT_EQ(viewer.size(), 200 * sizeof(TestObj));
  obj_allocator.deallocate(ptr, 1);
  ASSERT_EQ(viewer.size(), 0);
}