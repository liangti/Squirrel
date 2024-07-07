cmake_minimum_required(VERSION 3.6)

project(Squirrel C CXX)

set(CMAKE_CXX_STANDARD "$ENV{CMAKE_CXX_STANDARD}")
set(CMAKE_BUILD_TYPE "$ENV{CMAKE_BUILD_TYPE}")
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(
    squirrel
    src/compiler/types.cpp
    src/compiler/vtable.cpp
    src/memory/_block.c
    src/memory/reclaimer.cpp
    src/memory/allocator.cpp
    src/memory/_allocator_impl.cpp
    src/string/string.cpp
    src/thread/threadpool.cpp
)

target_compile_options(squirrel PRIVATE -fno-exceptions -Werror=return-type)

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