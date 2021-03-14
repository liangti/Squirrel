#include <memory/reclaimer.h>

namespace sql {

namespace {
static Block *_head = _block_head;
static Block *_top = _block_top;
static size_t _allocated = _block_allocated;

}; // anonymous namespace

void Reclaimer::run() {}

}; // namespace sql