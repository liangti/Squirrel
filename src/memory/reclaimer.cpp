#include <memory/reclaimer.h>

namespace sql{

extern "C"{
    Block* _block_head = nullptr;
    Block* _block_top = nullptr;
    size_t _block_allocated = 0;
};

namespace {
static Block* _head = _block_head;
static Block* _top = _block_top;
static size_t _allocated = _block_allocated;

}; // anonymous namespace

void Reclaimer::run(){

}

};