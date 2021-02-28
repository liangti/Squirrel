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

**Goal**:

- performance first
- easy to extend

**Not Goal**:

- implement allocation strategy
- manage blocks



## Utility

*May rename it in the future*

The goal is to provide a C++ API to manipulate blocks.

*NO* other modules other than utility should directly talk to block C API.

Memory allocator is built on top of it.

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

*May rename it in the future*

Do not own any block but can view the memory state.

Potentially it provides users a more insightful view of their memory allocation

U

## Allocator

The allocator is a reusable template to allocate memory for need.

It *should* make use of BlockManager to manipulate block.

It *could* implement its own memory allocation strategy, such as

- find free block before ask for a new one

In theory, it *should NOT* directly modify any block, such as

- split the block
- coalesce two free blocks

The principle is to let each allocation as fast as possible.

## Reclaimer

The memory reclaimer is for re-construct memory blocks for allocator.

Those operation should not be done in allocator since allocator has performance requirement.

The run of reclaimer is open for discussion, there are two potential options:

- in series, run it every N allocation(N is not determined yet)
- in parallel, run it in separate thread