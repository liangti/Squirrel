cmake_minimum_required(VERSION 3.6)

project(Squirrel C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(
    squirrel
    src/compiler/types.cpp
    src/compiler/vtable.cpp
    src/memory/block.c
    src/memory/reclaimer.cpp
    src/memory/allocator.cpp
    src/memory/utility.cpp
    src/string/string.cpp
    src/thread/threadpool.cpp
)

target_compile_options(squirrel PRIVATE -fno-exceptions)

target_include_directories(
    squirrel
    PRIVATE
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
)

include(CTest)

set(CMAKE_ROOT_DIR ${CMAKE_SOURCE_DIR})
if (BUILD_TESTING)
    enable_testing()
    add_subdirectory(t)
    add_subdirectory(benchmark)
endif()