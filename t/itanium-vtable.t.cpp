#include <compiler/vtable.h>
#include <gtest/gtest.h>

struct Left {
  virtual int left() { return 1; }
};
struct Right {
  virtual int right() { return 2; }
};
struct Up {
  virtual int up() { return 3; }
};
struct Down {
  virtual int down() { return 4; }
};

struct Move : public Left, public Right, public Up, public Down {};

TEST(test_vtable, test_get_object_offset) {
  sqrl::compiler::store_object_memory_layout<Move, Left, Right, Up, Down>();
  size_t move_id = typeid(Move).hash_code();
  size_t left_id = typeid(Left).hash_code();
  size_t right_id = typeid(Right).hash_code();
  size_t up_id = typeid(Up).hash_code();
  size_t down_id = typeid(Down).hash_code();
  int offset = 0;
  ASSERT_EQ(sqrl::details::get_object_offset(move_id, left_id), offset);
  offset += sizeof(Left);
  ASSERT_EQ(sqrl::details::get_object_offset(move_id, right_id), offset);
  offset += sizeof(Right);
  ASSERT_EQ(sqrl::details::get_object_offset(move_id, up_id), offset);
  offset += sizeof(Up);
  ASSERT_EQ(sqrl::details::get_object_offset(move_id, down_id), offset);
}

TEST(test_vtable, test_get_vtable_offset) {
  Move *move = new Move();
  Left *left = dynamic_cast<Left *>(move);
  Right *right = dynamic_cast<Right *>(move);
  Up *up = dynamic_cast<Up *>(move);
  Down *down = dynamic_cast<Down *>(move);

  int offset = 0;
  ASSERT_EQ(sqrl::details::get_vtable_offset(__gEt_vTaBle(left)), offset);
  offset -= sizeof(Left);
  ASSERT_EQ(sqrl::details::get_vtable_offset(__gEt_vTaBle(right)), offset);
  offset -= sizeof(Right);
  ASSERT_EQ(sqrl::details::get_vtable_offset(__gEt_vTaBle(up)), offset);
  offset -= sizeof(Up);
  ASSERT_EQ(sqrl::details::get_vtable_offset(__gEt_vTaBle(down)), offset);
}

TEST(test_vtable, test_get_vtable_typeid) {
  Move *move = new Move();
  Left *left = dynamic_cast<Left *>(move);
  Right *right = dynamic_cast<Right *>(move);
  Up *up = dynamic_cast<Up *>(move);
  Down *down = dynamic_cast<Down *>(move);

  ASSERT_EQ(sqrl::details::get_vtable_typeid(__gEt_vTaBle(move)),
            typeid(*move).hash_code());
  ASSERT_EQ(sqrl::details::get_vtable_typeid(__gEt_vTaBle(left)),
            typeid(*left).hash_code());
  ASSERT_EQ(sqrl::details::get_vtable_typeid(__gEt_vTaBle(right)),
            typeid(*right).hash_code());
  ASSERT_EQ(sqrl::details::get_vtable_typeid(__gEt_vTaBle(up)),
            typeid(*up).hash_code());
  ASSERT_EQ(sqrl::details::get_vtable_typeid(__gEt_vTaBle(down)),
            typeid(*down).hash_code());
}