#include <compiler/types.h>

namespace sqrl {

namespace {
// if <T1, T2> pair exists it means T1 is base class of T2
static std::unordered_set<std::pair<size_t, size_t>, pair_hash> type_edges;

}; // namespace

namespace compiler {
void insert_type_edge(size_t t1, size_t t2) { type_edges.emplace(t1, t2); }
}; // namespace compiler

namespace details {

bool is_base_of(size_t t1, size_t t2) {
  return type_edges.find(std::make_pair(t1, t2)) != type_edges.cend();
}

}; // namespace details

}; // namespace sqrl