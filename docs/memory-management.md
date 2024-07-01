# Memory Management

This document describes the design of memory management in Squirrel.

## Principle

The design of memory allocation modules should be:

- decouple as many as possible
- no transitive dependency
  - so that any child should directly talk to their parent but not visit their parent's parent or root

## Block

Implement in C.

**Design**:

- use system call to allocate new memory space on heap
- each block represents a piece of memory returns by system call
- each block contains metadata as header to describe the block status
  - is it being used
  - size of the data(length of the block)
  - pointer to next block
- provides methods to manipulate the block
  - get/set used
  - get/set size
  - split block
  - coalesce block 

**Details**

By default it uses `sbrk` to ask for new memory from OS, and will not return to OS until
program exits

For large block of memory it uses `mmap`, and will return memory to OS in release method

**Goal**:

- performance first
- easy to extend

**Not Goal**:

- implement allocation strategy
- manage blocks

## Allocator Implementation

`_allocator_impl.h`

Default implementation of allocator

*NO* other modules other than utility should directly talk to block C API.

This component contains:
- AllocatorDefaultImpl
- BlockAgent
- BlockManager
- BlockViewer

### BlockAgent

A C++ API to carry and get/set block. Act as an agent of a block.

Instead of directly visiting block C API, any module outside of utility should use BlockAgent

### BlockManager

Managing all blocks on hand.

In this layer one can implement memory management operations by using block C API, such as:

- split too large block
- coalesce neighborhood blocks
- ...

However, it *should NOT* involve a specific strategy.

Instead, it should just provide ability for upper level to have memory allocation strategy

Please notice that any operation directly on block should implement in block C module.

### BlockViewer

Do not own/change any block but can view the memory state.

Potentially it provides users a more insightful view of their memory allocation


## Allocator Interface

`allocator.h` provides basic interface(compatible with std::allocator)