#include <container/vector.h>
#include <gtest/gtest.h>
#include <memory/utility.h>
#include <metaprogramming/types.h>

#define V_SIZE 50

using namespace sqrl;

static BlockViewer viewer;

static int reference_count = 0;
// no default constructor
struct NoMove {
  NoMove(int x) : data(new int(x)) { reference_count++; }
  ~NoMove() { reference_count--; }
  NoMove() = delete;
  NoMove(NoMove &&) = delete;
  NoMove(const NoMove &other) : data(other.data) { reference_count++; }
  NoMove &operator=(const NoMove &other) {
    reference_count++;
    data = other.data;
    return *this;
  }
  int *data;
};

// no assignment operator method
struct NoCopy {
  NoCopy(int x) : data(new int(x)){};
  NoCopy(const NoCopy &) = delete;
  // only resize & push_back require = operator
  NoCopy &operator=(const NoCopy &other) {
    if (this != &other) {
      data = new int(-1);
    }
    return *this;
  };
  int *data;
};

// to ensure vector release resource correctly
class SafeVectorTest : public ::testing::Test {
protected:
  void SetUp() { reference_count = 0; }
  void TearDown() {
    EXPECT_EQ(reference_count, 0);
    reference_count = 0;
  }
};

TEST(test_vector, simple_create_access) {
  Vector<int> v;
  v.push_back(1);
  v.push_back(2);
  v.push_back(3);
  EXPECT_EQ(v[0], 1);
  EXPECT_EQ(v[1], 2);
  EXPECT_EQ(v[2], 3);
  EXPECT_EQ(v.size(), 3);
  EXPECT_EQ(viewer.memory_size(), align(sizeof(int) * V_SIZE));
}

TEST(test_vector, resize_when_exceed_capacity) {
  {
    Vector<int> v;
    size_t test_size = 2000;
    size_t vector_size = V_SIZE;
    for (size_t i = 0; i < test_size; i++) {
      v.push_back(i);
      // mock resize behavior
      if (i > vector_size) {
        vector_size *= 2;
      }
    }
    EXPECT_EQ(v.size(), test_size);
    for (size_t i = 0; i < test_size; i++) {
      EXPECT_EQ(v[i], i);
    }
    EXPECT_EQ(viewer.memory_size(), align(sizeof(int) * vector_size));
  }
  EXPECT_EQ(viewer.memory_size(), 0);
}

TEST(test_vector, initialize_by_initializer_list) {
  Vector<int> v = {1, 2, 3};
  EXPECT_EQ(v[0], 1);
  EXPECT_EQ(v[1], 2);
  EXPECT_EQ(v[2], 3);
  EXPECT_EQ(v.size(), 3);

  EXPECT_EQ(viewer.memory_size(), align(sizeof(int) * V_SIZE));
}

TEST(test_vector, vector_iterator) {
  Vector<int> v = {1, 2, 3};
  size_t element = 1;
  for (auto itr = v.begin(); itr != v.end(); itr++) {
    EXPECT_EQ(*itr, element++);
  }
  EXPECT_EQ(viewer.memory_size(), align(sizeof(int) * V_SIZE));
}

TEST(test_vector, initialize_by_shallow_copy) {
  Vector<int> v = {1, 2, 3};
  Vector<int> v2(v);
  size_t element = 1;
  for (auto itr = v.begin(); itr != v.end(); itr++) {
    EXPECT_EQ(*itr, element++);
  }
  EXPECT_EQ(viewer.memory_size(), align(sizeof(int) * V_SIZE) * 2);
}

TEST(test_vector, pop_back) {
  Vector<int> v = {1, 2, 3};
  v.pop_back();
  EXPECT_EQ(v.size(), 2);
  v.push_back(100);
  EXPECT_EQ(v.size(), 3);
  EXPECT_EQ(v[0], 1);
  EXPECT_EQ(v[1], 2);
  EXPECT_EQ(v[2], 100);
  EXPECT_EQ(viewer.memory_size(), align(sizeof(int) * V_SIZE));
}

TEST(test_vector, initialize_by_iterator) {
  Vector<int> v = {1, 2, 3};
  Vector<int> v2(v.cbegin(), v.cend());
  size_t element = 1;
  for (auto itr = v.cbegin(); itr != v.cend(); itr++) {
    EXPECT_EQ(*itr, element++);
  }
  EXPECT_EQ(viewer.memory_size(), align(sizeof(int) * V_SIZE) * 2);
}

TEST(test_vector, pointer_element) {
  Vector<int *> v;
  int *i = new int(1);
  v.push_back(i);
  EXPECT_EQ(*(v[0]), 1);
  EXPECT_EQ(viewer.memory_size(), align(sizeof(int *) * V_SIZE));
}

// it is hard to support reference type
// since reference require actual value at initialization
// for example: int& a; is invalid
// vector may not have initialize at the very beginning

// TEST(test_vector, reference_element){
//     Vector<sqrl::reference_wrapper<int>> v;
//     int i = 3;
//     v.push_back(i);
//     EXPECT_EQ(v[0], 1);
// }

TEST(test_vector, emplace_back_compatible_with_normal_item) {
  Vector<int> v;
  v.emplace_back(1);
  EXPECT_EQ(v.size(), 1);
  EXPECT_EQ(v[0], 1);
}

TEST(test_vector, initialize_with_no_default_constructor_object) {
  Vector<NoMove> v;
  size_t test_size = 20;
  for (int i = 0; i < test_size; i++) {
    v.emplace_back(i);
  }
  EXPECT_EQ(v.size(), test_size);
  for (int i = 0; i < test_size; i++) {
    EXPECT_EQ(*(v[i].data), i);
  }
}

TEST(test_vector, emplace_back_does_no_copy) {
  Vector<NoCopy> v;
  // resize() involves = operator
  // make size smaller than vector original size
  // to avoid resize()
  size_t test_size = 20;
  for (int i = 0; i < test_size; i++) {
    v.emplace_back(i);
  }
  EXPECT_EQ(v.size(), test_size);
  for (int i = 0; i < test_size; i++) {
    EXPECT_EQ(*(v[i].data), i);
  }
}

TEST_F(SafeVectorTest, destructor_is_called) {
  Vector<NoMove> v;
  size_t test_size = 2000; // explicitly trigger resize
  for (int i = 0; i < test_size; i++) {
    v.emplace_back(i);
  }
  EXPECT_EQ(reference_count, 2000);
  EXPECT_EQ(v.size(), test_size);
  for (int i = 0; i < test_size; i++) {
    EXPECT_EQ(*(v[i].data), i);
  }
}

// clear() method should call destructor
// but will make Vector::size() back to 0
TEST_F(SafeVectorTest, clear) {
  Vector<NoMove> v;
  size_t test_size = 20;
  for (int i = 0; i < test_size; i++) {
    v.emplace_back(i);
  }
  EXPECT_EQ(reference_count, 20);
  EXPECT_EQ(v.size(), 20);
  v.clear();
  EXPECT_EQ(reference_count, 0); // destructor is called
  EXPECT_EQ(v.size(), 0);
}